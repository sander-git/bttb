import os
import zipfile
import shutil
import subprocess
import glob

base_dir = "/home/sander/src/antigravity/project1/bttb_cpp"
build_dir = os.path.join(base_dir, "build")
version = "4.2.0"

os.makedirs(build_dir, exist_ok=True)

# 1. Compile Linux GTK4 binary & Debian package
print("Compiling Linux GTK4 binary and packaging DEB via CMake...")
subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."], cwd=build_dir, check=True)
subprocess.run(["make", "-j", "4"], cwd=build_dir, check=True)
subprocess.run(["make", "package"], cwd=build_dir, check=True)

# 2. Cross-compile Windows GUI binary
print("Cross-compiling Windows Win32 Native binary with exploit mitigations...")

c_compiler = "x86_64-w64-mingw32-gcc"
cpp_compiler = "x86_64-w64-mingw32-g++"
common_flags = [
    "-O3", "-static",
    "-DBLAKE3_NO_AVX512", "-DBLAKE3_NO_AVX2", "-DBLAKE3_NO_SSE41", "-DBLAKE3_NO_SSE2",
    "-mssse3",
    "-Isrc", "-Isrc/libpar3", "-Isrc/blake3", "-Isrc/leopard", "-Isrc/platform"
]

c_sources = glob.glob("src/libpar3/*.c") + [
    "src/platform/windows/get_absolute_path.c",
    "src/blake3/blake3.c",
    "src/blake3/blake3_dispatch.c",
    "src/blake3/blake3_portable.c"
]

cpp_sources = [
    "src/main_win32.cpp",
    "src/bttb_logic.cpp"
] + glob.glob("src/leopard/*.cpp")

obj_files = []

# Compile C files
for src in c_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".c", ".o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C file: {src} -> {obj_path}")
    cmd = [c_compiler, "-std=c99"] + common_flags + ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    obj_files.append(obj_path)

# Compile C++ files
for src in cpp_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".cpp", ".o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C++ file: {src} -> {obj_path}")
    cmd = [cpp_compiler, "-std=c++20"] + common_flags + ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    obj_files.append(obj_path)

# Link everything
print("Linking objects to build/bttb_win32.exe...")
link_cmd = [
    cpp_compiler, "-static", "-mwindows"
] + obj_files + [
    "src/bttb_rc.o",
    "-lcomctl32", "-lshell32", "-lole32", "-lcomdlg32", "-ldwmapi", "-luxtheme",
    "-Wl,--dynamicbase", "-Wl,--nxcompat", "-Wl,--high-entropy-va",
    "-o", "build/bttb_win32.exe"
]
subprocess.run(link_cmd, cwd=base_dir, check=True)

print("Stripping debug symbols from Windows binary to eliminate Defender ML false positives...")
subprocess.run(["x86_64-w64-mingw32-strip", "build/bttb_win32.exe"], cwd=base_dir, check=True)

# 3. Create Linux GTK4 release ZIP
linux_zip_name = f"bttb-cpp-{version}-Linux-GTK4"
linux_zip_path = os.path.join(build_dir, f"{linux_zip_name}.zip")
print(f"Creating {linux_zip_path}...")
with zipfile.ZipFile(linux_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb"), os.path.join(linux_zip_name, "bttb"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(linux_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(linux_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "src/bttb_embed.py"), os.path.join(linux_zip_name, "src/bttb_embed.py"))

# 4. Create Windows Win32 Native GUI release ZIP
win_zip_name = f"bttb-cpp-{version}-Win64-Native-GUI"
win_zip_path = os.path.join(build_dir, f"{win_zip_name}.zip")
print(f"Creating {win_zip_path}...")
with zipfile.ZipFile(win_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb_win32.exe"), os.path.join(win_zip_name, "bttb_win32.exe"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(win_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(win_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "src/bttb_embed.py"), os.path.join(win_zip_name, "src/bttb_embed.py"))

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
    for root, dirs, files in os.walk(src_dir):
        for f in files:
            file_path = os.path.join(root, f)
            rel_path = os.path.relpath(file_path, base_dir)
            z.write(file_path, os.path.join(source_zip_name, rel_path))
            
    scratch_dir = os.path.join(base_dir, "scratch")
    for f in os.listdir(scratch_dir):
        file_path = os.path.join(scratch_dir, f)
        if os.path.isfile(file_path):
            z.write(file_path, os.path.join(source_zip_name, "scratch", f))

print(f"All BTTB v{version} packages generated successfully!")
