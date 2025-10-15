#pragma once
#include "itkImage.h"
#include "itkExtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkStatisticsImageFilter.h"
#include <iostream>
#include <string>

namespace itkexp
{

// Write full 3D image (.nrrd or .nii)
template <typename TImage>
void exportVolume(const typename TImage::Pointer& image, const std::string& filename)
{
    using WriterType = itk::ImageFileWriter<TImage>;
    auto writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(image);
    writer->Update();

    std::cout << "✅ Wrote 3D image: " << filename << std::endl;
}

// Extract middle slice and save as .png (convert float → uchar)
template <typename TImage3D>
void exportOrthogonalSlicesToPNG(const typename TImage3D::Pointer& image,
                                 const std::string& outputPrefix)
{
    constexpr unsigned int Dimension = TImage3D::ImageDimension;
    static_assert(Dimension == 3, "exportOrthogonalSlicesToPNG expects a 3D image.");

    using InputPixelType = typename TImage3D::PixelType;
    using SliceType = itk::Image<InputPixelType, 2>;
    using ExtractType = itk::ExtractImageFilter<TImage3D, SliceType>;
    using RescaleType = itk::RescaleIntensityImageFilter<SliceType, SliceType>;
    using OutputSliceType = itk::Image<unsigned char, 2>;
    using CastType = itk::CastImageFilter<SliceType, OutputSliceType>;
    using WriterType = itk::ImageFileWriter<OutputSliceType>;

    const auto region = image->GetLargestPossibleRegion();
    const auto size = region.GetSize();

    // A helper lambda to extract and write one slice
    auto writeSlice = [&](unsigned int axis, size_t index, const std::string& name)
    {
        itk::Index<3> start = {0, 0, 0};
        itk::Size<3> sliceSize = size;

        start[axis] = static_cast<long>(index);
        sliceSize[axis] = 0;
        itk::ImageRegion<3> sliceRegion(start, sliceSize);

        auto extractor = ExtractType::New();
        extractor->SetInput(image);
        extractor->SetExtractionRegion(sliceRegion);
        extractor->SetDirectionCollapseToSubmatrix();
        extractor->Update();

        auto rescaler = RescaleType::New();
        rescaler->SetInput(extractor->GetOutput());
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(255);
        rescaler->Update();

        auto caster = CastType::New();
        caster->SetInput(rescaler->GetOutput());
        caster->Update();

        auto writer = WriterType::New();
        writer->SetFileName(name);
        writer->SetInput(caster->GetOutput());
        writer->Update();

        std::cout << "✅ Wrote " << name << " (axis " << axis
                  << ", index " << index << ")\n";
    };

    // Pick roughly the middle of each dimension
    const size_t xMid = size[0] / 2;
    const size_t yMid = size[1] / 2;
    const size_t zMid = size[2] / 2;

    writeSlice(2, zMid, outputPrefix + "_axial.png");      // Z-slice
    writeSlice(1, yMid, outputPrefix + "_coronal.png");    // Y-slice
    writeSlice(0, xMid, outputPrefix + "_sagittal.png");   // X-slice
}




} // namespace itkexp
