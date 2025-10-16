#include "registration/BSplineRegistration.hpp"
#include "registration/Registration.hpp"
#include "itkImageFileReader.h"
#include <iostream>
#include <array>

int main(int argc, char* argv[])
{
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0]
                  << " fixedImage movingImage outputImage mesh       \n"
                     "  mesh examples: 4,4,4 or 6,6,6\n";
        return EXIT_FAILURE;
    }

    const std::string fixedFile  = argv[1];
    const std::string movingFile = argv[2];
    const std::string outputFile = argv[3];
    const std::string meshStr    = argv[4];

    // Parse mesh
    std::array<unsigned int,3> mesh{4,4,4};
    if (sscanf(meshStr.c_str(), "%u,%u,%u", &mesh[0], &mesh[1], &mesh[2]) != 3) {
        std::cerr << "Invalid mesh string. Use e.g. 4,4,4\n";
        return EXIT_FAILURE;
    }

    constexpr unsigned int Dimension = 3;
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    try {
        auto fixedReader = itk::ImageFileReader<ImageType>::New();
        auto movingReader = itk::ImageFileReader<ImageType>::New();
        fixedReader->SetFileName(fixedFile);
        movingReader->SetFileName(movingFile);
        fixedReader->Update();
        movingReader->Update();

        auto fixed  = fixedReader->GetOutput();
        auto moving = movingReader->GetOutput();

        // Optional: do an **affine warm start** to help the B-spline
        std::cout << "🔧 (Optional) Affine warm start...\n";
        // 🔧 Optional affine warm start: save intermediate output for inspection
        const std::string affineTmpPath = "output/temp_affine_init.nrrd";
        std::cout << "🔧 Performing affine warm start → " << affineTmpPath << "\n";
        auto affineInit = itkexp::registerImages<ImageType>(fixed, moving, affineTmpPath);


        // Deformable refinement (B-spline)
        itkexp::bsplineRegister<ImageType>(fixed, moving, mesh, outputFile);
    }
    catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK Exception: " << e << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
