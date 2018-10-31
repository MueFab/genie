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
    printf "usage: $0\n";
    exit -1;
fi


# -----------------------------------------------------------------------------
# Check if required executables are available
# -----------------------------------------------------------------------------

readonly cut="/usr/bin/cut"
# readonly cmake="/usr/bin/cmake" # Linux
readonly cmake="/usr/local/Cellar/cmake/3.12.3/bin/cmake" # macOS w/ Homebrew
readonly git="/usr/bin/git"
readonly make="/usr/bin/make"
readonly mkdir="/bin/mkdir"

if [ ! -x $cmake ]; then
    printf "$SCRIPT_NAME: error: '$cmake' is not executable\n"
    exit -1
fi

if [ ! -x $cut ]; then
    printf "$SCRIPT_NAME: error: '$cut' is not executable\n"
    exit -1
fi

if [ ! -x $git ]; then
    printf "$SCRIPT_NAME: error: '$git' is not executable\n"
    exit -1
fi

if [ ! -x $make ]; then
    printf "$SCRIPT_NAME: error: '$make' is not executable\n"
    exit -1
fi

if [ ! -x $mkdir ]; then
    printf "$SCRIPT_NAME: error: '$mkdir' is not executable\n"
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
# Do the work
# -----------------------------------------------------------------------------

# Check if the 'build' directory exists - if not, create it
readonly build_dir="$git_root_dir/build/"
if [ ! -d $build_dir ]; then
    printf "$SCRIPT_NAME: creating build directory: $build_dir\n"
    command=( $mkdir -p $build_dir )
    do_or_die "${command[@]}"
fi

# cd into the build directory
command=( cd "$build_dir" )
do_or_die "${command[@]}"

# Run CMake
command=( $cmake -G "Unix Makefiles" .. )
do_or_die "${command[@]}"

# Run Make to build genie
command=( $make )
do_or_die "${command[@]}"


# -----------------------------------------------------------------------------
# Cleanup and exit
# -----------------------------------------------------------------------------

printf "$SCRIPT_NAME: success\n"
exit 0
