# ITK Explorer — Stage 2: Filtering

Stage 2 builds upon the I/O foundation by introducing **image filtering**.

---

## 🧠 Goal

Demonstrate ITK’s **pipeline processing model** using:
1. `SmoothingRecursiveGaussianImageFilter` – noise reduction
2. `GradientMagnitudeImageFilter` – edge strength computation

---

## ⚙️ Usage

```bash
./build/bin/itk_filters input_image.nrrd output_image.nrrd

# ITK Explorer — Stage 3: Intensity Normalization

---

## 🧠 Goal
Normalize medical image intensities into a controlled range using
`itk::RescaleIntensityImageFilter`.
This prepares images for consistent thresholding, segmentation, and visualization.

---

## ⚙️ Usage
```bash
./build/bin/itk_normalize input_image.nrrd output_image.nrrd rangeMax

