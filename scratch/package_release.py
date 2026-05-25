import os
import zipfile
import shutil
import subprocess

base_dir = "/home/sander/src/antigravity/project1/bttb_cpp"
build_dir = os.path.join(base_dir, "build")
version = "3.3.0"

os.makedirs(build_dir, exist_ok=True)

# 1. Compile Linux GTK4 binary
print("Compiling Linux GTK4 binary via CMake...")
subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."], cwd=build_dir, check=True)
subprocess.run(["make", "-j", "4"], cwd=build_dir, check=True)

# 2. Cross-compile Windows GUI binary
print("Cross-compiling Windows Win32 Native binary...")
win_cmd = [
    "x86_64-w64-mingw32-g++", "-O3", "-std=c++20", "-static", "-mwindows",
    "src/main_win32.cpp", "src/bttb_logic.cpp", "src/bttb_rc.o",
    "-lcomctl32", "-lshell32", "-lole32", "-lcomdlg32",
    "-o", "build/bttb_win32.exe"
]
subprocess.run(win_cmd, cwd=base_dir, check=True)

# 3. Create Linux GTK4 release ZIP
linux_zip_name = f"bttb-cpp-{version}-Linux-GTK4"
linux_zip_path = os.path.join(build_dir, f"{linux_zip_name}.zip")
print(f"Creating {linux_zip_path}...")
with zipfile.ZipFile(linux_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb"), os.path.join(linux_zip_name, "bttb"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(linux_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(linux_zip_name, "README.md"))

# 4. Create Windows Win32 Native GUI release ZIP
win_zip_name = f"bttb-cpp-{version}-Win64-Native-GUI"
win_zip_path = os.path.join(build_dir, f"{win_zip_name}.zip")
print(f"Creating {win_zip_path}...")
with zipfile.ZipFile(win_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb_win32.exe"), os.path.join(win_zip_name, "bttb_win32.exe"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(win_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(win_zip_name, "README.md"))

# 5. Compile Windows Installer using makensis
print("Compiling Windows Setup Installer via NSIS makensis...")
subprocess.run(["makensis", "scratch/bttb_installer.nsi"], cwd=base_dir, check=True)

# 6. Create Unified Source ZIP
source_zip_name = f"bttb-cpp-{version}-source-unified"
source_zip_path = os.path.join(build_dir, f"{source_zip_name}.zip")
print(f"Creating {source_zip_path}...")
with zipfile.ZipFile(source_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(base_dir, "CMakeLists.txt"), os.path.join(source_zip_name, "CMakeLists.txt"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(source_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(source_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "README_WINDOWS.md"), os.path.join(source_zip_name, "README_WINDOWS.md"))
    
    src_dir = os.path.join(base_dir, "src")
    for f in os.listdir(src_dir):
        file_path = os.path.join(src_dir, f)
        if os.path.isfile(file_path):
            z.write(file_path, os.path.join(source_zip_name, "src", f))
            
    scratch_dir = os.path.join(base_dir, "scratch")
    for f in os.listdir(scratch_dir):
        file_path = os.path.join(scratch_dir, f)
        if os.path.isfile(file_path):
            z.write(file_path, os.path.join(source_zip_name, "scratch", f))

print("All BTTB v3.3.0 packages generated successfully!")
