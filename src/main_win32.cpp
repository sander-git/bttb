#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include <iostream>
#include "bttb_logic.hpp"
#include "cli_engine.hpp"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comdlg32.lib")

// Custom Window Messages for thread-safe GUI updates
#define WM_SOLVER_LOG      (WM_USER + 1)
#define WM_SOLVER_PROGRESS (WM_USER + 2)
#define WM_SOLVER_FINISHED (WM_USER + 3)

#define WM_ISO_LOG         (WM_USER + 10)
#define WM_ISO_FINISHED    (WM_USER + 11)

// Control IDs
#define ID_BTN_SRC_BROWSE  1001
#define ID_BTN_DEST_BROWSE 1002
#define ID_BTN_START       1003
#define ID_BTN_STOP        1004
#define ID_CHK_MOVE        1006
#define ID_BTN_CREATE_ISO  1007
#define ID_CHK_SPAN        1009
#define ID_BTN_PREFS       1010
#define ID_CHK_TRACE       1011
#define ID_BTN_ABOUT       1012
#define ID_BTN_TEST        1017
#define ID_EDIT_SEMANTIC   1018

// ISO Dialog Control IDs
#define ID_BTN_ISO_SRC_BROWSE  2001
#define ID_BTN_ISO_PATH_BROWSE 2002
#define ID_BTN_ISO_GENERATE    2003
#define ID_BTN_ISO_CLOSE       2004

// Preferences Dialog Control IDs
#define ID_PREF_COMBO_MEDIA     3001
#define ID_PREF_EDIT_CAP        3002
#define ID_PREF_EDIT_CLUS       3003
#define ID_PREF_EDIT_SLACK      3004
#define ID_PREF_EDIT_TIME       3005
#define ID_PREF_EDIT_DEPTH      3006
#define ID_PREF_CHK_EMPTY       3007
#define ID_PREF_LIST_RULES      3008
#define ID_PREF_EDIT_PATTERN    3009
#define ID_PREF_CHK_FILES       3010
#define ID_PREF_CHK_FOLDERS     3011
#define ID_PREF_CHK_REGEX       3012
#define ID_PREF_BTN_ADD_RULE    3013
#define ID_PREF_BTN_DEL_RULE    3014
#define ID_PREF_BTN_OK          3015
#define ID_PREF_BTN_CANCEL      3016
#define ID_PREF_LABEL_CAP_MB    3017
#define ID_PREF_CHK_CONTEXT_MENU 3018
#define ID_PREF_CHK_UNREADABLE  3019

#define ID_BTN_ADD_FOLDERS      1013
#define ID_TREE_RESULTS         1014
#define ID_CHK_SYMLINK          1015
#define ID_BTN_HELP             1016

#define ID_FOLDER_LISTBOX       2001
#define ID_FOLDER_BTN_ADD       2002
#define ID_FOLDER_BTN_EDIT      2003
#define ID_FOLDER_BTN_REMOVE    2004
#define ID_FOLDER_BTN_CANCEL    2005
#define ID_FOLDER_BTN_OK        2006

// Global State
HWND g_hwndMain = NULL;
HWND g_editSrc = NULL;
HWND g_editDest = NULL;
HWND g_chkMove = NULL;
HWND g_chkSymlink = NULL;
HWND g_chkSpan = NULL;
HWND g_chkTrace = NULL;
HWND g_editLog = NULL;
HWND g_progress = NULL;
HWND g_hwndTreeView = NULL;
HWND g_btnStart = NULL;
HWND g_btnTest = NULL;
HWND g_btnStop = NULL;
HWND g_btnCreateIso = NULL;
HWND g_labelProgress = NULL;
HWND g_editSemantic = NULL;

// Folders List Dialog State
HWND g_hwndFolderList = NULL;
HWND g_listFolders = NULL;

// Preferences Dialog Global Controls
HWND g_hwndPref = NULL;
HWND g_comboPrefMedia = NULL;
HWND g_editPrefCap = NULL;
HWND g_labelPrefCapMB = NULL;
HWND g_editPrefClus = NULL;
HWND g_editPrefSlack = NULL;
HWND g_editPrefTime = NULL;
HWND g_editPrefDepth = NULL;
HWND g_chkPrefEmpty = NULL;
HWND g_chkPrefUnreadable = NULL;
HWND g_chkPrefContextMenu = NULL;
HWND g_listPrefRules = NULL;
HWND g_editPrefPattern = NULL;
HWND g_chkPrefFiles = NULL;
HWND g_chkPrefFolders = NULL;
HWND g_chkPrefRegex = NULL;
HWND g_btnPrefAddRule = NULL;
HWND g_btnPrefDelRule = NULL;

// ISO Dialog State
HWND g_hwndIso = NULL;
HWND g_editIsoSrc = NULL;
HWND g_editIsoPath = NULL;
HWND g_editIsoVol = NULL;
HWND g_editIsoLog = NULL;
HWND g_btnIsoGenerate = NULL;

bttb::BttbSolver g_solver;
std::jthread g_solver_thread;
std::jthread g_iso_thread;

// v3.3.0 Explorer Integration & Single Instance Globals
std::wstring g_folderToAdd = L"";
HANDLE g_hMutex = NULL;

// Unicode helper functions
#ifndef wstringToUtf8_defined
inline std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
inline std::wstring utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
#endif

// Registry management helper functions
void RegisterExplorerContextMenu(bool registerIt) {
    std::wstring subkey = L"Software\\Classes\\Directory\\shell\\BTTB";
    if (registerIt) {
        HKEY hKey;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            std::wstring menuText = L"Add to Burn to the Brim";
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (const BYTE*)menuText.c_str(), (DWORD)((menuText.size() + 1) * sizeof(wchar_t)));
            
            // Set Icon
            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            RegSetValueExW(hKey, L"Icon", 0, REG_SZ, (const BYTE*)exePath, (DWORD)((wcslen(exePath) + 1) * sizeof(wchar_t)));
            
            HKEY hSubKey;
            if (RegCreateKeyExW(hKey, L"command", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubKey, NULL) == ERROR_SUCCESS) {
                std::wstring cmd = L"\"" + std::wstring(exePath) + L"\" \"%1\"";
                RegSetValueExW(hSubKey, NULL, 0, REG_SZ, (const BYTE*)cmd.c_str(), (DWORD)((cmd.size() + 1) * sizeof(wchar_t)));
                RegCloseKey(hSubKey);
            }
            RegCloseKey(hKey);
        }
    } else {
        RegDeleteTreeW(HKEY_CURRENT_USER, subkey.c_str());
    }
}

bool IsExplorerContextMenuRegistered() {
    HKEY hKey;
    bool registered = false;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell\\BTTB", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        registered = true;
        RegCloseKey(hKey);
    }
    return registered;
}

void LoadRegistrySettings() {
    HKEY hKey;
    g_solver.skipUnreadable = true; // default
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\BurnToTheBrim", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD type, size, val;
        size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"SkipUnreadable", NULL, &type, (LPBYTE)&val, &size) == ERROR_SUCCESS) {
            g_solver.skipUnreadable = (val != 0);
        }
        RegCloseKey(hKey);
    }
}

