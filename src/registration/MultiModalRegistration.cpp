#include <iomanip>
#include <iostream>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkNormalVariateGenerator.h"
#include "itkTransformFileWriter.h"
#include "registration/MultiModalRegistration.h"

namespace Registration {

    // Observer implementation
    void RegistrationObserver::Execute(const itk::Object* object, const itk::EventObject& event)
    {
        if (!verbose)
            return;

        if (typeid(event) == typeid(itk::IterationEvent)) {
            iterationCount++;

            // Try to cast to different optimizer types
            const auto* gradientOptimizer =
                dynamic_cast<const itk::RegularStepGradientDescentOptimizerv4<double>*>(object);

            if (gradientOptimizer) {
                std::cout << "Iteration " << std::setw(4) << iterationCount
                          << " Metric: " << std::fixed << std::setprecision(6)
                          << gradientOptimizer->GetValue()
                          << " StepLength: " << gradientOptimizer->GetLearningRate() << std::endl;
            }
        } else if (typeid(event) == typeid(itk::StartEvent)) {
            iterationCount = 0;
            if (verbose) {
                std::cout << "\n=== Registration Started ===" << std::endl;
            }
        } else if (typeid(event) == typeid(itk::EndEvent)) {
            if (verbose) {
                std::cout << "=== Registration Completed ===" << std::endl;
            }
        }
    }

    // Load images
    bool MultiModalRegistration::LoadImages(const std::string& fixedPath,
                                            const std::string& movingPath)
    {
        using ReaderType = itk::ImageFileReader<ImageType>;

        try {
            // Load fixed image
            auto fixedReader = ReaderType::New();
            fixedReader->SetFileName(fixedPath);
            fixedReader->Update();
            fixedImage_ = fixedReader->GetOutput();

            std::cout << "Loaded fixed image: " << fixedPath << std::endl;
            std::cout << "  Size: " << fixedImage_->GetLargestPossibleRegion().GetSize()
                      << std::endl;

            // Load moving image
            auto movingReader = ReaderType::New();
            movingReader->SetFileName(movingPath);
            movingReader->Update();
            movingImage_ = movingReader->GetOutput();

            std::cout << "Loaded moving image: " << movingPath << std::endl;
            std::cout << "  Size: " << movingImage_->GetLargestPossibleRegion().GetSize()
                      << std::endl;

            return true;
        } catch (const itk::ExceptionObject& e) {
            std::cerr << "Error loading images: " << e << std::endl;
            return false;
        }
    }

    // Initialize transform
    TransformType::Pointer MultiModalRegistration::InitializeTransform()
    {
        auto transform = TransformType::New();

        // Use CenteredTransformInitializer to align centers
        using InitializerType =
            itk::CenteredTransformInitializer<TransformType, ImageType, ImageType>;

        auto initializer = InitializerType::New();
        initializer->SetTransform(transform);
        initializer->SetFixedImage(fixedImage_);
        initializer->SetMovingImage(movingImage_);
        initializer->GeometryOn();  // Use geometric centers
        initializer->InitializeTransform();

        if (params_.verbose) {
            std::cout << "\nInitial transform parameters:" << std::endl;
            std::cout << "  Center: " << transform->GetCenter() << std::endl;
            std::cout << "  Translation: " << transform->GetTranslation() << std::endl;
        }

        return transform;
    }

    // Main registration dispatcher
    RegistrationResult MultiModalRegistration::Register()
    {
        if (!fixedImage_ || !movingImage_) {
            RegistrationResult result;
            result.success = false;
            result.message = "Images not loaded";
            return result;
        }

        if (mode_ == RegistrationMode::MONO_MODAL) {
            std::cout << "\n=== Starting Mono-Modal Registration ===" << std::endl;
            std::cout << "Metric: Mean Squares" << std::endl;
            std::cout << "Optimizer: Regular Step Gradient Descent" << std::endl;
            return RegisterMonoModal();
        } else {
            std::cout << "\n=== Starting Multi-Modal Registration ===" << std::endl;
            std::cout << "Metric: Mattes Mutual Information" << std::endl;
            std::cout << "Optimizer: One Plus One Evolutionary" << std::endl;
            return RegisterMultiModal();
        }
    }

