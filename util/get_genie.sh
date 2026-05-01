#!/bin/bash
set -e
install_dir="$PWD/install" 
mkdir -p install

# Install htslib
git clone https://github.com/samtools/htslib.git
cd htslib
git checkout tags/1.17
git submodule update --init --recursive
autoreconf -i
./configure --prefix="$install_dir/usr"
make -j
make install
cd ..

if [ -f "$install_dir/usr/lib/libhts.so" ]; then
    HTS_SO="$install_dir/usr/lib/libhts.so"
elif [ -f "$install_dir/usr/lib64/libhts.so" ]; then
    HTS_SO="$install_dir/usr/lib64/libhts.so"
fi


#install Genie
git clone https://github.com/MueFab/genie.git
mv genie genie_repo
cd genie_repo
git checkout main
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX="$install_dir" -DHTSlib_INCLUDE_DIR="$install_dir/usr/include" -DHTSlib_LIBRARY="$HTS_SO" ..
make -j
make install
cd ..
cd ..

#link to genie executable
ln -s "$install_dir/usr/bin/genie" ./genie 

# Download example data
mkdir -p data
cd data
curl -L \
    https://seafile.cloud.uni-hannover.de/f/e32a272813834b6e888a/?dl=1 \
    --output ./example_1.fastq.gz \
    || { echo 'Could not download example fastq 1!' ; exit 1; }
gzip -df ./example_1.fastq.gz
curl -L \
    https://seafile.cloud.uni-hannover.de/f/7b5741e91b604d1bbd6c/?dl=1 \
    --output ./example_2.fastq.gz \
    || { echo 'Could not download example fastq 2!' ; exit 1; }
gzip -df ./example_2.fastq.gz
curl -L \
    https://seafile.cloud.uni-hannover.de/f/ed4f5f9570d14c9b9d24/?dl=1  \
    --output ./example.sam.gz \
    || { echo 'Could not download example sam file!' ; exit 1; }
gzip -df ./example.sam.gz
curl -L \
    https://seafile.cloud.uni-hannover.de/f/99a9c09bd5664cb8a785/?dl=1  \
    --output ./example.fa.gz \
    || { echo 'Could not download example fa file!' ; exit 1; }
gzip -df ./example.fa.gz

mkdir transcoded_mgrec
mkdir encoded
mkdir decoded
mkdir transcoded_legacy
