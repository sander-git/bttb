#include "bttb_logic.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <queue>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

namespace bttb {

#ifdef _WIN32
std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
std::wstring utf8ToWstring(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
std::filesystem::path utf8Path(const std::string& utf8Str) {
    return std::filesystem::path(utf8ToWstring(utf8Str));
}
std::string toUtf8Str(const std::filesystem::path& p) {
    return wstringToUtf8(p.wstring());
}
std::filesystem::path makeLongPath(const std::filesystem::path& p) {
    try {
        std::wstring absPath = std::filesystem::absolute(p).make_preferred().wstring();
        if (absPath.length() >= 240) {
            if (absPath.rfind(L"\\\\?\\", 0) != 0) {
                if (absPath.rfind(L"\\\\", 0) == 0) {
                    absPath = L"\\\\?\\UNC\\" + absPath.substr(2);
                } else {
                    absPath = L"\\\\?\\" + absPath;
                }
            }
        }
        return std::filesystem::path(absPath);
    } catch (...) {
        return p;
    }
}
#else
std::string wstringToUtf8(const std::wstring& wstr) {
    std::string str(wstr.begin(), wstr.end());
    return str;
}
std::wstring utf8ToWstring(const std::string& str) {
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}
std::filesystem::path utf8Path(const std::string& utf8Str) {
    return std::filesystem::path(utf8Str);
}
std::string toUtf8Str(const std::filesystem::path& p) {
    return p.string();
}
std::filesystem::path makeLongPath(const std::filesystem::path& p) {
    try {
        return std::filesystem::absolute(p);
    } catch (...) {
        return p;
    }
}
#endif

// Convert standard wildcards/globs to C++ regex patterns
std::regex globToRegex(const std::string& glob) {
    std::string regexStr = "^";
    for (char c : glob) {
        if (c == '*') {
            regexStr += ".*";
        } else if (c == '?') {
            regexStr += ".";
        } else if (c == '.' || c == '\\' || c == '+' || c == '^' || c == '$' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '|' || c == '!') {
            regexStr += "\\";
            regexStr += c;
        } else {
            regexStr += c;
        }
    }
    regexStr += "$";
    return std::regex(regexStr, std::regex_constants::icase);
}

// Parse human-readable sizes like 512MB, 2.5GB, 1.5TB
int64_t parseHumanSize(const std::string& input) {
    std::string s = input;
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    if (s.empty()) return 0;
    
    double multiplier = 1.0;
    std::string numPart = s;
    
    if (s.size() >= 2) {
        std::string suffix = s.substr(s.size() - 2);
        std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
        if (suffix == "kb") {
            multiplier = 1024.0;
            numPart = s.substr(0, s.size() - 2);
        } else if (suffix == "mb") {
            multiplier = 1024.0 * 1024.0;
            numPart = s.substr(0, s.size() - 2);
        } else if (suffix == "gb") {
            multiplier = 1024.0 * 1024.0 * 1024.0;
            numPart = s.substr(0, s.size() - 2);
        } else if (suffix == "tb") {
            multiplier = 1024.0 * 1024.0 * 1024.0 * 1024.0;
            numPart = s.substr(0, s.size() - 2);
        } else {
            char lastChar = std::tolower(s.back());
            if (lastChar == 'b') {
                multiplier = 1.0;
                numPart = s.substr(0, s.size() - 1);
            } else if (lastChar == 'k') {
                multiplier = 1024.0;
                numPart = s.substr(0, s.size() - 1);
            } else if (lastChar == 'm') {
                multiplier = 1024.0 * 1024.0;
                numPart = s.substr(0, s.size() - 1);
            } else if (lastChar == 'g') {
                multiplier = 1024.0 * 1024.0 * 1024.0;
                numPart = s.substr(0, s.size() - 1);
            } else if (lastChar == 't') {
                multiplier = 1024.0 * 1024.0 * 1024.0 * 1024.0;
                numPart = s.substr(0, s.size() - 1);
            }
        }
    } else if (s.size() == 1) {
        char lastChar = std::tolower(s.back());
        if (lastChar == 'b') { multiplier = 1.0; numPart = ""; }
        else if (lastChar == 'k') { multiplier = 1024.0; numPart = ""; }
        else if (lastChar == 'm') { multiplier = 1024.0 * 1024.0; numPart = ""; }
        else if (lastChar == 'g') { multiplier = 1024.0 * 1024.0 * 1024.0; numPart = ""; }
        else if (lastChar == 't') { multiplier = 1024.0 * 1024.0 * 1024.0 * 1024.0; numPart = ""; }
    }
    
    try {
        if (numPart.empty()) return 0;
        double val = std::stod(numPart);
        return static_cast<int64_t>(val * multiplier);
    } catch (...) {
        try {
            return std::stoll(s);
        } catch (...) {
            return 0;
        }
    }
}

// Ignore folders nested in other folders
std::vector<std::string> filterNestedDirectories(const std::vector<std::string>& dirs) {
    std::vector<std::filesystem::path> normPaths;
    for (const auto& d : dirs) {
        if (d.empty()) continue;
        try {
            normPaths.push_back(makeLongPath(utf8Path(d)).lexically_normal());
        } catch (...) {
            normPaths.push_back(utf8Path(d).lexically_normal());
        }
    }
    
    std::vector<std::string> filtered;
    for (size_t i = 0; i < normPaths.size(); ++i) {
        bool isNested = false;
        for (size_t j = 0; j < normPaths.size(); ++j) {
            if (i == j) continue;
            const auto& parent = normPaths[j];
            const auto& child = normPaths[i];
            
            auto pIt = parent.begin();
            auto cIt = child.begin();
            bool mismatch = false;
            while (pIt != parent.end()) {
                if (cIt == child.end() || *pIt != *cIt) {
                    mismatch = true;
                    break;
                }
                ++pIt;
                ++cIt;
            }
            if (!mismatch && cIt != child.end()) {
                isNested = true;
                break;
            }
        }
        
        if (!isNested) {
            bool alreadyIn = false;
            for (const auto& f : filtered) {
                try {
                    if (makeLongPath(utf8Path(f)).lexically_normal() == normPaths[i]) {
                        alreadyIn = true;
                        break;
                    }
                } catch (...) {
                    if (utf8Path(f).lexically_normal() == normPaths[i]) {
                        alreadyIn = true;
                        break;
                    }
                }
            }
            if (!alreadyIn) {
                filtered.push_back(dirs[i]);
            }
        }
    }
    return filtered;
}

// Cross-platform symbolic link creation helper with unprivileged flag and fallback
bool createPlatformSymlink(const std::string& target, const std::string& link, bool isDir) {
#ifdef _WIN32
    std::wstring wtarget = utf8ToWstring(target);
    std::wstring wlink = utf8ToWstring(link);
    std::wstring wtargetLong = makeLongPath(std::filesystem::path(wtarget)).wstring();
    std::wstring wlinkLong = makeLongPath(std::filesystem::path(wlink)).wstring();

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (hKernel32) {
        typedef BOOLEAN (WINAPI *CreateSymbolicLinkW_t)(LPCWSTR, LPCWSTR, DWORD);
        CreateSymbolicLinkW_t pCreateSymbolicLinkW = (CreateSymbolicLinkW_t)GetProcAddress(hKernel32, "CreateSymbolicLinkW");
        if (pCreateSymbolicLinkW) {
            DWORD flags = 0x2; // SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
            if (isDir) flags |= 0x1;
            if (pCreateSymbolicLinkW(wlinkLong.c_str(), wtargetLong.c_str(), flags)) {
                return true;
            }
        }
    }
    // Fallback to mklink
    std::wstring wcmd = L"cmd.exe /c mklink ";
    if (isDir) wcmd += L"/d ";
    wcmd += L"\"" + wlinkLong + L"\" \"" + wtargetLong + L"\"";
    return (_wsystem(wcmd.c_str()) == 0);
#else
    try {
        std::filesystem::path targetPath = makeLongPath(utf8Path(target));
        std::filesystem::path linkPath = makeLongPath(utf8Path(link));
        if (isDir) {
            std::filesystem::create_directory_symlink(targetPath, linkPath);
        } else {
            std::filesystem::create_symlink(targetPath, linkPath);
        }
        return true;
    } catch (...) {
        return false;
    }
#endif
}

BttbSolver::BttbSolver() {
    // Default log notifier
    logNotify = [](const std::string& msg, int) {
        std::cout << msg << std::endl;
    };
    progressNotify = [](double, double) {};
    recommendCapacityNotify = nullptr;
    skipUnreadable = true;

    // Semantic Packing
    semanticPrompt = "";
    enableSemanticPacking = false;
    testOnlyMode = false;
    semanticCoherenceFactor = 0.7;
}

void BttbSolver::addEntry(const std::string& relPath, const std::string& absPath, int64_t size, bool isDir) {
    // Check if it matches any grouping rules
    for (size_t i = 0; i < groupingRules.size(); ++i) {
        const auto& rule = groupingRules[i];
        if ((isDir && !rule.matchFolders) || (!isDir && !rule.matchFiles)) {
            continue;
        }
        
        // Extract filename for matching
        std::filesystem::path p(relPath);
        std::string filename = p.filename().string();
        
        bool matched = false;
        try {
            matched = std::regex_match(filename, rule.compiledRegex) || std::regex_match(relPath, rule.compiledRegex);
        } catch (...) {}
        
        if (matched) {
            std::string groupName = "[Group: " + rule.patternString + "]";
            // Find if we already have a consolidated entry for this grouping rule
            for (auto& entry : itemsToSplit) {
                if (entry->relativePath == groupName) {
                    entry->sizeBytes += size;
                    entry->sectorCount = (entry->sizeBytes + mediumInfo.sectorSize - 1) / mediumInfo.sectorSize;
                    if (entry->sectorCount == 0) entry->sectorCount = 1;
                    entry->groupedPaths.push_back(relPath);
                    entry->absoluteGroupedPaths.push_back(absPath);
                    return;
                }
            }
            
            // Create a new consolidated entry
            auto entry = std::make_unique<DirEntry>();
            entry->relativePath = groupName;
            entry->absolutePath = "";
            entry->sizeBytes = size;
            entry->sectorCount = (size + mediumInfo.sectorSize - 1) / mediumInfo.sectorSize;
            if (entry->sectorCount == 0) entry->sectorCount = 1;
            entry->isDirectory = false; // treated as a file group
            entry->groupedPaths.push_back(relPath);
            entry->absoluteGroupedPaths.push_back(absPath);
            itemsToSplit.push_back(std::move(entry));
            return;
        }
    }

    auto entry = std::make_unique<DirEntry>();
    entry->relativePath = relPath;
    entry->absolutePath = absPath;
    entry->sizeBytes = size;
    entry->sectorCount = (size + mediumInfo.sectorSize - 1) / mediumInfo.sectorSize;
    entry->isDirectory = isDir;
    
    // Check for SkipEmpty setting
    if (entry->sectorCount == 0 && skipEmpty) {
        // Enforce at least 1 sector if not skipping
        return;
    }
    if (entry->sectorCount == 0) {
        entry->sectorCount = 1;
    }
    
    itemsToSplit.push_back(std::move(entry));
}

int64_t BttbSolver::diveDepth(const std::filesystem::path& baseDir, const std::filesystem::path& currentSubpath, int depth) {
    int64_t totalSize = 0;
    std::filesystem::path fullPath = makeLongPath(baseDir / currentSubpath);
    std::filesystem::path baseDirLong = makeLongPath(baseDir);
    
    if (enableTrace) {
        logNotify("[TRACE] [Scan] Analyzing directory: " + toUtf8Str(fullPath) + " (split depth level: " + std::to_string(depth) + ")", 3);
    }
    
    try {
        if (!std::filesystem::exists(fullPath)) return 0;
    } catch (const std::exception& e) {
        logNotify("Warning: Directory '" + toUtf8Str(fullPath) + "' is unreadable (" + e.what() + ").", 2);
        if (!skipUnreadable) {
            stopRequested = true;
            logNotify("Scanning aborted by user due to unreadable directory.", 2);
        }
        return 0;
    }
    
    std::vector<std::filesystem::path> subdirs;
    
    try {
        for (auto it = std::filesystem::directory_iterator(fullPath); it != std::filesystem::directory_iterator(); ++it) {
            if (stopRequested) return 0;
            
            try {
                const auto& entry = *it;
                std::filesystem::path rel = currentSubpath / entry.path().filename();
                
                if (std::filesystem::is_directory(entry.path())) {
                    subdirs.push_back(rel);
                } else if (std::filesystem::is_regular_file(entry.path())) {
                    int64_t fileSize = std::filesystem::file_size(entry.path());
                    if (depth < 0) {
                        // Accumulate size recursively below waterlevel
                        totalSize += fileSize;
                    } else {
                        // Above or at split depth, add files directly
                        addEntry(toUtf8Str(rel), toUtf8Str(entry.path()), fileSize, false);
                    }
                }
            } catch (const std::exception& e) {
                logNotify("Warning: File or folder in '" + toUtf8Str(fullPath) + "' is unreadable (" + e.what() + ").", 2);
                if (!skipUnreadable) {
                    stopRequested = true;
                    logNotify("Scanning aborted by user due to unreadable item.", 2);
                    return 0;
                }
            }
        }
        
        // Process subdirectories
        for (const auto& subdir : subdirs) {
            if (stopRequested) return 0;
            
            if (depth == 0) {
                // Split depth boundary reached: this subdirectory is treated as an atomic item.
                // Call recursively with depth - 1 to sum all files within it.
                int64_t dirTotal = diveDepth(baseDirLong, subdir, depth - 1);
                addEntry(toUtf8Str(subdir), toUtf8Str(baseDirLong / subdir), dirTotal, true);
            } else {
                // Continue recursing downwards
                totalSize += diveDepth(baseDirLong, subdir, depth - 1);
            }
        }
    } catch (const std::exception& e) {
        logNotify("Warning: Failed to iterate directory '" + toUtf8Str(fullPath) + "' (" + e.what() + ").", 2);
        if (!skipUnreadable) {
            stopRequested = true;
            logNotify("Scanning aborted by user due to unreadable directory iteration.", 2);
        }
    }
    
    return totalSize;
}

void BttbSolver::scanDirectory() {
    itemsToSplit.clear();
    
    std::vector<std::string> activeDirs = sourceDirectories;
    if (activeDirs.empty() && !sourceDirectory.empty()) {
        activeDirs.push_back(sourceDirectory);
    }
    
    activeDirs = filterNestedDirectories(activeDirs);
    
    for (const auto& baseDir : activeDirs) {
        logNotify("Scanning folder: " + baseDir, 0);
        diveDepth(utf8Path(baseDir), "", splitDepth);
    }
    logNotify("Found items to fit: " + std::to_string(itemsToSplit.size()), 0);
}

// Comparator to sort by selected state, then by size (ascending)
bool compareDirEntries(const std::unique_ptr<DirEntry>& a, const std::unique_ptr<DirEntry>& b) {
    if (a->isSelected != b->isSelected) {
        return a->isSelected < b->isSelected;
    }
    return a->sectorCount < b->sectorCount;
}

// Backtracking solver
bool BttbSolver::findAWay(int64_t currentSectors, int poz) {
    exploredStates++;
    if (stopRequested || searchTimedOut) return false;

    // Check timeout periodically (every 10000 states to minimize chrono overhead)
    if (maxSearchTimeSeconds > 0 && (exploredStates % 10000 == 0)) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - solverStartTime).count();
        if (elapsed >= maxSearchTimeSeconds) {
            searchTimedOut = true;
            logNotify("Search time limit exceeded (" + std::to_string(maxSearchTimeSeconds) + " seconds).", 2);
            return false;
        }
    }
    
