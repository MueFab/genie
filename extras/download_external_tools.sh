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
readonly git="/usr/bin/git"
readonly mkdir="/bin/mkdir"
readonly rm="/bin/rm"
readonly unzip="/usr/bin/unzip"
# readonly wget="/usr/local/wget" # Linux
readonly wget="/usr/local/Cellar/wget/1.19.5/bin/wget" # macOS w/ Homebrew

if [ ! -x $cut ]; then
    printf "$SCRIPT_NAME: error: '$cut' is not executable\n"
    exit -1
fi

if [ ! -x $git ]; then
    printf "$SCRIPT_NAME: error: '$git' is not executable\n"
    exit -1
fi

if [ ! -x $mkdir ]; then
    printf "$SCRIPT_NAME: error: '$mkdir' is not executable\n"
    exit -1
fi

if [ ! -x $rm ]; then
    printf "$SCRIPT_NAME: error: '$rm' is not executable\n"
    exit -1
fi

if [ ! -x $unzip ]; then
    printf "$SCRIPT_NAME: error: '$unzip' is not executable\n"
    exit -1
fi

if [ ! -x $wget ]; then
    printf "$SCRIPT_NAME: error: '$wget' is not executable\n"
    exit -1
fi


# -----------------------------------------------------------------------------
# External tools URLs
# -----------------------------------------------------------------------------

readonly cppcheck_url="https://github.com/danmar/cppcheck/archive/1.85.zip"
readonly cppcheck_zip="1.85.zip"
readonly cppcheck_dir="cppcheck-1.85"

readonly cpplint_url="https://github.com/cpplint/cpplint/archive/1.3.0.zip"
readonly cpplint_zip="1.3.0.zip"
readonly cpplint_dir="cpplint-1.3.0"

readonly pycodestyle_url="https://github.com/PyCQA/pycodestyle/archive/2.4.0.zip"
readonly pycodestyle_zip="2.4.0.zip"
readonly pycodestyle_dir="pycodestyle-2.4.0"


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

# Check if the 'external' directory exists - if not, create it
readonly external_tools_dir="$git_root_dir/external_tools/"
if [ ! -d $external_tools_dir ]; then
    printf "$SCRIPT_NAME: creating external_tools directory: $external_tools_dir\n"
    command=( $mkdir -p $external_tools_dir )
    do_or_die "${command[@]}"
fi

# cd into the 'external' directory
command=( cd "$external_tools_dir" )
do_or_die "${command[@]}"

# Download and build cppcheck if it does not exist already
if [ ! -d $cppcheck_dir ]; then
    printf "$SCRIPT_NAME: downloading and building cppcheck\n"

    # Download cppcheck
    command=( $wget $cppcheck_url )
    do_or_die "${command[@]}"

    # Unzip cppcheck
    command=( $unzip $cppcheck_zip )
    do_or_die "${command[@]}"

    # Remove the cpplint_zip
    command=( $rm $cppcheck_zip )
    do_or_die "${command[@]}"

    # cd into the cppcheck directory
    command=( cd $cppcheck_dir )
    do_or_die "${command[@]}"

    # Create the 'build' directory for cppcheck
    command=( $mkdir -p build )
    do_or_die "${command[@]}"

    # cd into the 'build' directory
    command=( cd build )
    do_or_die "${command[@]}"

    # Call CMake
    command=( cmake -DCMAKE_INSTALL_PREFIX=. ..)
    do_or_die "${command[@]}"

    # Call make
    command=( make install )
    do_or_die "${command[@]}"
else
    printf "$SCRIPT_NAME: cppcheck already downloaded and built\n"
fi

# cd into the 'external' directory
command=( cd "$external_tools_dir" )
do_or_die "${command[@]}"

# Download cpplint if it does not exist already
if [ ! -d $cpplint_dir ]; then
    printf "$SCRIPT_NAME: downloading cpplint\n"

    # Download cpplint
    command=( $wget $cpplint_url )
    do_or_die "${command[@]}"

    # Unzip cpplint
    command=( $unzip $cpplint_zip )
    do_or_die "${command[@]}"

    # Remove the cpplint_zip
    command=( $rm $cpplint_zip )
    do_or_die "${command[@]}"
else
    printf "$SCRIPT_NAME: cpplint already downloaded\n"
fi

# cd into the 'external' directory
command=( cd "$external_tools_dir" )
do_or_die "${command[@]}"

# Download pycodestyle if it does not exist already
if [ ! -d $pycodestyle_dir ]; then
    printf "$SCRIPT_NAME: downloading pycodestyle\n"

    # Download cpplint
    command=( $wget $pycodestyle_url )
    do_or_die "${command[@]}"

    # Unzip cpplint
    command=( $unzip $pycodestyle_zip )
    do_or_die "${command[@]}"

    # Remove the cpplint_zip
    command=( $rm $pycodestyle_zip )
    do_or_die "${command[@]}"
else
    printf "$SCRIPT_NAME: pycodestyle already downloaded\n"
fi


# -----------------------------------------------------------------------------
# Cleanup and exit
# -----------------------------------------------------------------------------

printf "$SCRIPT_NAME: success\n"
exit 0
