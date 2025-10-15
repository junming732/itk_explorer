#pragma once
#include "itkImage.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include <iostream>

namespace itkexp
{

template <typename TImage>
typename TImage::Pointer registerImages(
    const typename TImage::Pointer& fixedImage,
    const typename TImage::Pointer& movingImage,
    const std::string& outputPath)
{
    using TransformType = itk::AffineTransform<double, TImage::ImageDimension>;
    using MetricType = itk::MeanSquaresImageToImageMetricv4<TImage, TImage>;
    using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
    using RegistrationType = itk::ImageRegistrationMethodv4<TImage, TImage, TransformType>;

    auto transform = TransformType::New();
    transform->SetIdentity();

    auto metric = MetricType::New();
    auto optimizer = OptimizerType::New();
    auto registration = RegistrationType::New();

    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetFixedImage(fixedImage);
    registration->SetMovingImage(movingImage);
    registration->SetInitialTransform(transform);
    registration->InPlaceOn();

    optimizer->SetLearningRate(1.0);
    optimizer->SetMinimumStepLength(0.001);
    optimizer->SetNumberOfIterations(200);
    optimizer->SetRelaxationFactor(0.7);

    try
    {
        std::cout << "🚀 Starting registration..." << std::endl;
        registration->Update();
        std::cout << "✅ Registration finished." << std::endl;
    }
    catch (itk::ExceptionObject& e)
    {
        std::cerr << "Registration failed: " << e << std::endl;
        return nullptr;
    }

    // Resample moving image
    using ResampleType = itk::ResampleImageFilter<TImage, TImage>;
    auto resampler = ResampleType::New();
    resampler->SetInput(movingImage);
    resampler->SetTransform(registration->GetTransform());
    resampler->SetReferenceImage(fixedImage);
    resampler->UseReferenceImageOn();
    resampler->Update();

    auto registeredImage = resampler->GetOutput();

    using WriterType = itk::ImageFileWriter<TImage>;
    auto writer = WriterType::New();
    writer->SetFileName(outputPath);
    writer->SetInput(registeredImage);
    writer->Update();

    std::cout << "💾 Registered image written to: " << outputPath << std::endl;
    return registeredImage;
}

} // namespace itkexp
