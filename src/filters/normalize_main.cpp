#include "filters/Intensity.hpp"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " inputImage outputImage rangeMax\n";
        return EXIT_FAILURE;
    }

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];
    const double rangeMax = std::stod(argv[3]);

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    try
    {
        auto reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(inputFile);
        reader->Update();

        auto input = reader->GetOutput();
        auto normalized = itkexp::rescaleIntensity<ImageType>(input, 0.0, rangeMax);

        auto writer = itk::ImageFileWriter<ImageType>::New();
        writer->SetFileName(outputFile);
        writer->SetInput(normalized);
        writer->Update();

        std::cout << "✅ Normalized image written to " << outputFile << std::endl;
    }
    catch (const itk::ExceptionObject& e)
    {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
