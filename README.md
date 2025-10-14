# ITK Explorer — Stage 1: Image I/O

This project incrementally explores the **Insight Toolkit (ITK)** using modern **C++20**.
Stage 1 focuses on **image reading and writing**, converting between NIfTI (`.nii`) and NRRD (`.nrrd`) formats.

---

## 🧠 Overview

The goal of this stage is to:
- Read medical image data from disk (e.g. `.nii`)
- Write it back in a different format (e.g. `.nrrd`)
- Verify ITK I/O pipelines and data structures

This stage does **no processing** — it simply demonstrates robust, RAII-safe image file I/O using ITK.

---

## 🧱 Project Structure

```
itk_explorer/
├── CMakeLists.txt
├── include/
│   └── io/
│       └── ImageIO.hpp
├── src/
│   └── io/
│       ├── io_main.cpp
│       └── CMakeLists.txt
├── data/
│   └── IXI651-Guys-1118-T1.nii        # sample input (not included in repo)
├── output/                            # generated .nrrd files
└── .githooks/                         # pre-commit hook (clang-format, clang-tidy)
```

---

## ⚙️ Build Instructions

### 1. Prerequisites

```bash
sudo apt update
sudo apt install build-essential cmake libinsighttoolkit4-dev libfmt-dev
```

(If ITK is not found automatically, set `ITK_DIR=/usr/lib/cmake/ITK-5.2`.)

### 2. Configure & Build

```bash
cmake -S . -B build
cmake --build build
```

### 3. Run

```bash
./build/bin/itk_io data/IXI651-Guys-1118-T1.nii output/IXI651-Guys-1118-T1.nrrd
```

Expected output:
```
Successfully wrote image to output/IXI651-Guys-1118-T1.nrrd
```

---

## 🧩 Notes

- `ImageIO.hpp` is a templated helper class using ITK’s `ImageFileReader` and `ImageFileWriter`.
- Error handling uses `fmt::format` (or `std::format` if available).
- All resources follow RAII principles.
- The `.githooks/pre-commit` runs `clang-format` and `clang-tidy` automatically.

---

## 📦 Next Stage (Coming Up)

**Stage 2: Filtering** — Apply basic ITK filters (Gaussian smoothing, gradient magnitude)
to demonstrate ITK’s pipeline and processing modules.

---


