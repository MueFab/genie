#!/usr/bin/env bash


# -----------------------------------------------------------------------------
# Get the Git root directory
# -----------------------------------------------------------------------------

readonly git_root_dir="$(git rev-parse --show-toplevel)"


# -----------------------------------------------------------------------------
# Gather all files to be checked
# -----------------------------------------------------------------------------

# Gather all C++ files
cpp_files=()
cpp_files+=("$git_root_dir"/source/gabac/*.cpp)
cpp_files+=("$git_root_dir"/source/gabac/*.h)
cpp_files+=("$git_root_dir"/source/gabacify/*.cpp)
cpp_files+=("$git_root_dir"/source/gabacify/*.h)
cpp_files+=("$git_root_dir"/tests/gabac/*.cpp)
cpp_files+=("$git_root_dir"/tests/gabac/*.h)

# Check if all C++ files exist
for cpp_file in "${cpp_files[@]}"; do
    if [ ! -f "$cpp_file" ]; then
        printf "Error: '%s' is not a regular file\\n" "$cpp_file"
        exit 1
    fi
done

# Gather all Python files
python_files=()
python_files+=("$git_root_dir"/source/python_api/*.py)
python_files+=("$git_root_dir"/tests/python_api/*.py)

# Check if all Python files exist
for python_file in "${python_files[@]}"; do
    if [ ! -f "$python_file" ]; then
        printf "Error: '%s' is not a regular file\\n" "$python_file"
        exit 1
    fi
done

# Gather all Bash files
bash_files=()
bash_files+=("$git_root_dir"/ci/*.sh)
bash_files+=("$git_root_dir"/scripts/*.sh)

# Check if all Bash files exist
for bash_file in "${bash_files[@]}"; do
    if [ ! -f "$bash_file" ]; then
        printf "Error: '%s' is not a regular file\\n" "$bash_file"
        exit 1
    fi
done


# -----------------------------------------------------------------------------
# Check if cppcheck, cpplint.py and pycodestyle.py are available
# -----------------------------------------------------------------------------

# cppcheck
readonly cppcheck="$git_root_dir/external_tools/cppcheck-1.85/build/bin/cppcheck"
if [ ! -f "$cppcheck" ]; then
    printf "Error: '%s' is not a regular file\\n" "$cppcheck"
    exit 1
fi

# cpplint.py
readonly cpplint_py="$git_root_dir/external_tools/cpplint-1.3.0/cpplint.py"
if [ ! -f "$cpplint_py" ]; then
    printf "Error: '%s' is not a regular file\\n" "$cpplint_py"
    exit 1
fi

# pycodestyle.py
readonly pycodestyle_py="$git_root_dir/external_tools/pycodestyle-2.4.0/pycodestyle.py"
if [ ! -f "$pycodestyle_py" ]; then
    printf "Error: '%s' is not a regular file\\n" "$pycodestyle_py"
    exit 1
fi


# -----------------------------------------------------------------------------
# cppcheck
# -----------------------------------------------------------------------------

# Do the work
for cpp_file in "${cpp_files[@]}"; do
    printf "Running cppcheck on: %s\\n" "$cpp_file"
    "$cppcheck" \
        --enable=all \
        --inconclusive \
        --language=c++ \
        --std=posix \
        --suppress=missingIncludeSystem \
        --suppress=unusedFunction \
        --suppress=unmatchedSuppression \
        -I "$git_root_dir"/source \
        "$cpp_file"
done


# -----------------------------------------------------------------------------
# cpplint
# -----------------------------------------------------------------------------

# Check for CPPLINT.cfg
cpplint_cfg="$git_root_dir/source/CPPLINT.cfg"
if [ ! -f "$cpplint_cfg" ]; then
    printf "Error: '%s' is not a regular file\\n" "$cpplint_cfg"
    exit 1
fi

# Do the work
for cpp_file in "${cpp_files[@]}"; do
    printf "Running cpplint on: %s\\n" "$cpp_file"
    python "$cpplint_py" "$cpp_file"
done


# -----------------------------------------------------------------------------
# pycodestyle
# -----------------------------------------------------------------------------

# Do the work
for python_file in "${python_files[@]}"; do
    printf "Running pycodestyle on: %s\\n" "$python_file"
    python "$pycodestyle_py" "$python_file"
done


# -----------------------------------------------------------------------------
# /shellcheck
# -----------------------------------------------------------------------------

# Do the work
for bash_file in "${bash_files[@]}"; do
    printf "Running shellcheck on: %s\\n" "$bash_file"
    shellcheck "$bash_file"
done
