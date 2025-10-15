#pragma once
#include "itkImage.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"

namespace itkexp {

template <typename TImage>
typename TImage::Pointer otsuThreshold(const typename TImage::Pointer& input)
{
    using FilterType = itk::OtsuThresholdImageFilter<TImage, TImage>;
    auto filter = FilterType::New();
    filter->SetInput(input);
    filter->SetInsideValue(0);
    filter->SetOutsideValue(1);
    filter->Update();
    return filter->GetOutput();
}

template <typename TImage>
typename itk::Image<unsigned short, TImage::ImageDimension>::Pointer
labelComponents(const typename TImage::Pointer& binaryMask)
{
    using LabelImageType = itk::Image<unsigned short, TImage::ImageDimension>;
    using ConnectedComponentType = itk::ConnectedComponentImageFilter<TImage, LabelImageType>;
    using RelabelType = itk::RelabelComponentImageFilter<LabelImageType, LabelImageType>;

    auto cc = ConnectedComponentType::New();
    cc->SetInput(binaryMask);
    cc->Update();

    auto relabel = RelabelType::New();
    relabel->SetInput(cc->GetOutput());
    relabel->Update();

    return relabel->GetOutput();
}

} // namespace itkexp