    // Mono-modal registration (Mean Squares + Gradient Descent)
    RegistrationResult MultiModalRegistration::RegisterMonoModal()
    {
        RegistrationResult result;
        auto               startTime = std::chrono::high_resolution_clock::now();

        try {
            // Setup metric
            using MetricType = itk::MeanSquaresImageToImageMetricv4<ImageType, ImageType>;
            auto metric      = MetricType::New();

            // Setup optimizer
            using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
            auto optimizer      = OptimizerType::New();

            optimizer->SetLearningRate(params_.learningRate);
            optimizer->SetMinimumStepLength(params_.minStepLength);
            optimizer->SetRelaxationFactor(params_.relaxationFactor);
            optimizer->SetNumberOfIterations(params_.maxIterations);
            optimizer->SetReturnBestParametersAndValue(true);

            // Add observer
            if (params_.verbose) {
                auto observer = RegistrationObserver::New();
                observer->SetVerbose(true);
                optimizer->AddObserver(itk::IterationEvent(), observer);
                optimizer->AddObserver(itk::StartEvent(), observer);
                optimizer->AddObserver(itk::EndEvent(), observer);
            }

            // Setup registration
            using RegistrationType =
                itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType>;
            auto registration = RegistrationType::New();

            registration->SetFixedImage(fixedImage_);
            registration->SetMovingImage(movingImage_);
            registration->SetMetric(metric);
            registration->SetOptimizer(optimizer);

            // Initialize transform
            auto initialTransform = InitializeTransform();
            registration->SetInitialTransform(initialTransform);

            // Multi-resolution pyramid
            typename RegistrationType::ShrinkFactorsArrayType shrinkFactors;
            shrinkFactors.SetSize(params_.pyramidLevels);

            typename RegistrationType::SmoothingSigmasArrayType smoothingSigmas;
            smoothingSigmas.SetSize(params_.pyramidLevels);

            // Setup pyramid schedule
            for (unsigned int level = 0; level < params_.pyramidLevels; ++level) {
                shrinkFactors[level] =
                    1 << (params_.pyramidLevels - 1 - level);  // 8, 4, 2, 1 for 4 levels
                smoothingSigmas[level] = params_.pyramidLevels - 1 - level;  // 3, 2, 1, 0
            }

            registration->SetNumberOfLevels(params_.pyramidLevels);
            registration->SetShrinkFactorsPerLevel(shrinkFactors);
            registration->SetSmoothingSigmasPerLevel(smoothingSigmas);
            registration->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(true);

            std::cout << "Pyramid levels: " << params_.pyramidLevels << std::endl;
            std::cout << "Max iterations: " << params_.maxIterations << std::endl;
            std::cout << "Learning rate: " << params_.learningRate << std::endl;
            std::cout << "Relaxation factor: " << params_.relaxationFactor << std::endl;

            // Perform registration
            registration->Update();

            // Get results
            result.transform = dynamic_cast<TransformType*>(registration->GetModifiableTransform());
            result.finalMetricValue = optimizer->GetValue();
            result.iterations       = optimizer->GetCurrentIteration();
            result.success          = true;
            result.message          = "Registration completed successfully";

            auto endTime          = std::chrono::high_resolution_clock::now();
            result.elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();

            std::cout << "\n=== Registration Results ===" << std::endl;
            std::cout << "Final metric value: " << result.finalMetricValue << std::endl;
            std::cout << "Iterations: " << result.iterations << std::endl;
            std::cout << "Elapsed time: " << result.elapsedSeconds << " seconds" << std::endl;
            std::cout << "Stop condition: " << optimizer->GetStopConditionDescription()
                      << std::endl;

            // Print transform parameters
            const auto* finalTransform = result.transform.GetPointer();
            std::cout << "\nFinal Transform Parameters:" << std::endl;
            std::cout << "  Rotation angles (radians):" << std::endl;
            std::cout << "    X: " << finalTransform->GetAngleX() << std::endl;
            std::cout << "    Y: " << finalTransform->GetAngleY() << std::endl;
            std::cout << "    Z: " << finalTransform->GetAngleZ() << std::endl;
            std::cout << "  Translation: " << finalTransform->GetTranslation() << std::endl;

        } catch (const itk::ExceptionObject& e) {
            result.success = false;
            result.message = std::string("Registration failed: ") + e.GetDescription();
            std::cerr << result.message << std::endl;
        }

        return result;
    }

