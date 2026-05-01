#!/bin/bash
set -e

# Environment and installation variables
ENV_NAME="genie-env"
SRC_DIR="./genie_deps_src"
CUR_DIR="${PWD}"
# Source conda so that the "conda" command is available
source "$(conda info --base)/etc/profile.d/conda.sh"

# Create a new conda environment using conda-forge and bioconda channels
conda create -y -n $ENV_NAME --strict-channel-priority \
    gcc_linux-64 \
    gxx_linux-64 \
    make \
    autoconf \
    automake \
    libtool \
    pkg-config \
    zlib \
    bzip2 \
    xz \
    curl \
    openssl \
    libdeflate \
    git \
    cmake

# Activate the new environment
conda activate $ENV_NAME

export CXX=x86_64-conda-linux-gnu-c++
export CC=x86_64-conda-linux-gnu-cc

# Install libbsc
mkdir -p "$SRC_DIR"
cd "$SRC_DIR"
rm -rf libbsc
git clone https://github.com/IlyaGrebnov/libbsc.git
cd libbsc
git checkout tags/v3.3.6
sed -i 's/clang++/g++/g' makefile
sed -i 's|PREFIX = /usr|PREFIX = /|' makefile
export DESTDIR=${CONDA_PREFIX}
make -j
make install
cd "$CUR_DIR"

# Clone the HTSlib repository
git clone https://github.com/samtools/htslib.git "${SRC_DIR}/htslib"
cd "${SRC_DIR}/htslib"

git submodule update --init --recursive

# If auxiliary files are missing, download them from GNU
if [ ! -f config.guess ] || [ ! -f config.sub ]; then
    echo "Downloading missing auxiliary files..."
    wget -O config.guess 'https://git.savannah.gnu.org/cgit/config.git/plain/config.guess'
    wget -O config.sub 'https://git.savannah.gnu.org/cgit/config.git/plain/config.sub'
    chmod +x config.guess config.sub
fi

export DESTDIR=""
# Prepare the build configuration and compile HTSlib
autoheader
autoconf
./configure --prefix="${CONDA_PREFIX}"
make -j
make install
cd "$CUR_DIR"
rm -rf "$SRC_DIR"

# Clone genie
git clone https://github.com/MueFab/genie.git

# Provide instructions to update your environment variables
echo ""
echo "Please run:"
echo "      conda activate ${ENV_NAME}"
echo "      export CXX=x86_64-conda-linux-gnu-c++"
echo "      export CC=x86_64-conda-linux-gnu-cc"
