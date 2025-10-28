#!/bin/bash
# experiment_motion_and_multimodal.sh
# Motion correction + Multi-modal registration

set -e

echo "╔═══════════════════════════════════════════════════════════╗"
echo "║  Motion Correction + Multi-Modal Registration            ║"
echo "╚═══════════════════════════════════════════════════════════╝"

# Setup
RESULTS="experiment_results"
mkdir -p "$RESULTS"

T1="$HOME/IXI-T1-images/IXI002-Guys-0828-T1.nii.gz"
T2="$HOME/IXI-T2-images/IXI002-Guys-0828-T2.nii.gz"
SLICE=75

# ============================================================
# PART 1: Motion Correction
# ============================================================
echo ""
echo "=== PART 1: Motion Correction ==="

echo "[1/5] Simulating motion..."
./build/bin/simulate_motion "$T1" "$RESULTS/T1_motion.nrrd" \
    2.0 -2.5 1.5 0.03 -0.02 0.01

echo "[2/5] Correcting motion..."
./build/bin/itk_multimodal_register \
    "$T1" "$RESULTS/T1_motion.nrrd" "$RESULTS/T1_corrected.nrrd" \
    --mode mono --verbose | tee "$RESULTS/motion_log.txt"

echo "[3/5] Extracting motion slices..."
./build/bin/itk_extract_slice "$T1" "$RESULTS/1_original.png" 2 $SLICE
./build/bin/itk_extract_slice "$RESULTS/T1_motion.nrrd" "$RESULTS/2_corrupted.png" 2 $SLICE
./build/bin/itk_extract_slice "$RESULTS/T1_corrected.nrrd" "$RESULTS/3_corrected.png" 2 $SLICE

# ============================================================
# PART 2: Multi-Modal Registration
# ============================================================
echo ""
echo "=== PART 2: Multi-Modal Registration ==="

echo "[4/5] Resampling T2 to T1 space (identity)..."
./build/bin/resample_to_reference "$T2" "$T1" "$RESULTS/T2_identity.nrrd"

echo "[5/5] Registering T2 to T1..."
./build/bin/itk_multimodal_register \
    "$T1" "$T2" "$RESULTS/T2_registered.nrrd" \
    --mode multi --verbose | tee "$RESULTS/multi_log.txt"

echo "[6/6] Extracting multi-modal slices..."
./build/bin/itk_extract_slice "$T1" "$RESULTS/4_T1_ref.png" 2 $SLICE
./build/bin/itk_extract_slice "$RESULTS/T2_identity.nrrd" "$RESULTS/5_T2_before.png" 2 $SLICE
./build/bin/itk_extract_slice "$RESULTS/T2_registered.nrrd" "$RESULTS/6_T2_after.png" 2 $SLICE

# ============================================================
# Summary
# ============================================================
MOTION_METRIC=$(grep "Final metric" "$RESULTS/motion_log.txt" | awk '{print $4}')
MULTI_METRIC=$(grep "Final metric" "$RESULTS/multi_log.txt" | awk '{print $4}')

echo ""
echo "╔═══════════════════════════════════════════════════════════╗"
echo "║                    RESULTS                                ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""
echo "Motion Correction:"
echo "  Final Metric: $MOTION_METRIC"
echo "  Images: 1_original → 2_corrupted → 3_corrected"
echo ""
echo "Multi-Modal Registration:"
echo "  Final Metric: $MULTI_METRIC"
echo "  Images: 4_T1_ref, 5_T2_before → 6_T2_after"
echo ""
echo "All results in: $RESULTS/"