# ITK Explorer — Stage 5: Visualization (Headless)

---

## 🧠 Goal
Generate lightweight, headless visual outputs from ITK pipelines.

---

## ⚙️ Outputs
| File | Description |
|------|--------------|
| `exported_volume.nrrd` | Full 3D image (for Slicer/ParaView) |
| `exported_slice.png`   | Middle-slice preview (quick check) |

---

## ⚙️ Usage
```bash
./build/bin/itk_visualize input_image.nrrd
