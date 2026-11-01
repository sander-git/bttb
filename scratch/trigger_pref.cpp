#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // Wait for the window to appear
    HWND hwnd = NULL;
    for (int i = 0; i < 20; ++i) {
        hwnd = FindWindowW(L"BttbWin32GUI", NULL);
        if (hwnd) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    if (!hwnd) {
        std::cerr << "Could not find main window!" << std::endl;
        return 1;
    }
    std::cout << "Found main window: " << hwnd << ". Sending ID_BTN_PREFS (1010)..." << std::endl;
    PostMessageW(hwnd, WM_COMMAND, 1010, 0);
    
    // Now wait for the MessageBox to appear (class is usually "#32770")
    HWND hwndMsg = NULL;
    for (int i = 0; i < 20; ++i) {
        hwndMsg = FindWindowW(L"#32770", NULL);
        if (hwndMsg) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    if (!hwndMsg) {
        std::cerr << "Could not find debug MessageBox!" << std::endl;
        // Let's close the main window
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
        return 1;
    }
    std::cout << "Found MessageBox! Title/text check..." << std::endl;
    // Get text of MessageBox
    wchar_t title[256] = {0};
    GetWindowTextW(hwndMsg, title, 256);
    std::wcout << L"MessageBox Title: " << title << std::endl;
    
    // Get static text (usually control ID 0xFFFF or similar)
    HWND hwndStatic = FindWindowExW(hwndMsg, NULL, L"STATIC", NULL);
    if (hwndStatic) {
        wchar_t text[256] = {0};
        GetWindowTextW(hwndStatic, text, 256);
        std::wcout << L"MessageBox Text: " << text << std::endl;
    }
    
    // Dismiss MessageBox
    SendMessageW(hwndMsg, WM_COMMAND, IDOK, 0);
    
    // Now find preferences window and close it too
    HWND hwndPref = NULL;
    for (int i = 0; i < 20; ++i) {
        hwndPref = FindWindowW(L"BttbWin32PrefDialog", NULL);
        if (hwndPref) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    if (hwndPref) {
        std::cout << "Closing preferences dialog..." << std::endl;
        PostMessageW(hwndPref, WM_CLOSE, 0, 0);
    }
    
    // Close main window
    PostMessageW(hwnd, WM_CLOSE, 0, 0);
    return 0;
}
