# How to build & run:
# 1. docker build -t genie .
# 2. docker run --rm -v /hostfs/work:/work genie run -i input.bam -o output.mgb

# Start from the Alpine Linux base image
FROM alpine:3.20 AS builder

# Install necessary packages: build dependencies and tools
RUN apk add --no-cache \
    build-base \
    cmake \
    curl \
    git \
    tar \
    xz \
    xz-dev \
    bzip2-dev \
    zlib-dev \
    ncurses-dev \
    libbz2 \
    autoconf \
    automake \
    libtool  \
    zstd-dev

RUN git clone https://github.com/samtools/htslib.git \
    && cd htslib \
    && git checkout tags/1.20 \
    && git submodule update --init --recursive \
    && autoreconf -i \
    && ./configure \
    && make -j \
    && make install \
    && cd .. \
    && rm -rf ./htslib \
    && mkdir /genie

# Install and build libbsc
RUN git clone https://github.com/IlyaGrebnov/libbsc.git \
    && cd libbsc \
    && sed -i 's/clang++/g++/g' makefile \
    && if [ "$(uname -m)" != "x86_64" ]; then sed -i '/CFLAGS += -mavx2/s/^/#/' makefile; fi \
    && make -j \
    && make install \
    && cd .. \
    && rm -rf ./libbsc

# Copy the CMake project files into /genie
COPY ./src /genie/src
COPY ./CMakeLists.txt /genie/CMakeLists.txt
COPY ./cmake /genie/cmake
COPY ./thirdparty /genie/thirdparty

# Enter the /genie directory and build the project with CMake
RUN cd /genie \
    && cmake . \
    && make -j \
    && make install \
    && cd .. \
    && rm -rf /genie

FROM alpine:3.20

RUN apk add --no-cache \
    build-base \
    xz \
    libbz2
    

COPY --from=builder /usr/local/bin/genie /usr/local/bin/genie
COPY --from=builder /usr/local/lib/libhts.so.3 /usr/local/lib/libhts.so.3

RUN mkdir /work

WORKDIR /work

# Default command: genie
ENTRYPOINT ["/usr/local/bin/genie"]
