#include "filters/Filtering.hpp"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " inputImage outputImage\n";
        return EXIT_FAILURE;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    try
    {
        // Read
        auto reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(inputFile);
        reader->Update();
        auto input = reader->GetOutput();

        // Apply filters
        auto smoothed = itkexp::applyGaussian<ImageType>(input, 1.0);
        auto gradient = itkexp::computeGradient<ImageType>(smoothed);

        // Write
        auto writer = itk::ImageFileWriter<ImageType>::New();
        writer->SetFileName(outputFile);
        writer->SetInput(gradient);
        writer->Update();

        std::cout << "✅ Filtered image written to " << outputFile << std::endl;
    }
    catch (const itk::ExceptionObject& e)
    {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