void SaveRegistrySettings() {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\BurnToTheBrim", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        DWORD val = g_solver.skipUnreadable ? 1 : 0;
        RegSetValueExW(hKey, L"SkipUnreadable", 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

// Append text helper for multiline Edit control
void AppendTextToLog(HWND hEdit, const std::string& text) {
    int len = GetWindowTextLength(hEdit);
    SendMessage(hEdit, EM_SETSEL, len, len);
    // Replace selection with new text (appends it)
    SendMessage(hEdit, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>((text + "\r\n").c_str()));
}

// Shell directory browser dialog helper
std::string BrowseForFolder(HWND hwnd, const std::string& title) {
    BROWSEINFO bi = {0};
    bi.hwndOwner = hwnd;
    bi.lpszTitle = title.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != nullptr) {
        char path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
    }
    return "";
}

// Native Save File Dialog helper
std::string SaveFileDialog(HWND hwnd, const std::string& title) {
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "ISO Image Files (*.iso)\0*.iso\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    
    if (GetSaveFileName(&ofn)) {
        return std::string(szFile);
    }
    return "";
}

// Background thread runner for popen genisoimage / mkisofs fallback
void RunIsoGeneration(HWND hwnd, std::string srcDir, std::string isoPath, std::string volLabel) {
    bool success = false;
    std::vector<std::string> tools = {"genisoimage", "mkisofs"};
    
    for (const auto& tool : tools) {
        std::string cmd = tool + " -o \"" + isoPath + "\" -V \"" + volLabel + "\" -J -R \"" + srcDir + "\" 2>&1";
        
        auto* pStartLog = new std::string("Executing command: " + cmd + "\r\n");
        PostMessage(hwnd, WM_ISO_LOG, 0, reinterpret_cast<LPARAM>(pStartLog));
        
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (!pipe) {
            continue;
        }
        
        std::string output = "";
        char buffer[256];
        bool commandNotFound = false;
        
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            std::string line(buffer);
            output += line;
            
            // Check for common command-not-found patterns
            if (line.find("not recognized") != std::string::npos ||
                line.find("Can't recognize") != std::string::npos ||
                line.find("not found") != std::string::npos ||
                line.find("No such file or directory") != std::string::npos) {
                commandNotFound = true;
            }
            
            auto* pLog = new std::string(buffer);
            PostMessage(hwnd, WM_ISO_LOG, 0, reinterpret_cast<LPARAM>(pLog));
        }
        
        int status = _pclose(pipe);
        
        if (status == 0 && !commandNotFound) {
            success = true;
            auto* pFinal = new std::string("\r\nSUCCESS: ISO Image successfully created using " + tool + ".\r\n");
            PostMessage(hwnd, WM_ISO_LOG, 0, reinterpret_cast<LPARAM>(pFinal));
            break;
        } else {
            auto* pRetry = new std::string("Failed to execute " + tool + ".\r\n");
            PostMessage(hwnd, WM_ISO_LOG, 0, reinterpret_cast<LPARAM>(pRetry));
        }
    }
    
    if (!success) {
        std::string errInstructions = 
            "\r\n==================================================\r\n"
            "ERROR: Neither 'genisoimage' nor 'mkisofs' could be found on your system.\r\n\r\n"
            "An ISO creation tool (like cdrtools or cdrkit) is required to build ISO images.\r\n"
            "Please follow one of these simple methods to install it on Windows:\r\n\r\n"
            "--------------------------------------------------\r\n"
            "METHOD 1: Using Scoop (Recommended - Easiest & Automatic PATH)\r\n"
            "  1. Open PowerShell and run this command to install Scoop (if not installed):\r\n"
            "     irm get.scoop.sh | iex\r\n"
            "  2. Install cdrtools (which includes mkisofs):\r\n"
            "     scoop install cdrtools\r\n"
            "  3. That's it! Scoop configures the PATH automatically. Re-open BTTB and try again.\r\n\r\n"
            "METHOD 2: Using Chocolatey (Automatic PATH)\r\n"
            "  1. Open an Administrator Command Prompt or PowerShell and run:\r\n"
            "     choco install cdrtools\r\n"
            "  2. That's it! Chocolatey configures the PATH automatically. Re-open BTTB and try again.\r\n\r\n"
            "METHOD 3: Using Cygwin (Unix environment on Windows)\r\n"
            "  1. Download and run Cygwin's 'setup-x86_64.exe' installer.\r\n"
            "  2. In the Package Selection screen, search for 'cdrkit' (contains genisoimage) or 'cdrtools' (contains mkisofs).\r\n"
            "  3. Change the selection from 'Skip' to the version you want to install, and finish setup.\r\n"
            "  4. Add 'C:\\cygwin64\\bin' to your Windows System Environment Variable PATH.\r\n"
            "  5. Re-open BTTB and try again.\r\n\r\n"
            "METHOD 4: Using MSYS2\r\n"
            "  1. Open your MSYS2 UCRT64 terminal and install cdrtools:\r\n"
            "     pacman -S mingw-w64-ucrt-x86_64-cdrtools\r\n"
            "  2. Add 'C:\\msys64\\ucrt64\\bin' to your Windows System Environment Variable PATH.\r\n"
            "  3. Re-open BTTB and try again.\r\n"
            "==================================================\r\n";
        auto* pErr = new std::string(errInstructions);
        PostMessage(hwnd, WM_ISO_LOG, 0, reinterpret_cast<LPARAM>(pErr));
    }
    
    PostMessage(hwnd, WM_ISO_FINISHED, 0, 0);
}

