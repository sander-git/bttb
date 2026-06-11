import os
import zipfile
import shutil
import subprocess
import glob

base_dir = "/home/sander/src/antigravity/project1/bttb_cpp"
build_dir = os.path.join(base_dir, "build")
version = "4.6.0"

os.makedirs(build_dir, exist_ok=True)

# 1. Compile Linux GTK4 binary & Debian package
print("Compiling Linux GTK4 binary and packaging DEB via CMake...")
subprocess.run(["cmake", "-DCMAKE_BUILD_TYPE=Release", ".."], cwd=build_dir, check=True)
subprocess.run(["make", "-j", "4"], cwd=build_dir, check=True)
subprocess.run(["make", "package"], cwd=build_dir, check=True)

# 2. Compile Windows Resource File
print("Compiling Windows Resource File with windres...")
subprocess.run(["x86_64-w64-mingw32-windres", "src/bttb.rc", "-o", "src/bttb_rc.o"], cwd=base_dir, check=True)

# 3. Cross-compile Windows GUI binaries
print("Cross-compiling Windows Win32 Native binary with exploit mitigations...")

c_compiler = "x86_64-w64-mingw32-gcc"
cpp_compiler = "x86_64-w64-mingw32-g++"
common_flags = [
    "-O2", "-static",
    "-mssse3",
    "-flto", "-fno-ident",
    "-Isrc", "-Isrc/libpar3", "-Isrc/blake3", "-Isrc/leopard", "-Isrc/platform"
]

c_sources = glob.glob("src/libpar3/*.c") + [
    "src/platform/windows/get_absolute_path.c",
    "src/blake3/blake3.c",
    "src/blake3/blake3_dispatch.c",
    "src/blake3/blake3_portable.c",
    "src/blake3/blake3_sse2.c",
    "src/blake3/blake3_sse41.c",
    "src/blake3/blake3_avx2.c",
    "src/blake3/blake3_avx512.c"
]

common_cpp_sources = [
    "src/main_win32.cpp",
    "src/bttb_logic.cpp",
    "src/bttb_locale.cpp"
]

leopard_sources = glob.glob("src/leopard/*.cpp")

# Compile C files (shared between both builds)
c_obj_files = []
for src in c_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".c", ".o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C file: {src} -> {obj_path}")
    cmd = [c_compiler, "-std=c99"] + common_flags
    if "blake3_sse2.c" in src:
        cmd.append("-msse2")
    elif "blake3_sse41.c" in src:
        cmd.append("-msse4.1")
    elif "blake3_avx2.c" in src:
        cmd.append("-mavx2")
    elif "blake3_avx512.c" in src:
        cmd.append("-mavx512vl")
    cmd += ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    c_obj_files.append(obj_path)

# Compile common C++ files (shared between both builds)
common_cpp_obj_files = []
for src in common_cpp_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".cpp", ".o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C++ file: {src} -> {obj_path}")
    cmd = [cpp_compiler, "-std=c++20"] + common_flags
    cmd += ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    common_cpp_obj_files.append(obj_path)

# Compile Leopard C++ files with AVX2 (-mavx2) for Native build
leopard_avx2_obj_files = []
for src in leopard_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".cpp", "_avx2.o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C++ file (AVX2): {src} -> {obj_path}")
    cmd = [cpp_compiler, "-std=c++20"] + [f for f in common_flags if f != "-mssse3"] + ["-mavx2"]
    cmd += ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    leopard_avx2_obj_files.append(obj_path)

# Compile Leopard C++ files with SSSE3 (-mssse3) for Compat build
leopard_compat_obj_files = []
for src in leopard_sources:
    flat_name = src.replace("/", "_").replace("\\", "_").replace(".cpp", "_compat.o")
    obj_path = os.path.join(build_dir, flat_name)
    print(f"Compiling C++ file (Compat): {src} -> {obj_path}")
    cmd = [cpp_compiler, "-std=c++20"] + common_flags
    cmd += ["-c", src, "-o", obj_path]
    subprocess.run(cmd, cwd=base_dir, check=True)
    leopard_compat_obj_files.append(obj_path)

# Link Native AVX2 executable
print("Linking objects to build/bttb_win32.exe (Native AVX2)...")
link_avx2_cmd = [
    cpp_compiler, "-static", "-mwindows", "-O2", "-flto"
] + c_obj_files + common_cpp_obj_files + leopard_avx2_obj_files + [
    "src/bttb_rc.o",
    "-lcomctl32", "-lshell32", "-lole32", "-lcomdlg32", "-ldwmapi", "-luxtheme",
    "-Wl,--dynamicbase", "-Wl,--nxcompat", "-Wl,--high-entropy-va", "-Wl,--stack,16777216",
    "-o", "build/bttb_win32.exe"
]
subprocess.run(link_avx2_cmd, cwd=base_dir, check=True)

# Link Compat SSSE3 executable
print("Linking objects to build/bttb_win32_compat.exe (Compat SSSE3)...")
link_compat_cmd = [
    cpp_compiler, "-static", "-mwindows", "-O2", "-flto"
] + c_obj_files + common_cpp_obj_files + leopard_compat_obj_files + [
    "src/bttb_rc.o",
    "-lcomctl32", "-lshell32", "-lole32", "-lcomdlg32", "-ldwmapi", "-luxtheme",
    "-Wl,--dynamicbase", "-Wl,--nxcompat", "-Wl,--high-entropy-va", "-Wl,--stack,16777216",
    "-o", "build/bttb_win32_compat.exe"
]
subprocess.run(link_compat_cmd, cwd=base_dir, check=True)

