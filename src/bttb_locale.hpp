#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <vector>

class BttbLocale {
public:
    static BttbLocale& getInstance() {
        static BttbLocale instance;
        return instance;
    }

    void load(const std::string& langCode);
    std::string get(const std::string& key, const std::string& fallback);
    std::string getLanguage() const { return currentLang; }
    
    // Detect system language
    std::string detectSystemLanguage();

#ifdef _WIN32
    // Convert UTF-8 to active ANSI code page
    static std::string utf8ToAnsi(const std::string& utf8);
#endif

private:
    BttbLocale() = default;
    std::unordered_map<std::string, std::string> translations;
    std::string currentLang = "en";
};

#define _T(key, fallback) BttbLocale::getInstance().get(key, fallback)
