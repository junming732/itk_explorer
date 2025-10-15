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
void exportSliceToPNG(const typename TImage3D::Pointer& image, const std::string& filename)
{
    constexpr unsigned int Dimension = TImage3D::ImageDimension;
    static_assert(Dimension == 3, "exportSliceToPNG expects a 3D image.");

    using InputPixelType = typename TImage3D::PixelType;
    using SliceType = itk::Image<InputPixelType, 2>;
    using ExtractType = itk::ExtractImageFilter<TImage3D, SliceType>;
    using RescaleType = itk::RescaleIntensityImageFilter<SliceType, SliceType>;
    using OutputSliceType = itk::Image<unsigned char, 2>;
    using CastType = itk::CastImageFilter<SliceType, OutputSliceType>;
    using WriterType = itk::ImageFileWriter<OutputSliceType>;
    using StatsType = itk::StatisticsImageFilter<SliceType>;

    const auto region = image->GetLargestPossibleRegion();
    const auto size = region.GetSize();

    // --- Step 1: Pick slice with max mean intensity ---
    double maxMean = 0.0;
    size_t bestZ = size[2] / 2;

    for (size_t z = size[2] / 4; z < 3 * size[2] / 4; ++z)
    {
        itk::Index<3> start = {0, 0, static_cast<long>(z)};
        itk::Size<3> sliceSize = {size[0], size[1], 0};
        itk::ImageRegion<3> sliceRegion(start, sliceSize);

        auto extractor = ExtractType::New();
        extractor->SetInput(image);
        extractor->SetExtractionRegion(sliceRegion);
        extractor->SetDirectionCollapseToSubmatrix();
        extractor->Update();

        auto stats = StatsType::New();
        stats->SetInput(extractor->GetOutput());
        stats->Update();

        double mean = stats->GetMean();
        if (mean > maxMean)
        {
            maxMean = mean;
            bestZ = z;
        }
    }

    // --- Step 2: Extract best slice ---
    itk::Index<3> start = {0, 0, static_cast<long>(bestZ)};
    itk::Size<3> sliceSize = {size[0], size[1], 0};
    itk::ImageRegion<3> sliceRegion(start, sliceSize);

    auto extractor = ExtractType::New();
    extractor->SetInput(image);
    extractor->SetExtractionRegion(sliceRegion);
    extractor->SetDirectionCollapseToSubmatrix();
    extractor->Update();

    // --- Step 3: Rescale and cast ---
    auto rescaler = RescaleType::New();
    rescaler->SetInput(extractor->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);
    rescaler->Update();

    auto caster = CastType::New();
    caster->SetInput(rescaler->GetOutput());
    caster->Update();

    auto writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(caster->GetOutput());
    writer->Update();

    std::cout << "✅ Wrote PNG slice (Z=" << bestZ
              << ", mean=" << maxMean << "): " << filename << std::endl;
}




} // namespace itkexp
