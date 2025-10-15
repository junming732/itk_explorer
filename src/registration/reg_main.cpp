#include "registration/Registration.hpp"
#include "itkImageFileReader.h"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0]
                  << " fixedImage.nrrd movingImage.nrrd outputRegistered.nrrd\n";
        return EXIT_FAILURE;
    }

    const std::string fixedFile = argv[1];
    const std::string movingFile = argv[2];
    const std::string outputFile = argv[3];

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    try
    {
        auto fixedReader = itk::ImageFileReader<ImageType>::New();
        auto movingReader = itk::ImageFileReader<ImageType>::New();

        fixedReader->SetFileName(fixedFile);
        movingReader->SetFileName(movingFile);
        fixedReader->Update();
        movingReader->Update();

        auto fixedImage = fixedReader->GetOutput();
        auto movingImage = movingReader->GetOutput();

        itkexp::registerImages<ImageType>(fixedImage, movingImage, outputFile);
    }
    catch (const itk::ExceptionObject& e)
    {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
