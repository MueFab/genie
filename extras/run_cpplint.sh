#!/usr/bin/env bash


# -----------------------------------------------------------------------------
# Constants
# -----------------------------------------------------------------------------

readonly SCRIPT_NAME="$(basename $0)"


# -----------------------------------------------------------------------------
# Functions
# -----------------------------------------------------------------------------

do_or_die ()
{
    local readonly command=("$@")
    "${command[@]}" #&> /dev/null
    local readonly return_code=$?
    if [ $return_code -ne 0 ]; then
        cmd="${command[@]}"
        printf "$SCRIPT_NAME: error: '$cmd' returned with non-zero status\n"
        exit -1
    fi
}


# -----------------------------------------------------------------------------
# Parse command line
# -----------------------------------------------------------------------------

if [ "$#" -ne 0 ]; then
    printf "$SCRIPT_NAME: usage: $0\n";
    exit -1;
fi


# -----------------------------------------------------------------------------
# Check if required executables are available
# -----------------------------------------------------------------------------

readonly cut="/usr/bin/cut"
readonly git="/usr/bin/git"
readonly python="/usr/bin/python"

if [ ! -x $cut ]; then
    printf "$SCRIPT_NAME: error: '$cut' is not executable\n"
    exit -1
fi

if [ ! -x $git ]; then
    printf "$SCRIPT_NAME: error: '$git' is not executable\n"
    exit -1
fi

if [ ! -x $python ]; then
    printf "$SCRIPT_NAME: error: '$python' is not executable\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# Get Git root directory
# -----------------------------------------------------------------------------

# Try to get the Git root directory
command=( $git rev-parse --show-toplevel )
printf "$SCRIPT_NAME: Git root directory: "
do_or_die "${command[@]}"
readonly git_root_dir=$($git rev-parse --show-toplevel )

# Check if git_root_dir was populated by the Git call above
if [ -z "$git_root_dir" ]; then
    printf "$SCRIPT_NAME: error: failed to get Git root directory\n"
    exit -1
fi

# Also check if the 1st character returned by 'git rev-parse --show-toplevel'
# above is a '/' (slash))
this_should_be_a_slash=$(printf '%s' "$git_root_dir" | $cut -c1)
if [ ! "$this_should_be_a_slash" = "/" ]; then
    printf "$SCRIPT_NAME: error: failed to get Git root directory\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# Check if required external tools are available
# -----------------------------------------------------------------------------

readonly cpplint_py="$git_root_dir/external_tools/cpplint-1.3.0/cpplint.py"

if [ ! -f $cpplint_py ]; then
    printf "$SCRIPT_NAME: error: '$cpplint_py' is not a regular file\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# Check if required folders and files are available
# -----------------------------------------------------------------------------

source_dir="$git_root_dir/source"

if [ ! -d $source_dir ]; then
    printf "$SCRIPT_NAME: error: '$source_dir' is not a directory\n"
    exit -1
fi

cpplint_cfg="$git_root_dir/source/CPPLINT.cfg"

if [ ! -f $cpplint_cfg ]; then
    printf "$SCRIPT_NAME: error: '$cpplint_cfg' is not a regular file\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# List all files to be checked
# -----------------------------------------------------------------------------

files=()

# gabac
files+=("$source_dir/gabac/analysis.cpp")
files+=("$source_dir/gabac/analysis.h")
files+=("$source_dir/gabac/binary_arithmetic_decoder.cpp")
files+=("$source_dir/gabac/binary_arithmetic_decoder.h")
files+=("$source_dir/gabac/binary_arithmetic_encoder.cpp")
files+=("$source_dir/gabac/binary_arithmetic_encoder.h")
files+=("$source_dir/gabac/bit_input_stream.cpp")
files+=("$source_dir/gabac/bit_input_stream.h")
files+=("$source_dir/gabac/bit_output_stream.cpp")
files+=("$source_dir/gabac/bit_output_stream.h")
files+=("$source_dir/gabac/cabac_tables.h")
files+=("$source_dir/gabac/configuration.cpp")
files+=("$source_dir/gabac/configuration.h")
files+=("$source_dir/gabac/context_model.cpp")
files+=("$source_dir/gabac/context_model.h")
files+=("$source_dir/gabac/context_selector.cpp")
files+=("$source_dir/gabac/context_selector.h")
files+=("$source_dir/gabac/context_tables.cpp")
files+=("$source_dir/gabac/context_tables.h")
files+=("$source_dir/gabac/decoding.cpp")
files+=("$source_dir/gabac/decoding.h")
files+=("$source_dir/gabac/diff_coding.cpp")
files+=("$source_dir/gabac/diff_coding.h")
files+=("$source_dir/gabac/encoding.cpp")
files+=("$source_dir/gabac/encoding.h")
files+=("$source_dir/gabac/equality_coding.cpp")
files+=("$source_dir/gabac/equality_coding.h")
files+=("$source_dir/gabac/exceptions.cpp")
files+=("$source_dir/gabac/exceptions.h")
files+=("$source_dir/gabac/lut_transform.cpp")
files+=("$source_dir/gabac/lut_transform.h")
files+=("$source_dir/gabac/match_coding.cpp")
files+=("$source_dir/gabac/match_coding.h")
files+=("$source_dir/gabac/reader.cpp")
files+=("$source_dir/gabac/reader.h")
files+=("$source_dir/gabac/release.cpp")
files+=("$source_dir/gabac/release.h")
files+=("$source_dir/gabac/return_codes.h")
files+=("$source_dir/gabac/rle_coding.cpp")
files+=("$source_dir/gabac/rle_coding.h")
files+=("$source_dir/gabac/writer.cpp")
files+=("$source_dir/gabac/writer.h")

# gabacify
files+=("$source_dir/gabacify/encode.cpp")
files+=("$source_dir/gabacify/encode.h")
files+=("$source_dir/gabacify/exceptions.cpp")
files+=("$source_dir/gabacify/exceptions.h")
files+=("$source_dir/gabacify/file.cpp")
files+=("$source_dir/gabacify/file.h")
files+=("$source_dir/gabacify/input_file.cpp")
files+=("$source_dir/gabacify/input_file.h")
files+=("$source_dir/gabacify/log.cpp")
files+=("$source_dir/gabacify/log.h")
files+=("$source_dir/gabacify/main.cpp")
files+=("$source_dir/gabacify/output_file.cpp")
files+=("$source_dir/gabacify/output_file.h")
files+=("$source_dir/gabacify/program_options.cpp")
files+=("$source_dir/gabacify/program_options.h")


# -----------------------------------------------------------------------------
# Do the work
# -----------------------------------------------------------------------------

for file in "${files[@]}"; do
    printf "$SCRIPT_NAME: running cpplint on: $file\n"

    if [ ! -f $file ]; then
        printf "$SCRIPT_NAME: error: '$file' is not a regular file\n"
        exit -1
    fi

    $python $cpplint_py $file

    if [ $? -ne 0 ]; then
        printf "$SCRIPT_NAME: cpplint returned with non-zero status\n"
        printf "$SCRIPT_NAME: continuing ...\n"
        # exit -1
    fi
done


# -----------------------------------------------------------------------------
# Cleanup and exit
# -----------------------------------------------------------------------------

printf "$SCRIPT_NAME: done\n"
exit 0
