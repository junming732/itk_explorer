/**
 * extract_slice_main.cpp
 *
 * Extract and save a 2D slice from a 3D medical image for visualization.
 * Useful for headless environments where ITK-SNAP is unavailable.
 *
 * Usage:
 *   extract_slice_main <input_3d_image> <output_png> [--slice N] [--axis 0|1|2]
 *
 * Example:
 *   extract_slice_main data/brain.nii.gz output/slice.png --slice 90 --axis 2
 */

#include <itkExtractImageFilter.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkPNGImageIO.h>
#include <itkRescaleIntensityImageFilter.h>

#include <iostream>
#include <string>

using ImageType3D = itk::Image<float, 3>;
using ImageType2D = itk::Image<unsigned char, 2>;

struct CommandLineArgs
{
    std::string inputPath;
    std::string outputPath;
    int         sliceIndex = -1;  // -1 means use middle slice
    int         axis       = 2;   // 0=sagittal, 1=coronal, 2=axial (default)
};

CommandLineArgs ParseCommandLine(int argc, char* argv[])
{
    CommandLineArgs args;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_3d> <output_png> [--slice N] [--axis 0|1|2]\n";
        std::cerr << "  --slice N : Extract slice at index N (default: middle slice)\n";
        std::cerr << "  --axis A  : 0=sagittal, 1=coronal, 2=axial (default: 2)\n";
        std::exit(1);
    }

    args.inputPath  = argv[1];
    args.outputPath = argv[2];

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--slice" && i + 1 < argc) {
            args.sliceIndex = std::stoi(argv[++i]);
        } else if (arg == "--axis" && i + 1 < argc) {
            args.axis = std::stoi(argv[++i]);
            if (args.axis < 0 || args.axis > 2) {
                std::cerr << "Error: axis must be 0, 1, or 2\n";
                std::exit(1);
            }
        }
    }

    return args;
}

int main(int argc, char* argv[])
{
    auto args = ParseCommandLine(argc, argv);

    try {
        // Read 3D image
        std::cout << "Reading: " << args.inputPath << std::endl;
        auto reader = itk::ImageFileReader<ImageType3D>::New();
        reader->SetFileName(args.inputPath);
        reader->Update();

        ImageType3D::Pointer    image3D = reader->GetOutput();
        ImageType3D::RegionType region  = image3D->GetLargestPossibleRegion();
        ImageType3D::SizeType   size    = region.GetSize();

        // Determine slice index (use middle if not specified)
        int sliceIndex = args.sliceIndex;
        if (sliceIndex < 0) {
            sliceIndex = size[args.axis] / 2;
        }

        // Check bounds
        if (sliceIndex < 0 || sliceIndex >= static_cast<int>(size[args.axis])) {
            std::cerr << "Error: Slice index " << sliceIndex << " out of bounds [0, "
                      << size[args.axis] - 1 << "]\n";
            return 1;
        }

        std::cout << "Image size: " << size[0] << " x " << size[1] << " x " << size[2] << std::endl;
        std::cout << "Extracting slice " << sliceIndex << " along axis " << args.axis << std::endl;

        // Set up extraction region
        ImageType3D::SizeType extractSize = size;
        extractSize[args.axis]            = 0;  // Collapse this dimension

        ImageType3D::IndexType start = region.GetIndex();
        start[args.axis]             = sliceIndex;

        ImageType3D::RegionType extractRegion;
        extractRegion.SetSize(extractSize);
        extractRegion.SetIndex(start);

        // Extract 2D slice
        auto extractor = itk::ExtractImageFilter<ImageType3D, ImageType3D>::New();
        extractor->SetInput(image3D);
        extractor->SetExtractionRegion(extractRegion);
        extractor->SetDirectionCollapseToIdentity();
        extractor->Update();

        // Convert to 2D image (ITK requires proper dimension reduction)
        ImageType3D::Pointer slice3D = extractor->GetOutput();

        // Manual conversion to proper 2D image
        ImageType3D::SizeType  slice3DSize = slice3D->GetLargestPossibleRegion().GetSize();
        ImageType2D::SizeType  slice2DSize;
        ImageType2D::IndexType slice2DStart;
        slice2DStart.Fill(0);

        // Map 3D dimensions to 2D (skip the collapsed axis)
        int dim2D = 0;
        for (int d = 0; d < 3; ++d) {
            if (d != args.axis) {
                slice2DSize[dim2D++] = slice3DSize[d];
            }
        }

        ImageType2D::RegionType slice2DRegion;
        slice2DRegion.SetSize(slice2DSize);
        slice2DRegion.SetIndex(slice2DStart);

        auto slice2D = ImageType2D::New();
        slice2D->SetRegions(slice2DRegion);
        slice2D->Allocate();

        // Copy pixel values with rescaling to [0, 255]
        itk::ImageRegionConstIterator<ImageType3D> it3D(slice3D,
                                                        slice3D->GetLargestPossibleRegion());
        itk::ImageRegionIterator<ImageType2D>      it2D(slice2D, slice2DRegion);

        // First pass: find min/max for scaling
        float minVal = itk::NumericTraits<float>::max();
        float maxVal = itk::NumericTraits<float>::NonpositiveMin();

        for (it3D.GoToBegin(); !it3D.IsAtEnd(); ++it3D) {
            float val = it3D.Get();
            if (val < minVal)
                minVal = val;
            if (val > maxVal)
                maxVal = val;
        }

        std::cout << "Intensity range: [" << minVal << ", " << maxVal << "]" << std::endl;

        // Second pass: rescale and copy
        float range = maxVal - minVal;
        if (range < 1e-6)
            range = 1.0;  // Avoid division by zero

        for (it3D.GoToBegin(), it2D.GoToBegin(); !it3D.IsAtEnd(); ++it3D, ++it2D) {
            float normalized = (it3D.Get() - minVal) / range;
            it2D.Set(static_cast<unsigned char>(normalized * 255));
        }

        // Write PNG
        std::cout << "Writing: " << args.outputPath << std::endl;
        auto writer = itk::ImageFileWriter<ImageType2D>::New();
        writer->SetFileName(args.outputPath);
        writer->SetInput(slice2D);

        auto pngIO = itk::PNGImageIO::New();
        writer->SetImageIO(pngIO);

        writer->Update();

        std::cout << "Success! Slice extracted to: " << args.outputPath << std::endl;
        std::cout << "Slice dimensions: " << slice2DSize[0] << " x " << slice2DSize[1] << std::endl;

        return 0;

    } catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK Error: " << e << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}