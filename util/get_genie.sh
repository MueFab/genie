git clone https://github.com/samtools/htslib.git
cd htslib
git checkout tags/1.17
git submodule update --init --recursive
autoreconf -i
mkdir install
./configure –prefix=`echo $PWD`/install
make -j
make install
cd ..

#git clone https://github.com/MueFab/genie.git
#cd genie
#git checkout develop
#mkdir build
#cd build
#cmake ..
#make -j
