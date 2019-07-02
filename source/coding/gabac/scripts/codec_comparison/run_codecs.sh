#!/usr/bin/env bash


# -----------------------------------------------------------------------------
# Parse command line
# -----------------------------------------------------------------------------

if [ "$#" -ne 1 ]; then
    printf "Usage: %s <test_file>\\n" "$0"
    exit 1
fi

readonly test_file="$1"
if [ ! -f "$test_file" ]; then
    printf "Error: '%s' is not a regular file\\n" "$test_file"
    exit 1
fi


# -----------------------------------------------------------------------------
# Check if required executables are available
# -----------------------------------------------------------------------------

readonly gzip="/usr/bin/gzip"
readonly bzip2="/usr/bin/bzip2"
readonly gabacify="/Users/janvoges/Repositories/gabac-closed/build/gabacify"
readonly xz="/usr/local/Cellar/xz/5.2.4/bin/xz"
readonly rans="/Users/janvoges/Repositories/rans/build/rans"
# readonly time="/usr/bin/time" # Linux
readonly time="/usr/local/Cellar/gnu-time/1.9/bin/gtime" # macOS w/ Homebrew

if [ ! -x "$gzip" ]; then
    printf "Error: '%s' is not executable\\n" "$gzip"
    exit 1
fi

if [ ! -x "$bzip2" ]; then
    printf "Error: '%s' is not executable\\n" "$bzip2"
    exit 1
fi

if [ ! -x "$gabacify" ]; then
    printf "Error: '%s' is not executable\\n" "$gabacify"
    exit 1
fi

if [ ! -x "$xz" ]; then
    printf "Error: '%s' is not executable\\n" "$xz"
    exit 1
fi

if [ ! -x "$rans" ]; then
    printf "Error: '%s' is not executable\\n" "$rans"
    exit 1
fi

if [ ! -x "$time" ]; then
    printf "Error: '%s' is not executable\\n" "$time"
    exit 1
fi


# -----------------------------------------------------------------------------
# Set up of log file
# -----------------------------------------------------------------------------

readonly log_file="$test_file.codec_stats.txt"
printf "Writing statistics to: %s\\n" "$log_file"
if [ -f "$log_file" ]; then
    printf "Error: '%s' already exists\\n" "$log_file"
    exit 1
fi
{ \
    printf "        codec  "; \
    printf "uncompressed_size  "; \
    printf "compressed_size  "; \
    printf "compression_time_(s)  "; \
    printf "compression_max_RSS_(kB)  "; \
    printf "decompression_time_(s)  "; \
    printf "decompression_max_RSS_(kB)  "; \
    printf "\\n"; \
} >>"$log_file"


# -----------------------------------------------------------------------------
# GNU time format
# -----------------------------------------------------------------------------

# The GNU time format:
#   %e      Elapsed real time in seconds
#   %M      Maximum resident set size of the process during its lifetime, in
#           kilobytes
readonly time_format="%e %M"


# -----------------------------------------------------------------------------
# Codecs and commands
# -----------------------------------------------------------------------------

codecs=()
codecs+=("gzip")
codecs+=("bzip2")
codecs+=("xz")
codecs+=("rans0")
codecs+=("rans1")
codecs+=("gabac-a")
codecs+=("gabac")
#
compression_commands=()
compression_commands+=("$time --format=\"$time_format\" -o $test_file.gzip.enc.time $gzip -9 -c $test_file >$test_file.gzip")
compression_commands+=("$time --format=\"$time_format\" -o $test_file.bzip2.enc.time $bzip2 -9 -c $test_file >$test_file.bzip2")
compression_commands+=("$time --format=\"$time_format\" -o $test_file.xz.enc.time $xz -9 -c $test_file >$test_file.xz")
compression_commands+=("$time --format=\"$time_format\" -o $test_file.rans0.enc.time $rans -o0 < $test_file >$test_file.rans0")
compression_commands+=("$time --format=\"$time_format\" -o $test_file.rans1.enc.time $rans -o1 < $test_file >$test_file.rans1")
compression_commands+=("$time --format=\"$time_format\" -o $test_file.gabac-a.enc.time $gabacify encode -i $test_file -o $test_file.gabac-a 1>$test_file.gabac-a.enc.stdout 2>$test_file.gabac-a.enc.stderr") # Will generate $test_file.gabac_configuration.json
compression_commands+=("$time --format=\"$time_format\" -o $test_file.gabac.enc.time $gabacify encode -i $test_file -c $test_file.gabac_configuration.json -o $test_file.gabac 1>$test_file.gabac.enc.stdout 2>$test_file.gabac.enc.stderr")
#
decompression_commands=()
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.gzip.dec.time $gzip -d -c $test_file.gzip >$test_file.gzip.recon")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.bzip2.dec.time $bzip2 -d -c $test_file.bzip2 >$test_file.bzip2.recon")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.xz.dec.time $xz -d -c $test_file.xz >$test_file.xz.recon")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.rans0.dec.time $rans -d < $test_file.rans0 >$test_file.rans0.recon")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.rans1.dec.time $rans -d < $test_file.rans1 >$test_file.rans1.recon")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.gabac-a.dec.time $gabacify decode -i $test_file.gabac-a -c $test_file.gabac_configuration.json -o $test_file.gabac-a.recon 1>$test_file.gabac-a.dec.stdout 2>$test_file.gabac-a.dec.stderr")
decompression_commands+=("$time --format=\"$time_format\" -o $test_file.gabac.dec.time $gabacify decode -i $test_file.gabac -c $test_file.gabac_configuration.json -o $test_file.gabac.recon 1>$test_file.gabac.dec.stdout 2>$test_file.gabac.dec.stderr")


# -----------------------------------------------------------------------------
# Do the work
# -----------------------------------------------------------------------------

for i in "${!codecs[@]}"; do
    codec="${codecs[$i]}"
    compression_command="${compression_commands[$i]}"
    decompression_command="${decompression_commands[$i]}"

    # Compression
    printf "Compressing with: %s\\n" "$codec"
    eval "$compression_command"

    # Decompression
    printf "Decompressing with: %s\\n" "$codec"
    eval "$decompression_command"

    # Test roundtrip
    printf "Validating roundtrip\\n"
    diff "$test_file" "$test_file.$codec.recon" || exit 1

    # Logging
    uncompressed_size=$(wc -c "$test_file" | awk '{print $1}')
    compressed_size=$(wc -c "$test_file.$codec" | awk '{print $1}')
    compression_time=$(awk '{print $1}' "$test_file.$codec.enc.time")
    compression_max_rss=$(awk '{print $2}' "$test_file.$codec.enc.time")
    decompression_time=$(awk '{print $1}' "$test_file.$codec.dec.time")
    decompression_max_rss=$(awk '{print $2}' "$test_file.$codec.dec.time")
    { \
        printf "%13s" "$codec"; \
        printf "%19s" "$uncompressed_size"; \
        printf "%17s" "$compressed_size"; \
        printf "%22s" "$compression_time"; \
        printf "%26s" "$compression_max_rss"; \
        printf "%24s" "$decompression_time"; \
        printf "%28s" "$decompression_max_rss"; \
        printf "\\n"; \
    } >>"$log_file"

    # Cleanup
    rm "$test_file.$codec"
    rm "$test_file.$codec.recon"
    rm "$test_file.$codec.enc.time"
    rm "$test_file.$codec.dec.time"
done
