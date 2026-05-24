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
#define ID_CHK_MOVE        1006
#define ID_BTN_CREATE_ISO  1007
#define ID_CHK_SPAN        1009
#define ID_BTN_PREFS       1010
#define ID_CHK_TRACE       1011

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

// Global State
HWND g_hwndMain = NULL;
HWND g_editSrc = NULL;
HWND g_editDest = NULL;
HWND g_chkMove = NULL;
HWND g_chkSpan = NULL;
HWND g_chkTrace = NULL;
HWND g_editLog = NULL;
HWND g_progress = NULL;
HWND g_btnStart = NULL;
HWND g_btnStop = NULL;
HWND g_btnCreateIso = NULL;
HWND g_labelProgress = NULL;

// Preferences Dialog Global Controls
HWND g_hwndPref = NULL;
HWND g_comboPrefMedia = NULL;
HWND g_editPrefCap = NULL;
HWND g_editPrefClus = NULL;
HWND g_editPrefSlack = NULL;
HWND g_editPrefTime = NULL;
HWND g_editPrefDepth = NULL;
HWND g_chkPrefEmpty = NULL;
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
            SendMessage(g_comboPrefMedia, CB_ADDSTRING, 0, (LPARAM)"Custom Size");
            
            CreateWindow("STATIC", "Capacity (Bytes):", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefCap = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_CAP, NULL, NULL);
            
            y += 30;
            // Row 2: Cluster & Slack
            CreateWindow("STATIC", "Cluster (Bytes):", WS_CHILD | WS_VISIBLE, 20, y + 2, 120, 20, hwnd, NULL, NULL, NULL);
            g_editPrefClus = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 150, y, 120, 22, hwnd, (HMENU)ID_PREF_EDIT_CLUS, NULL, NULL);
            
            CreateWindow("STATIC", "Slack (Bytes):", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefSlack = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_SLACK, NULL, NULL);
            
            y += 30;
            // Row 3: Max Search Time & Split Depth
            CreateWindow("STATIC", "Max Search Time (s):", WS_CHILD | WS_VISIBLE, 20, y + 2, 130, 20, hwnd, NULL, NULL, NULL);
            g_editPrefTime = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 150, y, 120, 22, hwnd, (HMENU)ID_PREF_EDIT_TIME, NULL, NULL);
            
            CreateWindow("STATIC", "Split depth:", WS_CHILD | WS_VISIBLE, 280, y + 2, 80, 20, hwnd, NULL, NULL, NULL);
            g_editPrefDepth = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_NUMBER, 370, y, 100, 22, hwnd, (HMENU)ID_PREF_EDIT_DEPTH, NULL, NULL);
            
            y += 30;
            // Row 4: Skip Empty
            g_chkPrefEmpty = CreateWindow("BUTTON", "Skip empty files/folders", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 150, y, 320, 20, hwnd, (HMENU)ID_PREF_CHK_EMPTY, NULL, NULL);
            
            y += 30;
            // Group: Grouping Rules
            CreateWindow("BUTTON", "File/Folder Grouping Rules", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 460, 260, hwnd, NULL, NULL, NULL);
            
            // Rules ListView
            g_listPrefRules = CreateWindowEx(0, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | WS_BORDER, 24, y + 20, 436, 110, hwnd, (HMENU)ID_PREF_LIST_RULES, NULL, NULL);
            // Add columns
            {
                LVCOLUMN lvc;
                lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
                
                lvc.pszText = (LPSTR)"Pattern"; lvc.cx = 200; ListView_InsertColumn(g_listPrefRules, 0, &lvc);
                lvc.pszText = (LPSTR)"Files"; lvc.cx = 60; ListView_InsertColumn(g_listPrefRules, 1, &lvc);
                lvc.pszText = (LPSTR)"Folders"; lvc.cx = 60; ListView_InsertColumn(g_listPrefRules, 2, &lvc);
                lvc.pszText = (LPSTR)"Type"; lvc.cx = 70; ListView_InsertColumn(g_listPrefRules, 3, &lvc);
            }
            // Set extended style for full row select
            ListView_SetExtendedListViewStyle(g_listPrefRules, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
            
            // Rule inputs inside group box
            CreateWindow("STATIC", "Pattern:", WS_CHILD | WS_VISIBLE, 24, y + 142, 60, 20, hwnd, NULL, NULL, NULL);
            g_editPrefPattern = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 90, y + 140, 370, 22, hwnd, (HMENU)ID_PREF_EDIT_PATTERN, NULL, NULL);
            
            g_chkPrefFiles = CreateWindow("BUTTON", "Match Files", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 90, y + 170, 100, 20, hwnd, (HMENU)ID_PREF_CHK_FILES, NULL, NULL);
            SendMessage(g_chkPrefFiles, BM_SETCHECK, BST_CHECKED, 0);
            
            g_chkPrefFolders = CreateWindow("BUTTON", "Match Folders", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 200, y + 170, 110, 20, hwnd, (HMENU)ID_PREF_CHK_FOLDERS, NULL, NULL);
            SendMessage(g_chkPrefFolders, BM_SETCHECK, BST_CHECKED, 0);
            
            g_chkPrefRegex = CreateWindow("BUTTON", "Regex Pattern", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 320, y + 170, 110, 20, hwnd, (HMENU)ID_PREF_CHK_REGEX, NULL, NULL);
            
            g_btnPrefAddRule = CreateWindow("BUTTON", "Add Rule", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, y + 200, 100, 25, hwnd, (HMENU)ID_PREF_BTN_ADD_RULE, NULL, NULL);
            g_btnPrefDelRule = CreateWindow("BUTTON", "Remove Selected", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 200, y + 200, 150, 25, hwnd, (HMENU)ID_PREF_BTN_DEL_RULE, NULL, NULL);
            
            y += 280;
            // OK / Cancel Action Buttons
            CreateWindow("BUTTON", "OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 250, y, 100, 30, hwnd, (HMENU)ID_PREF_BTN_OK, NULL, NULL);
            CreateWindow("BUTTON", "Cancel", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 360, y, 100, 30, hwnd, (HMENU)ID_PREF_BTN_CANCEL, NULL, NULL);
            
            // Set Font
            HFONT hFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "MS Shell Dlg");
            EnumChildWindows(hwnd, [](HWND hwndChild, LPARAM lParam) -> BOOL {
                SendMessage(hwndChild, WM_SETFONT, lParam, TRUE);
                return TRUE;
            }, (LPARAM)hFont);
            
            // Load Solver Settings into Controls
            SetWindowText(g_editPrefCap, std::to_string(g_solver.mediumInfo.capacityBytes).c_str());
            SetWindowText(g_editPrefClus, std::to_string(g_solver.mediumInfo.sectorSize).c_str());
            SetWindowText(g_editPrefSlack, std::to_string(g_solver.mediumInfo.slackBytes).c_str());
            SetWindowText(g_editPrefTime, std::to_string(g_solver.maxSearchTimeSeconds).c_str());
            SetWindowText(g_editPrefDepth, std::to_string(g_solver.splitDepth).c_str());
            SendMessage(g_chkPrefEmpty, BM_SETCHECK, g_solver.skipEmpty ? BST_CHECKED : BST_UNCHECKED, 0);
            
            // Select correct Media combobox index
            int index = 10; // Custom Size
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
            
            SendMessage(g_comboPrefMedia, CB_SETCURSEL, index, 0);
            EnableWindow(g_editPrefCap, (index == 10) ? TRUE : FALSE);
            
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
            
            if (wmId == ID_PREF_COMBO_MEDIA && wmEvent == CBN_SELCHANGE) {
                int index = SendMessage(g_comboPrefMedia, CB_GETCURSEL, 0, 0);
                if (index == 0) { // CD 650
                    SetWindowText(g_editPrefCap, "681574400");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 1) { // CD 700
                    SetWindowText(g_editPrefCap, "734003200");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 2) { // DVD 4.7
                    SetWindowText(g_editPrefCap, "4700000000");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 3) { // DVD DL 8.5
                    SetWindowText(g_editPrefCap, "8500000000");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 4) { // BD 25
                    SetWindowText(g_editPrefCap, "25000000000");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 5) { // BD DL 50
                    SetWindowText(g_editPrefCap, "50000000000");
                    SetWindowText(g_editPrefClus, "2048");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 6) { // USB 8
                    SetWindowText(g_editPrefCap, "8000000000");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 7) { // USB 16
                    SetWindowText(g_editPrefCap, "16000000000");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 8) { // USB 32
                    SetWindowText(g_editPrefCap, "32000000000");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else if (index == 9) { // USB 64
                    SetWindowText(g_editPrefCap, "64000000000");
                    SetWindowText(g_editPrefClus, "4096");
                    EnableWindow(g_editPrefCap, FALSE);
                } else { // Custom
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
                // Save settings back to g_solver
                char buf[64];
                
                GetWindowText(g_editPrefCap, buf, 64);
                g_solver.mediumInfo.capacityBytes = std::stoll(buf);
                
                GetWindowText(g_editPrefClus, buf, 64);
                g_solver.mediumInfo.sectorSize = std::stoll(buf);
                
                GetWindowText(g_editPrefSlack, buf, 64);
                g_solver.mediumInfo.slackBytes = std::stoll(buf);
                
                GetWindowText(g_editPrefTime, buf, 64);
                g_solver.maxSearchTimeSeconds = std::stoi(buf);
                
                GetWindowText(g_editPrefDepth, buf, 64);
                g_solver.splitDepth = std::stoi(buf);
                
                g_solver.skipEmpty = (SendMessage(g_chkPrefEmpty, BM_GETCHECK, 0, 0) == BST_CHECKED);
                
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
            CreateWindow("BUTTON", "Directories setup", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 160, hwnd, NULL, NULL, NULL);
            
            CreateWindow("STATIC", "Source folder:", WS_CHILD | WS_VISIBLE, 24, y + 26, 100, 20, hwnd, NULL, NULL, NULL);
            g_editSrc = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 130, y + 24, 270, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y + 22, 90, 25, hwnd, (HMENU)ID_BTN_SRC_BROWSE, NULL, NULL);
            
            CreateWindow("STATIC", "Target folder:", WS_CHILD | WS_VISIBLE, 24, y + 58, 100, 20, hwnd, NULL, NULL, NULL);
            g_editDest = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 130, y + 56, 270, 22, hwnd, NULL, NULL, NULL);
            CreateWindow("BUTTON", "Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 410, y + 54, 90, 25, hwnd, (HMENU)ID_BTN_DEST_BROWSE, NULL, NULL);
            
            g_chkMove = CreateWindow("BUTTON", "Move/organize fitted folders/files to target folder", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, y + 84, 350, 20, hwnd, (HMENU)ID_CHK_MOVE, NULL, NULL);
            g_chkSpan = CreateWindow("BUTTON", "Span across multiple volumes (Volume_1, Volume_2, etc.)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, y + 106, 360, 20, hwnd, (HMENU)ID_CHK_SPAN, NULL, NULL);
            g_chkTrace = CreateWindow("BUTTON", "Enable detailed solver diagnostic tracing (Trace)", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 130, y + 128, 360, 20, hwnd, (HMENU)ID_CHK_TRACE, NULL, NULL);
            
            y += 175;
            
            // Group 2: Progress
            CreateWindow("BUTTON", "Fitted Capacity Status", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 60, hwnd, NULL, NULL, NULL);
            g_progress = CreateWindow(PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE, 24, y + 24, 350, 20, hwnd, NULL, NULL, NULL);
            g_labelProgress = CreateWindow("STATIC", "Filled: 0.00%", WS_CHILD | WS_VISIBLE, 385, y + 26, 120, 20, hwnd, NULL, NULL, NULL);
            
            y += 75;
            
            // Group 3: Log Output
            CreateWindow("BUTTON", "Solver Output Log", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, y, 510, 160, hwnd, NULL, NULL, NULL);
            g_editLog = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_READONLY, 24, y + 24, 480, 120, hwnd, NULL, NULL, NULL);
            
            y += 175;
            
            // Control Action Buttons
            g_btnStart = CreateWindow("BUTTON", "Start Optimal Solving", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, y, 160, 30, hwnd, (HMENU)ID_BTN_START, NULL, NULL);
            g_btnStop = CreateWindow("BUTTON", "Stop", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 190, y, 70, 30, hwnd, (HMENU)ID_BTN_STOP, NULL, NULL);
            CreateWindow("BUTTON", "Preferences...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, y, 110, 30, hwnd, (HMENU)ID_BTN_PREFS, NULL, NULL);
            g_btnCreateIso = CreateWindow("BUTTON", "Create ISO image...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 390, y, 130, 30, hwnd, (HMENU)ID_BTN_CREATE_ISO, NULL, NULL);
            
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
                g_solver.spanMultipleVolumes = (IsDlgButtonChecked(hwnd, ID_CHK_SPAN) == BST_CHECKED);
                g_solver.enableTrace = (IsDlgButtonChecked(hwnd, ID_CHK_TRACE) == BST_CHECKED);
                
                // Setup logs
                SetWindowText(g_editLog, "");
                AppendTextToLog(g_editLog, "Starting solver background calculations...");
                
                // Disable UI inputs
                EnableWindow(g_btnStart, FALSE);
                EnableWindow(g_btnStop, TRUE);
                EnableWindow(g_chkMove, FALSE);
                EnableWindow(g_chkSpan, FALSE);
                EnableWindow(g_chkTrace, FALSE);
                
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
            EnableWindow(g_btnStop, FALSE);
            EnableWindow(g_chkMove, TRUE);
            EnableWindow(g_chkSpan, TRUE);
            EnableWindow(g_chkTrace, TRUE);
            
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
    
    // Register Preferences Dialog Window Class
    WNDCLASSEX wcPref = {0};
    wcPref.cbSize = sizeof(WNDCLASSEX);
    wcPref.style = CS_HREDRAW | CS_VREDRAW;
    wcPref.lpfnWndProc = PrefWndProc;
    wcPref.hInstance = hInstance;
    wcPref.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcPref.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wcPref.lpszClassName = "BttbWin32PrefDialog";
    
    if (!RegisterClassEx(&wcPref)) {
        MessageBox(NULL, "Preferences Dialog Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    // Create Main Window
    g_hwndMain = CreateWindowEx(
        WS_EX_CONTROLPARENT,
        "BttbWin32GUI",
        "Burn to the Brim (Native Win32 GUI)",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 550, 520, // Clean layout is much more compact!
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
