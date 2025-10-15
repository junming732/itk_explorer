# 🧩 ITK Explorer — Stage 6: Image Registration

## Overview
This stage demonstrates **3D image registration** in ITK.
It aligns a *moving* image to a *fixed* reference image using an affine transform,
Mean Squares metric, and Regular Step Gradient Descent optimizer.

## 🔧 Build
```bash
cmake -S . -B build
cmake --build build --target itk_register
```

## 🚀 Run
```bash
./build/bin/itk_register data/fixed.nrrd data/moving.nrrd output/registered.nrrd
```

## 🧠 What Happens
1. Loads fixed and moving 3D volumes
2. Performs affine registration
3. Resamples moving image into fixed space
4. Saves result as `registered.nrrd`

## ✅ Example Output
```
🚀 Starting registration...
✅ Registration finished.
💾 Registered image written to: output/registered.nrrd
```
