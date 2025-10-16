#include "registration/Registration.hpp"      // affine
#include "registration/BSplineRegistration.hpp" // optional deformable
#include "itkImageFileReader.h"
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " fixedImage inputDir outputDir [--bspline 4,4,4]\n";
        return EXIT_FAILURE;
    }

    const std::string fixedFile = argv[1];
    fs::path inputDir  = argv[2];
    fs::path outputDir = argv[3];
    bool useBSpline = false;
    std::array<unsigned int,3> mesh{4,4,4};

    if (argc >= 6 && std::string(argv[4]) == "--bspline") {
        useBSpline = true;
        if (sscanf(argv[5], "%u,%u,%u", &mesh[0], &mesh[1], &mesh[2]) != 3) {
            std::cerr << "Invalid mesh string. Use e.g. 4,4,4\n";
            return EXIT_FAILURE;
        }
    }

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Input dir not found: " << inputDir << "\n";
        return EXIT_FAILURE;
    }
    fs::create_directories(outputDir);

    using ImageType = itk::Image<float,3>;

    // Load fixed once
    auto fixedReader = itk::ImageFileReader<ImageType>::New();
    fixedReader->SetFileName(fixedFile);
    fixedReader->Update();
    auto fixed = fixedReader->GetOutput();

    std::vector<fs::path> files;
    for (auto& p : fs::directory_iterator(inputDir)) {
        if (!p.is_regular_file()) continue;
        const auto ext = p.path().extension().string();
        if (ext == ".nii" || ext == ".gz" || ext == ".nrrd" || ext == ".mha" || ext == ".mhd")
            files.push_back(p.path());
    }
    if (files.empty()) {
        std::cerr << "No images found in " << inputDir << "\n";
        return EXIT_FAILURE;
    }

    std::cout << "Found " << files.size() << " images. Starting batch...\n";

    for (const auto& f : files) {
        try {
            // Skip if file is the same as fixed
            if (fs::equivalent(fixedFile, f)) continue;

            auto movingReader = itk::ImageFileReader<ImageType>::New();
            movingReader->SetFileName(f.string());
            movingReader->Update();
            auto moving = movingReader->GetOutput();

            auto outPath = outputDir / (f.stem().string() + "_reg.nrrd");

            // Stage 1: Affine
            auto affined = itkexp::registerImages<ImageType>(fixed, moving, outPath.string());

            // Optional stage 2: B-spline refinement
            if (useBSpline) {
                auto bsOut = outputDir / (f.stem().string() + "_bspline.nrrd");
                itkexp::bsplineRegister<ImageType>(fixed, affined, mesh, bsOut.string());
            }

        } catch (const itk::ExceptionObject& e) {
            std::cerr << "Failed on " << f << " : " << e << "\n";
        }
    }

    std::cout << "✅ Batch done. Outputs in " << outputDir << "\n";
    return EXIT_SUCCESS;
}
