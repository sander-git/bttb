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
#define ID_COMBO_MEDIA     1005
#define ID_CHK_MOVE        1006
#define ID_BTN_CREATE_ISO  1007
#define ID_CHK_EMPTY       1008

// ISO Dialog Control IDs
#define ID_BTN_ISO_SRC_BROWSE  2001
#define ID_BTN_ISO_PATH_BROWSE 2002
#define ID_BTN_ISO_GENERATE    2003
#define ID_BTN_ISO_CLOSE       2004

// Global State
HWND g_hwndMain = NULL;
HWND g_editSrc = NULL;
HWND g_editDest = NULL;
HWND g_chkMove = NULL;
HWND g_comboMedia = NULL;
HWND g_editCap = NULL;
HWND g_editClus = NULL;
HWND g_editSlack = NULL;
HWND g_editDepth = NULL;
HWND g_chkEmpty = NULL;
HWND g_editLog = NULL;
HWND g_progress = NULL;
HWND g_btnStart = NULL;
HWND g_btnStop = NULL;
HWND g_btnCreateIso = NULL;
HWND g_labelProgress = NULL;

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

// Window Procedure for Main Window
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            // Initialize Common Controls for Progress Bar
            INITCOMMONCONTROLSEX icex;
            icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
            icex.dwICC = ICC_PROGRESS_CLASS;
            InitCommonControlsEx(&icex);
            
            // Layout margins and variables
            int y = 16;
            
            // Group 1: Folders Selection
            CreateWindow("BUTTON", "Directories setup", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 115, hwnd, NULL, NULL, NULL);
            
            CreateWindow("STATIC", "Source folder:", WS_CHILD | WS_VISIBLE, 24, y + 26, 100, 20, hwnd, NULL, NULL, NULL);
            g_editSrc = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 130, y + 24, 270, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y + 22, 90, 25, hwnd, (HMENU)ID_BTN_SRC_BROWSE, NULL, NULL);
            
            CreateWindow("STATIC", "Target folder:", WS_CHILD | WS_VISIBLE, 24, y + 58, 100, 20, hwnd, NULL, NULL, NULL);
            g_editDest = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 130, y + 56, 270, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y + 54, 90, 25, hwnd, (HMENU)ID_BTN_DEST_BROWSE, NULL, NULL);
            
            g_chkMove = CreateWindow("BUTTON", "Move/organize fitted folders/files to target folder", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, y + 84, 350, 20, hwnd, (HMENU)ID_CHK_MOVE, NULL, NULL);
            
            y += 130;
            
            // Group 2: Media and Algorithm Settings
            CreateWindow("BUTTON", "Medium & Solver settings", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 140, hwnd, NULL, NULL, NULL);
            
            CreateWindow("STATIC", "Select Medium:", WS_CHILD | WS_VISIBLE, 24, y + 26, 100, 20, hwnd, NULL, NULL, NULL);
            g_comboMedia = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 130, y + 24, 120, 150, hwnd, (HMENU)ID_COMBO_MEDIA, NULL, NULL);
            SendMessage(g_comboMedia, CB_ADDSTRING, 0, (LPARAM)"CD (650 MB)");
            SendMessage(g_comboMedia, CB_ADDSTRING, 0, (LPARAM)"CD (700 MB)");
            SendMessage(g_comboMedia, CB_ADDSTRING, 0, (LPARAM)"DVD (4.7 GB)");
            SendMessage(g_comboMedia, CB_ADDSTRING, 0, (LPARAM)"DVD DL (8.5 GB)");
            SendMessage(g_comboMedia, CB_ADDSTRING, 0, (LPARAM)"Custom Capacity");
            SendMessage(g_comboMedia, CB_SETCURSEL, 1, 0); // Default CD 700MB
            
            CreateWindow("STATIC", "Capacity (Bytes):", WS_CHILD | WS_VISIBLE, 270, y + 26, 110, 20, hwnd, NULL, NULL, NULL);
            g_editCap = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "734003200", WS_CHILD | WS_VISIBLE | ES_NUMBER, 380, y + 24, 120, 22, hwnd, NULL, NULL, NULL);
            EnableWindow(g_editCap, FALSE);
            
            CreateWindow("STATIC", "Cluster (Bytes):", WS_CHILD | WS_VISIBLE, 24, y + 60, 100, 20, hwnd, NULL, NULL, NULL);
            g_editClus = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "2048", WS_CHILD | WS_VISIBLE | ES_NUMBER, 130, y + 58, 120, 22, hwnd, NULL, NULL, NULL);
            
            CreateWindow("STATIC", "Slack (Bytes):", WS_CHILD | WS_VISIBLE, 270, y + 60, 110, 20, hwnd, NULL, NULL, NULL);
            g_editSlack = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER, 380, y + 58, 120, 22, hwnd, NULL, NULL, NULL);
            
            CreateWindow("STATIC", "Split depth:", WS_CHILD | WS_VISIBLE, 24, y + 94, 100, 20, hwnd, NULL, NULL, NULL);
            g_editDepth = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER, 130, y + 92, 120, 22, hwnd, NULL, NULL, NULL);
            
            g_chkEmpty = CreateWindow("BUTTON", "Skip empty files/folders", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 270, y + 92, 230, 20, hwnd, (HMENU)ID_CHK_EMPTY, NULL, NULL);
            SendMessage(g_chkEmpty, BM_SETCHECK, BST_CHECKED, 0);
            
            y += 155;
            
            // Group 3: Progress
            CreateWindow("BUTTON", "Fitted Capacity Status", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 60, hwnd, NULL, NULL, NULL);
            g_progress = CreateWindow(PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 24, y + 24, 350, 20, hwnd, NULL, NULL, NULL);
            g_labelProgress = CreateWindow("STATIC", "Filled: 0.00%", WS_CHILD | WS_VISIBLE, 385, y + 26, 120, 20, hwnd, NULL, NULL, NULL);
            
            y += 75;
            
            // Group 4: Log Output
            CreateWindow("BUTTON", "Solver Output Log", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 140, hwnd, NULL, NULL, NULL);
            g_editLog = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 24, y + 24, 480, 100, hwnd, NULL, NULL, NULL);
            
            y += 155;
            
            // Control Action Buttons
            g_btnStart = CreateWindow("BUTTON", "Start Optimal Solving", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 40, y, 170, 30, hwnd, (HMENU)ID_BTN_START, NULL, NULL);
            g_btnStop = CreateWindow("BUTTON", "Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 220, y, 80, 30, hwnd, (HMENU)ID_BTN_STOP, NULL, NULL);
            g_btnCreateIso = CreateWindow("BUTTON", "Create ISO image...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 310, y, 190, 30, hwnd, (HMENU)ID_BTN_CREATE_ISO, NULL, NULL);
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
            
            if (wmId == ID_COMBO_MEDIA && wmEvent == CBN_SELCHANGE) {
                int index = SendMessage(g_comboMedia, CB_GETCURSEL, 0, 0);
                if (index == 0) { // CD 650
                    SetWindowText(g_editCap, "681574400");
                    EnableWindow(g_editCap, FALSE);
                } else if (index == 1) { // CD 700
                    SetWindowText(g_editCap, "734003200");
                    EnableWindow(g_editCap, FALSE);
                } else if (index == 2) { // DVD 4.7
                    SetWindowText(g_editCap, "4700000000");
                    EnableWindow(g_editCap, FALSE);
                } else if (index == 3) { // DVD DL 8.5
                    SetWindowText(g_editCap, "8500000000");
                    EnableWindow(g_editCap, FALSE);
                } else { // Custom
                    EnableWindow(g_editCap, TRUE);
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
            
            if (wmId == ID_BTN_START) {
                char src[MAX_PATH];
                char dest[MAX_PATH];
                GetWindowText(g_editSrc, src, MAX_PATH);
                GetWindowText(g_editDest, dest, MAX_PATH);
                
                if (strlen(src) == 0) {
                    MessageBox(hwnd, "Please select a valid source folder first.", "Error", MB_ICONERROR);
                    break;
                }
                
                // Configure Solver variables
                g_solver.sourceDirectory = src;
                g_solver.targetDirectory = dest;
                g_solver.moveFiles = (IsDlgButtonChecked(hwnd, ID_CHK_MOVE) == BST_CHECKED);
                g_solver.skipEmpty = (IsDlgButtonChecked(hwnd, ID_CHK_EMPTY) == BST_CHECKED);
                
                char buf[32];
                GetWindowText(g_editCap, buf, 32);
                g_solver.mediumInfo.capacityBytes = std::stoll(buf);
                
                GetWindowText(g_editClus, buf, 32);
                g_solver.mediumInfo.sectorSize = std::stoll(buf);
                
                GetWindowText(g_editSlack, buf, 32);
                g_solver.mediumInfo.slackBytes = std::stoll(buf);
                
                GetWindowText(g_editDepth, buf, 32);
                g_solver.splitDepth = std::stoi(buf);
                
                // Setup logs
                SetWindowText(g_editLog, "");
                AppendTextToLog(g_editLog, "Starting solver background calculations...");
                
                // Disable UI inputs
                EnableWindow(g_btnStart, FALSE);
                EnableWindow(g_btnStop, TRUE);
                
                // Wire thread safe UI messages
                g_solver.logNotify = [hwnd](const std::string& msg, int type) {
                    auto* pStr = new std::string(msg);
                    PostMessage(hwnd, WM_SOLVER_LOG, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(pStr));
                };
                
                g_solver.progressNotify = [hwnd](double disc, double overall) {
                    int percent = static_cast<int>(disc * 10000.0); // 2 decimal places precision
                    PostMessage(hwnd, WM_SOLVER_PROGRESS, static_cast<WPARAM>(percent), 0);
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
            
            // Re-enable start
            EnableWindow(g_btnStart, TRUE);
            EnableWindow(g_btnStop, FALSE);
            
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
    // Register Main Window Class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = "BttbWin32GUI";
    
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
    
    if (!RegisterClassEx(&wcIso)) {
        MessageBox(NULL, "ISO Dialog Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create Main Window
    g_hwndMain = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        "BttbWin32GUI",
        "Burn to the Brim (Native Win32 GUI)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 550, 600,
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
    return Msg.wParam;
}
