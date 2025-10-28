#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "landmarks/LandmarkIO.h"

namespace Registration {

    LandmarkListType LandmarkIO::ReadLandmarks(const std::string& filename)
    {
        LandmarkListType landmarks;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open landmarks file: " << filename << std::endl;
            return landmarks;
        }

        std::string line;
        int         lineNum = 0;

        while (std::getline(file, line)) {
            lineNum++;

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream  iss(line);
            std::string         token;
            std::vector<double> coords;

            // Parse comma-separated values
            while (std::getline(iss, token, ',')) {
                try {
                    coords.push_back(std::stod(token));
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Invalid number on line " << lineNum << ": " << token
                              << std::endl;
                }
            }

            if (coords.size() == 3) {
                LandmarkType point;
                point[0] = coords[0];
                point[1] = coords[1];
                point[2] = coords[2];
                landmarks.push_back(point);
            } else {
                std::cerr << "Warning: Line " << lineNum << " does not have 3 coordinates"
                          << std::endl;
            }
        }

        file.close();

        std::cout << "Read " << landmarks.size() << " landmarks from " << filename << std::endl;

        return landmarks;
    }

    bool LandmarkIO::WriteLandmarks(const std::string& filename, const LandmarkListType& landmarks)
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot create landmarks file: " << filename << std::endl;
            return false;
        }

        file << std::fixed << std::setprecision(6);

        for (const auto& landmark : landmarks) {
            file << landmark[0] << "," << landmark[1] << "," << landmark[2] << "\n";
        }

        file.close();

        std::cout << "Wrote " << landmarks.size() << " landmarks to " << filename << std::endl;

        return true;
    }

    bool LandmarkIO::ValidateLandmarksFile(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        int         validLines = 0;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string        token;
            int                count = 0;

            while (std::getline(iss, token, ',')) {
                count++;
            }

            if (count == 3) {
                validLines++;
            }
        }

        file.close();

        return validLines > 0;
    }

    void LandmarkIO::PrintLandmarks(const LandmarkListType& landmarks, const std::string& label)
    {
        std::cout << "\n" << label << " (" << landmarks.size() << " points):" << std::endl;
        std::cout << std::fixed << std::setprecision(3);

        for (size_t i = 0; i < landmarks.size(); ++i) {
            std::cout << "  " << std::setw(3) << i << ": "
                      << "(" << std::setw(8) << landmarks[i][0] << ", " << std::setw(8)
                      << landmarks[i][1] << ", " << std::setw(8) << landmarks[i][2] << ")"
                      << std::endl;
        }
    }

}  // namespace Registration