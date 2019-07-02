#!/usr/bin/env bash

# Get the Git root directory
readonly git_root_dir="$(git rev-parse --show-toplevel)"

# We plan to use the 'tmp' directory for our tests - check if it already
# exists; otherwise create it and proceed
readonly tmp_dir="$git_root_dir/tmp"
if [ -d "$tmp_dir" ]; then
    printf "Directory '%s' already exists; aborting roundtrip tests\\n" "$tmp_dir"
    exit -1
fi
printf "Creating 'tmp' directory: %s\\n" "$tmp_dir"
mkdir -p "$tmp_dir" || exit -1

# Get the gabacify executable
readonly gabacify="$git_root_dir/build/bin/gabacify"
if [ ! -x "$gabacify" ]; then
    printf "gabacify '%s' is not executable; aborting roundtrip tests\\n" "$gabacify"
    exit -1
fi

# Gather the test files
input_files=()
configuration_files=()
bytestream_files=()
uncompressed_files=()
#
input_files+=("$git_root_dir/resources/input_files/one_million_zero_bytes")
configuration_files+=("$git_root_dir/resources/configuration_files/equality_coding.json")
bytestream_files+=("$git_root_dir/tmp/one_million_zero_bytes.gabac_bytestream")
uncompressed_files+=("$git_root_dir/tmp/one_million_zero_bytes.gabac_uncompressed")
#
input_files+=("$git_root_dir/resources/input_files/one_million_zero_bytes")
configuration_files+=("$git_root_dir/resources/configuration_files/match_coding.json")
bytestream_files+=("$git_root_dir/tmp/one_million_zero_bytes.gabac_bytestream")
uncompressed_files+=("$git_root_dir/tmp/one_million_zero_bytes.gabac_uncompressed")
#
input_files+=("$git_root_dir/resources/input_files/one_mebibyte_random")
configuration_files+=("$git_root_dir/resources/configuration_files/rle_coding.json")
bytestream_files+=("$git_root_dir/tmp/one_mebibyte_random.gabac_bytestream")
uncompressed_files+=("$git_root_dir/tmp/one_mebibyte_random.gabac_uncompressed")

# Do the test roundtrips
for i in "${!input_files[@]}"; do
    printf "Running roundtrip %s\\n" "$i"

    # Gather the i-th files
    input_file=${input_files[$i]}
    configuration_file=${configuration_files[$i]}
    bytestream_file=${bytestream_files[$i]}
    uncompressed_file=${uncompressed_files[$i]}

    # Encode
    "$gabacify" encode \
        -l error \
        -i "$input_file" \
        -c "$configuration_file" \
        -o "$bytestream_file" || exit -1

    # Decode
    "$gabacify" decode \
        -l error \
        -i "$bytestream_file" \
        -c "$configuration_file" \
        -o "$uncompressed_file" || exit -1

    # Check
    diff "$input_file" "$uncompressed_file" || exit -1

    printf "Success\\n"
    
    rm "$bytestream_file" "$uncompressed_file"
done

# Delete the 'tmp' directory
rm -rf "$tmp_dir" || exit -1
printf "Removed 'tmp' directory: %s\\n" "$tmp_dir"
