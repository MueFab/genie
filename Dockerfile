# Start from the Alpine Linux base image
FROM alpine:latest

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
    libtool \
    libgomp

# Install htslib from source
RUN git clone https://github.com/samtools/htslib.git \
    && cd htslib \
    && git submodule update --init --recursive \
    && autoreconf -i \
    && ./configure \
    && make -j \
    && make install \
    && cd .. \
    && rm -rf ./htslib

# Create the /genie directory
RUN mkdir /genie

# Copy the CMake project files into /genie
COPY ./src /genie/src
COPY ./CMakeLists.txt /genie/CMakeLists.txt
COPY ./cmake /genie/cmake
COPY ./thirdparty /genie/thirdparty

# Enter the /genie directory and build the project with CMake
RUN cd /genie \
    && cmake . \
    && make -j && make install

# Clean up unnecessary files
RUN apk del git curl cmake autoconf automake
RUN rm -rf /genie

RUN mkdir /work

WORKDIR /work

# Default command: start an interactive shell
CMD ["/bin/sh"]

