#pragma once
#include "itkImage.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkBSplineTransform.h"
#include "itkBSplineTransformInitializer.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileWriter.h"
#include <array>
#include <iostream>

namespace itkexp
{

template <typename TImage, unsigned int SplineOrder = 3>
typename TImage::Pointer
bsplineRegister(const typename TImage::Pointer& fixed,
                const typename TImage::Pointer& moving,
                const std::array<unsigned int, TImage::ImageDimension>& meshSize, // control grid cells per dim
                const std::string& outputPath)
{
    constexpr unsigned int Dim = TImage::ImageDimension;
    using TransformType = itk::BSplineTransform<double, Dim, SplineOrder>;
    using InitializerType = itk::BSplineTransformInitializer<TransformType, TImage>;
    using MetricType = itk::MattesMutualInformationImageToImageMetricv4<TImage, TImage>;
    using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
    using RegistrationType = itk::ImageRegistrationMethodv4<TImage, TImage, TransformType>;

    // --- Build transform and initialize it over fixed image domain
    auto transform = TransformType::New();
    auto initializer = InitializerType::New();
    initializer->SetTransform(transform);
    initializer->SetImage(fixed);
    typename InitializerType::MeshSizeType meshSizeITK;
    for (unsigned int i = 0; i < Dim; ++i)
        meshSizeITK[i] = meshSize[i];
    initializer->SetTransformDomainMeshSize(meshSizeITK);

    initializer->InitializeTransform();

    // Start from zero displacement
    transform->SetIdentity();

    // --- Metric (robust for inter/intra subject)
    auto metric = MetricType::New();
    metric->SetNumberOfHistogramBins(50);
    metric->SetUseMovingImageGradientFilter(false);
    metric->SetUseFixedImageGradientFilter(false);

    // --- Optimizer
    auto optimizer = OptimizerType::New();
    optimizer->SetLearningRate(1.0);
    optimizer->SetMinimumStepLength(0.0005);
    optimizer->SetRelaxationFactor(0.7);
    optimizer->SetNumberOfIterations(200);

    // --- Registration
    auto registration = RegistrationType::New();
    registration->SetFixedImage(fixed);
    registration->SetMovingImage(moving);
    registration->SetMetric(metric);
    registration->SetOptimizer(optimizer);
    registration->SetInitialTransform(transform);
    registration->InPlaceOn();

    std::cout << "🚀 B-spline registration: mesh = {";
    for (unsigned i = 0; i < Dim; ++i)
        std::cout << meshSize[i] << (i + 1 < Dim ? "," : "");
    std::cout << "}, order = " << SplineOrder << "\n";

    try {
        registration->Update();
        std::cout << "✅ B-spline registration finished.\n";
    } catch (const itk::ExceptionObject& e) {
        std::cerr << "B-spline registration failed: " << e << std::endl;
        return nullptr;
    }

    // --- Resample moving into fixed space
    using ResampleType = itk::ResampleImageFilter<TImage, TImage>;
    auto resampler = ResampleType::New();
    resampler->SetInput(moving);
    resampler->SetTransform(registration->GetTransform());
    resampler->SetReferenceImage(fixed);
    resampler->UseReferenceImageOn();
    resampler->Update();

    using WriterType = itk::ImageFileWriter<TImage>;
    auto writer = WriterType::New();
    writer->SetFileName(outputPath);
    writer->SetInput(resampler->GetOutput());
    writer->Update();

    std::cout << "💾 B-spline result written: " << outputPath << std::endl;
    return resampler->GetOutput();
}
} // namespace itkexp
