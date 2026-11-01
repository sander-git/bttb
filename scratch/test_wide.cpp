#include <iostream>
#include <string.h>
#include <wchar.h>

int main() {
    const wchar_t* s = L"Français";
    std::cout << "Len: " << wcslen(s) << std::endl;
    for (int i = 0; s[i] != 0; ++i) {
        std::cout << i << ": " << (unsigned int)s[i] << std::endl;
    }
    return 0;
}
