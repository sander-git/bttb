# Burn to the Brim (C++ Port)

**Burn to the Brim (BTTB)** is a modern C++20 desktop application designed to optimally fit files and folders onto target storage mediums (such as CDs, DVDs, or Custom sizes).

This is a complete port of the classic Delphi application to **C++20** supporting:
1. **Linux**: Natively compiled GTK 4 desktop application running on Ubuntu 24.04+.
2. **Windows**: Native, lightweight Win32 SDK GUI desktop application running natively on Windows with zero external DLL dependencies.

It solves the subset-sum / bin packing problem instantly using an advanced recursive backtracking solver, matching files efficiently, and providing file organization and ISO creation capabilities.

---

## Features

- **Backtracking Bin Packing Solver**: Employs the highly optimized subset-sum algorithm from the original BTTB version, executing in milliseconds with a fluid non-blocking background thread.
- **Directory Split Constraints**: Control directory structures by configuring a `splitDepth` waterlevel (atomic vs recursive scanning).
- **Modern Rule Groupings**: Group files/directories together on the same medium using standard wildcards (globs) or powerful regular expressions (`std::regex`).
- **File Organization**: Copy or move matched selections directly to target organizing folders automatically.
- **ISO Creation Integration**: Generate ISO 9660 / Joliet / Rock Ridge images natively via standard command line utilities (`genisoimage`/`mkisofs`) with real-time log capturing.
- **Vibrant User Interfaces**:
  - **Linux (GTK 4)**: Standard GNOME Adwaita aesthetic featuring a HeaderBar, scrolling side results tree, interactive progress tracking, and detailed colored log viewer.
  - **Windows (Win32 Native)**: Ultra-lightweight (~160KB standalone binary), statically linked, utilizing standard native Windows SDK controls, comctl32 progress elements, and Windows Shell folder selectors.

---

## Windows Installation & Running

### 1. Download and Run

#### Official release
Use the Microsoft Store to download the latest version of the application [here](https://apps.microsoft.com/detail/9NCCJ2HSFBB4?hl=en-us&gl=NL&ocid=pdpshare).

#### Manual installation
Simply extract the release package `bttb-cpp-*-Win64-Native-GUI.zip` and run:
* **`bttb_win32.exe`**: Double-click to launch the native Windows GUI application instantly!

### 2. Creating ISO Images on Windows (Optional)
To use the **Create ISO Image** feature, a command-line ISO generation utility (`genisoimage.exe` or `mkisofs.exe`) must be installed on your Windows system and added to your environment `PATH`. 

Here are the simplest ways to install it:

* **Method 1: Using Scoop (Recommended - Easiest & Automatic PATH)**
  1. Open PowerShell and run this command to install Scoop (if not installed):
     ```powershell
     irm get.scoop.sh | iex
     ```
  2. Install `cdrtools` (which includes `mkisofs`):
     ```powershell
     scoop install cdrtools
     ```
  3. Restart Burn to the Brim and create your ISO!

* **Method 2: Using Chocolatey (Automatic PATH)**
  1. Open an Administrator PowerShell and run:
     ```powershell
     choco install cdrtools
     ```
  2. Restart Burn to the Brim and create your ISO!

* **Method 3: Using Cygwin**
  1. Run the Cygwin `setup-x86_64.exe` installer.
  2. In the Package Selection screen, search for and select the `cdrkit` (contains `genisoimage`) or `cdrtools` (contains `mkisofs`) package.
  3. Add `C:\cygwin64\bin` to your Windows System environment variable `PATH`.
  4. Restart Burn to the Brim.

* **Method 4: Using MSYS2**
  1. Open your **MSYS2 UCRT64** terminal and install cdrtools:
     ```bash
     pacman -S mingw-w64-ucrt-x86_64-cdrtools
     ```
  2. Add `C:\msys64\ucrt64\bin` to your Windows System environment variable `PATH`.
  3. Restart Burn to the Brim.

---

## Linux Installation & Running (Ubuntu 24.04)

### 1. Install Dependencies
```bash
sudo apt update
sudo apt install -y libgtk-4-dev build-essential cmake pkg-config genisoimage
```

### 2. Building from Source (Linux)
Compile the application from the root of the project folder:
```bash
# Create and navigate to the build folder
mkdir -p build
cd build

# Configure and compile
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

This compiles:
1. `bttb`: The GTK 4 desktop GUI application.
2. `test_solver`: An offline programmatic unit test suite.

### 3. Running on Linux
```bash
# Launch the desktop app
./build/bttb

# Run the unit tests
./build/test_solver
```

---

## Building from Source (Windows MSYS2 UCRT64)

To build the native GTK 4 Windows GUI from source:
1. Open the **MSYS2 UCRT64** terminal.
2. Update system and install the compiler, CMake, and GTK 4:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gtk4
   ```
3. Navigate to the project root directory and build:
   ```bash
   mkdir build && cd build
   cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
   mingw32-make -j$(nproc)
   ```

---

## Generating Release Packages and Installers (Cross-Compilation on Ubuntu)

To cross-compile the Windows binaries, build the `.msi` installers, and generate the Linux `.deb` packages on an Ubuntu development machine:

### 1. Install Prerequisites

Install the required build tools, cross-compilers, and packaging utilities:
```bash
sudo apt update
sudo apt install -y \
  build-essential \
  cmake \
  pkg-config \
  libgtk-4-dev \
  genisoimage \
  gcc-mingw-w64-x86-64 \
  g++-mingw-w64-x86-64 \
  msitools
```

### 2. Run the Release Packaging Script

Navigate to the project root directory and run the packaging script:
```bash
python scratch/package_release.py
```

This script automates the entire packaging pipeline:
1. **Linux Compilation**: Compiles the native GTK 4 application and generates the `.deb` package via CMake.
2. **Windows Cross-Compilation**: Uses MinGW-w64 to build the native 64-bit Windows GUI executables (`bttb_win32.exe` and `bttb_win32_compat.exe`) with exploit mitigations and strips symbols.
3. **Release ZIPs**: Packages all portable ZIP archives for Linux and Windows releases.
4. **MSI Installer Creation**: Converts the plain-text `LICENSE` file into a formatted `License.rtf` and runs `wixl` to compile the enterprise-grade `.msi` installers in the `build/` directory:
   * `bttb-cpp-4.4.0-Win64-Installer.msi` (Native AVX2 release)
   * `bttb-cpp-4.4.0-Win64-Compat-Installer.msi` (Compatibility SSSE3 release)
5. **Unified Source Archive**: Generates the unified source code ZIP.

---

## License

This program is free software; you can redistribute it and/or modify it under the terms of the **GNU General Public License as published by the Free Software Foundation; version 2 of the License** (GPLv2). See the accompanying [LICENSE](LICENSE) file for the full terms.
