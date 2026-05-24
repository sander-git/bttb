#include "bttb_logic.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <queue>

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

BttbSolver::BttbSolver() {
    // Default log notifier
    logNotify = [](const std::string& msg, int) {
        std::cout << msg << std::endl;
    };
    progressNotify = [](double, double) {};
}

void BttbSolver::addEntry(const std::string& relPath, int64_t size, bool isDir) {
    auto entry = std::make_unique<DirEntry>();
    entry->relativePath = relPath;
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

int64_t BttbSolver::diveDepth(const std::filesystem::path& currentSubpath, int depth) {
    int64_t totalSize = 0;
    std::filesystem::path fullPath = std::filesystem::path(sourceDirectory) / currentSubpath;
    
    if (!std::filesystem::exists(fullPath)) return 0;
    
    std::vector<std::filesystem::path> subdirs;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(fullPath)) {
            if (stopRequested) return 0;
            
            std::filesystem::path rel = std::filesystem::relative(entry.path(), sourceDirectory);
            
            if (entry.is_directory()) {
                subdirs.push_back(rel);
            } else if (entry.is_regular_file()) {
                int64_t fileSize = entry.file_size();
                if (depth < 0) {
                    // Accumulate size recursively below waterlevel
                    totalSize += fileSize;
                } else {
                    // Above or at split depth, add files directly
                    addEntry(rel.string(), fileSize, false);
                }
            }
        }
        
        // Process subdirectories
        for (const auto& subdir : subdirs) {
            if (stopRequested) return 0;
            
            if (depth == 0) {
                // Split depth boundary reached: this subdirectory is treated as an atomic item.
                // Call recursively with depth - 1 to sum all files within it.
                int64_t dirTotal = diveDepth(subdir, depth - 1);
                addEntry(subdir.string(), dirTotal, true);
            } else {
                // Continue recursing downwards
                totalSize += diveDepth(subdir, depth - 1);
            }
        }
    } catch (const std::exception& e) {
        logNotify("Error scanning directory " + currentSubpath.string() + ": " + e.what(), 2);
    }
    
    return totalSize;
}

void BttbSolver::scanDirectory() {
    itemsToSplit.clear();
    logNotify("Scanning folder: " + sourceDirectory, 0);
    diveDepth("", splitDepth);
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
    if (stopRequested || searchTimedOut) return false;
    
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
    
    scanDirectory();
    if (itemsToSplit.empty()) {
        logNotify("No items to fit.", 2);
        return;
    }
    
    // Convert capacities to sectors
    maxSectors = mediumInfo.capacityBytes / mediumInfo.sectorSize;
    slackSectors = mediumInfo.slackBytes / mediumInfo.sectorSize;
    
    logNotify("Medium Capacity: " + std::to_string(maxSectors) + " sectors (" + std::to_string(mediumInfo.capacityBytes) + " bytes)", 0);
    
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
    
    logNotify("Solving optimal bin selection...", 0);
    
    // Start recursion in backtracking
    findAWay(0, itemsToSplit.size() - 1);
    
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    logNotify("Finished solving. Elapsed time: " + std::to_string(elapsed) + " ms", 1);
    
    if (bestSelectionIndices.empty()) {
        logNotify("Could not fit any items onto the medium.", 2);
        return;
    }
    
    double percent = (double)currentBestSectors / maxSectors * 100.0;
    logNotify("Optimal selection covers: " + std::to_string(percent) + "%", 1);
    logNotify("Selected items:", 1);
    
    for (int idx : bestSelectionIndices) {
        logNotify(" - " + itemsToSplit[idx]->relativePath + " (" + std::to_string(itemsToSplit[idx]->sizeBytes) + " bytes)", 0);
    }
    
    // Perform copy/move if target directory specified
    if (moveFiles && !targetDirectory.empty()) {
        std::filesystem::create_directories(targetDirectory);
        for (int idx : bestSelectionIndices) {
            if (stopRequested) break;
            
            std::filesystem::path src = std::filesystem::path(sourceDirectory) / itemsToSplit[idx]->relativePath;
            std::filesystem::path dest = std::filesystem::path(targetDirectory) / itemsToSplit[idx]->relativePath;
            
            logNotify("Organizing item: " + itemsToSplit[idx]->relativePath, 0);
            try {
                std::filesystem::create_directories(dest.parent_path());
                if (std::filesystem::is_directory(src)) {
                    std::filesystem::copy(src, dest, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
                    std::filesystem::remove_all(src);
                } else {
                    std::filesystem::copy_file(src, dest, std::filesystem::copy_options::overwrite_existing);
                    std::filesystem::remove(src);
                }
            } catch (const std::exception& e) {
                logNotify("Failed to organize " + itemsToSplit[idx]->relativePath + ": " + e.what(), 2);
            }
        }
        logNotify("Completed file organization.", 1);
    }
}

} // namespace bttb