    if (poz >= 0 && (itemsToSplit[poz]->prefixSumSectors + currentSectors < currentBestSectors)) {
        prunedStates++;
        if (enableTrace && (prunedStates % 10000 == 0)) {
            logNotify("[TRACE] Pruning branch at index " + std::to_string(poz) + " (" + itemsToSplit[poz]->relativePath + "): currentBest=" + std::to_string(currentBestSectors) + " sectors", 0);
        }
    }
    
    if (enableTrace && (exploredStates % 10000 == 0)) {
        logNotify("[TRACE] Explored: " + std::to_string(exploredStates) + " states, Pruned: " + std::to_string(prunedStates) + " branches (currentBest=" + std::to_string(currentBestSectors) + " sectors)", 0);
    }
    
    if (poz < 0 || (itemsToSplit[poz]->prefixSumSectors + currentSectors < currentBestSectors)) {
        return false;
    }
    
    int64_t testSectors = currentSectors + itemsToSplit[poz]->sectorCount;
    
    if (testSectors <= maxSectors) {
        itemsToSplit[poz]->isSelected = true;
        
        if (!findAWay(testSectors, poz - 1)) {
            if (testSectors >= currentBestSectors) {
                if (testSectors > currentBestSectors) {
                    currentBestSectors = testSectors;
                    minNumberOfClusters = INT_MAX;
                    
                    double percentage = (double)currentBestSectors / maxSectors * 100.0;
                    logNotify("New best space utilization: " + std::to_string(percentage) + "%", 3);

                    // Check for early termination if within slack tolerance
                    if (currentBestSectors >= maxSectors - slackSectors) {
                        searchTimedOut = true;
                        logNotify("Selection within slack tolerance (" + std::to_string(percentage) + "%) found. Terminating search early.", 1);
                    }
                }
                
                // Keep clusters together (minimize number of folder boundaries)
                int numClusters = 0;
                for (const auto& item : itemsToSplit) {
                    if (item->isSelected) {
                        numClusters += countClusters(item->relativePath);
                    }
                }
                
                if (numClusters < minNumberOfClusters) {
                    minNumberOfClusters = numClusters;
                    saveBestSelection();
                }
            }
        }
        
        itemsToSplit[poz]->isSelected = false;
        findAWay(currentSectors, poz - 1);
        return true;
    } else {
        return findAWay(currentSectors, poz - 1);
    }
}

