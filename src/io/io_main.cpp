#include "io/ImageIO.hpp"
#include <iostream>
#include <filesystem>

using namespace itkexp;

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: itk_io <input_image> <output_image>\n";
        return EXIT_FAILURE;
    }

    const std::filesystem::path inputPath = argv[1];
    const std::filesystem::path outputPath = argv[2];

    try {
        using PixelType = float;
        constexpr unsigned int Dimension = 3;
        using IO = ImageIO<PixelType, Dimension>;

        auto image = IO::readImage(inputPath.string());
        IO::printImageInfo(image);
        IO::writeImage(image, outputPath.string());

        std::cout << std::format("Successfully wrote image to {}\n", outputPath.string());
    } catch (const std::exception& e) {
        std::cerr << "❌ " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
