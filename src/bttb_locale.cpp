#include "bttb_locale.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

std::string BttbLocale::detectSystemLanguage() {
#ifdef _WIN32
    LANGID langId = GetUserDefaultUILanguage();
    WORD lang = PRIMARYLANGID(langId);
    if (lang == LANG_GERMAN) return "de";
    if (lang == LANG_DUTCH) return "nl";
    if (lang == LANG_FRENCH) return "fr";
    if (lang == LANG_SPANISH) return "es";
#else
    const char* langEnv = std::getenv("LANG");
    if (!langEnv) langEnv = std::getenv("LC_ALL");
    if (!langEnv) langEnv = std::getenv("LC_MESSAGES");
    if (langEnv) {
        std::string l(langEnv);
        if (l.length() >= 2) {
            std::string code = l.substr(0, 2);
            if (code == "de" || code == "nl" || code == "fr" || code == "es") {
                return code;
            }
        }
    }
#endif
    return "en";
}

static std::string parsePoString(const std::string& line) {
    if (line.length() < 2 || line.front() != '"' || line.back() != '"') {
        return "";
    }
    std::string inner = line.substr(1, line.length() - 2);
    std::string res;
    res.reserve(inner.length());
    for (size_t i = 0; i < inner.length(); ++i) {
        if (inner[i] == '\\' && i + 1 < inner.length()) {
            char next = inner[i + 1];
            if (next == 'n') {
                res += '\n';
                i++;
            } else if (next == 'r') {
                res += '\r';
                i++;
            } else if (next == 't') {
                res += '\t';
                i++;
            } else if (next == '\\') {
                res += '\\';
                i++;
            } else if (next == '"') {
                res += '"';
                i++;
            } else {
                res += '\\';
            }
        } else {
            res += inner[i];
        }
    }
    return res;
}

void BttbLocale::load(const std::string& langCode) {
    translations.clear();
    currentLang = langCode;
    if (langCode == "en") {
        return; // Use English fallbacks directly
    }

    std::filesystem::path langFile = langCode + ".po";
    std::vector<std::filesystem::path> searchPaths;

#ifdef _WIN32
    wchar_t szPath[MAX_PATH];
    if (GetModuleFileNameW(NULL, szPath, MAX_PATH)) {
        std::filesystem::path exeDir = std::filesystem::path(szPath).parent_path();
        searchPaths.push_back(exeDir / "lang" / langFile);
        searchPaths.push_back(exeDir / langFile);
        searchPaths.push_back(exeDir / ".." / "lang" / langFile);
    }
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        std::filesystem::path exeDir = std::filesystem::path(std::string(result, count)).parent_path();
        searchPaths.push_back(exeDir / "lang" / langFile);
        searchPaths.push_back(exeDir / langFile);
        searchPaths.push_back(exeDir / ".." / "lang" / langFile);
        searchPaths.push_back(exeDir / ".." / "share" / "bttb" / "lang" / langFile);
    }
    searchPaths.push_back(std::filesystem::path("/usr/share/bttb/lang") / langFile);
#endif

    searchPaths.push_back(std::filesystem::current_path() / "lang" / langFile);
    searchPaths.push_back(std::filesystem::current_path() / langFile);
    searchPaths.push_back(std::filesystem::current_path() / ".." / "lang" / langFile);

    std::filesystem::path foundPath;
    for (const auto& p : searchPaths) {
        if (std::filesystem::exists(p)) {
            foundPath = p;
            break;
        }
    }

    if (foundPath.empty()) {
        std::cout << "[LOCALE] ERROR: No .po file found for language: " << langCode << std::endl;
        for (const auto& p : searchPaths) {
            std::cout << "  Searched: " << p << " (exists: " << std::filesystem::exists(p) << ")" << std::endl;
        }
        return; // Fallback: translations map remains empty, so fallback English string is returned
    }

    std::cout << "[LOCALE] SUCCESS: Found .po file at: " << foundPath << std::endl;

    std::ifstream infile(foundPath);
    if (!infile.is_open()) {
        std::cout << "[LOCALE] ERROR: Could not open file: " << foundPath << std::endl;
        return;
    }

    std::string line;
    std::string currentMsgId;
    std::string currentMsgStr;
    enum State { STATE_NONE, STATE_MSGID, STATE_MSGSTR };
    State state = STATE_NONE;

    while (std::getline(infile, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        size_t first = line.find_first_not_of(" \t");
        if (first == std::string::npos) {
            continue; // Empty line
        }
        std::string trimmed = line.substr(first);
        if (trimmed.front() == '#') {
            continue; // Comment line
        }

        if (trimmed.rfind("msgid", 0) == 0) {
            if (!currentMsgId.empty()) {
#ifdef _WIN32
                translations[currentMsgId] = utf8ToAnsi(currentMsgStr);
#else
                translations[currentMsgId] = currentMsgStr;
#endif
            }
            currentMsgId.clear();
            currentMsgStr.clear();
            
            std::string content = trimmed.substr(5);
            size_t c_first = content.find_first_not_of(" \t");
            if (c_first != std::string::npos) {
                currentMsgId = parsePoString(content.substr(c_first));
            }
            state = STATE_MSGID;
        } else if (trimmed.rfind("msgstr", 0) == 0) {
            std::string content = trimmed.substr(6);
            size_t c_first = content.find_first_not_of(" \t");
            if (c_first != std::string::npos) {
                currentMsgStr = parsePoString(content.substr(c_first));
            }
            state = STATE_MSGSTR;
        } else if (trimmed.front() == '"') {
            std::string part = parsePoString(trimmed);
            if (state == STATE_MSGID) {
                currentMsgId += part;
            } else if (state == STATE_MSGSTR) {
                currentMsgStr += part;
            }
        }
    }
    if (!currentMsgId.empty()) {
#ifdef _WIN32
        translations[currentMsgId] = utf8ToAnsi(currentMsgStr);
#else
        translations[currentMsgId] = currentMsgStr;
#endif
    }
}

#ifdef _WIN32
std::string BttbLocale::utf8ToAnsi(const std::string& utf8) {
    if (utf8.empty()) return "";
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    if (wlen <= 0) return utf8;
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
    if (wstr.back() == L'\0') wstr.pop_back();

    int alen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (alen <= 0) return utf8;
    std::string ansi(alen, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &ansi[0], alen, nullptr, nullptr);
    if (ansi.back() == '\0') ansi.pop_back();
    return ansi;
}
#endif

std::string BttbLocale::get(const std::string& key, const std::string& fallback) {
    auto it = translations.find(key);
    if (it != translations.end()) {
        return it->second;
    }
#ifdef _WIN32
    return utf8ToAnsi(fallback);
#else
    return fallback;
#endif
}
