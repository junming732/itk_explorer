#include <iostream>

#include "itkExtractImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"

int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cout << "Usage: " << argv[0] << " <input.nii.gz> <output.png> <axis> <slice>\n";
        std::cout << "  axis: 0=sagittal, 1=coronal, 2=axial\n";
        return 1;
    }

    std::string inputPath  = argv[1];
    std::string outputPath = argv[2];
    int         axis       = std::stoi(argv[3]);
    int         sliceNum   = std::stoi(argv[4]);

    using InputPixelType  = float;
    using OutputPixelType = unsigned char;
    using InputImageType  = itk::Image<InputPixelType, 3>;
    using OutputImageType = itk::Image<OutputPixelType, 2>;

    // Read 3D image
    auto reader = itk::ImageFileReader<InputImageType>::New();
    reader->SetFileName(inputPath);

    try {
        reader->Update();
    } catch (const itk::ExceptionObject& e) {
        std::cerr << "Error reading: " << e << std::endl;
        return 1;
    }

    InputImageType::Pointer    image  = reader->GetOutput();
    InputImageType::RegionType region = image->GetLargestPossibleRegion();
    InputImageType::SizeType   size   = region.GetSize();

    std::cout << "Image size: " << size[0] << " x " << size[1] << " x " << size[2] << std::endl;
    std::cout << "Extracting slice " << sliceNum << " along axis " << axis << std::endl;

    // Set up extraction region - collapse one dimension
    InputImageType::RegionType extractionRegion = region;
    InputImageType::SizeType   extractSize      = size;
    InputImageType::IndexType  extractIndex     = region.GetIndex();

    // Set the slice dimension to 0 (collapse to 2D)
    extractSize[axis]  = 0;
    extractIndex[axis] = sliceNum;

    extractionRegion.SetSize(extractSize);
    extractionRegion.SetIndex(extractIndex);

    // Extract filter with direction collapse
    using ExtractFilterType = itk::ExtractImageFilter<InputImageType, OutputImageType>;
    auto extractor          = ExtractFilterType::New();
    extractor->SetInput(image);
    extractor->SetExtractionRegion(extractionRegion);
    extractor->SetDirectionCollapseToIdentity();  // Critical for 3D→2D

    // Rescale to 0-255 for PNG
    using RescaleFilterType = itk::RescaleIntensityImageFilter<OutputImageType, OutputImageType>;
    auto rescaler           = RescaleFilterType::New();
    rescaler->SetInput(extractor->GetOutput());
    rescaler->SetOutputMinimum(0);
    rescaler->SetOutputMaximum(255);

    // Write PNG
    auto writer = itk::ImageFileWriter<OutputImageType>::New();
    writer->SetFileName(outputPath);
    writer->SetInput(rescaler->GetOutput());

    try {
        writer->Update();
        std::cout << "Saved: " << outputPath << std::endl;
    } catch (const itk::ExceptionObject& e) {
        std::cerr << "Error writing: " << e << std::endl;
        return 1;
    }

    return 0;
}