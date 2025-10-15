#pragma once
#include "itkImage.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include <memory>

namespace itkexp {

template <typename TImage>
typename TImage::Pointer applyGaussian(const typename TImage::Pointer& input, double sigma)
{
    using FilterType = itk::SmoothingRecursiveGaussianImageFilter<TImage, TImage>;
    auto filter = FilterType::New();
    filter->SetInput(input);
    filter->SetSigma(sigma);
    filter->Update();
    return filter->GetOutput();
}

template <typename TImage>
typename TImage::Pointer computeGradient(const typename TImage::Pointer& input)
{
    using FilterType = itk::GradientMagnitudeImageFilter<TImage, TImage>;
    auto filter = FilterType::New();
    filter->SetInput(input);
    filter->Update();
    return filter->GetOutput();
}

} // namespace itkexp
