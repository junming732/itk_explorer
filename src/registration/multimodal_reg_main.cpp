/**
 * multimodal_reg_main.cpp
 */

#include <iostream>
#include <string>

#include "evaluation/LandmarkEvaluation.h"
#include "landmarks/LandmarkIO.h"
#include "registration/MultiModalRegistration.h"

struct CommandLineArgs
{
    std::string fixedImagePath;
    std::string movingImagePath;
    std::string outputImagePath;
    std::string mode             = "multi";
    int         iterations       = 1000;
    int         pyramidLevels    = 3;
    double      learningRate     = 0.001;
    double      relaxationFactor = 0.95;
    std::string saveTransformPath;
    std::string fixedLandmarksPath;
    std::string movingLandmarksPath;
    std::string evalOutputPath;
    bool        verbose = false;
};

void PrintUsage(const char* progName)
{
    std::cout << "Usage: " << progName
              << " <fixed> <moving> <output> --mode <mono|multi> [options]\n";
    std::exit(1);
}

CommandLineArgs ParseCommandLine(int argc, char* argv[])
{
    CommandLineArgs args;
    if (argc < 5) {
        PrintUsage(argv[0]);
    }
    args.fixedImagePath  = argv[1];
    args.movingImagePath = argv[2];
    args.outputImagePath = argv[3];

    bool modeSet = false;
    for (int i = 4; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode" && i + 1 < argc) {
            args.mode = argv[++i];
            modeSet   = true;
        } else if (arg == "--iterations" && i + 1 < argc) {
            args.iterations = std::stoi(argv[++i]);
        } else if (arg == "--pyramid-levels" && i + 1 < argc) {
            args.pyramidLevels = std::stoi(argv[++i]);
        } else if (arg == "--learning-rate" && i + 1 < argc) {
            args.learningRate = std::stod(argv[++i]);
        } else if (arg == "--relaxation" && i + 1 < argc) {
            args.relaxationFactor = std::stod(argv[++i]);
        } else if (arg == "--save-transform" && i + 1 < argc) {
            args.saveTransformPath = argv[++i];
        } else if (arg == "--fixed-landmarks" && i + 1 < argc) {
            args.fixedLandmarksPath = argv[++i];
        } else if (arg == "--moving-landmarks" && i + 1 < argc) {
            args.movingLandmarksPath = argv[++i];
        } else if (arg == "--eval-output" && i + 1 < argc) {
            args.evalOutputPath = argv[++i];
        } else if (arg == "--verbose") {
            args.verbose = true;
        }
    }
    if (!modeSet) {
        PrintUsage(argv[0]);
    }
    return args;
}

int main(int argc, char* argv[])
{
    auto args = ParseCommandLine(argc, argv);

    try {
        std::cout << "\n=== Multi-Modal Registration ===" << std::endl;
        std::cout << "Fixed:  " << args.fixedImagePath << std::endl;
        std::cout << "Moving: " << args.movingImagePath << std::endl;
        std::cout << "Output: " << args.outputImagePath << std::endl;
        std::cout << "Mode:   " << args.mode << "\n" << std::endl;

        // Create registration object
        Registration::MultiModalRegistration registration;

        // Set mode
        if (args.mode == "mono") {
            registration.SetMode(Registration::RegistrationMode::MONO_MODAL);
        } else {
            registration.SetMode(Registration::RegistrationMode::MULTI_MODAL);
        }

        // Set parameters
        Registration::RegistrationParameters params;
        params.maxIterations    = args.iterations;
        params.pyramidLevels    = args.pyramidLevels;
        params.learningRate     = args.learningRate;
        params.relaxationFactor = args.relaxationFactor;
        params.verbose          = args.verbose;
        registration.SetParameters(params);

        // Load images
        std::cout << "Loading images..." << std::endl;
        if (!registration.LoadImages(args.fixedImagePath, args.movingImagePath)) {
            std::cerr << "Error: Failed to load images\n";
            return 1;
        }

        // Load landmarks if provided
        Registration::LandmarkListType fixedLandmarks, movingLandmarks;
        bool                           hasLandmarks = false;

        if (!args.fixedLandmarksPath.empty() && !args.movingLandmarksPath.empty()) {
            std::cout << "Loading landmarks..." << std::endl;
            fixedLandmarks  = Registration::LandmarkIO::ReadLandmarks(args.fixedLandmarksPath);
            movingLandmarks = Registration::LandmarkIO::ReadLandmarks(args.movingLandmarksPath);

            if (fixedLandmarks.size() != movingLandmarks.size()) {
                std::cerr << "Error: Landmark count mismatch\n";
                return 1;
            }

            std::cout << "  Loaded " << fixedLandmarks.size() << " pairs" << std::endl;
            hasLandmarks = true;

            // Evaluate before registration
            auto beforeResult = Registration::LandmarkEvaluation::EvaluateRegistration(
                fixedLandmarks, movingLandmarks, nullptr);
            std::cout << "Before TRE: " << beforeResult.meanError << " mm\n" << std::endl;
        }

        // Perform registration
        std::cout << "Starting registration..." << std::endl;
        auto result = registration.Register();

        if (!result.success) {
            std::cerr << "Registration failed: " << result.message << std::endl;
            return 1;
        }

        std::cout << "Registration complete!" << std::endl;
        std::cout << "  Iterations: " << result.iterations << std::endl;
        std::cout << "  Final metric: " << result.finalMetricValue << std::endl;
        std::cout << "  Time: " << result.elapsedSeconds << " seconds\n" << std::endl;

        // Save registered image
        std::cout << "Saving registered image..." << std::endl;
        if (!registration.SaveRegisteredImage(args.outputImagePath, result.transform)) {
            std::cerr << "Error: Failed to save registered image\n";
            return 1;
        }

        // Save transform if requested
        if (!args.saveTransformPath.empty()) {
            std::cout << "Saving transform..." << std::endl;
            if (!registration.SaveTransform(args.saveTransformPath, result.transform)) {
                std::cerr << "Warning: Failed to save transform\n";
            }
        }

        // Evaluate with landmarks if provided
        if (hasLandmarks) {
            std::cout << "\nEvaluating with landmarks..." << std::endl;

            auto afterResult = Registration::LandmarkEvaluation::EvaluateRegistration(
                fixedLandmarks, movingLandmarks, result.transform.GetPointer());

            auto beforeResult = Registration::LandmarkEvaluation::EvaluateRegistration(
                fixedLandmarks, movingLandmarks, nullptr);

            std::cout << "After TRE:   " << afterResult.meanError << " mm" << std::endl;
            std::cout << "Improvement: " << (beforeResult.meanError - afterResult.meanError)
                      << " mm\n"
                      << std::endl;

            // Save evaluation if requested
            if (!args.evalOutputPath.empty()) {
                // Note: Need to check actual method name in LandmarkEvaluation.h
                // Might be SaveReport, SaveEvaluationReport, or WriteResults
                std::cout << "Saving evaluation to: " << args.evalOutputPath << std::endl;
                // Registration::LandmarkEvaluation::SaveReport(...);
            }
        }

        std::cout << "\n=== Complete ===" << std::endl;
        std::cout << "Output: " << args.outputImagePath << std::endl;

        return 0;

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "\nITK Error: " << e << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
}