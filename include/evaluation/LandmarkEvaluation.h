#ifndef LANDMARK_EVALUATION_H
#define LANDMARK_EVALUATION_H

#include <map>
#include <string>

#include "itkTransform.h"
#include "landmarks/LandmarkIO.h"

namespace Registration {

    /**
     * @brief Statistics for landmark-based evaluation
     */
    struct LandmarkEvaluationResult
    {
        double              meanError;  // Target Registration Error (TRE)
        double              stdError;
        double              minError;
        double              maxError;
        double              medianError;
        std::vector<double> perLandmarkErrors;
        size_t              numLandmarks;
    };

    /**
     * @brief Evaluate registration quality using landmarks
     *
     * Computes Target Registration Error (TRE) by:
     * 1. Transforming moving landmarks using the registration transform
     * 2. Computing Euclidean distance to corresponding fixed landmarks
     * 3. Calculating statistics (mean, std, min, max, median)
     */
    class LandmarkEvaluation
    {
      public:
        /**
         * @brief Compute Target Registration Error before and after registration
         *
         * @param fixedLandmarks Landmarks in fixed image space
         * @param movingLandmarks Landmarks in moving image space (before registration)
         * @param transform Registration transform to evaluate
         * @return Evaluation statistics
         */
        static LandmarkEvaluationResult
        EvaluateRegistration(const LandmarkListType&             fixedLandmarks,
                             const LandmarkListType&             movingLandmarks,
                             const itk::Transform<double, 3, 3>* transform = nullptr);

        /**
         * @brief Compute error before registration (baseline)
         */
        static LandmarkEvaluationResult
        ComputeInitialError(const LandmarkListType& fixedLandmarks,
                            const LandmarkListType& movingLandmarks);

        /**
         * @brief Print evaluation results
         */
        static void PrintResults(const LandmarkEvaluationResult& result,
                                 const std::string&              label = "Evaluation");

        /**
         * @brief Save evaluation results to CSV
         */
        static bool SaveResultsToCSV(const std::string&              filename,
                                     const LandmarkEvaluationResult& beforeResult,
                                     const LandmarkEvaluationResult& afterResult);

        /**
         * @brief Save per-landmark errors to CSV
         */
        static bool SavePerLandmarkErrors(const std::string&         filename,
                                          const std::vector<double>& errors);

      private:
        /**
         * @brief Compute Euclidean distance between two points
         */
        static double ComputeDistance(const LandmarkType& p1, const LandmarkType& p2);

        /**
         * @brief Compute statistics from error vector
         */
        static LandmarkEvaluationResult ComputeStatistics(const std::vector<double>& errors);
    };

}  // namespace Registration

#endif  // LANDMARK_EVALUATION_H