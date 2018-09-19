#!/usr/bin/env bash


# -----------------------------------------------------------------------------
# Constants
# -----------------------------------------------------------------------------

readonly SCRIPT_NAME="$(basename $0)"


# -----------------------------------------------------------------------------
# Parse command line.
# -----------------------------------------------------------------------------

if [ "$#" -ne 0 ]; then
    printf "usage: $0\n";
    exit -1;
fi


# -----------------------------------------------------------------------------
# Check if required executables are available.
# -----------------------------------------------------------------------------

# !!!!!!!!!!!!!!! YOUR CUSTOM PATHS MIGHT BE REQUIRED HERE !!!!!!!!!!!!!!!!!!!!
readonly cut="/usr/bin/cut"
readonly cmake="/Applications/CMake.app/Contents/bin/cmake"
readonly git="/usr/bin/git"
readonly make="/usr/bin/make"
readonly mkdir="/bin/mkdir"
# !!!!!!!!!!!!!!! YOUR CUSTOM PATHS MIGHT BE REQUIRED HERE !!!!!!!!!!!!!!!!!!!!

if [ ! -x $cmake ]; then
    printf "$SCRIPT_NAME: binary file '$cmake' is not executable\n"
    exit -1
fi

if [ ! -x $cut ]; then
    printf "$SCRIPT_NAME: binary file '$cut' is not executable\n"
    exit -1
fi

if [ ! -x $git ]; then
    printf "$SCRIPT_NAME: binary file '$git' is not executable\n"
    exit -1
fi

if [ ! -x $make ]; then
    printf "$SCRIPT_NAME: binary file '$make' is not executable\n"
    exit -1
fi

if [ ! -x $mkdir ]; then
    printf "$SCRIPT_NAME: binary file '$mkdir' is not executable\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# Functions
# -----------------------------------------------------------------------------

safe_execute ()
{
    local readonly command=("$@")
    "${command[@]}"
    # "${command[@]}" &> /dev/null
    local readonly return_code=$?
    if [ $return_code -ne 0 ]; then
        cmd="${command[@]}"
        printf "$SCRIPT_NAME: error: '$cmd' returned with non-zero status\n"
        exit -1
    fi
}


# -----------------------------------------------------------------------------
# Do the work.
# -----------------------------------------------------------------------------

# Try to get the Git root directory.
command=( $git rev-parse --show-toplevel )
safe_execute "${command[@]}" &> /dev/null
readonly git_root_dir=$($git rev-parse --show-toplevel ) # It worked once. ;)

# Check if git_root_dir was populated by the Git call above.
if [ -z "$git_root_dir" ]; then
    printf "$SCRIPT_NAME: error: failed to get Git root directory\n"
    exit -1
fi

# Also check if the 1st character returned by 'git rev-parse --show-toplevel'
# above is a '/' (slash)).
this_should_be_a_slash=$(printf '%s' "$git_root_dir" | $cut -c1)
if [ ! "$this_should_be_a_slash" = "/" ]; then
    printf "$SCRIPT_NAME: error: failed to get Git root directory\n"
    exit -1
fi

# Check if the build directory exists - if not, create it.
readonly build_dir="$git_root_dir/build/"
if [ ! -d $build_dir ]; then
    printf "$SCRIPT_NAME: creating build directory: $build_dir\n"
    command=( $mkdir -p $build_dir )
    safe_execute "${command[@]}"
fi
command=( cd "$build_dir" )
safe_execute "${command[@]}"

# Run CMake.
command=( $cmake -G "Unix Makefiles" .. )
safe_execute "${command[@]}"

# Run Make.
command=( $make )
safe_execute "${command[@]}"

# We're done.
printf "$SCRIPT_NAME: success\n"
exit 0
