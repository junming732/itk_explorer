/**
 * ITK Multi-Modal Registration
 *
 * Main executable for mono-modal and multi-modal rigid registration
 *
 * Usage:
 *   itk_multimodal_register fixed.nii moving.nii output.nrrd [options]
 */

#include <cstring>
#include <iostream>
#include <string>

#include "evaluation/LandmarkEvaluation.h"
#include "landmarks/LandmarkIO.h"
#include "registration/MultiModalRegistration.h"

void PrintUsage(const char* progName)
{
    std::cout << "\nITK Multi-Modal Rigid Registration\n" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << progName << " <fixed> <moving> <output> [options]\n" << std::endl;

    std::cout << "Required Arguments:" << std::endl;
    std::cout << "  fixed          Fixed image (NIfTI format)" << std::endl;
    std::cout << "  moving         Moving image (NIfTI format)" << std::endl;
    std::cout << "  output         Output registered image (.nrrd)" << std::endl;

    std::cout << "\nOptions:" << std::endl;
    std::cout << "  --mode MODE              'mono' or 'multi' (default: multi)" << std::endl;
    std::cout << "  --iterations N           Maximum iterations (default: 1000)" << std::endl;
    std::cout << "  --pyramid-levels N       Pyramid levels (default: 3)" << std::endl;
    std::cout << "  --learning-rate R        Learning rate (default: 0.001)" << std::endl;
    std::cout << "  --save-transform FILE    Save transform (.tfm)" << std::endl;
    std::cout << "  --fixed-landmarks FILE   Fixed landmarks CSV" << std::endl;
    std::cout << "  --moving-landmarks FILE  Moving landmarks CSV" << std::endl;
    std::cout << "  --eval-output FILE       Evaluation results CSV" << std::endl;
    std::cout << "  --verbose                Print progress" << std::endl;
    std::cout << "  --help                   Show this help" << std::endl;

    std::cout << "\nExamples:" << std::endl;
    std::cout << "  # Multi-modal T1-T2" << std::endl;
    std::cout << "  " << progName << " T1.nii.gz T2.nii.gz output.nrrd --mode multi\n" << std::endl;

    std::cout << "  # Mono-modal with landmarks" << std::endl;
    std::cout << "  " << progName << " fixed.nii moving.nii output.nrrd --mode mono \\"
              << std::endl;
    std::cout << "    --fixed-landmarks fixed_lm.csv --moving-landmarks moving_lm.csv" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            PrintUsage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    // Required arguments
    std::string fixedPath  = argv[1];
    std::string movingPath = argv[2];
    std::string outputPath = argv[3];

    // Optional arguments
    Registration::RegistrationMode       mode = Registration::RegistrationMode::MULTI_MODAL;
    Registration::RegistrationParameters params;
    params.maxIterations    = 1000;
    params.pyramidLevels    = 3;
    params.learningRate     = 0.001;
    params.relaxationFactor = 0.95;
    params.initialRadius    = 7e-05;
    params.verbose          = false;

    std::string transformPath;
    std::string fixedLandmarksPath;
    std::string movingLandmarksPath;
    std::string evalOutputPath;

    // Parse options
    for (int i = 4; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--mode" && i + 1 < argc) {
            std::string modeStr = argv[++i];
            if (modeStr == "mono") {
                mode = Registration::RegistrationMode::MONO_MODAL;
            } else if (modeStr == "multi") {
                mode = Registration::RegistrationMode::MULTI_MODAL;
            } else {
                std::cerr << "Error: Invalid mode. Use 'mono' or 'multi'." << std::endl;
                return EXIT_FAILURE;
            }
        } else if (arg == "--iterations" && i + 1 < argc) {
            params.maxIterations = std::stoi(argv[++i]);
        } else if (arg == "--pyramid-levels" && i + 1 < argc) {
            params.pyramidLevels = std::stoi(argv[++i]);
        } else if (arg == "--learning-rate" && i + 1 < argc) {
            params.learningRate = std::stod(argv[++i]);
        } else if (arg == "--save-transform" && i + 1 < argc) {
            transformPath = argv[++i];
        } else if (arg == "--fixed-landmarks" && i + 1 < argc) {
            fixedLandmarksPath = argv[++i];
        } else if (arg == "--moving-landmarks" && i + 1 < argc) {
            movingLandmarksPath = argv[++i];
        } else if (arg == "--eval-output" && i + 1 < argc) {
            evalOutputPath = argv[++i];
        } else if (arg == "--verbose") {
            params.verbose = true;
        }
    }

    // Print configuration
    std::cout << "\n========================================" << std::endl;
    std::cout << "ITK Multi-Modal Registration" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Fixed:      " << fixedPath << std::endl;
    std::cout << "  Moving:     " << movingPath << std::endl;
    std::cout << "  Output:     " << outputPath << std::endl;
    std::cout << "  Mode:       "
              << (mode == Registration::RegistrationMode::MONO_MODAL ? "Mono-modal" : "Multi-modal")
              << std::endl;
    std::cout << "  Iterations: " << params.maxIterations << std::endl;
    std::cout << "  Pyramid:    " << params.pyramidLevels << std::endl;

    // Create registration
    Registration::MultiModalRegistration registration;
    registration.SetMode(mode);
    registration.SetParameters(params);

    // Load images
    if (!registration.LoadImages(fixedPath, movingPath)) {
        return EXIT_FAILURE;
    }

    // Load landmarks if provided
    Registration::LandmarkListType fixedLandmarks, movingLandmarks;
    bool                           hasLandmarks = false;

    if (!fixedLandmarksPath.empty() && !movingLandmarksPath.empty()) {
        fixedLandmarks  = Registration::LandmarkIO::ReadLandmarks(fixedLandmarksPath);
        movingLandmarks = Registration::LandmarkIO::ReadLandmarks(movingLandmarksPath);

        if (fixedLandmarks.size() == movingLandmarks.size() && !fixedLandmarks.empty()) {
            hasLandmarks       = true;
            auto initialResult = Registration::LandmarkEvaluation::ComputeInitialError(
                fixedLandmarks, movingLandmarks);
            Registration::LandmarkEvaluation::PrintResults(initialResult, "Before Registration");
        }
    }

    // Register
    auto result = registration.Register();

    if (!result.success) {
        std::cerr << "Registration failed: " << result.message << std::endl;
        return EXIT_FAILURE;
    }

    // Evaluate with landmarks
    if (hasLandmarks) {
        auto finalResult = Registration::LandmarkEvaluation::EvaluateRegistration(
            fixedLandmarks, movingLandmarks, result.transform);
        Registration::LandmarkEvaluation::PrintResults(finalResult, "After Registration");

        if (!evalOutputPath.empty()) {
            auto initialResult = Registration::LandmarkEvaluation::ComputeInitialError(
                fixedLandmarks, movingLandmarks);
            Registration::LandmarkEvaluation::SaveResultsToCSV(evalOutputPath, initialResult,
                                                               finalResult);
        }
    }

    // Save results
    if (!registration.SaveRegisteredImage(outputPath, result.transform)) {
        return EXIT_FAILURE;
    }

    if (!transformPath.empty()) {
        registration.SaveTransform(transformPath, result.transform);
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "Registration completed successfully!" << std::endl;
    std::cout << "========================================\n" << std::endl;

    return EXIT_SUCCESS;
}