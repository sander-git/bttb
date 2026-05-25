#include "bttb_logic.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <queue>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

namespace bttb {

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
            normPaths.push_back(std::filesystem::absolute(d).lexically_normal());
        } catch (...) {
            normPaths.push_back(std::filesystem::path(d).lexically_normal());
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
                    if (std::filesystem::absolute(f).lexically_normal() == normPaths[i]) {
                        alreadyIn = true;
                        break;
                    }
                } catch (...) {
                    if (std::filesystem::path(f).lexically_normal() == normPaths[i]) {
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
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
    if (hKernel32) {
        typedef BOOLEAN (WINAPI *CreateSymbolicLinkA_t)(LPCSTR, LPCSTR, DWORD);
        CreateSymbolicLinkA_t pCreateSymbolicLinkA = (CreateSymbolicLinkA_t)GetProcAddress(hKernel32, "CreateSymbolicLinkA");
        if (pCreateSymbolicLinkA) {
            DWORD flags = 0x2; // SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
            if (isDir) flags |= 0x1; // SYMBOLIC_LINK_FLAG_DIRECTORY
            if (pCreateSymbolicLinkA(link.c_str(), target.c_str(), flags)) {
                return true;
            }
        }
    }
    // Fallback to mklink
    std::string cmd = "cmd.exe /c mklink ";
    if (isDir) cmd += "/d ";
    cmd += "\"" + link + "\" \"" + target + "\"";
    return (std::system(cmd.c_str()) == 0);
#else
    try {
        std::filesystem::path targetPath(target);
        std::filesystem::path linkPath(link);
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
    std::filesystem::path fullPath = baseDir / currentSubpath;
    
    if (!std::filesystem::exists(fullPath)) return 0;
    
    std::vector<std::filesystem::path> subdirs;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) {
            if (stopRequested) return 0;
            
            std::filesystem::path rel = std::filesystem::relative(entry.path(), baseDir);
            
            if (entry.is_directory()) {
                subdirs.push_back(rel);
            } else if (entry.is_regular_file()) {
                int64_t fileSize = entry.file_size();
                if (depth < 0) {
                    // Accumulate size recursively below waterlevel
                    totalSize += fileSize;
                } else {
                    // Above or at split depth, add files directly
                    addEntry(rel.string(), entry.path().string(), fileSize, false);
                }
            }
        }
        
        // Process subdirectories
        for (const auto& subdir : subdirs) {
            if (stopRequested) return 0;
            
            if (depth == 0) {
                // Split depth boundary reached: this subdirectory is treated as an atomic item.
                // Call recursively with depth - 1 to sum all files within it.
                int64_t dirTotal = diveDepth(baseDir, subdir, depth - 1);
                addEntry(subdir.string(), (baseDir / subdir).string(), dirTotal, true);
            } else {
                // Continue recursing downwards
                totalSize += diveDepth(baseDir, subdir, depth - 1);
            }
        }
    } catch (const std::exception& e) {
        logNotify("Error scanning directory " + currentSubpath.string() + ": " + e.what(), 2);
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
        diveDepth(baseDir, "", splitDepth);
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
    
    scanDirectory();
    if (itemsToSplit.empty()) {
        logNotify("No items to fit.", 2);
        return;
    }
    
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
            vol.itemPaths.push_back(itemsToSplit[idx]->relativePath);
            vol.itemSizes.push_back(itemsToSplit[idx]->sizeBytes);
            vol.totalBytes += itemsToSplit[idx]->sizeBytes;
        }
        packedVolumes.push_back(vol);
        
        // Perform copy/move/symlink if target directory specified
        if (!targetDirectory.empty()) {
            std::filesystem::path volDestDir = targetDirectory;
            if (spanMultipleVolumes) {
                volDestDir = std::filesystem::path(targetDirectory) / ("Volume_" + std::to_string(volumeIndex));
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
                        
                        std::filesystem::path src = std::filesystem::absolute(absItem);
                        std::filesystem::path dest = volDestDir / relItem;
                        
                        try {
                            std::filesystem::create_directories(dest.parent_path());
                            if (std::filesystem::exists(src)) {
                                if (createSymlinks) {
                                    bool isDir = std::filesystem::is_directory(src);
                                    if (std::filesystem::exists(dest)) {
                                        std::filesystem::remove_all(dest);
                                    }
                                    if (!createPlatformSymlink(src.string(), dest.string(), isDir)) {
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
    
    if ((moveFiles || createSymlinks) && !targetDirectory.empty()) {
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

} // namespace bttb
