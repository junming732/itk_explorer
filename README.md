# 🧠 ITK Explorer — Incremental ITK C++20 Project

## Overview
**ITK Explorer** is a modular, incremental project for learning and demonstrating the **Insight Toolkit (ITK)** using modern **C++20** and **CMake**.
Each stage introduces new ITK concepts — from basic I/O to advanced registration and evaluation.

Project built and tested on:
- **Ubuntu 22.04**
- **ITK 5.2**
- **VTK 9.1**
- **CMake ≥ 3.22**
- **GCC ≥ 11**
Compatible with C++20 and fully formatted with clang-format.

---

## 🧩 Stage Summary

| Stage | Focus | Key Concepts |
|--------|--------|--------------|
| **1** | Image I/O | Read/Write NIfTI, NRRD via `itk::ImageFileReader` / `itk::ImageFileWriter` |
| **2** | Filtering | Gaussian smoothing, gradient magnitude, ITK pipeline basics |
| **3** | Segmentation | Threshold-based segmentation with connected components |
| **4** | Feature Extraction | Normalization, intensity statistics |
| **5** | Visualization | Headless PNG slice export, 3D volume writing (`.vti`, `.nrrd`) |
| **6** | Affine Registration | `itk::CenteredTransformInitializer`, Mattes MI metric |
| **7** | B-spline Registration & Batch Automation | Deformable registration, looping over subjects |
| **8** | Evaluation Metrics | Compute MSE, NCC, Dice and log results to `metrics.csv` |

---

## 🛠️ Build Instructions

```bash
# Clean build
rm -rf build
cmake -S . -B build
cmake --build build -j
```

All executables are placed under `build/bin/`.

---

## 🚀 Example Workflows

### Stage 1 — Read/Write
```bash
./build/bin/itk_io data/IXI651-Guys-1118-T1.nii output/IXI651-T1.nrrd
```

### Stage 2 — Filtering
```bash
./build/bin/itk_filter output/IXI651-T1.nrrd output/IXI651-T1-gradient.nrrd
```

### Stage 3 — Segmentation
```bash
./build/bin/itk_segment output/IXI651-T1.nrrd output/IXI651-T1-seg.nrrd
```

### Stage 5 — Visualization (Headless)
```bash
./build/bin/itk_visualize output/IXI651-T1-seg.nrrd
```
Outputs PNG slices:
```
output/exported_axial.png
output/exported_coronal.png
output/exported_sagittal.png
```

### Stage 6 — Affine Registration
```bash
./build/bin/itk_register data/fixed.nii data/moving.nii output/affine_registered.nrrd
```

### Stage 7 — B-spline Registration
```bash
./build/bin/itk_bspline_register   data/IXI651-Guys-1118-T1.nii   data/IXI300-Guys-0880-T1.nii   output/IXI300-T1_bspline.nrrd   6,6,6
```

### Stage 7 — Batch Mode
```bash
./build/bin/itk_batch_register data/fixed.nii data/IXI-T1 output/batch
```
Automatically logs metrics after each run.

### Stage 8 — Evaluation Metrics
```bash
./build/bin/itk_metrics fixed.nii moving.nii output/registered.nrrd --csv output/metrics.csv
```
Appends results to:
```
output/metrics.csv
```

---

## 📦 Directory Structure

```
itk_explorer/
├── include/
│   ├── io/
│   ├── filters/
│   ├── segmentation/
│   ├── registration/
│   ├── visualization/
│   └── evaluation/
├── src/
│   ├── io/
│   ├── filters/
│   ├── segmentation/
│   ├── registration/
│   ├── visualization/
│   └── evaluation/
├── data/
├── output/
└── build/
```

---

## 📊 Output Artifacts
- `.nrrd` — ITK-compatible volume images
- `.png` — Slice previews for visualization
- `metrics.csv` — Quantitative evaluation (Stage 8)

