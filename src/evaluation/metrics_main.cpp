#include "evaluation/Metrics.hpp"

#include <fstream>
#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

// Usage:
//   itk_metrics fixed moving registered [fixed_labels registered_labels] [--csv output/metrics.csv]
// Computes MSE/NCC before (fixed vs moving) and after (fixed vs registered).
// If label images are provided, computes Dice as well.

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cerr
            << "Usage: " << argv[0]
            << " fixed moving registered [fixed_labels registered_labels] [--csv metrics.csv]\n";
        return EXIT_FAILURE;
    }

    std::string fixedPath  = argv[1];
    std::string movingPath = argv[2];
    std::string regPath    = argv[3];

    std::string fixedLabPath, regLabPath, csvPath = "output/metrics.csv";
    for (int i = 4; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--csv" && i + 1 < argc) {
            csvPath = argv[++i];
            continue;
        }
        if (fixedLabPath.empty()) {
            fixedLabPath = a;
            continue;
        }
        if (regLabPath.empty()) {
            regLabPath = a;
            continue;
        }
    }

    constexpr unsigned int Dim = 3;
    using Pixel                = float;
    using Image                = itk::Image<Pixel, Dim>;
    using LabelPixel           = unsigned short;
    using LabelImage           = itk::Image<LabelPixel, Dim>;

    try {
        auto read = [](const std::string& p) {
            using Reader = itk::ImageFileReader<Image>;
            auto r       = Reader::New();
            r->SetFileName(p);
            r->Update();
            return r->GetOutput();
        };

        auto fixed  = read(fixedPath);
        auto moving = read(movingPath);
        auto reg    = read(regPath);

        const double mse_before = itkexp::computeMSE<Image>(fixed, moving);
        const double ncc_before = itkexp::computeNCC<Image>(fixed, moving);
        const double mse_after  = itkexp::computeMSE<Image>(fixed, reg);
        const double ncc_after  = itkexp::computeNCC<Image>(fixed, reg);

        double dice = -1.0;
        if (!fixedLabPath.empty() && !regLabPath.empty()) {
            using LReader = itk::ImageFileReader<LabelImage>;
            auto r1       = LReader::New();
            r1->SetFileName(fixedLabPath);
            r1->Update();
            auto r2 = LReader::New();
            r2->SetFileName(regLabPath);
            r2->Update();
            dice = itkexp::computeDice<LabelImage>(r1->GetOutput(), r2->GetOutput());
        }

        // Print summary
        std::cout << "== Metrics ==\n"
                  << "MSE  before: " << mse_before << "\n"
                  << "NCC  before: " << ncc_before << "\n"
                  << "MSE   after: " << mse_after << "\n"
                  << "NCC   after: " << ncc_after << "\n";
        if (dice >= 0.0)
            std::cout << "Dice (labels): " << dice << "\n";

        // Write CSV header if file doesn't exist
        bool writeHeader = true;
        {
            std::ifstream fin(csvPath);
            writeHeader = !fin.good();
        }
        std::ofstream csv(csvPath, std::ios::app);
        if (!csv) {
            std::cerr << "Failed to open CSV for writing: " << csvPath << "\n";
        } else {
            if (writeHeader) {
                csv << "fixed,moving,registered,mse_before,ncc_before,mse_after,ncc_after,dice\n";
            }
            csv << fixedPath << "," << movingPath << "," << regPath << "," << mse_before << ","
                << ncc_before << "," << mse_after << "," << ncc_after << ","
                << (dice >= 0.0 ? std::to_string(dice) : "") << "\n";
            std::cout << "📄 Metrics appended to " << csvPath << "\n";
        }
    } catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
