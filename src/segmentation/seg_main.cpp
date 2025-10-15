#include "segmentation/Segmentation.hpp"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " inputImage outputMask\n";
        return EXIT_FAILURE;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;
    using LabelImageType = itk::Image<unsigned short, Dimension>;

    try
    {
        auto reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(inputFile);
        reader->Update();

        // 1. Otsu threshold
        auto mask = itkexp::otsuThreshold<ImageType>(reader->GetOutput());

        // 2. Connected component labeling
        auto labels = itkexp::labelComponents<ImageType>(mask);

        auto writer = itk::ImageFileWriter<LabelImageType>::New();
        writer->SetFileName(outputFile);
        writer->SetInput(labels);
        writer->Update();

        std::cout << "✅ Segmentation written to " << outputFile << std::endl;
    }
    catch (const itk::ExceptionObject& e)
    {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
