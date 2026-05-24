#include "bttb_logic.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <queue>
#include <cmath>

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
    exploredStates++;
    if (stopRequested || searchTimedOut) return false;
    
    if (poz >= 0 && (itemsToSplit[poz]->prefixSumSectors + currentSectors < currentBestSectors)) {
        prunedStates++;
        if (enableTrace && (prunedStates % 200000 == 0)) {
            logNotify("[TRACE] Pruning branch at index " + std::to_string(poz) + " (" + itemsToSplit[poz]->relativePath + "): currentBest=" + std::to_string(currentBestSectors) + " sectors", 0);
        }
    }
    
    if (enableTrace && (exploredStates % 500000 == 0)) {
        logNotify("[TRACE] Explored: " + std::to_string(exploredStates) + " states, Pruned: " + std::to_string(prunedStates) + " branches...", 0);
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
        
        // Perform copy/move if target directory specified
        if (!targetDirectory.empty()) {
            std::filesystem::path volDestDir = targetDirectory;
            if (spanMultipleVolumes) {
                volDestDir = std::filesystem::path(targetDirectory) / ("Volume_" + std::to_string(volumeIndex));
            }
            
            if (moveFiles) {
                std::filesystem::create_directories(volDestDir);
                for (int idx : bestSelectionIndices) {
                    if (stopRequested) break;
                    
                    std::filesystem::path src = std::filesystem::path(sourceDirectory) / itemsToSplit[idx]->relativePath;
                    std::filesystem::path dest = volDestDir / itemsToSplit[idx]->relativePath;
                    
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
    
    if (moveFiles && !targetDirectory.empty()) {
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
