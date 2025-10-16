#pragma once
#include <cmath>
#include <stdexcept>
#include <type_traits>

#include "itkImage.h"
#include "itkImageRegionConstIterator.h"
#include "itkLabelOverlapMeasuresImageFilter.h"

namespace itkexp {

    // Mean Squared Error between two same-sized images
    template <typename TImage>
    double computeMSE(const typename TImage::Pointer& a, const typename TImage::Pointer& b)
    {
        if (!a || !b)
            throw std::invalid_argument("computeMSE: null image");
        if (a->GetLargestPossibleRegion().GetSize() != b->GetLargestPossibleRegion().GetSize())
            throw std::runtime_error("computeMSE: images must have same size");

        using Iterator = itk::ImageRegionConstIterator<TImage>;
        Iterator itA(a, a->GetLargestPossibleRegion());
        Iterator itB(b, b->GetLargestPossibleRegion());

        long double sum = 0.0;
        size_t      n   = 0;
        for (itA.GoToBegin(), itB.GoToBegin(); !itA.IsAtEnd(); ++itA, ++itB) {
            const long double d =
                static_cast<long double>(itA.Get()) - static_cast<long double>(itB.Get());
            sum += d * d;
            ++n;
        }
        return n ? static_cast<double>(sum / n) : 0.0;
    }

    // Normalized Cross-Correlation (Pearson correlation coefficient)
    template <typename TImage>
    double computeNCC(const typename TImage::Pointer& a, const typename TImage::Pointer& b)
    {
        if (!a || !b)
            throw std::invalid_argument("computeNCC: null image");
        if (a->GetLargestPossibleRegion().GetSize() != b->GetLargestPossibleRegion().GetSize())
            throw std::runtime_error("computeNCC: images must have same size");

        using Iterator = itk::ImageRegionConstIterator<TImage>;
        Iterator itA(a, a->GetLargestPossibleRegion());
        Iterator itB(b, b->GetLargestPossibleRegion());

        long double sumA = 0.0, sumB = 0.0, sumAA = 0.0, sumBB = 0.0, sumAB = 0.0;
        size_t      n = 0;
        for (itA.GoToBegin(), itB.GoToBegin(); !itA.IsAtEnd(); ++itA, ++itB) {
            const long double va = static_cast<long double>(itA.Get());
            const long double vb = static_cast<long double>(itB.Get());
            sumA += va;
            sumB += vb;
            sumAA += va * va;
            sumBB += vb * vb;
            sumAB += va * vb;
            ++n;
        }
        if (!n)
            return 0.0;
        const long double meanA = sumA / n;
        const long double meanB = sumB / n;
        const long double cov   = (sumAB / n) - meanA * meanB;
        const long double varA  = (sumAA / n) - meanA * meanA;
        const long double varB  = (sumBB / n) - meanB * meanB;
        const long double denom = std::sqrt(std::max<long double>(varA * varB, 0.0L));
        if (denom == 0.0L)
            return 0.0;
        return static_cast<double>(cov / denom);
    }

    // Dice coefficient for label maps (same size). Labels are treated as multi-label.
    // Returns Dice for foreground union of all labels; for per-label, use ITK filter directly.
    template <typename TLabelImage>
    double computeDice(const typename TLabelImage::Pointer& gt,
                       const typename TLabelImage::Pointer& pred)
    {
        if (!gt || !pred)
            throw std::invalid_argument("computeDice: null image");
        if (gt->GetLargestPossibleRegion().GetSize() != pred->GetLargestPossibleRegion().GetSize())
            throw std::runtime_error("computeDice: images must have same size");

        using Filter                     = itk::LabelOverlapMeasuresImageFilter<TLabelImage>;
        typename Filter::Pointer overlap = Filter::New();
        overlap->SetSourceImage(gt);
        overlap->SetTargetImage(pred);
        overlap->Update();
        // Union-over-all-labels Dice (not per-label)
        return overlap->GetDiceCoefficient();
    }

}  // namespace itkexp
