# Stage 8 — Evaluation Metrics

Compute quantitative metrics to evaluate registration quality.

## Build
```bash
cmake -S . -B build
cmake --build build --target itk_metrics -j
```

## Run
```bash
# Without labels
./build/bin/itk_metrics fixed.nii.gz moving.nii.gz output/registered.nrrd --csv output/metrics.csv

# With label maps (for Dice)
./build/bin/itk_metrics fixed.nii.gz moving.nii.gz output/registered.nrrd fixed_labels.nii.gz registered_labels.nii.gz --csv output/metrics.csv
```

## Metrics
- **MSE** (lower is better)
- **NCC** (Pearson correlation, higher is better, range ~[-1,1])
- **Dice** for label maps (0..1, higher is better)
