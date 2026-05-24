# Compiling Burn to the Brim on Windows (MSYS2 UCRT64)

This guide provides comprehensive, step-by-step instructions to natively build and run **Burn to the Brim (BTTB)** on Windows. 

To remain fully compliant with open-source licensing terms (GPLv2/LGPL), we do not distribute pre-compiled DLLs. Instead, we use **MSYS2**, the standard, modern, and officially supported cross-development environment for running GTK 4 natively on Windows.

---

## Prerequisites: Setting Up MSYS2

### Step 1: Install MSYS2
1. Download and run the official MSYS2 installer from:
   **[https://www.msys2.org/](https://www.msys2.org/)**
2. Install it in the default directory (usually `C:\msys64`).

### Step 2: Open UCRT64 Terminal
1. Once installed, search for and open the **MSYS2 UCRT64** terminal from your Windows Start Menu.
   *(UCRT64 is the modern Windows runtime environment that links against the Microsoft Universal C Runtime, matching the standard Linux C++ library behavior).*

### Step 3: Install Compiler, CMake, and GTK 4
Execute the following commands in the UCRT64 terminal to update your package manager and install the necessary building packages:

```bash
# Update package databases and core system packages
pacman -Syu

# Install the GCC compilation toolchain
pacman -S mingw-w64-ucrt-x86_64-toolchain

# Install CMake build tools
pacman -S mingw-w64-ucrt-x86_64-cmake

# Install GTK 4 development libraries
pacman -S mingw-w64-ucrt-x86_64-gtk4
```

---

## Compiling the Application

Once your MSYS2 UCRT64 environment is set up, compile the application from source:

### Step 1: Navigate to the Source Folder
Inside the UCRT64 terminal, change directory to the folder containing this source tree (using standard Unix-style paths):
```bash
# Example: If your source is in C:\Users\YourName\Documents\bttb_cpp
cd /c/Users/YourName/Documents/bttb_cpp
```

### Step 2: Configure with CMake
Create a build directory and configure the C++ project:
```bash
# Create build directory
mkdir build
cd build

# Configure CMake using MinGW Makefiles
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
```

### Step 3: Compile
Run the compiler to build the executables:
```bash
mingw32-make -j$(nproc)
```

This compiles:
1. `bttb.exe` - The main GTK 4 Graphical User Interface (GUI) application.
2. `test_solver.exe` - An offline bin-packer verification tool.

---

## Running the Application

To run the application natively on Windows:
```bash
# Launch the GTK 4 GUI
./bttb.exe

# Launch the solver logic verification test
./test_solver.exe
```

---

## Creating ISO Images on Windows

To use the **Create ISO Image** feature, a command-line ISO generation utility (`genisoimage.exe` or `mkisofs.exe`) must be installed on your Windows system and added to your environment `PATH`. 

Here are the simplest ways to install it:

### Method 1: Using Scoop (Recommended - Easiest)
Scoop is a command-line installer for Windows that configures PATH automatically:
1. Open PowerShell and install Scoop (if not already installed):
   ```powershell
   irm get.scoop.sh | iex
   ```
2. Install the `cdrtools` package (contains `mkisofs`):
   ```powershell
   scoop install cdrtools
   ```
3. Restart Burn to the Brim and create your ISO!

### Method 2: Using Chocolatey
Chocolatey is another highly popular Windows package manager that configures PATH automatically:
1. Open an Administrator PowerShell and run:
   ```powershell
   choco install cdrtools
   ```
2. Restart Burn to the Brim and create your ISO!

### Method 3: Using Cygwin
1. Run the Cygwin `setup-x86_64.exe` installer.
2. In the package selection screen, search for and select the `cdrkit` (contains `genisoimage`) or `cdrtools` (contains `mkisofs`) package.
3. Finish the installation.
4. Add `C:\cygwin64\bin` to your Windows System environment variable `PATH`.
5. Restart Burn to the Brim.

### Method 4: Using MSYS2
1. Open your **MSYS2 UCRT64** terminal and run:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-cdrtools
   ```
2. Add `C:\msys64\ucrt64\bin` to your Windows System environment variable `PATH`.
3. Restart Burn to the Brim.

---

## License

This program is free software; you can redistribute it and/or modify it under the terms of the **GNU General Public License as published by the Free Software Foundation; version 2 of the License** (GPLv2). See the accompanying [LICENSE](LICENSE) file for the full terms.