# Strip debug symbols
print("Stripping debug symbols from Windows binaries to eliminate Defender ML false positives...")
subprocess.run(["x86_64-w64-mingw32-strip", "--strip-debug", "build/bttb_win32.exe"], cwd=base_dir, check=True)
subprocess.run(["x86_64-w64-mingw32-strip", "--strip-debug", "build/bttb_win32_compat.exe"], cwd=base_dir, check=True)

# 4. Create release ZIPs

# 4.1 Linux GTK4 ZIP
linux_zip_name = f"bttb-cpp-{version}-Linux-GTK4"
linux_zip_path = os.path.join(build_dir, f"{linux_zip_name}.zip")
print(f"Creating {linux_zip_path}...")
with zipfile.ZipFile(linux_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb"), os.path.join(linux_zip_name, "bttb"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(linux_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(linux_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "src/bttb_embed.py"), os.path.join(linux_zip_name, "src/bttb_embed.py"))
    for f in glob.glob("lang/*.po"):
        z.write(os.path.join(base_dir, f), os.path.join(linux_zip_name, f))

# 4.2 Windows Win32 Native GUI release ZIP (AVX2)
win_zip_name = f"bttb-cpp-{version}-Win64-Native-GUI"
win_zip_path = os.path.join(build_dir, f"{win_zip_name}.zip")
print(f"Creating {win_zip_path}...")
with zipfile.ZipFile(win_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb_win32.exe"), os.path.join(win_zip_name, "bttb_win32.exe"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(win_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(win_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "src/bttb_embed.py"), os.path.join(win_zip_name, "src/bttb_embed.py"))
    for f in glob.glob("lang/*.po"):
        z.write(os.path.join(base_dir, f), os.path.join(win_zip_name, f))

# 4.3 Windows Win32 Compat GUI release ZIP (SSSE3)
win_compat_zip_name = f"bttb-cpp-{version}-Win64-Compat-GUI"
win_compat_zip_path = os.path.join(build_dir, f"{win_compat_zip_name}.zip")
print(f"Creating {win_compat_zip_path}...")
with zipfile.ZipFile(win_compat_zip_path, 'w', zipfile.ZIP_DEFLATED) as z:
    z.write(os.path.join(build_dir, "bttb_win32_compat.exe"), os.path.join(win_compat_zip_name, "bttb_win32_compat.exe"))
    z.write(os.path.join(base_dir, "LICENSE"), os.path.join(win_compat_zip_name, "LICENSE"))
    z.write(os.path.join(base_dir, "README.md"), os.path.join(win_compat_zip_name, "README.md"))
    z.write(os.path.join(base_dir, "src/bttb_embed.py"), os.path.join(win_compat_zip_name, "src/bttb_embed.py"))
    for f in glob.glob("lang/*.po"):
        z.write(os.path.join(base_dir, f), os.path.join(win_compat_zip_name, f))

# 5. Compile Setup Installers via msitools (wixl)
print("Generating License.rtf from LICENSE...")
txt_path = os.path.join(base_dir, "LICENSE")
rtf_path = os.path.join(base_dir, "License.rtf")
with open(txt_path, "r", encoding="utf-8") as f:
    text = f.read()
rtf_text = text.replace("\\", "\\\\").replace("{", "\\{").replace("}", "\\}")
rtf_lines = [line.replace("\t", "\\tab ") + "\\par" for line in rtf_text.splitlines()]
rtf_content = "{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0\\fnil\\fcharset0 Courier New;}}\\viewkind4\\uc1\\pard\\f0\\fs18\n" + "\n".join(rtf_lines) + "\n}"
with open(rtf_path, "w", encoding="utf-8") as f:
    f.write(rtf_content)

print("Compiling Windows Native Setup Installer via wixl...")
subprocess.run([
    "wixl",
    "-o", f"build/bttb-cpp-{version}-Win64-Installer.msi",
    "--arch", "x64",
    "--ext", "ui",
    "-D", f"VERSION={version}",
    "-D", "EXE_FILE=bttb_win32.exe",
    "-D", "APP_NAME=Burn to the Brim",
    "-D", "UPGRADE_CODE=4E75A9C6-8E1A-4C2F-9E07-4C0F733526E3",
    "scratch/bttb_installer.wxs"
], cwd=base_dir, check=True)

print("Compiling Windows Compat Setup Installer via wixl...")
subprocess.run([
    "wixl",
    "-o", f"build/bttb-cpp-{version}-Win64-Compat-Installer.msi",
    "--arch", "x64",
    "--ext", "ui",
    "-D", f"VERSION={version}",
    "-D", "EXE_FILE=bttb_win32_compat.exe",
    "-D", "APP_NAME=Burn to the Brim (Compat)",
    "-D", "UPGRADE_CODE=5F86BA07-9F2B-4D3C-AF18-5D1A844637F4",
    "scratch/bttb_installer.wxs"
], cwd=base_dir, check=True)

# Clean up generated License.rtf
if os.path.exists(rtf_path):
    os.remove(rtf_path)

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
