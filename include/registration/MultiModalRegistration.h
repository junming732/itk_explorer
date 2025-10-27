#ifndef MULTIMODAL_REGISTRATION_H
#define MULTIMODAL_REGISTRATION_H

#include <chrono>
#include <string>

#include "itkCenteredTransformInitializer.h"
#include "itkCommand.h"
#include "itkEuler3DTransform.h"
#include "itkImage.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkOnePlusOneEvolutionaryOptimizerv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkResampleImageFilter.h"

namespace Registration {

    constexpr unsigned int Dimension = 3;
    using PixelType                  = float;
    using ImageType                  = itk::Image<PixelType, Dimension>;
    using TransformType              = itk::Euler3DTransform<double>;

    /**
     * @brief Registration mode: mono-modal vs multi-modal
     */
    enum class RegistrationMode
    {
        MONO_MODAL,  // Same modality (T1-T1, T2-T2) - uses Mean Squares
        MULTI_MODAL  // Different modalities (T1-T2) - uses Mutual Information
    };

    /**
     * @brief Parameters for registration
     */
    struct RegistrationParameters
    {
        unsigned int maxIterations    = 1000;
        unsigned int pyramidLevels    = 3;
        double       learningRate     = 0.001;
        double       relaxationFactor = 0.95;
        double       minStepLength    = 0.0001;
        double       initialRadius    = 7e-05;  // For OnePlusOne optimizer
        bool         verbose          = false;
    };

    /**
     * @brief Results from registration
     */
    struct RegistrationResult
    {
        TransformType::Pointer transform;
        double                 finalMetricValue;
        unsigned int           iterations;
        double                 elapsedSeconds;
        bool                   success;
        std::string            message;
    };

    /**
     * @brief Observer to monitor registration progress
     */
    class RegistrationObserver : public itk::Command
    {
      public:
        using Self       = RegistrationObserver;
        using Superclass = itk::Command;
        using Pointer    = itk::SmartPointer<Self>;

        itkNewMacro(Self);

        void SetVerbose(bool v) { verbose = v; }

      protected:
        RegistrationObserver() = default;

        void Execute(itk::Object* caller, const itk::EventObject& event) override
        {
            Execute((const itk::Object*)caller, event);
        }

        void Execute(const itk::Object* object, const itk::EventObject& event) override;

      private:
        bool         verbose        = false;
        unsigned int iterationCount = 0;
    };

    /**
     * @brief Main class for multi-modal rigid registration
     *
     * Supports both mono-modal (Mean Squares) and multi-modal (Mutual Information)
     * registration with rigid transform (rotation + translation only).
     */
    class MultiModalRegistration
    {
      public:
        MultiModalRegistration()  = default;
        ~MultiModalRegistration() = default;

        /**
         * @brief Set registration mode
         */
        void SetMode(RegistrationMode mode) { mode_ = mode; }

        /**
         * @brief Set registration parameters
         */
        void SetParameters(const RegistrationParameters& params) { params_ = params; }

        /**
         * @brief Load fixed and moving images
         */
        bool LoadImages(const std::string& fixedPath, const std::string& movingPath);

        /**
         * @brief Perform registration
         * @return RegistrationResult containing transform and metrics
         */
        RegistrationResult Register();

        /**
         * @brief Apply transform to moving image
         * @param transform The transformation to apply
         * @return Registered image
         */
        ImageType::Pointer ApplyTransform(const TransformType::Pointer& transform);

        /**
         * @brief Save registered image
         */
        bool SaveRegisteredImage(const std::string&            outputPath,
                                 const TransformType::Pointer& transform);

        /**
         * @brief Save transform to file
         */
        bool SaveTransform(const std::string& outputPath, const TransformType::Pointer& transform);

        /**
         * @brief Get fixed image
         */
        ImageType::Pointer GetFixedImage() const { return fixedImage_; }

        /**
         * @brief Get moving image
         */
        ImageType::Pointer GetMovingImage() const { return movingImage_; }

      private:
        /**
         * @brief Setup mono-modal registration (Mean Squares + Gradient Descent)
         */
        RegistrationResult RegisterMonoModal();

        /**
         * @brief Setup multi-modal registration (Mutual Information + Evolutionary)
         */
        RegistrationResult RegisterMultiModal();

        /**
         * @brief Initialize transform using image centers
         */
        TransformType::Pointer InitializeTransform();

        ImageType::Pointer     fixedImage_;
        ImageType::Pointer     movingImage_;
        RegistrationMode       mode_ = RegistrationMode::MULTI_MODAL;
        RegistrationParameters params_;
    };

}  // namespace Registration

#endif  // MULTIMODAL_REGISTRATION_H