    // Multi-modal registration (Mutual Information + Evolutionary)
    RegistrationResult MultiModalRegistration::RegisterMultiModal()
    {
        RegistrationResult result;
        auto               startTime = std::chrono::high_resolution_clock::now();

        try {
            // Setup metric
            using MetricType =
                itk::MattesMutualInformationImageToImageMetricv4<ImageType, ImageType>;
            auto metric = MetricType::New();
            metric->SetNumberOfHistogramBins(50);

            // Setup optimizer
            using OptimizerType = itk::OnePlusOneEvolutionaryOptimizerv4<double>;
            auto optimizer      = OptimizerType::New();

            // Setup random number generator
            using GeneratorType = itk::Statistics::NormalVariateGenerator;
            auto generator      = GeneratorType::New();
            generator->Initialize(12345);  // Seed for reproducibility

            optimizer->SetNormalVariateGenerator(generator);
            optimizer->SetMaximumIteration(params_.maxIterations);
            optimizer->Initialize(params_.initialRadius);
            optimizer->SetEpsilon(1e-6);

            // Setup registration
            using RegistrationType =
                itk::ImageRegistrationMethodv4<ImageType, ImageType, TransformType>;
            auto registration = RegistrationType::New();

            registration->SetFixedImage(fixedImage_);
            registration->SetMovingImage(movingImage_);
            registration->SetMetric(metric);
            registration->SetOptimizer(optimizer);

            // Initialize transform
            auto initialTransform = InitializeTransform();
            registration->SetInitialTransform(initialTransform);

            // Multi-resolution pyramid
            typename RegistrationType::ShrinkFactorsArrayType shrinkFactors;
            shrinkFactors.SetSize(params_.pyramidLevels);

            typename RegistrationType::SmoothingSigmasArrayType smoothingSigmas;
            smoothingSigmas.SetSize(params_.pyramidLevels);

            for (unsigned int level = 0; level < params_.pyramidLevels; ++level) {
                shrinkFactors[level]   = 1 << (params_.pyramidLevels - 1 - level);
                smoothingSigmas[level] = params_.pyramidLevels - 1 - level;
            }

            registration->SetNumberOfLevels(params_.pyramidLevels);
            registration->SetShrinkFactorsPerLevel(shrinkFactors);
            registration->SetSmoothingSigmasPerLevel(smoothingSigmas);
            registration->SetSmoothingSigmasAreSpecifiedInPhysicalUnits(true);

            std::cout << "Pyramid levels: " << params_.pyramidLevels << std::endl;
            std::cout << "Max iterations: " << params_.maxIterations << std::endl;
            std::cout << "Initial radius: " << params_.initialRadius << std::endl;

            // Perform registration
            registration->Update();

            // Get results
            result.transform = dynamic_cast<TransformType*>(registration->GetModifiableTransform());
            result.finalMetricValue = optimizer->GetValue();
            result.iterations =
                optimizer->GetMaximumIteration();  // Evolutionary doesn't track current iteration
            result.success = true;
            result.message = "Registration completed successfully";

            auto endTime          = std::chrono::high_resolution_clock::now();
            result.elapsedSeconds = std::chrono::duration<double>(endTime - startTime).count();

            std::cout << "\n=== Registration Results ===" << std::endl;
            std::cout << "Final metric value (MI): " << result.finalMetricValue << std::endl;
            std::cout << "Elapsed time: " << result.elapsedSeconds << " seconds" << std::endl;

            // Print transform parameters
            const auto* finalTransform = result.transform.GetPointer();
            std::cout << "\nFinal Transform Parameters:" << std::endl;
            std::cout << "  Rotation angles (radians):" << std::endl;
            std::cout << "    X: " << finalTransform->GetAngleX() << std::endl;
            std::cout << "    Y: " << finalTransform->GetAngleY() << std::endl;
            std::cout << "    Z: " << finalTransform->GetAngleZ() << std::endl;
            std::cout << "  Translation: " << finalTransform->GetTranslation() << std::endl;

        } catch (const itk::ExceptionObject& e) {
            result.success = false;
            result.message = std::string("Registration failed: ") + e.GetDescription();
            std::cerr << result.message << std::endl;
        }

        return result;
    }

    // Apply transform
    ImageType::Pointer
    MultiModalRegistration::ApplyTransform(const TransformType::Pointer& transform)
    {
        using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
        auto resampler           = ResampleFilterType::New();

        resampler->SetTransform(transform);
        resampler->SetInput(movingImage_);
        resampler->SetSize(fixedImage_->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputOrigin(fixedImage_->GetOrigin());
        resampler->SetOutputSpacing(fixedImage_->GetSpacing());
        resampler->SetOutputDirection(fixedImage_->GetDirection());
        resampler->SetDefaultPixelValue(0);

        resampler->Update();
        return resampler->GetOutput();
    }

    // Save registered image
    bool MultiModalRegistration::SaveRegisteredImage(const std::string&            outputPath,
                                                     const TransformType::Pointer& transform)
    {
        try {
            auto registeredImage = ApplyTransform(transform);

            using WriterType = itk::ImageFileWriter<ImageType>;
            auto writer      = WriterType::New();
            writer->SetFileName(outputPath);
            writer->SetInput(registeredImage);
            writer->Update();

            std::cout << "Saved registered image to: " << outputPath << std::endl;
            return true;
        } catch (const itk::ExceptionObject& e) {
            std::cerr << "Error saving registered image: " << e << std::endl;
            return false;
        }
    }

    // Save transform
    bool MultiModalRegistration::SaveTransform(const std::string&            outputPath,
                                               const TransformType::Pointer& transform)
    {
        try {
            using TransformWriterType = itk::TransformFileWriterTemplate<double>;
            auto writer               = TransformWriterType::New();
            writer->SetFileName(outputPath);
            writer->SetInput(transform);
            writer->Update();

            std::cout << "Saved transform to: " << outputPath << std::endl;
            return true;
        } catch (const itk::ExceptionObject& e) {
            std::cerr << "Error saving transform: " << e << std::endl;
            return false;
        }
    }

}  // namespace Registration