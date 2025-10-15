#include "visualization/Visualization.hpp"
#include "itkImageFileReader.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " inputImage\n";
        return EXIT_FAILURE;
    }

    const std::string inputFile = argv[1];

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    try
    {
        auto reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(inputFile);
        reader->Update();

        auto image = reader->GetOutput();

        itkexp::exportVolume<ImageType>(image, "output/exported_volume.nrrd");
        itkexp::exportSliceToPNG<ImageType>(image, "output/exported_slice.png");

        std::cout << "🏁 Headless visualization complete (no GUI needed)\n";
    }
    catch (const itk::ExceptionObject& e)
    {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
