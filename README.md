# Burn to the Brim (C++ Port)

**Burn to the Brim (BTTB)** is a modern C++20 desktop application designed to optimally fit files and folders onto target storage mediums (such as CDs, DVDs, or Custom sizes).

This is a complete port of the classic Delphi application to **C++20** and **GTK 4** running natively on **Ubuntu 24.04**. It solves the subset-sum / bin packing problem instantly using an advanced recursive backtracking solver, matching files efficiently, and providing file organization and ISO creation capabilities.

---

## Features

- **Backtracking Bin Packing Solver**: Employs the highly optimized subset-sum algorithm from the original BTTB version, executing in milliseconds with a fluid non-blocking background thread.
- **Directory Split Constraints**: Control directory structures by configuring a `splitDepth` waterlevel (atomic vs recursive scanning).
- **Modern Rule Groupings**: Group files/directories together on the same medium using standard wildcards (globs) or powerful regular expressions (`std::regex`).
- **File Organization**: Copy or move matched selections directly to target organizing folders automatically.
- **ISO Creation Integration**: Generate ISO 9660 / Joliet / Rock Ridge images natively via standard Linux command line utilities (`genisoimage`/`mkisofs`) with real-time log capturing.
- **Modern GNOME Look**: Standard GNOME Adwaita aesthetic featuring a HeaderBar, scrolling side results tree, interactive progress tracking, and detailed colored log viewer.

---

## Requirements

To build and run the application on Ubuntu 24.04, install the following package dependencies:

```bash
sudo apt update
sudo apt install -y libgtk-4-dev build-essential cmake pkg-config genisoimage
```

---

## Building the Application

Compile the application from the root of the `bttb_cpp` project folder:

```bash
# Create and navigate to the build folder
mkdir -p build
cd build

# Configure the project with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Compile all targets
make -j$(nproc)
```

This compiles two main executables in the `build/` folder:
1. `bttb`: The desktop GUI application.
2. `test_solver`: An offline programmatic unit test for solver correctness.

---

## Running the Application

### Running the Desktop App
```bash
./bttb
```

### Running the Verification Test Suite
```bash
./test_solver
```

---

## License

This program is free software; you can redistribute it and/or modify it under the terms of the **GNU General Public License as published by the Free Software Foundation; version 2 of the License** (GPLv2). See the accompanying [LICENSE](LICENSE) file for the full terms.
