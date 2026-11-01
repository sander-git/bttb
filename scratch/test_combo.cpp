#include <windows.h>
#include <iostream>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        // Create a combobox using CreateWindowExW
        HWND hCombo = CreateWindowExW(0, L"COMBOBOX", L"", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 
                                      10, 10, 200, 200, hwnd, NULL, NULL, NULL);
        std::cout << "Is parent Unicode: " << IsWindowUnicode(hwnd) << std::endl;
        std::cout << "Is combo Unicode: " << IsWindowUnicode(hCombo) << std::endl;
        
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Franais\u00e7ois"); // "Franaançois" with ç
        SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Fran\u00e7ais"); // Français
        
        // Let's get the text back to see if it's correct
        wchar_t buf[256] = {0};
        SendMessageW(hCombo, CB_GETLBTEXT, 1, (LPARAM)buf);
        std::wcout << L"Text: " << buf << std::endl;
        for (int i = 0; buf[i] != 0; ++i) {
            std::cout << i << ": " << (unsigned int)buf[i] << std::endl;
        }
        
        PostQuitMessage(0);
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int main() {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"TestClass";
    RegisterClassExW(&wc);
    
    HWND hwnd = CreateWindowExW(0, L"TestClass", L"Test", WS_OVERLAPPEDWINDOW, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, NULL, NULL);
    return 0;
}
