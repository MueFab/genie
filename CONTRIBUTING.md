# Contributing to Genie

Thank you for your interest in contributing to Genie — the first open-source ISO/IEC MPEG-G encoder for genomic data. Whether you're fixing bugs, improving documentation, or implementing new features, your help is welcome!

---

## 📦 Getting Started

1. **Fork** the repo on GitHub.
2. **Clone** your fork locally:

```bash
git clone https://github.com/YOUR_USERNAME/genie.git
cd genie
git checkout develop
```

Please base all your changes on the `develop` branch. Pull requests to the `main` branch will be rejected except if for release purposes.

---

## 🧱 Building Genie Locally (Anaconda-based)

We recommend building Genie inside a dedicated Anaconda environment for dependency isolation.

### ✅ Step-by-step Instructions

1. **Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html)** if you haven't already.

2. **Run the provided install script** to set up the environment and install dependencies:

```bash
bash install.sh
```

This script will:
- Create a `genie-env` conda environment
- Install required build tools and libraries
- Clone and build `libbsc` and `HTSlib`
- Clone the Genie repository

3. **Activate your environment and set environment variables**:

```bash
conda activate genie-env
export CXX=x86_64-conda-linux-gnu-c++
export CC=x86_64-conda-linux-gnu-cc
```

4. **Build Genie using the helper script**:

```bash
cd genie
bash ci/build.sh release   # For optimized build
# or
bash ci/build.sh debug     # For development with tests and docs
# or
bash ci/build.sh doc       # To build API documentation (requires Doxygen)
```

The compiled binary will be located inside the `cmake-build-*` directory.

---

## 🧱 Building Genie Using System Libraries (Bare Metal)

If you prefer not to use Docker or Conda, you can build Genie using system-installed dependencies. This method gives you full control over your environment and may better integrate with existing toolchains — but requires manual setup.

### ✅ System Requirements

- Linux (Ubuntu/Fedora) or WSL2 (on Windows)
- C++17-compliant compiler (GCC 7.5+, Clang 6+)
- CMake ≥ 3.20
- `make` and other build essentials

### 📦 Required Libraries

Install the following development packages using your system’s package manager:

<details>
<summary><strong>Ubuntu (apt)</strong></summary>

```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    libomp-dev \
    libhts-dev \
    liblzma-dev \
    libzstd-dev \
    zlib1g-dev \
    libbz2-dev \
    curl \
    git
```

</details>

<details>
<summary><strong>Fedora (dnf)</strong></summary>

```bash
sudo dnf install -y \
    gcc \
    gcc-c++ \
    make \
    cmake \
    libomp-devel \
    htslib-devel \
    xz-devel \
    zstd-devel \
    zlib-devel \
    bzip2-devel \
    curl \
    git
```

</details>

> ⚠️ **libbsc is not available via package managers**. You must build and install it manually.

### 🧰 Installing libbsc (Manual Step)

```bash
git clone https://github.com/IlyaGrebnov/libbsc.git
cd libbsc
git checkout tags/v3.3.6
make -j
sudo make install
```

This installs `libbsc.a` and its headers to system-wide directories.

---

### 🏗️ Building Genie

1. **Clone the Genie repository**:

```bash
git clone https://github.com/MueFab/genie.git
cd genie
```

2. **Configure and build using CMake**:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
make -j
```

3. **Run Genie**:

```bash
./genie run -h
```

---

## ✅ Verifying the Build

After a successful build, test Genie with the example commands in the [README](./README.md#verifying).

---

## 🛠️ Code Style Guidelines

- Follow **C++17** and [Google style guide](https://google.github.io/styleguide/cppguide.html) conventions.
- Use clear and descriptive naming.
- Keep functions modular and focused.
- Use inline comments to explain non-obvious logic.

We recommend using `clang-format` for consistent formatting.

---

## 🔄 Submitting Changes

1. Create a feature branch:

```bash
git checkout -b feature/my-awesome-feature
```

2. Make your changes and commit with a clear message:

```bash
git commit -m "Add support for XYZ"
```

3. Push and open a pull request against the `develop` branch:

```bash
git push origin feature/my-awesome-feature
```

Make sure to:
- Describe your changes clearly.
- Link related issues if applicable.
- Pass CI checks before requesting a review.

---

## 🐞 Reporting Bugs / Requesting Features

Please open a [GitHub issue](https://github.com/MueFab/genie/issues) with the following info:

- Clear summary
- Steps to reproduce (for bugs)
- Suggested behavior or use case (for features)

---

## 🤝 Code of Conduct

This project adopts the [Contributor Covenant](https://www.contributor-covenant.org/). Be respectful, inclusive, and constructive.

---

## 👨‍🔬 Contact

For technical or research-related inquiries:

- **Fabian Müntefering** – [muenteferi@tnt.uni-hannover.de](mailto:muenteferi@tnt.uni-hannover.de)
- **Mikel Hernaez** – [mhernaez@unav.es](mailto:mhernaez@unav.es)
- **Jan Voges** – [voges@tnt.uni-hannover.de](mailto:voges@tnt.uni-hannover.de)

---

Thank you for helping make Genie a reliable and efficient tool for genomic data compression!
