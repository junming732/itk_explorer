#ifndef LANDMARK_IO_H
#define LANDMARK_IO_H

#include <string>
#include <vector>

#include "itkPoint.h"

namespace Registration {

    /**
     * @brief 3D point type for landmarks
     */
    using LandmarkType     = itk::Point<double, 3>;
    using LandmarkListType = std::vector<LandmarkType>;

    /**
     * @brief Utilities for reading and writing landmarks from/to CSV files
     */
    class LandmarkIO
    {
      public:
        /**
         * @brief Read landmarks from CSV file
         * @param filename Path to CSV file (format: x,y,z per line)
         * @return Vector of 3D points
         */
        static LandmarkListType ReadLandmarks(const std::string& filename);

        /**
         * @brief Write landmarks to CSV file
         * @param filename Output CSV file path
         * @param landmarks Vector of 3D points
         * @return true if successful
         */
        static bool WriteLandmarks(const std::string& filename, const LandmarkListType& landmarks);

        /**
         * @brief Check if landmarks file exists and is valid
         */
        static bool ValidateLandmarksFile(const std::string& filename);

        /**
         * @brief Print landmarks to console
         */
        static void PrintLandmarks(const LandmarkListType& landmarks,
                                   const std::string&      label = "Landmarks");
    };

}  // namespace Registration

#endif  // LANDMARK_IO_H