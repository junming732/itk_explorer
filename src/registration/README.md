# ITK Explorer — Stage 6–7: Registration

## Overview
This module adds affine, B-spline (deformable), and batch registration to align 3D medical images using ITK 5.2.

| Executable | Description |
|-------------|-------------|
| itk_register | Affine registration |
| itk_bspline_register | Nonlinear B-spline registration |
| itk_batch_register | Batch registration across subjects |

## Build
```bash
cmake -S . -B build
cmake --build build --target itk_register itk_bspline_register itk_batch_register -j
```

## Run
**Affine**
```bash
./build/bin/itk_register fixed.nii.gz moving.nii.gz output_affine.nrrd
```

**Deformable (B-spline)**
```bash
./build/bin/itk_bspline_register fixed.nii.gz moving.nii.gz output_bspline.nrrd 6,6,6
```

**Batch**
```bash
./build/bin/itk_batch_register fixed.nii.gz data/IXI-T1 output/batch --bspline 4,4,4
```

## Notes
- Uses Mattes Mutual Information (works for inter/intra subject)
- Outputs .nrrd images viewable in 3D Slicer or ITK-SNAP
- ITK 5.2 compatible
