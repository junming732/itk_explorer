#pragma once
#include "itkImage.h"
#include "itkRescaleIntensityImageFilter.h"

namespace itkexp {

template <typename TImage>
typename TImage::Pointer rescaleIntensity(const typename TImage::Pointer& input,
                                          typename TImage::PixelType minValue,
                                          typename TImage::PixelType maxValue)
{
    using FilterType = itk::RescaleIntensityImageFilter<TImage, TImage>;
    auto filter = FilterType::New();
    filter->SetInput(input);
    filter->SetOutputMinimum(minValue);
    filter->SetOutputMaximum(maxValue);
    filter->Update();
    return filter->GetOutput();
}

} // namespace itkexp