// ISO Dialog Window Procedure
LRESULT CALLBACK IsoWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            int y = 20;
            
            CreateWindow("STATIC", "Source Directory:", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editIsoSrc = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 150, y, 250, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y - 2, 90, 25, hwnd, (HMENU)ID_BTN_ISO_SRC_BROWSE, NULL, NULL);
            
            y += 30;
            CreateWindow("STATIC", "Target ISO File:", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editIsoPath = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 150, y, 250, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y - 2, 90, 25, hwnd, (HMENU)ID_BTN_ISO_PATH_BROWSE, NULL, NULL);
            
            y += 30;
            CreateWindow("STATIC", "Volume Label:", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editIsoVol = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "BTTB_BACKUP", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 150, y, 150, 22, hwnd, NULL, NULL, NULL);
            
            y += 35;
            CreateWindow("BUTTON", "Execution Log", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 200, hwnd, NULL, NULL, NULL);
            g_editIsoLog = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 24, y + 20, 480, 165, hwnd, NULL, NULL, NULL);
            
            y += 215;
            g_btnIsoGenerate = CreateWindow("BUTTON", "Generate", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 240, y, 120, 30, hwnd, (HMENU)ID_BTN_ISO_GENERATE, NULL, NULL);
            CreateWindow("BUTTON", "Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 370, y, 120, 30, hwnd, (HMENU)ID_BTN_ISO_CLOSE, NULL, NULL);
            
            // Set font
            HFONT hFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                SendMessage(hwndChild, WM_SETFONT, lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont);
            
            break;
        }
        
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            
            if (wmId == ID_BTN_ISO_SRC_BROWSE) {
                std::string path = BrowseForFolder(hwnd, "Select Source Directory");
                if (!path.empty()) {
                    SetWindowText(g_editIsoSrc, path.c_str());
                }
            }
            
            if (wmId == ID_BTN_ISO_PATH_BROWSE) {
                std::string path = SaveFileDialog(hwnd, "Select Target ISO File");
                if (!path.empty()) {
                    SetWindowText(g_editIsoPath, path.c_str());
                }
            }
            
            if (wmId == ID_BTN_ISO_GENERATE) {
                char src[MAX_PATH];
                char iso[MAX_PATH];
                char vol[128];
                GetWindowText(g_editIsoSrc, src, MAX_PATH);
                GetWindowText(g_editIsoPath, iso, MAX_PATH);
                GetWindowText(g_editIsoVol, vol, 128);
                
                if (strlen(src) == 0 || strlen(iso) == 0) {
                    MessageBox(hwnd, "Please specify both Source Directory and Target ISO File.", "Error", MB_ICONERROR);
                    break;
                }
                
                // Disable generate button
                EnableWindow(g_btnIsoGenerate, FALSE);
                SetWindowText(g_editIsoLog, "");
                
                // Spawn background thread
                g_iso_thread = std::jthread(RunIsoGeneration, hwnd, std::string(src), std::string(iso), std::string(vol));
            }
            
            if (wmId == ID_BTN_ISO_CLOSE) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            
            break;
        }
        
        case WM_ISO_LOG: {
            auto* pStr = reinterpret_cast<std::string*>(lParam);
            AppendTextToLog(g_editIsoLog, *pStr);
            delete pStr;
            break;
        }
        
        case WM_ISO_FINISHED: {
            EnableWindow(g_btnIsoGenerate, TRUE);
            if (g_iso_thread.joinable()) {
                g_iso_thread.join();
            }
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            if (g_iso_thread.joinable()) {
                g_iso_thread.join();
            }
            EnableWindow(g_hwndMain, TRUE);
            SetForegroundWindow(g_hwndMain);
            g_hwndIso = NULL;
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Window Procedure for Folder List Dialog
LRESULT CALLBACK FolderListWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindow("STATIC", "Complete list of source folders:", WS_CHILD | WS_VISIBLE, 12, 12, 300, 20, hwnd, NULL, NULL, NULL);
            
            g_listFolders = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "", WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_NOTIFY | WS_VSCROLL, 12, 36, 460, 200, hwnd, (HMENU)ID_FOLDER_LISTBOX, NULL, NULL);
            
            CreateWindow("BUTTON", "Add...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 12, 250, 80, 28, hwnd, (HMENU)ID_FOLDER_BTN_ADD, NULL, NULL);
            CreateWindow("BUTTON", "Edit...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 250, 80, 28, hwnd, (HMENU)ID_FOLDER_BTN_EDIT, NULL, NULL);
            CreateWindow("BUTTON", "Remove", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 188, 250, 80, 28, hwnd, (HMENU)ID_FOLDER_BTN_REMOVE, NULL, NULL);
            
            CreateWindow("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 250, 80, 28, hwnd, (HMENU)ID_FOLDER_BTN_CANCEL, NULL, NULL);
            CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 390, 250, 80, 28, hwnd, (HMENU)ID_FOLDER_BTN_OK, NULL, NULL);
            
            HFONT hFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                SendMessage(hwndChild, WM_SETFONT, lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont);
            
            char srcText[2048] = {0};
            GetWindowText(g_editSrc, srcText, 2048);
            std::string path;
            for (int i = 0; srcText[i] != '\0'; ++i) {
                if (srcText[i] == ';') {
                    if (!path.empty()) {
                        SendMessage(g_listFolders, LB_ADDSTRING, 0, (LPARAM)path.c_str());
                        path.clear();
                    }
                } else {
                    path += srcText[i];
                }
            }
            if (!path.empty()) {
                SendMessage(g_listFolders, LB_ADDSTRING, 0, (LPARAM)path.c_str());
            }
            break;
        }
        
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (wmId == ID_FOLDER_BTN_ADD) {
                std::string path = BrowseForFolder(hwnd, "Add Source Folder");
                if (!path.empty()) {
                    SendMessage(g_listFolders, LB_ADDSTRING, 0, (LPARAM)path.c_str());
                }
            }
            
            if (wmId == ID_FOLDER_BTN_EDIT) {
                int sel = SendMessage(g_listFolders, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR) {
                    char currentPath[MAX_PATH] = {0};
                    SendMessage(g_listFolders, LB_GETTEXT, sel, (LPARAM)currentPath);
                    
                    std::string path = BrowseForFolder(hwnd, "Edit Source Folder");
                    if (!path.empty()) {
                        SendMessage(g_listFolders, LB_DELETESTRING, sel, 0);
                        SendMessage(g_listFolders, LB_INSERTSTRING, sel, (LPARAM)path.c_str());
                        SendMessage(g_listFolders, LB_SETCURSEL, sel, 0);
                    }
                }
            }
            
            if (wmId == ID_FOLDER_BTN_REMOVE) {
                int sel = SendMessage(g_listFolders, LB_GETCURSEL, 0, 0);
                if (sel != LB_ERR) {
                    SendMessage(g_listFolders, LB_DELETESTRING, sel, 0);
                }
            }
            
            if (wmId == ID_FOLDER_BTN_CANCEL) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            
            if (wmId == ID_FOLDER_BTN_OK) {
                int count = SendMessage(g_listFolders, LB_GETCOUNT, 0, 0);
                std::string joined;
                for (int i = 0; i < count; ++i) {
                    char buf[MAX_PATH];
                    SendMessage(g_listFolders, LB_GETTEXT, i, (LPARAM)buf);
                    if (strlen(buf) > 0) {
                        if (!joined.empty()) joined += ";";
                        joined += buf;
                    }
                }
                SetWindowText(g_editSrc, joined.c_str());
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            EnableWindow(g_hwndMain, TRUE);
            SetForegroundWindow(g_hwndMain);
            g_hwndFolderList = NULL;
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Preferences Dialog Window Procedure
LRESULT CALLBACK PrefWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_LISTVIEW_CLASSES;
            InitCommonControlsEx(&icex);
            
            int y = 15;
            
            // Row 1: Media Selection & Capacity
            CreateWindow("STATIC", "Select Medium:", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_comboPrefMedia = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 150, y, 120, 250, hwnd, (HMENU)ID_PREF_COMBO_MEDIA, NULL, NULL);
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"CD (650 MB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"CD (700 MB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"DVD (4.7 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"DVD DL (8.5 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"BD (25 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"BD DL (50 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (8 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (16 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (32 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (64 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (256 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"USB (512 GB)");
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"Custom Size");
            
            CreateWindow("STATIC", "Capacity:", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefCap = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_CAP, NULL, NULL);
            g_labelPrefCapMB = CreateWindow("STATIC", "(0.00 MB)", WS_CHILD | WS_VISIBLE, 370, y + 23, 100, 15, hwnd, (HMENU)ID_PREF_LABEL_CAP_MB, NULL, NULL);
            
            y += 45;
            // Row 2: Cluster & Slack
            CreateWindow("STATIC", "Cluster (Bytes):", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editPrefClus = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 150, y, 120, 22, hwnd, (HMENU)ID_PREF_EDIT_CLUS, NULL, NULL);
            
            CreateWindow("STATIC", "Slack (Bytes):", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefSlack = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_SLACK, NULL, NULL);
            
            y += 30;
            // Row 3: Max Search Time & Split Depth
            CreateWindow("STATIC", "Max Search Time (s):", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editPrefTime = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 150, y, 120, 22, hwnd, (HMENU)ID_PREF_EDIT_TIME, NULL, NULL);
            
            CreateWindow("STATIC", "Split Depth:", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefDepth = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_DEPTH, NULL, NULL);
            
            y += 30;
            // Row 4: Skip Empty & Skip Unreadable
            g_chkPrefEmpty = CreateWindow("BUTTON", "Skip Empty Folders / Files", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, y, 220, 20, hwnd, (HMENU)ID_PREF_CHK_EMPTY, NULL, NULL);
            g_chkPrefUnreadable = CreateWindow("BUTTON", "Skip Unreadable Files (Graceful)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 250, y, 230, 20, hwnd, (HMENU)ID_PREF_CHK_UNREADABLE, NULL, NULL);
            
            y += 30;
            // Row 4.5: Context Menu Integration
            g_chkPrefContextMenu = CreateWindow("BUTTON", "Integrate with Windows Explorer Context Menu", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 20, y, 400, 20, hwnd, (HMENU)ID_PREF_CHK_CONTEXT_MENU, NULL, NULL);
            
            y += 30;
            // Group 5: Grouping Rules
            CreateWindow("BUTTON", "File / Folder Grouping Rules", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, y, 480, 240, hwnd, NULL, NULL, NULL);
            
            g_listPrefRules = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 20, y + 20, 460, 110, hwnd, (HMENU)ID_PREF_LIST_RULES, NULL, NULL);
            ListView_SetExtendedListViewStyle(g_listPrefRules, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            
            LVCOLUMN lvc = {0};
            lvc.mask = LVCF_TEXT | LVCF_WIDTH;
            lvc.cx = 180;
            lvc.pszText = const_cast<LPSTR>("Pattern");
            ListView_InsertColumn(g_listPrefRules, 0, &lvc);
            
            lvc.cx = 70;
            lvc.pszText = const_cast<LPSTR>("Files");
            ListView_InsertColumn(g_listPrefRules, 1, &lvc);
            
            lvc.cx = 70;
            lvc.pszText = const_cast<LPSTR>("Folders");
            ListView_InsertColumn(g_listPrefRules, 2, &lvc);
            
            lvc.cx = 100;
            lvc.pszText = const_cast<LPSTR>("Type");
            ListView_InsertColumn(g_listPrefRules, 3, &lvc);
            
            g_editPrefPattern = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 20, y + 140, 200, 22, hwnd, (HMENU)ID_PREF_EDIT_PATTERN, NULL, NULL);
            SendMessage(g_editPrefPattern, EM_SETCUEBANNER, FALSE, (LPARAM)L"Pattern (*.mp3 or regex)");
            
            g_chkPrefFiles = CreateWindow("BUTTON", "Match Files", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 230, y + 140, 100, 20, hwnd, (HMENU)ID_PREF_CHK_FILES, NULL, NULL);
            SendMessage(g_chkPrefFiles, BM_SETCHECK, BST_CHECKED, 0);
            
            g_chkPrefFolders = CreateWindow("BUTTON", "Match Folders", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 230, y + 170, 110, 20, hwnd, (HMENU)ID_PREF_CHK_FOLDERS, NULL, NULL);
            SendMessage(g_chkPrefFolders, BM_SETCHECK, BST_CHECKED, 0);
            
            g_chkPrefRegex = CreateWindow("BUTTON", "Regex Pattern", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 320, y + 170, 110, 20, hwnd, (HMENU)ID_PREF_CHK_REGEX, NULL, NULL);
            
            g_btnPrefAddRule = CreateWindow("BUTTON", "Add Rule", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, y + 200, 100, 25, hwnd, (HMENU)ID_PREF_BTN_ADD_RULE, NULL, NULL);
            g_btnPrefDelRule = CreateWindow("BUTTON", "Remove Selected", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, y + 200, 150, 25, hwnd, (HMENU)ID_PREF_BTN_DEL_RULE, NULL, NULL);
            
            y += 245;
            // OK / Cancel Action Buttons
            CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 280, y, 100, 30, hwnd, (HMENU)ID_PREF_BTN_OK, NULL, NULL);
            CreateWindow("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 390, y, 100, 30, hwnd, (HMENU)ID_PREF_BTN_CANCEL, NULL, NULL);
            
            // Set Font
            HFONT hFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                SendMessage(hwndChild, WM_SETFONT, lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont);
            
            // Load Solver Settings into Controls
            if (g_solver.mediumInfo.capacityBytes == 681574400) SetWindowText(g_editPrefCap, "650 MB");
            else if (g_solver.mediumInfo.capacityBytes == 734003200) SetWindowText(g_editPrefCap, "700 MB");
            else if (g_solver.mediumInfo.capacityBytes == 4700000000LL) SetWindowText(g_editPrefCap, "4.7 GB");
            else if (g_solver.mediumInfo.capacityBytes == 8500000000LL) SetWindowText(g_editPrefCap, "8.5 GB");
            else if (g_solver.mediumInfo.capacityBytes == 25000000000LL) SetWindowText(g_editPrefCap, "25 GB");
            else if (g_solver.mediumInfo.capacityBytes == 50000000000LL) SetWindowText(g_editPrefCap, "50 GB");
            else if (g_solver.mediumInfo.capacityBytes == 8000000000LL) SetWindowText(g_editPrefCap, "8 GB");
            else if (g_solver.mediumInfo.capacityBytes == 16000000000LL) SetWindowText(g_editPrefCap, "16 GB");
            else if (g_solver.mediumInfo.capacityBytes == 32000000000LL) SetWindowText(g_editPrefCap, "32 GB");
            else if (g_solver.mediumInfo.capacityBytes == 64000000000LL) SetWindowText(g_editPrefCap, "64 GB");
            else if (g_solver.mediumInfo.capacityBytes == 256000000000LL) SetWindowText(g_editPrefCap, "256 GB");
            else if (g_solver.mediumInfo.capacityBytes == 512000000000LL) SetWindowText(g_editPrefCap, "512 GB");
            else {
                double gb = (double)g_solver.mediumInfo.capacityBytes / (1024.0 * 1024.0 * 1024.0);
                char buf[64];
                snprintf(buf, sizeof(buf), "%.3f GB", gb);
                SetWindowText(g_editPrefCap, buf);
            }
            
            SetWindowText(g_editPrefClus, std::to_string(g_solver.mediumInfo.sectorSize).c_str());
            SetWindowText(g_editPrefSlack, std::to_string(g_solver.mediumInfo.slackBytes).c_str());
            SetWindowText(g_editPrefTime, std::to_string(g_solver.maxSearchTimeSeconds).c_str());
            SetWindowText(g_editPrefDepth, std::to_string(g_solver.splitDepth).c_str());
            SendMessage(g_chkPrefEmpty, BM_SETCHECK, g_solver.skipEmpty ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(g_chkPrefUnreadable, BM_SETCHECK, g_solver.skipUnreadable ? BST_CHECKED : BST_UNCHECKED, 0);
            SendMessage(g_chkPrefContextMenu, BM_SETCHECK, IsExplorerContextMenuRegistered() ? BST_CHECKED : BST_UNCHECKED, 0);
            
            // Select correct Media combobox index
            int index = 12; // Custom Size
            if (g_solver.mediumInfo.capacityBytes == 681574400) index = 0;
            else if (g_solver.mediumInfo.capacityBytes == 734003200) index = 1;
            else if (g_solver.mediumInfo.capacityBytes == 4700000000LL) index = 2;
            else if (g_solver.mediumInfo.capacityBytes == 8500000000LL) index = 3;
            else if (g_solver.mediumInfo.capacityBytes == 25000000000LL) index = 4;
            else if (g_solver.mediumInfo.capacityBytes == 50000000000LL) index = 5;
            else if (g_solver.mediumInfo.capacityBytes == 8000000000LL) index = 6;
            else if (g_solver.mediumInfo.capacityBytes == 16000000000LL) index = 7;
            else if (g_solver.mediumInfo.capacityBytes == 32000000000LL) index = 8;
            else if (g_solver.mediumInfo.capacityBytes == 64000000000LL) index = 9;
            else if (g_solver.mediumInfo.capacityBytes == 256000000000LL) index = 10;
            else if (g_solver.mediumInfo.capacityBytes == 512000000000LL) index = 11;
            
            SendMessage(g_comboPrefMedia, CB_SETCURSEL, index, 0);
            EnableWindow(g_editPrefCap, (index == 12) ? TRUE : FALSE);
            
            // Initial dynamic calculation
            SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(ID_PREF_EDIT_CAP, EN_CHANGE), (LPARAM)g_editPrefCap);
            
            // Load Rules list
            int rowIndex = 0;
            for (const auto& rule : g_solver.groupingRules) {
                LVITEM lvi = {0};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = rowIndex;
                lvi.iSubItem = 0;
                lvi.pszText = const_cast<LPSTR>(rule.patternString.c_str());
                ListView_InsertItem(g_listPrefRules, &lvi);
                
                ListView_SetItemText(g_listPrefRules, rowIndex, 1, const_cast<LPSTR>(rule.matchFiles ? "Yes" : "No"));
                ListView_SetItemText(g_listPrefRules, rowIndex, 2, const_cast<LPSTR>(rule.matchFolders ? "Yes" : "No"));
                ListView_SetItemText(g_listPrefRules, rowIndex, 3, const_cast<LPSTR>(rule.isRegex ? "Regex" : "Glob"));
                
                rowIndex++;
            }
            
            break;
        }
        
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            
            if (wmId == ID_PREF_EDIT_CAP && wmEvent == EN_CHANGE) {
                char buf[128] = {0};
                GetWindowText(g_editPrefCap, buf, sizeof(buf));
                int64_t bytes = bttb::parseHumanSize(buf);
                double mb = (double)bytes / (1024.0 * 1024.0);
                char labelBuf[128];
                snprintf(labelBuf, sizeof(labelBuf), "(%.2f MB)", mb);
                SetWindowText(g_labelPrefCapMB, labelBuf);
            }
            
            if (wmId == ID_PREF_COMBO_MEDIA && wmEvent == CBN_SELCHANGE) {
                int index = SendMessage(g_comboPrefMedia, CB_GETCURSEL, 0, 0);
                if (index == 0) { // CD 650
                    SetWindowText(g_editPrefCap, "650 MB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 1) { // CD 700
                    SetWindowText(g_editPrefCap, "700 MB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 2) { // DVD 4.7
                    SetWindowText(g_editPrefCap, "4.7 GB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 3) { // DVD DL 8.5
                    SetWindowText(g_editPrefCap, "8.5 GB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 4) { // BD 25
                    SetWindowText(g_editPrefCap, "25 GB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 5) { // BD DL 50
                    SetWindowText(g_editPrefCap, "50 GB");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 6) { // USB 8
                    SetWindowText(g_editPrefCap, "8 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 7) { // USB 16
                    SetWindowText(g_editPrefCap, "16 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 8) { // USB 32
                    SetWindowText(g_editPrefCap, "32 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 9) { // USB 64
                    SetWindowText(g_editPrefCap, "64 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 10) { // USB 256
                    SetWindowText(g_editPrefCap, "256 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 11) { // USB 512
                    SetWindowText(g_editPrefCap, "512 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else { // Custom
                    SetWindowText(g_editPrefCap, "64 GB");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, TRUE);
                }
            }
            
            if (wmId == ID_PREF_BTN_ADD_RULE) {
                char pattern[256];
                GetWindowText(g_editPrefPattern, pattern, 256);
                if (strlen(pattern) == 0) break;
                
                bool matchFiles = (SendMessage(g_chkPrefFiles, BM_GETCHECK, 0, 0) == BST_CHECKED);
                bool matchFolders = (SendMessage(g_chkPrefFolders, BM_GETCHECK, 0, 0) == BST_CHECKED);
                bool isRegex = (SendMessage(g_chkPrefRegex, BM_GETCHECK, 0, 0) == BST_CHECKED);
                
                int count = ListView_GetItemCount(g_listPrefRules);
                
                LVITEM lvi = {0};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = count;
                lvi.iSubItem = 0;
                lvi.pszText = pattern;
                ListView_InsertItem(g_listPrefRules, &lvi);
                
                ListView_SetItemText(g_listPrefRules, count, 1, const_cast<LPSTR>(matchFiles ? "Yes" : "No"));
                ListView_SetItemText(g_listPrefRules, count, 2, const_cast<LPSTR>(matchFolders ? "Yes" : "No"));
                ListView_SetItemText(g_listPrefRules, count, 3, const_cast<LPSTR>(isRegex ? "Regex" : "Glob"));
                
                SetWindowText(g_editPrefPattern, "");
            }
            
            if (wmId == ID_PREF_BTN_DEL_RULE) {
                int selected = ListView_GetNextItem(g_listPrefRules, -1, LVNI_SELECTED);
                if (selected != -1) {
                    ListView_DeleteItem(g_listPrefRules, selected);
                }
            }
            
            if (wmId == ID_PREF_BTN_OK) {
                char buf[64];
                char capText[128];
                
                GetWindowText(g_editPrefCap, capText, 128);
                g_solver.mediumInfo.capacityBytes = bttb::parseHumanSize(capText);
                
                GetWindowText(g_editPrefClus, buf, 64);
                g_solver.mediumInfo.sectorSize = std::stoll(buf);
                
                GetWindowText(g_editPrefSlack, buf, 64);
                g_solver.mediumInfo.slackBytes = std::stoll(buf);
                
                GetWindowText(g_editPrefTime, buf, 64);
                g_solver.maxSearchTimeSeconds = std::stoi(buf);
                
                GetWindowText(g_editPrefDepth, buf, 64);
                g_solver.splitDepth = std::stoi(buf);
                
                g_solver.skipEmpty = (SendMessage(g_chkPrefEmpty, BM_GETCHECK, 0, 0) == BST_CHECKED);
                g_solver.skipUnreadable = (SendMessage(g_chkPrefUnreadable, BM_GETCHECK, 0, 0) == BST_CHECKED);
                
                bool enableMenu = (SendMessage(g_chkPrefContextMenu, BM_GETCHECK, 0, 0) == BST_CHECKED);
                RegisterExplorerContextMenu(enableMenu);
                
                SaveRegistrySettings();
                
                // Save grouping rules
                g_solver.groupingRules.clear();
                int count = ListView_GetItemCount(g_listPrefRules);
                for (int i = 0; i < count; ++i) {
                    char pattern[256] = {0};
                    char matchFiles[16] = {0};
                    char matchFolders[16] = {0};
                    char type[16] = {0};
                    
                    ListView_GetItemText(g_listPrefRules, i, 0, pattern, 256);
                    ListView_GetItemText(g_listPrefRules, i, 1, matchFiles, 16);
                    ListView_GetItemText(g_listPrefRules, i, 2, matchFolders, 16);
                    ListView_GetItemText(g_listPrefRules, i, 3, type, 16);
                    
                    bttb::GroupRule rule;
                    rule.patternString = pattern;
                    rule.matchFiles = (strcmp(matchFiles, "Yes") == 0);
                    rule.matchFolders = (strcmp(matchFolders, "Yes") == 0);
                    rule.isRegex = (strcmp(type, "Regex") == 0);
                    
                    try {
                        if (rule.isRegex) {
                            rule.compiledRegex = std::regex(rule.patternString, std::regex_constants::icase);
                        } else {
                            rule.compiledRegex = bttb::globToRegex(rule.patternString);
                        }
                        g_solver.groupingRules.push_back(rule);
                    } catch (...) {
                        // ignore malformed regex
                    }
                }
                
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            
            if (wmId == ID_PREF_BTN_CANCEL) {
                SendMessage(hwnd, WM_CLOSE, 0, 0);
            }
            
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            EnableWindow(g_hwndMain, TRUE);
            SetForegroundWindow(g_hwndMain);
            g_hwndPref = NULL;
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Populate standard Win32 TreeView control with solved volumes and unfitted items
void PopulateTreeView(HWND hwndTV) {
    TreeView_DeleteAllItems(hwndTV);
    
    // 1. Walk through each solved volume in packedVolumes
    for (const auto& vol : g_solver.packedVolumes) {
        char vol_name[256];
        snprintf(vol_name, sizeof(vol_name), "Volume %d (Total: %.2f MB)", vol.volumeIndex, (double)vol.totalBytes / (1024.0 * 1024.0));
        
        TVINSERTSTRUCT tvis = {0};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = const_cast<LPSTR>(vol_name);
        
        HTREEITEM hParent = TreeView_InsertItem(hwndTV, &tvis);
        
        for (size_t i = 0; i < vol.itemPaths.size(); ++i) {
            char child_name[512];
            snprintf(child_name, sizeof(child_name), "%s (%lld bytes)", vol.itemPaths[i].c_str(), static_cast<long long>(vol.itemSizes[i]));
            
            TVINSERTSTRUCT tvisChild = {0};
            tvisChild.hParent = hParent;
            tvisChild.hInsertAfter = TVI_LAST;
            tvisChild.item.mask = TVIF_TEXT;
            tvisChild.item.pszText = const_cast<LPSTR>(child_name);
            
            HTREEITEM hChild = TreeView_InsertItem(hwndTV, &tvisChild);
            
            // If it is a semantic or rules group, add nested files under it!
            if (i < vol.itemGroupedPaths.size() && !vol.itemGroupedPaths[i].empty()) {
                for (const auto& subPath : vol.itemGroupedPaths[i]) {
                    char sub_name[512];
                    snprintf(sub_name, sizeof(sub_name), "%s", subPath.c_str());
                    
                    TVINSERTSTRUCT tvisSub = {0};
                    tvisSub.hParent = hChild;
                    tvisSub.hInsertAfter = TVI_LAST;
                    tvisSub.item.mask = TVIF_TEXT;
                    tvisSub.item.pszText = const_cast<LPSTR>(sub_name);
                    
                    TreeView_InsertItem(hwndTV, &tvisSub);
                }
                TreeView_Expand(hwndTV, hChild, TVE_EXPAND);
            }
        }
        
        TreeView_Expand(hwndTV, hParent, TVE_EXPAND);
     }
     
     // 2. Add remaining (unfitted) items
     if (!g_solver.itemsToSplit.empty()) {
         int64_t unfitted_bytes = 0;
         for (const auto& item : g_solver.itemsToSplit) {
             unfitted_bytes += item->sizeBytes;
         }
         
         char unfitted_label[256];
         snprintf(unfitted_label, sizeof(unfitted_label), "Remaining Items (Total: %.2f MB)", (double)unfitted_bytes / (1024.0 * 1024.0));
         
         TVINSERTSTRUCT tvis = {0};
         tvis.hParent = TVI_ROOT;
         tvis.hInsertAfter = TVI_LAST;
         tvis.item.mask = TVIF_TEXT;
         tvis.item.pszText = const_cast<LPSTR>(unfitted_label);
         
         HTREEITEM hParent = TreeView_InsertItem(hwndTV, &tvis);
         
         for (const auto& item : g_solver.itemsToSplit) {
             char child_name[512];
             snprintf(child_name, sizeof(child_name), "%s (%lld bytes)", item->relativePath.c_str(), static_cast<long long>(item->sizeBytes));
             
             TVINSERTSTRUCT tvisChild = {0};
             tvisChild.hParent = hParent;
             tvisChild.hInsertAfter = TVI_LAST;
             tvisChild.item.mask = TVIF_TEXT;
             tvisChild.item.pszText = const_cast<LPSTR>(child_name);
             
             HTREEITEM hChild = TreeView_InsertItem(hwndTV, &tvisChild);
             
             // If it is a group, add nested files under it!
             if (!item->groupedPaths.empty()) {
                 for (const auto& subPath : item->groupedPaths) {
                     char sub_name[512];
                     snprintf(sub_name, sizeof(sub_name), "%s", subPath.c_str());
                     
                     TVINSERTSTRUCT tvisSub = {0};
                     tvisSub.hParent = hChild;
                     tvisSub.hInsertAfter = TVI_LAST;
                     tvisSub.item.mask = TVIF_TEXT;
                     tvisSub.item.pszText = const_cast<LPSTR>(sub_name);
                     
                     TreeView_InsertItem(hwndTV, &tvisSub);
                 }
                 TreeView_Expand(hwndTV, hChild, TVE_EXPAND);
             }
         }
         
         TreeView_Expand(hwndTV, hParent, TVE_EXPAND);
     }
}

// Window Procedure for Main Window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COPYDATA: {
            PCOPYDATASTRUCT pcds = (PCOPYDATASTRUCT)lParam;
            if (pcds && pcds->dwData == 1) {
                wchar_t* wpath = (wchar_t*)pcds->lpData;
                std::string utf8Path = wstringToUtf8(wpath);
                
                char currentText[4096] = {0};
                GetWindowText(g_editSrc, currentText, sizeof(currentText));
                
                std::string text(currentText);
                if (text.empty()) {
                    text = utf8Path;
                } else {
                    if (text.find(utf8Path) == std::string::npos) {
                        text += ";" + utf8Path;
                    }
                }
                SetWindowText(g_editSrc, text.c_str());
                
                // Bring main window to foreground
                ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
            }
            return TRUE;
        }
        
        case WM_CREATE: {
            // Initialize Common Controls for Progress Bar and TreeView
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_PROGRESS_CLASS | ICC_TREEVIEW_CLASSES;
            InitCommonControlsEx(&icex);
            
            // Standard left-side results explorer TreeView
            g_hwndTreeView = CreateWindowEx(
                WS_EX_CLIENTEDGE,
                WC_TREEVIEW,
                "",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT,
                12, 24, 260, 440,
                hwnd,
                (HMENU)ID_TREE_RESULTS,
                GetModuleHandle(NULL),
                NULL
            );
            
            // Layout shifted to the right by 272px for right side groups
            int y = 16;
            
            // Group 1: Folders Selection (height expanded to 205 to fit semantic packing text box)
            CreateWindow("BUTTON", "Directories setup", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 284, y, 510, 205, hwnd, NULL, NULL, NULL);
            
            CreateWindow("BUTTON", "+", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 296, y + 22, 30, 25, hwnd, (HMENU)ID_BTN_ADD_FOLDERS, NULL, NULL);
            CreateWindow("STATIC", "Source folder:", WS_CHILD | WS_VISIBLE, 332, y + 26, 80, 20, hwnd, NULL, NULL, NULL);
            g_editSrc = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 418, y + 24, 260, 22, hwnd, NULL, NULL, NULL);
            LoadRegistrySettings();
            if (!g_folderToAdd.empty()) {
                std::string utf8Folder = wstringToUtf8(g_folderToAdd);
                SetWindowText(g_editSrc, utf8Folder.c_str());
            }
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 688, y + 22, 90, 25, hwnd, (HMENU)ID_BTN_SRC_BROWSE, NULL, NULL);
            
            CreateWindow("STATIC", "Target folder:", WS_CHILD | WS_VISIBLE, 296, y + 58, 110, 20, hwnd, NULL, NULL, NULL);
            g_editDest = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 418, y + 56, 260, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 688, y + 54, 90, 25, hwnd, (HMENU)ID_BTN_DEST_BROWSE, NULL, NULL);
            
            // v4.0.0 Semantic Prompt Control at y + 82
            CreateWindow("STATIC", "Semantic prompt:", WS_CHILD | WS_VISIBLE, 296, y + 84, 120, 20, hwnd, NULL, NULL, NULL);
            g_editSemantic = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 418, y + 82, 360, 22, hwnd, (HMENU)ID_EDIT_SEMANTIC, NULL, NULL);
            
            // Checkboxes shifted down to accommodate semantic prompt control
            g_chkMove = CreateWindow("BUTTON", "Move/organize fitted folders/files to target folder", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 418, y + 108, 360, 20, hwnd, (HMENU)ID_CHK_MOVE, NULL, NULL);
            g_chkSymlink = CreateWindow("BUTTON", "Create symbolic links in target folder (Default)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 418, y + 130, 360, 20, hwnd, (HMENU)ID_CHK_SYMLINK, NULL, NULL);
            SendMessage(g_chkSymlink, BM_SETCHECK, BST_CHECKED, 0); // Checked by default
            
            g_chkSpan = CreateWindow("BUTTON", "Span across multiple volumes (Volume_1, Volume_2, etc.)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 418, y + 152, 360, 20, hwnd, (HMENU)ID_CHK_SPAN, NULL, NULL);
            g_chkTrace = CreateWindow("BUTTON", "Enable detailed solver diagnostic tracing (Trace)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 418, y + 174, 360, 20, hwnd, (HMENU)ID_CHK_TRACE, NULL, NULL);
            
            // Group 2: Progress (Shifted to y=230)
            CreateWindow("BUTTON", "Fitted Capacity Status", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 284, 230, 510, 60, hwnd, NULL, NULL, NULL);
            g_progress = CreateWindow(PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 296, 254, 350, 20, hwnd, NULL, NULL, NULL);
            g_labelProgress = CreateWindow("STATIC", "Filled: 0.00%", WS_CHILD | WS_VISIBLE, 657, 256, 120, 20, hwnd, NULL, NULL, NULL);
            
            // Group 3: Log Output (Shifted to y=300)
            CreateWindow("BUTTON", "Solver Output Log", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 284, 300, 510, 160, hwnd, NULL, NULL, NULL);
            g_editLog = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 296, 324, 480, 120, hwnd, NULL, NULL, NULL);
            
            // Bottom Action buttons row (Shifted to y=475)
            g_btnTest = CreateWindow("BUTTON", "Test", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 284, 475, 70, 30, hwnd, (HMENU)ID_BTN_TEST, NULL, NULL);
            g_btnStart = CreateWindow("BUTTON", "Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, 475, 70, 30, hwnd, (HMENU)ID_BTN_START, NULL, NULL);
            g_btnStop = CreateWindow("BUTTON", "Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 436, 475, 50, 30, hwnd, (HMENU)ID_BTN_STOP, NULL, NULL);
            CreateWindow("BUTTON", "Preferences...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 492, 475, 95, 30, hwnd, (HMENU)ID_BTN_PREFS, NULL, NULL);
            g_btnCreateIso = CreateWindow("BUTTON", "Create ISO...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 593, 475, 95, 30, hwnd, (HMENU)ID_BTN_CREATE_ISO, NULL, NULL);
            CreateWindow("BUTTON", "Help", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 694, 475, 50, 30, hwnd, (HMENU)ID_BTN_HELP, NULL, NULL);
            CreateWindow("BUTTON", "About...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 750, 475, 54, 30, hwnd, (HMENU)ID_BTN_ABOUT, NULL, NULL);
            
            EnableWindow(g_btnStop, FALSE);
            
            // Setup visual font styles
            HFONT hFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                SendMessage(hwndChild, WM_SETFONT, lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont);
            
            break;
        }
        
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            
            if (wmId == ID_CHK_MOVE) {
                if (IsDlgButtonChecked(hwnd, ID_CHK_MOVE) == BST_CHECKED) {
                    CheckDlgButton(hwnd, ID_CHK_SYMLINK, BST_UNCHECKED);
                }
            }
            if (wmId == ID_CHK_SYMLINK) {
                if (IsDlgButtonChecked(hwnd, ID_CHK_SYMLINK) == BST_CHECKED) {
                    CheckDlgButton(hwnd, ID_CHK_MOVE, BST_UNCHECKED);
                }
            }
            
            if (wmId == ID_BTN_SRC_BROWSE) {
                std::string path = BrowseForFolder(hwnd, "Select Source Folder");
                if (!path.empty()) {
                    SetWindowText(g_editSrc, path.c_str());
                }
            }
            
            if (wmId == ID_BTN_DEST_BROWSE) {
                std::string path = BrowseForFolder(hwnd, "Select Target Folder");
                if (!path.empty()) {
                    SetWindowText(g_editDest, path.c_str());
                }
            }
            
            if (wmId == ID_BTN_ADD_FOLDERS) {
                if (g_hwndFolderList != NULL) {
                    SetForegroundWindow(g_hwndFolderList);
                    break;
                }
                
                // Disable main window
                EnableWindow(hwnd, FALSE);
                
                // Create Folder List Window
                g_hwndFolderList = CreateWindowEx(
                    WS_EX_CONTROLPARENT,
                    "BttbWin32FolderListDialog",
                    "Manage Source Folders",
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                    CW_USEDEFAULT, CW_USEDEFAULT, 500, 330,
                    hwnd, NULL, GetModuleHandle(NULL), NULL
                );
                
                if (g_hwndFolderList != NULL) {
                    ShowWindow(g_hwndFolderList, SW_SHOW);
                } else {
                    EnableWindow(hwnd, TRUE);
                }
            }
            
            if (wmId == ID_BTN_START || wmId == ID_BTN_TEST) {
                char src[2048];
                char dest[MAX_PATH];
                GetWindowText(g_editSrc, src, 2048);
                GetWindowText(g_editDest, dest, MAX_PATH);
                
                if (strlen(src) == 0) {
                    MessageBox(hwnd, "Please select a valid source folder first.", "Error", MB_ICONERROR);
                    break;
                }
                
                // Parse semicolon separated list of source directories
                g_solver.sourceDirectories.clear();
                std::string current = "";
                for (int i = 0; src[i] != '\0'; ++i) {
                    if (src[i] == ';') {
                        if (!current.empty()) {
                            g_solver.sourceDirectories.push_back(current);
                            current.clear();
                        }
                    } else {
                        current += src[i];
                    }
                }
                if (!current.empty()) {
                    g_solver.sourceDirectories.push_back(current);
                }
                
                if (g_solver.sourceDirectories.empty()) {
                    MessageBox(hwnd, "Source directory list must not be empty.", "Error", MB_ICONERROR);
                    break;
                }
                
                bool allExist = true;
                for (const auto& d : g_solver.sourceDirectories) {
                    if (!std::filesystem::exists(d)) {
                        MessageBox(hwnd, ("Error: Source directory does not exist:\n" + d).c_str(), "Error", MB_ICONERROR);
                        allExist = false;
                        break;
                    }
                }
                if (!allExist) break;
                
                g_solver.sourceDirectory = g_solver.sourceDirectories.front(); // backward compatibility fallback
                g_solver.targetDirectory = dest;
                g_solver.moveFiles = (IsDlgButtonChecked(hwnd, ID_CHK_MOVE) == BST_CHECKED);
                g_solver.createSymlinks = (IsDlgButtonChecked(hwnd, ID_CHK_SYMLINK) == BST_CHECKED);
                g_solver.spanMultipleVolumes = (IsDlgButtonChecked(hwnd, ID_CHK_SPAN) == BST_CHECKED);
                g_solver.enableTrace = (IsDlgButtonChecked(hwnd, ID_CHK_TRACE) == BST_CHECKED);
                
                // Retrieve semantic prompt
                char semanticPromptBuf[1024];
                GetWindowText(g_editSemantic, semanticPromptBuf, 1024);
                g_solver.semanticPrompt = semanticPromptBuf;
                g_solver.enableSemanticPacking = !g_solver.semanticPrompt.empty();
                
                // Configure test mode
                g_solver.testOnlyMode = (wmId == ID_BTN_TEST);
                
                // Clear old logs and tree
                SetWindowText(g_editLog, "");
                TreeView_DeleteAllItems(g_hwndTreeView);
                if (g_solver.testOnlyMode) {
                    AppendTextToLog(g_editLog, "Starting test packing simulation...");
                } else {
                    AppendTextToLog(g_editLog, "Starting solver background calculations...");
                }
                
                // Disable UI inputs
                EnableWindow(g_btnStart, FALSE);
                EnableWindow(g_btnTest, FALSE);
                EnableWindow(g_btnStop, TRUE);
                EnableWindow(g_chkMove, FALSE);
                EnableWindow(g_chkSymlink, FALSE);
                EnableWindow(g_chkSpan, FALSE);
                EnableWindow(g_chkTrace, FALSE);
                EnableWindow(g_editSemantic, FALSE);
                
                // Wire thread safe UI messages
                g_solver.logNotify = [hwnd](const std::string& msg, int type) {
                    auto* pStr = new std::string(msg);
                    PostMessage(hwnd, WM_SOLVER_LOG, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(pStr));
                };
                
                g_solver.progressNotify = [hwnd](double disc, double overall) {
                    int percent = static_cast<int>(disc * 10000.0); // 2 decimal places precision
                    PostMessage(hwnd, WM_SOLVER_PROGRESS, static_cast<WPARAM>(percent), 0);
                };
                
                g_solver.recommendCapacityNotify = [hwnd](int64_t recommendedBytes) -> bool {
                    double recMB = (double)recommendedBytes / (1024.0 * 1024.0);
                    char buf[512];
                    snprintf(buf, sizeof(buf), 
                        "The largest scanned item requires a volume capacity of at least %.2f MB.\n\n"
                        "Would you like to automatically increase the volume capacity to %.2f MB and retry packing?", 
                        recMB, recMB);
                    int res = MessageBox(hwnd, buf, "Volume Capacity Recommendation", MB_YESNO | MB_ICONWARNING | MB_SETFOREGROUND);
                    return (res == IDYES);
                };
                
                // Run background solver thread
                g_solver_thread = std::jthread([hwnd]() {
                    g_solver.run();
                    PostMessage(hwnd, WM_SOLVER_FINISHED, 0, 0);
                });
            }
            
            if (wmId == ID_BTN_STOP) {
                g_solver.stopRequested = true;
                AppendTextToLog(g_editLog, "Cancellation requested by user...");
                EnableWindow(g_btnStop, FALSE);
            }
            
            if (wmId == ID_BTN_PREFS) {
                if (g_hwndPref != NULL) {
                    SetForegroundWindow(g_hwndPref);
                    break;
                }
                
                // Disable main window
                EnableWindow(hwnd, FALSE);
                
                // Create Preferences Window
                g_hwndPref = CreateWindowEx(
                    WS_EX_CONTROLPARENT,
                    "BttbWin32PrefDialog",
                    "Preferences",
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                    CW_USEDEFAULT, CW_USEDEFAULT, 500, 600,
                    hwnd, NULL, GetModuleHandle(NULL), NULL
                );
                
                if (g_hwndPref != NULL) {
                    ShowWindow(g_hwndPref, SW_SHOW);
                } else {
                    EnableWindow(hwnd, TRUE);
                }
            }
            
            if (wmId == ID_BTN_CREATE_ISO) {
                if (g_hwndIso != NULL) {
                    SetForegroundWindow(g_hwndIso);
                    break;
                }
                
                // Get the source directory from the main window to pre-populate
                char srcDir[MAX_PATH] = {0};
                GetWindowText(g_editSrc, srcDir, MAX_PATH);
                
                // Disable main window
                EnableWindow(hwnd, FALSE);
                
                // Create ISO Window
                g_hwndIso = CreateWindowEx(
                    WS_EX_CONTROLPARENT,
                    "BttbWin32ISODialog",
                    "Create ISO Image",
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                    CW_USEDEFAULT, CW_USEDEFAULT, 550, 410,
                    hwnd, NULL, GetModuleHandle(NULL), NULL
                );
                
                if (g_hwndIso != NULL) {
                    // Pre-populate source directory
                    SetWindowText(g_editIsoSrc, srcDir);
                    
                    // Set default target ISO path
                    std::string defaultIso = "";
                    if (strlen(srcDir) > 0) {
                        try {
                            defaultIso = (std::filesystem::path(srcDir).parent_path() / "output.iso").string();
                        } catch (...) {
                            defaultIso = "C:\\output.iso";
                        }
                    } else {
                        defaultIso = "C:\\output.iso";
                    }
                    SetWindowText(g_editIsoPath, defaultIso.c_str());
                    
                    ShowWindow(g_hwndIso, SW_SHOW);
                } else {
                    EnableWindow(hwnd, TRUE);
                }
            }
            
            if (wmId == ID_BTN_HELP) {
                std::string helpText = 
                    "Burn to the Brim (BTTB) Help Guide\r\n"
                    "=============================\r\n\r\n"
                    "1. Directory Split Depth\r\n"
                    "Determines the folder nesting level at which items are split:\r\n"
                    " - Depth 0 (Default): Top-level files and folders are treated as separate items.\r\n"
                    " - Depth 1: Splitting occurs one level deeper, keeping top-level folders intact but splitting their immediate subfolders.\r\n\r\n"
                    "2. Max Search Time\r\n"
                    "The maximum seconds the backtracking solver is allowed to run. If reached, the best selection found up to that point is used.\r\n\r\n"
                    "3. Spanning Slack\r\n"
                    "Allows early solver termination once a volume is packed within this number of bytes from the absolute maximum capacity (e.g. 2048 bytes).\r\n\r\n"
                    "4. File/Folder Grouping Rules\r\n"
                    "Force matching items to remain grouped together on the same volume (e.g., matching '*.mp3' or regex '^album_.*').\r\n\r\n"
                    "5. Multiple Source Folders (+)\r\n"
                    "Click '+' to specify multiple source folders. BTTB acts as if they are in a single root folder. Nested source paths are ignored.\r\n\r\n"
                    "6. Create Symbolic Links\r\n"
                    "Instead of copying/moving files to the target folder, BTTB creates lightweight symbolic links pointing back to your original files.\r\n\r\n"
                    "7. Neural Semantic Packing & MiniLM Setup Guide\r\n"
                    "By specifying a semantic prompt, BTTB groups files with similar content using context-aware deep learning embeddings.\r\n"
                    "To use the preferred, high-accuracy MiniLM neural model, you must install Python 3 and sentence-transformers:\r\n"
                    " - Step 1: Ensure Python 3 & pip are installed.\r\n"
                    "   (Linux: run 'sudo apt install python3 python3-pip python3-venv')\r\n"
                    "   (Windows: Install from https://www.python.org/ and check 'Add Python to PATH')\r\n"
                    " - Step 2: Install sentence-transformers via terminal/command prompt:\r\n"
                    "   Option A (Recommended for simplicity):\r\n"
                    "     pip install sentence-transformers\r\n"
                    "   Option B (Virtual environment isolation):\r\n"
                    "     python3 -m venv bttb_env\r\n"
                    "     source bttb_env/bin/activate  # (Windows: bttb_env\\Scripts\\activate)\r\n"
                    "     pip install sentence-transformers\r\n"
                    " - Step 3: Restart Burn to the Brim to automatically load MiniLM! If not found, BTTB falls back gracefully to a localized character TF-IDF projector.";
                MessageBox(hwnd, helpText.c_str(), "Help - Burn to the Brim", MB_OK | MB_ICONINFORMATION);
            }
            
            if (wmId == ID_BTN_ABOUT) {
                std::string aboutText = 
                    "Burn to the Brim (BTTB)\r\n"
                    "Version 4.0.0\r\n\r\n"
                    "Authors:\r\n"
                    "Sander Raaijmakers, Elwin Oost and the Burn to the Brim team\r\n\r\n"
                    "Licensing:\r\n"
                    "This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; version 2 of the License (GPLv2).\r\n\r\n"
                    "Features in v4.0.0:\r\n"
                    "- Entropy-Aware Semantic Packing based on MiniLM embeddings\r\n"
                    "- Interactive Test Simulation Mode with volume coherence metrics\r\n"
                    "- Unicode & Long Path (>256 characters) support\r\n"
                    "- Hybrid GUI / CLI integrated binary execution\r\n"
                    "- Optional Windows Explorer Context Menu integration\r\n"
                    "- Smart adaptive capacity recommendation & retrying";
                MessageBox(hwnd, aboutText.c_str(), "About Burn to the Brim", MB_OK | MB_ICONINFORMATION);
            }
            
            break;
        }
        
        case WM_SOLVER_LOG: {
            auto* pStr = reinterpret_cast<std::string*>(lParam);
            int type = static_cast<int>(wParam);
            std::string prefix = "";
            if (type == 1) prefix = "[SUCCESS] ";
            else if (type == 2) prefix = "[ERROR] ";
            else if (type == 3) prefix = "[IMPORTANT] ";
            
            AppendTextToLog(g_editLog, prefix + *pStr);
            delete pStr;
            break;
        }
        
        case WM_SOLVER_PROGRESS: {
            int val = static_cast<int>(wParam);
            SendMessage(g_progress, PBM_SETPOS, val / 100, 0);
            
            char label[64];
            snprintf(label, sizeof(label), "Filled: %.2f%%", (double)val / 100.0);
            SetWindowText(g_labelProgress, label);
            break;
        }
        
        case WM_SOLVER_FINISHED: {
            AppendTextToLog(g_editLog, "\nSolver processing complete.");
            
            // Re-enable start and control inputs
            EnableWindow(g_btnStart, TRUE);
            EnableWindow(g_btnTest, TRUE);
            EnableWindow(g_btnStop, FALSE);
            EnableWindow(g_chkMove, TRUE);
            EnableWindow(g_chkSymlink, TRUE);
            EnableWindow(g_chkSpan, TRUE);
            EnableWindow(g_chkTrace, TRUE);
            EnableWindow(g_editSemantic, TRUE);
            
            PopulateTreeView(g_hwndTreeView);
            
            if (g_solver_thread.joinable()) {
                g_solver_thread.join();
            }
            break;
        }
        
        case WM_DESTROY: {
            if (g_solver_thread.joinable()) {
                g_solver_thread.join();
            }
            PostQuitMessage(0);
            break;
        }
        
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Parse Command Line Arguments in Unicode
    int nArgs = 0;
    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    std::vector<std::string> argsUtf8;
    std::vector<char*> argvUtf8;
    
    if (szArglist) {
        for (int i = 0; i < nArgs; ++i) {
            std::string utf8 = wstringToUtf8(szArglist[i]);
            argsUtf8.push_back(utf8);
        }
        for (size_t i = 0; i < argsUtf8.size(); ++i) {
            argvUtf8.push_back(&argsUtf8[i][0]);
        }
        argvUtf8.push_back(nullptr);
    }
    
    int c_argc = (int)argsUtf8.size();
    char** c_argv = argvUtf8.data();
    
    // 2. Check if CLI mode is triggered
    if (c_argc > 1 && bttb::isCliModeTriggered(c_argc, c_argv)) {
        // Attach console to the parent command line prompt
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            // Redirect standard stream handles
            FILE* fp;
            freopen_s(&fp, "CONOUT$", "w", stdout);
            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
            std::ios_base::sync_with_stdio();
        }
        int status = bttb::runCliEngine(c_argc, c_argv);
        if (szArglist) LocalFree(szArglist);
        return status;
    }
    
    // 3. Detect initial folder path passed via arguments (e.g. context menus)
    if (szArglist && nArgs > 1) {
        std::wstring firstArg = szArglist[1];
        if (!firstArg.empty() && firstArg[0] != L'-') {
            g_folderToAdd = firstArg;
        }
    }
    if (szArglist) LocalFree(szArglist);
    
    // 4. Single-Instance Named Mutex verification
    g_hMutex = CreateMutexW(NULL, FALSE, L"Local\\BTTB_SingleInstanceMutex");
    if (g_hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
        // Find existing instance window
        HWND hwndExisting = FindWindow("BttbWin32GUI", NULL);
        if (hwndExisting) {
            if (!g_folderToAdd.empty()) {
                COPYDATASTRUCT cds;
                cds.dwData = 1; // "add folder" custom action code
                cds.cbData = (DWORD)((g_folderToAdd.size() + 1) * sizeof(wchar_t));
                cds.lpData = (void*)g_folderToAdd.c_str();
                SendMessageW(hwndExisting, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
            } else {
                ShowWindow(hwndExisting, SW_RESTORE);
                SetForegroundWindow(hwndExisting);
            }
        }
        if (g_hMutex) CloseHandle(g_hMutex);
        return 0; // Terminate this secondary instance
    }

    // Register Main Window Class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "BttbWin32GUI";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wc.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Register ISO Dialog Window Class
    WNDCLASSEX wcIso = {0};
    wcIso.cbSize = sizeof(WNDCLASSEX);
    wcIso.style = CS_HREDRAW | CS_VREDRAW;
    wcIso.lpfnWndProc = IsoWndProc;
    wcIso.hInstance = hInstance;
    wcIso.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcIso.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcIso.lpszClassName = "BttbWin32ISODialog";
    wcIso.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wcIso.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    
    if (!RegisterClassEx(&wcIso)) {
        MessageBox(NULL, "ISO Dialog Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Register Preferences Dialog Window Class
    WNDCLASSEX wcPref = {0};
    wcPref.cbSize = sizeof(WNDCLASSEX);
    wcPref.style = CS_HREDRAW | CS_VREDRAW;
    wcPref.lpfnWndProc = PrefWndProc;
    wcPref.hInstance = hInstance;
    wcPref.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcPref.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcPref.lpszClassName = "BttbWin32PrefDialog";
    wcPref.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wcPref.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    
    if (!RegisterClassEx(&wcPref)) {
        MessageBox(NULL, "Preferences Dialog Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Register Folder List Dialog Window Class
    WNDCLASSEX wcFolderList = {0};
    wcFolderList.cbSize = sizeof(WNDCLASSEX);
    wcFolderList.style = CS_HREDRAW | CS_VREDRAW;
    wcFolderList.lpfnWndProc = FolderListWndProc;
    wcFolderList.hInstance = hInstance;
    wcFolderList.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcFolderList.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcFolderList.lpszClassName = "BttbWin32FolderListDialog";
    wcFolderList.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(1));
    wcFolderList.hIconSm = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    
    if (!RegisterClassEx(&wcFolderList)) {
        MessageBox(NULL, "Folder List Dialog Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create Main Window (width=830, height=540 to fit results TreeView beautifully!)
    g_hwndMain = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        "BttbWin32GUI",
        "Burn to the Brim (Native Win32 GUI)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 830, 565,
        NULL, NULL, hInstance, NULL
    );
    
    if (g_hwndMain == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);
    
    // Message Loop
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    if (g_hMutex) CloseHandle(g_hMutex);
    return Msg.wParam;
}
