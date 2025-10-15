# ITK Explorer — Stage 4: Segmentation

---

## 🧠 Goal
Perform basic segmentation using intensity-based thresholding and connected components.

---

## ⚙️ Workflow

1. **Otsu Thresholding**
   - Automatically computes an intensity threshold.
   - Produces a binary mask separating background and foreground.

2. **Connected Components**
   - Labels contiguous regions in the binary mask.
   - Optionally relabels them by size.

---

## ⚙️ Usage

```bash
./build/bin/itk_segment input_image.nrrd output_labels.nrrd