void BttbSolver::saveBestSelection() {
    bestSelectionIndices.clear();
    for (size_t i = 0; i < itemsToSplit.size(); ++i) {
        if (itemsToSplit[i]->isSelected) {
            bestSelectionIndices.push_back(i);
        }
    }
}

int BttbSolver::countClusters(const std::string& path) {
    int count = 1;
    for (char c : path) {
        if (c == '/' || c == '\\') {
            count++;
        }
    }
    return count;
}

void BttbSolver::run() {
    stopRequested = false;
    searchTimedOut = false;
    packedVolumes.clear();
    
    while (!stopRequested) {
        scanDirectory();
        if (itemsToSplit.empty()) {
            logNotify("No items to fit.", 2);
            return;
        }

        if (enableSemanticPacking && !semanticPrompt.empty()) {
            runSemanticClustering();
        }
        
        // Find maximum file size encountered
        int64_t maxItemSize = 0;
        std::string maxItemName = "";
        for (const auto& item : itemsToSplit) {
            if (item->sizeBytes > maxItemSize) {
                maxItemSize = item->sizeBytes;
                maxItemName = item->relativePath;
            }
        }
        
        // Check if the maximum file size exceeds the medium capacity
        if (maxItemSize > mediumInfo.capacityBytes) {
            logNotify("Warning: Item '" + maxItemName + "' (" + std::to_string(maxItemSize) + " bytes) is larger than target volume size (" + std::to_string(mediumInfo.capacityBytes) + " bytes).", 2);
            if (recommendCapacityNotify) {
                int64_t recommendedBytes = ((maxItemSize + mediumInfo.sectorSize - 1) / mediumInfo.sectorSize) * mediumInfo.sectorSize;
                if (recommendCapacityNotify(recommendedBytes)) {
                    mediumInfo.capacityBytes = recommendedBytes;
                    logNotify("Capacity adapted to " + std::to_string(recommendedBytes) + " bytes. Retrying solver...", 1);
                    continue;
                } else {
                    logNotify("Solver aborted: dataset contains files exceeding capacity.", 2);
                    return;
                }
            } else {
                logNotify("Solver aborted: dataset contains files exceeding capacity. Adapt capacity or enable interactive retrying.", 2);
                return;
            }
        }
        break;
    }
    
    if (stopRequested) return;
    
    // Convert capacities to sectors
    maxSectors = mediumInfo.capacityBytes / mediumInfo.sectorSize;
    slackSectors = mediumInfo.slackBytes / mediumInfo.sectorSize;
    
    exploredStates = 0;
    prunedStates = 0;
    
    // Estimation process
    int64_t totalBytes = 0;
    for (const auto& item : itemsToSplit) {
        totalBytes += item->sizeBytes;
    }
    double totalMB = (double)totalBytes / (1024.0 * 1024.0);
    double mediumMB = (double)mediumInfo.capacityBytes / (1024.0 * 1024.0);
    int estimatedVolumes = std::max<int>(1, std::ceil((double)totalBytes / mediumInfo.capacityBytes));
    
    logNotify("--- Volume Packing Estimation ---", 3);
    logNotify("Total items found: " + std::to_string(itemsToSplit.size()), 0);
    logNotify("Total dataset size: " + std::to_string(totalBytes) + " bytes (" + std::to_string(totalMB) + " MB)", 0);
    logNotify("Target volume capacity: " + std::to_string(mediumInfo.capacityBytes) + " bytes (" + std::to_string(mediumMB) + " MB)", 0);
    logNotify("Theoretical minimum volumes required: " + std::to_string(estimatedVolumes), 1);
    
    if (enableTrace) {
        logNotify("[TRACE] Detailed estimation breakdown:", 0);
        logNotify("[TRACE]  - Dataset to medium ratio: " + std::to_string((double)totalBytes / mediumInfo.capacityBytes), 0);
        logNotify("[TRACE]  - Solver backtracking sector limit: " + std::to_string(maxSectors) + " sectors", 0);
        logNotify("[TRACE]  - Sector size cluster alignment: " + std::to_string(mediumInfo.sectorSize) + " bytes", 0);
        logNotify("[TRACE]  - Slack tolerance: " + std::to_string(mediumInfo.slackBytes) + " bytes (" + std::to_string(slackSectors) + " sectors)", 0);
    }
    
    logNotify("Medium Capacity: " + std::to_string(maxSectors) + " sectors (" + std::to_string(mediumInfo.capacityBytes) + " bytes)", 0);
    
    int volumeIndex = 1;
    
    while (!itemsToSplit.empty() && !stopRequested) {
        searchTimedOut = false;
        for (auto& item : itemsToSplit) {
            item->isSelected = false;
        }
        
        if (spanMultipleVolumes) {
            logNotify("\r\n--- SOLVING FOR VOLUME " + std::to_string(volumeIndex) + " ---", 3);
        }
        
        // Sort items initially ascending by size
        std::sort(itemsToSplit.begin(), itemsToSplit.end(), compareDirEntries);
        
        // Compute prefix sums for quick backtracking pruning
        itemsToSplit[0]->prefixSumSectors = itemsToSplit[0]->sectorCount;
        for (size_t i = 1; i < itemsToSplit.size(); ++i) {
            itemsToSplit[i]->prefixSumSectors = itemsToSplit[i]->sectorCount + itemsToSplit[i-1]->prefixSumSectors;
        }
        
        currentBestSectors = 0;
        minNumberOfClusters = INT_MAX;
        bestSelectionIndices.clear();
        
        auto startTime = std::chrono::steady_clock::now();
        solverStartTime = startTime;
        
        logNotify("Solving optimal bin selection...", 0);
        
        // Start recursion in backtracking
        findAWay(0, itemsToSplit.size() - 1);
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        logNotify("Finished solving. Elapsed time: " + std::to_string(elapsed) + " ms", 1);
        
        if (bestSelectionIndices.empty()) {
            logNotify("Could not fit any remaining items onto the medium.", 2);
            break;
        }
        
        double percent = (double)currentBestSectors / maxSectors * 100.0;
        logNotify("Optimal selection covers: " + std::to_string(percent) + "%", 1);
        if (spanMultipleVolumes) {
            logNotify("Selected items for Volume " + std::to_string(volumeIndex) + ":", 1);
        } else {
            logNotify("Selected items:", 1);
        }
        
        PackedVolume vol;
        vol.volumeIndex = volumeIndex;
        vol.totalBytes = 0;
        for (int idx : bestSelectionIndices) {
            logNotify(" - " + itemsToSplit[idx]->relativePath + " (" + std::to_string(itemsToSplit[idx]->sizeBytes) + " bytes)", 0);
            if (!itemsToSplit[idx]->groupedPaths.empty()) {
                for (const auto& subPath : itemsToSplit[idx]->groupedPaths) {
                    logNotify("     -> " + subPath, 0);
                }
            }
            vol.itemPaths.push_back(itemsToSplit[idx]->relativePath);
            vol.itemSizes.push_back(itemsToSplit[idx]->sizeBytes);
            vol.itemGroupedPaths.push_back(itemsToSplit[idx]->groupedPaths);
            vol.totalBytes += itemsToSplit[idx]->sizeBytes;
        }
        packedVolumes.push_back(vol);
        
        // Perform copy/move/symlink if target directory specified
        if (!targetDirectory.empty() && !testOnlyMode) {
            std::filesystem::path volDestDir = makeLongPath(utf8Path(targetDirectory));
            if (spanMultipleVolumes) {
                volDestDir = makeLongPath(std::filesystem::path(utf8Path(targetDirectory)) / ("Volume_" + std::to_string(volumeIndex)));
            }
            
            if (moveFiles || createSymlinks) {
                std::filesystem::create_directories(volDestDir);
                for (int idx : bestSelectionIndices) {
                    if (stopRequested) break;
                    
                    std::vector<std::string> relPaths;
                    std::vector<std::string> absPaths;
                    if (!itemsToSplit[idx]->groupedPaths.empty()) {
                        relPaths = itemsToSplit[idx]->groupedPaths;
                        absPaths = itemsToSplit[idx]->absoluteGroupedPaths;
                    } else {
                        relPaths.push_back(itemsToSplit[idx]->relativePath);
                        absPaths.push_back(itemsToSplit[idx]->absolutePath);
                    }
                    
                    logNotify("Organizing item: " + itemsToSplit[idx]->relativePath, 0);
                    for (size_t i = 0; i < relPaths.size(); ++i) {
                        const auto& relItem = relPaths[i];
                        const auto& absItem = absPaths[i];
                        
                        std::filesystem::path src = makeLongPath(utf8Path(absItem));
                        std::filesystem::path dest = makeLongPath(volDestDir / utf8Path(relItem));
                        
                        try {
                            std::filesystem::create_directories(dest.parent_path());
                            if (std::filesystem::exists(src)) {
                                if (createSymlinks) {
                                    bool isDir = std::filesystem::is_directory(src);
                                    if (std::filesystem::exists(dest)) {
                                        std::filesystem::remove_all(dest);
                                    }
                                    if (!createPlatformSymlink(toUtf8Str(src), toUtf8Str(dest), isDir)) {
                                        logNotify("Failed to create symlink for " + relItem + " (retrying with copy)", 2);
                                        if (isDir) {
                                            std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                                        } else {
                                            std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
                                        }
                                    }
                                } else if (moveFiles) {
                                    if (std::filesystem::is_directory(src)) {
                                        std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                                        std::filesystem::remove_all(src);
                                    } else {
                                        std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
                                        std::filesystem::remove(src);
                                    }
                                }
                            }
                        } catch (const std::exception& e) {
                            logNotify("Failed to organize " + relItem + ": " + e.what(), 2);
                        }
                    }
                }
            }
        }
        
        // Build list of remaining items that were NOT selected
        std::vector<std::unique_ptr<DirEntry>> remainingItems;
        std::vector<bool> isSelectedMap(itemsToSplit.size(), false);
        for (int idx : bestSelectionIndices) {
            isSelectedMap[idx] = true;
        }
        
        for (size_t i = 0; i < itemsToSplit.size(); ++i) {
            if (!isSelectedMap[i]) {
                remainingItems.push_back(std::move(itemsToSplit[i]));
            }
        }
        
        itemsToSplit = std::move(remainingItems);
        
        if (!spanMultipleVolumes) {
            // If not spanning, only do one iteration!
            break;
        }
        
        volumeIndex++;
    }
    
    if (testOnlyMode) {
        logNotify("--- TEST SIMULATION COMPLETE ---", 3);
        logNotify("No files were copied, symlinked, or moved on disk.", 1);
    } else if ((moveFiles || createSymlinks) && !targetDirectory.empty()) {
        logNotify("Completed file organization.", 1);
    }
    
    if (enableTrace) {
        logNotify("\r\n[TRACE] Backtracking search performance summary:", 3);
        logNotify("[TRACE]  - Total states explored: " + std::to_string(exploredStates), 0);
        logNotify("[TRACE]  - Total branches pruned: " + std::to_string(prunedStates), 0);
        double efficiency = exploredStates > 0 ? ((double)prunedStates / exploredStates * 100.0) : 0.0;
        logNotify("[TRACE]  - Backtracking pruning efficiency: " + std::to_string(efficiency) + "%", 1);
    }
}

