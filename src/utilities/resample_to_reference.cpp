/**
 * resample_to_reference.cpp
 * Resamples moving image to fixed image space using identity transform
 * Use this to create "before" comparison images
 */

#include <itkIdentityTransform.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

int main(int argc, char* argv[])
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <moving> <fixed_reference> <output>\n";
        std::cerr << "\nResamples moving image to fixed image space.\n";
        std::cerr << "Uses identity transform (no registration).\n";
        std::cerr << "\nExample:\n";
        std::cerr << "  " << argv[0] << " T2.nii.gz T1.nii.gz T2_in_T1_space.nrrd\n";
        return 1;
    }

    // Type definitions
    using ImageType          = itk::Image<float, 3>;
    using ReaderType         = itk::ImageFileReader<ImageType>;
    using WriterType         = itk::ImageFileWriter<ImageType>;
    using TransformType      = itk::IdentityTransform<double, 3>;
    using InterpolatorType   = itk::LinearInterpolateImageFunction<ImageType, double>;
    using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;

    try {
        // Read moving image
        std::cout << "Reading moving image: " << argv[1] << std::endl;
        auto movingReader = ReaderType::New();
        movingReader->SetFileName(argv[1]);
        movingReader->Update();

        // Read fixed image (reference)
        std::cout << "Reading fixed image: " << argv[2] << std::endl;
        auto fixedReader = ReaderType::New();
        fixedReader->SetFileName(argv[2]);
        fixedReader->Update();

        ImageType::Pointer movingImage = movingReader->GetOutput();
        ImageType::Pointer fixedImage  = fixedReader->GetOutput();

        // Create identity transform (no rotation, no translation)
        auto transform = TransformType::New();

        // Create interpolator
        auto interpolator = InterpolatorType::New();

        // Create resampler
        std::cout << "Resampling to reference space..." << std::endl;
        auto resampler = ResampleFilterType::New();
        resampler->SetTransform(transform);
        resampler->SetInterpolator(interpolator);
        resampler->SetInput(movingImage);

        // Use fixed image as reference (determines output geometry)
        resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
        resampler->SetOutputSpacing(fixedImage->GetSpacing());
        resampler->SetOutputOrigin(fixedImage->GetOrigin());
        resampler->SetOutputDirection(fixedImage->GetDirection());
        resampler->SetDefaultPixelValue(0);

        resampler->Update();

        // Write output
        std::cout << "Writing output: " << argv[3] << std::endl;
        auto writer = WriterType::New();
        writer->SetFileName(argv[3]);
        writer->SetInput(resampler->GetOutput());
        writer->Update();

        std::cout << "✓ Successfully resampled to reference space" << std::endl;
        std::cout << "\nOutput details:" << std::endl;
        std::cout << "  Size: " << fixedImage->GetLargestPossibleRegion().GetSize() << std::endl;
        std::cout << "  Spacing: " << fixedImage->GetSpacing() << std::endl;
        std::cout << "  Origin: " << fixedImage->GetOrigin() << std::endl;

        return 0;

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "Error: " << e << std::endl;
        return 1;
    }
}