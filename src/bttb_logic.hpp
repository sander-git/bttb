#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <regex>
#include <atomic>
#include <functional>

namespace bttb {

struct CDInfo {
    std::string name;
    int64_t capacityBytes = 0; // standard medium capacity
    int64_t sectorSize = 2048; // CD/DVD standard sector size (2KB)
    int64_t slackBytes = 0;    // allowable slack/tolerance
};

struct DirEntry {
    std::string relativePath;  // path relative to the scanned base directory
    int64_t sizeBytes = 0;     // original size
    int64_t sectorCount = 0;   // size in sectors
    bool isDirectory = false;
    bool isSelected = false;   // current backtracking selection
    int64_t prefixSumSectors = 0; // sum of sectors from 0 to current index
    std::vector<std::string> groupedPaths; // Consolidated paths for grouping rules
};

struct GroupRule {
    bool matchFiles = true;
    bool matchFolders = true;
    std::string patternString;
    std::regex compiledRegex;
    bool isRegex = false;
};

struct PackedVolume {
    int volumeIndex = 0;
    int64_t totalBytes = 0;
    std::vector<std::string> itemPaths;
    std::vector<int64_t> itemSizes;
};

// Callback types for thread-safe UI notifications
using LogCallback = std::function<void(const std::string&, int msgType)>;
using ProgressCallback = std::function<void(double currentMediumProgress, double overallProgress)>;

class BttbSolver {
public:
    friend class MainWindow;
    BttbSolver();
    ~BttbSolver() = default;

    // Configuration
    std::string sourceDirectory;
    std::string targetDirectory;
    bool moveFiles = false;
    bool skipEmpty = true;
    bool spanMultipleVolumes = false;
    int maxSearchTimeSeconds = 10;
    int splitDepth = 0; // 0 = split folders at root level, 1 = one level down, etc.
    CDInfo mediumInfo;
    std::vector<GroupRule> groupingRules;
    std::vector<PackedVolume> packedVolumes;
    bool enableTrace = false;
    uint64_t exploredStates = 0;
    uint64_t prunedStates = 0;
    std::chrono::steady_clock::time_point solverStartTime;

    // Control
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> searchTimedOut{false};

    // Callback notifications
    LogCallback logNotify;
    ProgressCallback progressNotify;

    // Public API
    void run();

private:
    std::vector<std::unique_ptr<DirEntry>> itemsToSplit;
    std::vector<int> bestSelectionIndices;
    int64_t currentBestSectors = 0;
    int64_t maxSectors = 0;
    int64_t slackSectors = 0;
    int64_t minNumberOfClusters = 0;

    void scanDirectory();
    int64_t diveDepth(const std::filesystem::path& currentSubpath, int depth);
    void addEntry(const std::string& relPath, int64_t size, bool isDir);
    
    // Core subset-sum recursive backtracking search
    bool findAWay(int64_t currentSectors, int poz);
    
    void saveBestSelection();
    int countClusters(const std::string& path);
};

// Utility function to convert glob string (*.mp3) to std::regex
std::regex globToRegex(const std::string& glob);

} // namespace bttb
