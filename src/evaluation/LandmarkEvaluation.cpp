#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

#include "evaluation/LandmarkEvaluation.h"

namespace Registration {

    double LandmarkEvaluation::ComputeDistance(const LandmarkType& p1, const LandmarkType& p2)
    {
        double dx = p1[0] - p2[0];
        double dy = p1[1] - p2[1];
        double dz = p1[2] - p2[2];
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    LandmarkEvaluationResult
    LandmarkEvaluation::ComputeStatistics(const std::vector<double>& errors)
    {
        LandmarkEvaluationResult result;
        result.numLandmarks      = errors.size();
        result.perLandmarkErrors = errors;

        if (errors.empty()) {
            result.meanError   = 0.0;
            result.stdError    = 0.0;
            result.minError    = 0.0;
            result.maxError    = 0.0;
            result.medianError = 0.0;
            return result;
        }

        // Mean
        result.meanError = std::accumulate(errors.begin(), errors.end(), 0.0) / errors.size();

        // Standard deviation
        double variance = 0.0;
        for (double error : errors) {
            variance += (error - result.meanError) * (error - result.meanError);
        }
        result.stdError = std::sqrt(variance / errors.size());

        // Min and Max
        result.minError = *std::min_element(errors.begin(), errors.end());
        result.maxError = *std::max_element(errors.begin(), errors.end());

        // Median
        std::vector<double> sortedErrors = errors;
        std::sort(sortedErrors.begin(), sortedErrors.end());
        size_t mid = sortedErrors.size() / 2;
        if (sortedErrors.size() % 2 == 0) {
            result.medianError = (sortedErrors[mid - 1] + sortedErrors[mid]) / 2.0;
        } else {
            result.medianError = sortedErrors[mid];
        }

        return result;
    }

    LandmarkEvaluationResult
    LandmarkEvaluation::ComputeInitialError(const LandmarkListType& fixedLandmarks,
                                            const LandmarkListType& movingLandmarks)
    {

        return EvaluateRegistration(fixedLandmarks, movingLandmarks, nullptr);
    }

    LandmarkEvaluationResult
    LandmarkEvaluation::EvaluateRegistration(const LandmarkListType&             fixedLandmarks,
                                             const LandmarkListType&             movingLandmarks,
                                             const itk::Transform<double, 3, 3>* transform)
    {

        if (fixedLandmarks.size() != movingLandmarks.size()) {
            std::cerr << "Error: Number of landmarks mismatch! "
                      << "Fixed: " << fixedLandmarks.size()
                      << ", Moving: " << movingLandmarks.size() << std::endl;
            return LandmarkEvaluationResult();
        }

        std::vector<double> errors;
        errors.reserve(fixedLandmarks.size());

        for (size_t i = 0; i < fixedLandmarks.size(); ++i) {
            LandmarkType transformedPoint;

            if (transform != nullptr) {
                // Transform moving landmark
                transformedPoint = transform->TransformPoint(movingLandmarks[i]);
            } else {
                // No transform - compute initial error
                transformedPoint = movingLandmarks[i];
            }

            // Compute distance to corresponding fixed landmark
            double error = ComputeDistance(fixedLandmarks[i], transformedPoint);
            errors.push_back(error);
        }

        return ComputeStatistics(errors);
    }

    void LandmarkEvaluation::PrintResults(const LandmarkEvaluationResult& result,
                                          const std::string&              label)
    {
        std::cout << "\n=== " << label << " ===" << std::endl;
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Number of landmarks: " << result.numLandmarks << std::endl;
        std::cout << "Mean TRE:   " << result.meanError << " mm" << std::endl;
        std::cout << "Std Dev:    " << result.stdError << " mm" << std::endl;
        std::cout << "Median TRE: " << result.medianError << " mm" << std::endl;
        std::cout << "Min TRE:    " << result.minError << " mm" << std::endl;
        std::cout << "Max TRE:    " << result.maxError << " mm" << std::endl;
    }

    bool LandmarkEvaluation::SaveResultsToCSV(const std::string&              filename,
                                              const LandmarkEvaluationResult& beforeResult,
                                              const LandmarkEvaluationResult& afterResult)
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create results file: " << filename << std::endl;
            return false;
        }

        file << std::fixed << std::setprecision(6);

        // Header
        file << "Metric,Before,After,Improvement\n";

        // Mean TRE
        file << "Mean TRE (mm)," << beforeResult.meanError << "," << afterResult.meanError << ","
             << (beforeResult.meanError - afterResult.meanError) << "\n";

        // Std Dev
        file << "Std Dev (mm)," << beforeResult.stdError << "," << afterResult.stdError << ","
             << (beforeResult.stdError - afterResult.stdError) << "\n";

        // Median
        file << "Median TRE (mm)," << beforeResult.medianError << "," << afterResult.medianError
             << "," << (beforeResult.medianError - afterResult.medianError) << "\n";

        // Min
        file << "Min TRE (mm)," << beforeResult.minError << "," << afterResult.minError << ","
             << (beforeResult.minError - afterResult.minError) << "\n";

        // Max
        file << "Max TRE (mm)," << beforeResult.maxError << "," << afterResult.maxError << ","
             << (beforeResult.maxError - afterResult.maxError) << "\n";

        file.close();

        std::cout << "Saved evaluation results to: " << filename << std::endl;

        return true;
    }

    bool LandmarkEvaluation::SavePerLandmarkErrors(const std::string&         filename,
                                                   const std::vector<double>& errors)
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create per-landmark errors file: " << filename << std::endl;
            return false;
        }

        file << std::fixed << std::setprecision(6);
        file << "Landmark,Error (mm)\n";

        for (size_t i = 0; i < errors.size(); ++i) {
            file << i << "," << errors[i] << "\n";
        }

        file.close();

        std::cout << "Saved per-landmark errors to: " << filename << std::endl;

        return true;
    }

}  // namespace Registration