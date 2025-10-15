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
