import os
import sys
import shutil
import subprocess
import re

# Paths
QUASI_BIN = "/home/sander/src/antigravity/project1/quasi-msys2/root/ucrt64/bin"
EXE_PATH = "/home/sander/src/antigravity/project1/bttb_cpp/build/bttb_win_gui.exe"
RELEASE_DIR = "/home/sander/src/antigravity/project1/bttb_cpp/build/release_win"

# Exclude list for standard Windows system DLLs
SYSTEM_DLLS = {
    "kernel32.dll", "user32.dll", "gdi32.dll", "shell32.dll", "ole32.dll",
    "ws2_32.dll", "advapi32.dll", "msvcrt.dll", "ucrtbase.dll", "ntdll.dll",
    "comdlg32.dll", "shlwapi.dll", "winmm.dll", "setupapi.dll", "imm32.dll",
    "opengl32.dll", "glu32.dll", "credui.dll", "crypt32.dll", "dnsapi.dll",
    "dwmapi.dll", "iphlpapi.dll", "mpr.dll", "ncrypt.dll", "netapi32.dll",
    "odbc32.dll", "oleaut32.dll", "secur32.dll", "shell32.dll", "shlwapi.dll",
    "userenv.dll", "version.dll", "winhttp.dll", "wininet.dll", "winspool.drv",
    "wtsapi32.dll", "gdiplus.dll", "comctl32.dll", "usp10.dll", "msimg32.dll",
    "d3d11.dll", "dxgi.dll", "dwrite.dll"
}

def get_dependencies(file_path):
    """Run objdump to extract imported DLL names"""
    try:
        output = subprocess.check_output(["x86_64-w64-mingw32-objdump", "-p", file_path], text=True)
    except Exception as e:
        print(f"Error running objdump on {file_path}: {e}")
        return []
    
    dlls = []
    # Match lines like "DLL Name: libgtk-4-0.dll"
    for line in output.splitlines():
        if "DLL Name:" in line:
            dll_name = line.split("DLL Name:")[1].strip()
            dlls.append(dll_name)
    return dlls

def recursive_bundle(target_file, copied_dlls):
    """Recursively identify and copy DLL dependencies"""
    imports = get_dependencies(target_file)
    for dll in imports:
        dll_lower = dll.lower()
        if dll_lower in SYSTEM_DLLS:
            continue
        
        # Look for the DLL in our local MSYS2 ucrt64/bin sandbox
        source_dll_path = os.path.join(QUASI_BIN, dll)
        if not os.path.exists(source_dll_path):
            # Case insensitive check
            found = False
            for f in os.listdir(QUASI_BIN):
                if f.lower() == dll_lower:
                    source_dll_path = os.path.join(QUASI_BIN, f)
                    found = True
                    break
            if not found:
                # If not found in MSYS2, assume it is a Windows system DLL
                print(f"Skipping (assumed system DLL): {dll}")
                continue
        
        dest_dll_path = os.path.join(RELEASE_DIR, dll)
        if dll_lower not in copied_dlls:
            print(f"Bundling: {dll}")
            shutil.copy2(source_dll_path, dest_dll_path)
            copied_dlls.add(dll_lower)
            # Recurse down into the copied DLL
            recursive_bundle(dest_dll_path, copied_dlls)

def main():
    print(f"Creating release directory: {RELEASE_DIR}")
    if os.path.exists(RELEASE_DIR):
        shutil.rmtree(RELEASE_DIR)
    os.makedirs(RELEASE_DIR)
    
    # Copy main GUI executable
    print(f"Copying main GUI executable: {EXE_PATH}")
    dest_exe = os.path.join(RELEASE_DIR, "bttb_win_gui.exe")
    shutil.copy2(EXE_PATH, dest_exe)
    
    copied_dlls = set()
    print("Crawling and copying dependencies recursively...")
    recursive_bundle(dest_exe, copied_dlls)
    
    # Copy assets (adwaita icons or standard GTK resources) if needed
    # Standard UCRT64 GTK4 needs gschemas and themes to display nicely.
    # Let's copy share/glib-2.0/schemas and share/icons/Adwaita so the UI renders correctly!
    quasi_share = "/home/sander/src/antigravity/project1/quasi-msys2/root/ucrt64/share"
    dest_share = os.path.join(RELEASE_DIR, "share")
    
    # Copy schemas
    src_schemas = os.path.join(quasi_share, "glib-2.0", "schemas")
    if os.path.exists(src_schemas):
        dest_schemas = os.path.join(dest_share, "glib-2.0", "schemas")
        os.makedirs(dest_schemas)
        print("Bundling GLib schemas...")
        for f in os.listdir(src_schemas):
            if f.endswith(".xml") or f.endswith(".compiled"):
                shutil.copy2(os.path.join(src_schemas, f), os.path.join(dest_schemas, f))
                
    # Copy Adwaita icons and standard themes
    src_icons = os.path.join(quasi_share, "icons", "Adwaita")
    if os.path.exists(src_icons):
        dest_icons = os.path.join(dest_share, "icons", "Adwaita")
        print("Bundling Adwaita icons (standard themes)...")
        shutil.copytree(src_icons, dest_icons, ignore=shutil.ignore_patterns("*.svg", "*.png")) # metadata/index only to keep lightweight
        
    print("\nDLL and Asset Bundling Complete!")
    print(f"Release files are fully prepared in: {RELEASE_DIR}")

if __name__ == "__main__":
    main()
