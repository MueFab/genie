#!/bin/bash
# set -e # Removed to allow full matrix reporting

# Project Backend Isolation Verification Matrix
# Automates the verification of Contact, Genotype, and Likelihood modules 
# across all three backends (std, eigen, xtensor).

BACKENDS=("std" "eigen" "xtensor")
MODULES=("contact" "genotype" "likelihood")

# Override with arguments if provided
if [ "$#" -ge 1 ]; then
    MODULES=("$1")
fi
if [ "$#" -ge 2 ]; then
    BACKENDS=("$2")
fi

# Root directory of the project
ROOT_DIR=$(git rev-parse --show-toplevel)
cd "$ROOT_DIR"

mkdir -p tmp/builds

echo "Starting Backend Isolation Matrix Verification..."
echo "Full logs available in tmp/builds/matrix_results.log"

# Clear previous results
> tmp/builds/matrix_results.log

# Track statuses
declare -A STATUS_MATRIX

# Concurrency control
JOBS=${JOBS:-$(nproc)}
if [ "$JOBS" -gt 8 ]; then JOBS=8; fi # Safety cap to prevent thrashing

for MOD in "${MODULES[@]}"; do
    MOD_UPPER=$(echo "$MOD" | tr '[:lower:]' '[:upper:]')
    for BACK in "${BACKENDS[@]}"; do
        BUILD_DIR="tmp/builds/build-${MOD}-${BACK}"
        echo "" >> tmp/builds/matrix_results.log
        echo "----------------------------------------------------------" >> tmp/builds/matrix_results.log
        echo "Module: ${MOD} | Backend: ${BACK}" >> tmp/builds/matrix_results.log
        echo "----------------------------------------------------------" >> tmp/builds/matrix_results.log
        
        printf "Checking %-10s | %-8s ... " "${MOD}" "${BACK}"

        # Configure
        cmake -B "$BUILD_DIR" -S . \
            -DGENIE_BACKEND=${BACK} \
            -DGENIE_${MOD_UPPER}_BACKEND=${BACK} \
            -DBUILD_TESTS=ON \
            -DGENIE_BACKEND_ISOLATION_CHECK=ON \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_POLICY_VERSION_MINIMUM=3.5 >> tmp/builds/matrix_results.log 2>&1
            
        if [ $? -ne 0 ]; then
            echo "[FAILED CONFIG]"
            STATUS_MATRIX["${MOD}_${BACK}"]="FAIL_CFG"
            continue
        fi

        # Build
        make -C "$BUILD_DIR" genie-test-${MOD} -j${JOBS} >> tmp/builds/matrix_results.log 2>&1
        
        if [ $? -ne 0 ]; then
            echo "[FAILED BUILD]"
            STATUS_MATRIX["${MOD}_${BACK}"]="FAIL_BLD"
            continue
        fi

        # Verify Linkage (nm check)
        LIB_PATH="$BUILD_DIR/lib/libgenie-${MOD}.a"
        LINK_STATUS="PASS"
        if [ "$BACK" == "std" ]; then
            if nm "$LIB_PATH" | grep -Ei "Eigen|xt::" > /dev/null; then
               LINK_STATUS="LEAK"
            fi
        fi
        
        if [ "$LINK_STATUS" == "LEAK" ]; then
            echo "[FAILED LINK-LEAK]"
            STATUS_MATRIX["${MOD}_${BACK}"]="FAIL_LNK"
            continue
        fi

        # Run Tests
        test_bin="$BUILD_DIR/bin/genie-test-${MOD}"
        if [ -f "$test_bin" ]; then
            if "$test_bin" --gtest_filter=-*GoldenMaster* >> tmp/builds/matrix_results.log 2>&1; then
                echo "[PASSED]"
                STATUS_MATRIX["${MOD}_${BACK}"]="PASSED"
            else
                echo "[FAILED TEST]"
                STATUS_MATRIX["${MOD}_${BACK}"]="FAIL_TST"
            fi
        else
            echo "[FAILED NO-BIN]"
            STATUS_MATRIX["${MOD}_${BACK}"]="FAIL_BIN"
        fi
    done
done

echo ""
echo "=========================================================="
echo "Final Matrix Summary"
echo "=========================================================="
printf "%-12s | %-10s | %-10s | %-10s\n" "Module" "STD" "EIGEN" "XTENSOR"
echo "----------------------------------------------------------"
for MOD in "${MODULES[@]}"; do
    printf "%-12s | %-10s | %-10s | %-10s\n" \
        "${MOD}" \
        "${STATUS_MATRIX[${MOD}_std]:-EMPTY}" \
        "${STATUS_MATRIX[${MOD}_eigen]:-EMPTY}" \
        "${STATUS_MATRIX[${MOD}_xtensor]:-EMPTY}"
done
echo "=========================================================="
echo ""

# Exit with error if any failed
if grep -q "FAIL" <<< "${STATUS_MATRIX[@]}"; then
    exit 1
fi