double BttbSolver::computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) return 0.0;
    double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a > 1e-9 && norm_b > 1e-9) {
        return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
    }
    return 0.0;
}

void BttbSolver::runSemanticClustering() {
    if (itemsToSplit.empty()) return;

    logNotify("Generating semantic embeddings...", 3);

    auto escapeJson = [](const std::string& str) -> std::string {
        std::string escaped = "";
        for (char c : str) {
            if (c == '"') escaped += "\\\"";
            else if (c == '\\') escaped += "\\\\";
            else if (c == '\n') escaped += "\\n";
            else if (c == '\r') escaped += "\\r";
            else if (c == '\t') escaped += "\\t";
            else escaped += c;
        }
        return escaped;
    };

    // Serialize to structured JSON object with prompt, relative paths, and sizes
    std::string jsonStr = "{\n  \"prompt\": \"" + escapeJson(semanticPrompt) + "\",\n  \"items\": [\n";
    for (size_t i = 0; i < itemsToSplit.size(); ++i) {
        if (i > 0) jsonStr += ",\n";
        jsonStr += "    { \"path\": \"" + escapeJson(itemsToSplit[i]->relativePath) + "\", \"size\": " + std::to_string(itemsToSplit[i]->sizeBytes) + " }";
    }
    jsonStr += "\n  ]\n}";

    // 3. Find the path to bttb_embed.py
    std::string scriptPath = "src/bttb_embed.py";
    if (!std::filesystem::exists(scriptPath)) {
        scriptPath = "../src/bttb_embed.py";
    }
    if (!std::filesystem::exists(scriptPath)) {
        scriptPath = "./bttb_embed.py";
    }
    if (!std::filesystem::exists(scriptPath)) {
        scriptPath = "/usr/share/bttb/bttb_embed.py";
    }
    if (!std::filesystem::exists(scriptPath)) {
        scriptPath = "/usr/bin/bttb_embed.py";
    }

    // 4. Run bttb_embed.py subprocess via temporary files
    std::filesystem::path tempDir = std::filesystem::temp_directory_path();
    std::string tempIn = (tempDir / "bttb_embed_in.json").string();
    std::string tempOut = (tempDir / "bttb_embed_out.json").string();

    try {
        std::ofstream in(tempIn);
        in << jsonStr;
        in.close();
    } catch (const std::exception& e) {
        logNotify("Failed to write temporary embedding input file: " + std::string(e.what()), 2);
        return;
    }

    std::string tempErr = (tempDir / "bttb_embed_err.log").string();
    std::string cmd = "python3 \"" + scriptPath + "\" < \"" + tempIn + "\" > \"" + tempOut + "\" 2> \"" + tempErr + "\"";
    int ret = std::system(cmd.c_str());

    // Read and log stderr if it contains warnings/tutorials
    if (std::filesystem::exists(tempErr)) {
        std::ifstream errFile(tempErr);
        std::string errLine;
        while (std::getline(errFile, errLine)) {
            if (!errLine.empty()) {
                logNotify(errLine, 2); // Log directly to UI listbox
            }
        }
        errFile.close();
        try {
            std::filesystem::remove(tempErr);
        } catch (...) {}
    }

    if (ret != 0) {
        logNotify("Warning: Subprocess python embedding engine failed or python3 is not installed.", 2);
        logNotify("-> Semantic grouping will fall back to local string matching metrics.", 3);
        // Clean up and return
        try {
            std::filesystem::remove(tempIn);
            std::filesystem::remove(tempOut);
        } catch (...) {}
        return;
    }

    // 5. Read output JSON file
    std::string outJson;
    try {
        std::ifstream in(tempOut);
        std::stringstream buffer;
        buffer << in.rdbuf();
        outJson = buffer.str();
        in.close();
        
        std::filesystem::remove(tempIn);
        std::filesystem::remove(tempOut);
    } catch (const std::exception& e) {
        logNotify("Failed to read temporary embedding output file: " + std::string(e.what()), 2);
        return;
    }

    // 6. Parse the 2D JSON float array: [[f, f, ...], [f, f, ...]]
    std::vector<std::vector<float>> parsedEmbeddings;
    size_t pos = 0;
    while ((pos = outJson.find('[', pos)) != std::string::npos) {
        if (pos == 0 || (pos > 0 && outJson[pos - 1] == '[' && parsedEmbeddings.empty())) {
            pos++;
            continue;
        }
        
        size_t end = outJson.find(']', pos);
        if (end == std::string::npos) break;
        
        std::string vecStr = outJson.substr(pos, end - pos);
        std::vector<float> vec;
        std::stringstream ss(vecStr);
        std::string valStr;
        while (std::getline(ss, valStr, ',')) {
            try {
                vec.push_back(std::stof(valStr));
            } catch (...) {}
        }
        
        if (!vec.empty()) {
            parsedEmbeddings.push_back(vec);
        }
        pos = end + 1;
    }

    if (parsedEmbeddings.empty()) {
        logNotify("Warning: Embedded output is empty.", 2);
        return;
    }

    // 7. Assign embeddings
    std::vector<float> promptVec;
    size_t embedIdx = 0;
    if (!semanticPrompt.empty() && embedIdx < parsedEmbeddings.size()) {
        promptVec = parsedEmbeddings[embedIdx++];
    }

    for (size_t i = 0; i < itemsToSplit.size(); ++i) {
        if (embedIdx < parsedEmbeddings.size()) {
            itemsToSplit[i]->embedding = parsedEmbeddings[embedIdx++];
        }
    }

    // 8. Agglomerative Hierarchical Clustering based on Cosine Similarity
    logNotify("Running agglomerative semantic clustering (threshold=0.6)...", 3);
    
    std::vector<int> clusterMap(itemsToSplit.size());
    for (size_t i = 0; i < itemsToSplit.size(); ++i) {
        clusterMap[i] = i;
    }

    size_t N = itemsToSplit.size();
    std::vector<std::vector<double>> sim(N, std::vector<double>(N, 0.0));
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = i + 1; j < N; ++j) {
            double s = 0.0;
            bool usedEmbedding = false;
            if (!itemsToSplit[i]->embedding.empty() && !itemsToSplit[j]->embedding.empty()) {
                s = computeCosineSimilarity(itemsToSplit[i]->embedding, itemsToSplit[j]->embedding);
                usedEmbedding = true;
            } else {
                std::string s1 = std::filesystem::path(itemsToSplit[i]->relativePath).filename().string();
                std::string s2 = std::filesystem::path(itemsToSplit[j]->relativePath).filename().string();
                std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
                std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
                int matches = 0;
                for (char c : s1) {
                    if (s2.find(c) != std::string::npos) matches++;
                }
                s = (double)matches / std::max<size_t>(1, s1.size() + s2.size() - matches);
            }

            if (enableTrace) {
                std::string method = usedEmbedding ? "MiniLM Cosine" : "Local character TF-IDF fallback";
                logNotify("[TRACE] [Semantic] Initial similarity between '" + itemsToSplit[i]->relativePath + "' and '" + itemsToSplit[j]->relativePath + "' using " + method + ": " + std::to_string(s), 3);
            }

            if (!promptVec.empty() && !itemsToSplit[i]->embedding.empty() && !itemsToSplit[j]->embedding.empty()) {
                double pSimI = computeCosineSimilarity(itemsToSplit[i]->embedding, promptVec);
                double pSimJ = computeCosineSimilarity(itemsToSplit[j]->embedding, promptVec);
                if (pSimI > 0.4 && pSimJ > 0.4) {
                    double oldS = s;
                    s += 0.25;
                    if (enableTrace) {
                        logNotify("[TRACE] [Semantic] Boosting similarity for '" + itemsToSplit[i]->relativePath + "' & '" + itemsToSplit[j]->relativePath + "' due to semantic prompt match (Old: " + std::to_string(oldS) + ", New: " + std::to_string(s) + ")", 3);
                    }
                }
            }

            sim[i][j] = s;
            sim[j][i] = s;
        }
    }

    while (true) {
        double maxSim = -1.0;
        int mergeA = -1;
        int mergeB = -1;

        for (size_t i = 0; i < N; ++i) {
            for (size_t j = i + 1; j < N; ++j) {
                if (clusterMap[i] != clusterMap[j] && sim[i][j] > maxSim) {
                    maxSim = sim[i][j];
                    mergeA = i;
                    mergeB = j;
                }
            }
        }

        if (maxSim < 0.6 || mergeA == -1) {
            break;
        }

        if (enableTrace) {
            logNotify("[TRACE] [Semantic] Merging cluster containing '" + itemsToSplit[mergeA]->relativePath + "' and '" + itemsToSplit[mergeB]->relativePath + "' (similarity: " + std::to_string(maxSim) + ")", 3);
        }

        int cA = clusterMap[mergeA];
        int cB = clusterMap[mergeB];
        for (size_t i = 0; i < N; ++i) {
            if (clusterMap[i] == cB) {
                clusterMap[i] = cA;
            }
        }
    }

    std::vector<std::unique_ptr<DirEntry>> clusteredItems;
    std::vector<bool> processed(N, false);

    for (size_t i = 0; i < N; ++i) {
        if (processed[i]) continue;
        int c = clusterMap[i];
        
        std::vector<size_t> groupIndices;
        for (size_t j = i; j < N; ++j) {
            if (clusterMap[j] == c) {
                groupIndices.push_back(j);
                processed[j] = true;
            }
        }

        if (groupIndices.size() == 1) {
            clusteredItems.push_back(std::move(itemsToSplit[groupIndices[0]]));
        } else {
            int64_t maxGroupSize = mediumInfo.capacityBytes * 0.4;
            if (maxGroupSize <= 0) maxGroupSize = 1024 * 1024 * 100; // fallback 100MB
            
            std::vector<size_t> currentSubgroup;
            int64_t currentSubgroupSize = 0;
            int subgroupCounter = 1;
            
            auto flushSubgroup = [&](const std::vector<size_t>& indices) {
                if (indices.empty()) return;
                if (indices.size() == 1) {
                    clusteredItems.push_back(std::move(itemsToSplit[indices[0]]));
                } else {
                    auto entry = std::make_unique<DirEntry>();
                    if (indices.size() < groupIndices.size()) {
                        entry->relativePath = "[Semantic Group #" + std::to_string(c + 1) + "_" + std::to_string(subgroupCounter) + "]";
                    } else {
                        entry->relativePath = "[Semantic Group #" + std::to_string(c + 1) + "]";
                    }
                    entry->absolutePath = "";
                    entry->sizeBytes = 0;
                    entry->isDirectory = false;
                    
                    entry->embedding.assign(384, 0.0f);
                    
                    for (size_t idx : indices) {
                        const auto& item = itemsToSplit[idx];
                        entry->sizeBytes += item->sizeBytes;
                        
                        if (!item->groupedPaths.empty()) {
                            for (size_t k = 0; k < item->groupedPaths.size(); ++k) {
                                entry->groupedPaths.push_back(item->groupedPaths[k]);
                                entry->absoluteGroupedPaths.push_back(item->absoluteGroupedPaths[k]);
                            }
                        } else {
                            entry->groupedPaths.push_back(item->relativePath);
                            entry->absoluteGroupedPaths.push_back(item->absolutePath);
                        }

                        if (!item->embedding.empty()) {
                            for (size_t k = 0; k < 384; ++k) {
                                entry->embedding[k] += item->embedding[k];
                            }
                        }
                    }

                    float norm = 0.0f;
                    for (size_t k = 0; k < 384; ++k) {
                        entry->embedding[k] /= indices.size();
                        norm += entry->embedding[k] * entry->embedding[k];
                    }
                    norm = std::sqrt(norm);
                    if (norm > 1e-9) {
                        for (size_t k = 0; k < 384; ++k) {
                            entry->embedding[k] /= norm;
                        }
                    }

                    entry->sectorCount = (entry->sizeBytes + mediumInfo.sectorSize - 1) / mediumInfo.sectorSize;
                    if (entry->sectorCount == 0) entry->sectorCount = 1;
                    
                    clusteredItems.push_back(std::move(entry));
                }
            };
            
            for (size_t idx : groupIndices) {
                int64_t itemSize = itemsToSplit[idx]->sizeBytes;
                if (currentSubgroupSize + itemSize > maxGroupSize && !currentSubgroup.empty()) {
                    flushSubgroup(currentSubgroup);
                    currentSubgroup.clear();
                    currentSubgroupSize = 0;
                    subgroupCounter++;
                }
                currentSubgroup.push_back(idx);
                currentSubgroupSize += itemSize;
            }
            flushSubgroup(currentSubgroup);
        }
    }

    itemsToSplit = std::move(clusteredItems);
    logNotify("Semantic clustering completed. Total consolidated groups: " + std::to_string(itemsToSplit.size()), 1);
}

} // namespace bttb
