#pragma once
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMetaDataDictionary.h>

#include <iostream>
#include <string>
#include <filesystem>
#include <format>

namespace itkexp {

template <typename TPixel, unsigned int VDimension>
class ImageIO {
public:
    using ImageType = itk::Image<TPixel, VDimension>;
    using ImagePointer = typename ImageType::Pointer;
    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;

    [[nodiscard]] static ImagePointer readImage(const std::filesystem::path& filename) {
        auto reader = ReaderType::New();
        reader->SetFileName(filename.string());
        try {
            reader->Update();
        } catch (const itk::ExceptionObject& err) {
            throw std::runtime_error(std::format(
                "Error reading '{}': {}", filename.string(), err.GetDescription()));
        }
        // Return smart pointer — caller shares ownership with ITK pipeline
        return reader->GetOutput();
    }

    static void writeImage(const ImagePointer& image, const std::filesystem::path& filename) {
        if (!image) {
            throw std::invalid_argument("Null image pointer passed to writeImage().");
        }

        auto writer = WriterType::New();
        writer->SetFileName(filename.string());
        writer->SetInput(image);

        try {
            writer->Update();
        } catch (const itk::ExceptionObject& err) {
            throw std::runtime_error(std::format(
                "Error writing '{}': {}", filename.string(), err.GetDescription()));
        }
    }

    static void printImageInfo(const ImagePointer& image) {
        if (!image) {
            std::cerr << "⚠️ No image loaded.\n";
            return;
        }

        const auto& region = image->GetLargestPossibleRegion();
        const auto& size = region.GetSize();
        const auto& spacing = image->GetSpacing();
        const auto& origin = image->GetOrigin();
        const auto& direction = image->GetDirection();

        std::cout << "\n=== Image Info ===\n";
        std::cout << std::format("Size: {} x {} x {}\n", size[0], size[1], VDimension > 2 ? size[2] : 1);
        std::cout << std::format("Spacing: {:.3f}, {:.3f}, {:.3f}\n", spacing[0], spacing[1], VDimension > 2 ? spacing[2] : 0.0);
        std::cout << std::format("Origin: {:.3f}, {:.3f}, {:.3f}\n", origin[0], origin[1], VDimension > 2 ? origin[2] : 0.0);
        std::cout << "Direction:\n" << direction << "\n";
    }
};

} // namespace itkexp
