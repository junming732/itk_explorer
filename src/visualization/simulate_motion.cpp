/**
 * simulate_motion.cpp
 *
 * Simulate patient motion by applying small random rigid transform to an image.
 * Used to demonstrate motion correction capabilities.
 */

#include <iostream>
#include <random>

#include "itkEuler3DTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"

using ImageType     = itk::Image<float, 3>;
using TransformType = itk::Euler3DTransform<double>;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input> <output> [tx ty tz rx ry rz]\n";
        std::cout << "  tx,ty,tz: translation in mm (default: random -5 to 5)\n";
        std::cout << "  rx,ry,rz: rotation in radians (default: random -0.1 to 0.1)\n";
        return 1;
    }

    std::string inputPath  = argv[1];
    std::string outputPath = argv[2];

    // Parse motion parameters or use random
    double tx, ty, tz, rx, ry, rz;

    if (argc >= 9) {
        tx = std::stod(argv[3]);
        ty = std::stod(argv[4]);
        tz = std::stod(argv[5]);
        rx = std::stod(argv[6]);
        ry = std::stod(argv[7]);
        rz = std::stod(argv[8]);
    } else {
        // Generate random motion
        std::random_device               rd;
        std::mt19937                     gen(rd());
        std::uniform_real_distribution<> trans_dist(-5.0, 5.0);  // ±5mm
        std::uniform_real_distribution<> rot_dist(-0.1, 0.1);    // ±0.1 rad (~5.7°)

        tx = trans_dist(gen);
        ty = trans_dist(gen);
        tz = trans_dist(gen);
        rx = rot_dist(gen);
        ry = rot_dist(gen);
        rz = rot_dist(gen);
    }

    std::cout << "Simulating motion:\n";
    std::cout << "  Translation: [" << tx << ", " << ty << ", " << tz << "] mm\n";
    std::cout << "  Rotation: [" << rx << ", " << ry << ", " << rz << "] rad\n";
    std::cout << "            = [" << (rx * 180 / 3.14159) << ", " << (ry * 180 / 3.14159) << ", "
              << (rz * 180 / 3.14159) << "] degrees\n";

    try {
        // Read input image
        auto reader = itk::ImageFileReader<ImageType>::New();
        reader->SetFileName(inputPath);
        reader->Update();

        ImageType::Pointer inputImage = reader->GetOutput();

        // Create transform
        auto transform = TransformType::New();

        // Set center (use image center)
        ImageType::PointType center;
        ImageType::IndexType centerIndex;
        ImageType::SizeType  size = inputImage->GetLargestPossibleRegion().GetSize();

        for (unsigned int i = 0; i < 3; ++i) {
            centerIndex[i] = size[i] / 2;
        }
        inputImage->TransformIndexToPhysicalPoint(centerIndex, center);
        transform->SetCenter(center);

        // Set rotation and translation
        transform->SetRotation(rx, ry, rz);
        TransformType::OutputVectorType translation;
        translation[0] = tx;
        translation[1] = ty;
        translation[2] = tz;
        transform->SetTranslation(translation);

        // Resample image with motion
        auto resampler = itk::ResampleImageFilter<ImageType, ImageType>::New();
        resampler->SetInput(inputImage);
        resampler->SetTransform(transform);
        resampler->SetSize(inputImage->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputSpacing(inputImage->GetSpacing());
        resampler->SetOutputOrigin(inputImage->GetOrigin());
        resampler->SetOutputDirection(inputImage->GetDirection());
        resampler->SetDefaultPixelValue(0);

        auto interpolator = itk::LinearInterpolateImageFunction<ImageType, double>::New();
        resampler->SetInterpolator(interpolator);

        // Write output
        auto writer = itk::ImageFileWriter<ImageType>::New();
        writer->SetFileName(outputPath);
        writer->SetInput(resampler->GetOutput());
        writer->Update();

        std::cout << "Motion-corrupted image saved: " << outputPath << "\n";

        return 0;

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "Error: " << e << std::endl;
        return 1;
    }
}