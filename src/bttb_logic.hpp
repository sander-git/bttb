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

struct CustomVolume {
    std::string name;
    int64_t capacityBytes = 0;
    int64_t sectorSize = 2048;
    int64_t slackBytes = 0;
};

struct DirEntry {
    std::string relativePath;  // path relative to the scanned base directory
    std::string absolutePath;  // actual full absolute path on filesystem
    int64_t sizeBytes = 0;     // original size
    int64_t sectorCount = 0;   // size in sectors
    bool isDirectory = false;
    bool isSelected = false;   // current backtracking selection
    int64_t prefixSumSectors = 0; // sum of sectors from 0 to current index
    std::vector<std::string> groupedPaths; // Consolidated paths for grouping rules
    std::vector<std::string> absoluteGroupedPaths; // Consolidated absolute paths matching groupedPaths
    std::vector<float> embedding; // 384 float MiniLM semantic embedding
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
    std::vector<std::vector<std::string>> itemGroupedPaths;
    std::vector<std::string> itemDates;
};

// Callback types for thread-safe UI notifications
using LogCallback = std::function<void(const std::string&, int msgType)>;
using ProgressCallback = std::function<void(double currentMediumProgress, double overallProgress)>;
using RecommendCapacityCallback = std::function<bool(int64_t recommendedCapacityBytes)>;
using TimeLeftCallback = std::function<void(double secondsLeft)>;

class BttbSolver {
public:
    friend class MainWindow;
    BttbSolver();
    ~BttbSolver() = default;

    // Configuration
    std::string sourceDirectory;
    std::vector<std::string> sourceDirectories;
    std::string targetDirectory;
    bool moveFiles = false;
    bool createSymlinks = true;
    bool skipEmpty = true;
    bool skipUnreadable = true;
    bool spanMultipleVolumes = false;
    int maxSearchTimeSeconds = 10;
    int splitDepth = 0; // 0 = split folders at root level, 1 = one level down, etc.
    CDInfo mediumInfo;
    std::vector<GroupRule> groupingRules;
    std::vector<PackedVolume> packedVolumes;
    std::vector<std::unique_ptr<DirEntry>> itemsToSplit;
    bool enableTrace = false;
    uint64_t exploredStates = 0;
    uint64_t prunedStates = 0;
    std::chrono::steady_clock::time_point solverStartTime;

    // Semantic Packing (Milestone v4.0.0)
    std::string semanticPrompt;
    bool enableSemanticPacking = false;
    bool testOnlyMode = false;
    double semanticCoherenceFactor = 0.7;

    // v4.1.0-4.1.1 Enhancements
    std::vector<CustomVolume> customVolumes;
    bool ruleBasedWins = true;
    int lastSelectedVolumeIndex = 2;
    bool enableAutoVolume = false;
    bool enableDarkTheme = false;

    // v4.2.0 PAR3 & JSON Enhancements
    bool enablePar3 = false;
    int64_t par3BlockSize = 2048;
    int par3RedundancyPercent = 10;
    
    // v4.3.0 Localization Enhancement
    std::string language = "auto";


    // Control
    std::atomic<bool> stopRequested{false};
    std::atomic<bool> searchTimedOut{false};

    // Callback notifications
    LogCallback logNotify;
    ProgressCallback progressNotify;
    RecommendCapacityCallback recommendCapacityNotify;
    TimeLeftCallback timeLeftNotify;

    // Public API
    void run();
    void loadSettings();
    void saveSettings();

    // Semantic Helpers
    double computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);
    void runSemanticClustering();

private:
    std::vector<int> bestSelectionIndices;
    int64_t currentBestSectors = 0;
    int64_t maxSectors = 0;
    int64_t slackSectors = 0;
    int64_t minNumberOfClusters = 0;

    void scanDirectory();
    int64_t diveDepth(const std::filesystem::path& baseDir, const std::filesystem::path& currentSubpath, int depth);
    void addEntry(const std::string& relPath, const std::string& absPath, int64_t size, bool isDir);
    
    // Core subset-sum recursive backtracking search
    bool findAWay(int64_t currentSectors, int poz, int selectedFileCount);
    
    void saveBestSelection();
    int countClusters(const std::string& path);


};

// Utility function to convert glob string (*.mp3) to std::regex
std::regex globToRegex(const std::string& glob);

// Parse human-readable sizes like 512MB, 2.5GB, 1.5TB
int64_t parseHumanSize(const std::string& input);

// Ignore folders nested in other folders
std::vector<std::string> filterNestedDirectories(const std::vector<std::string>& dirs);

// Unicode & Long Path utility conversions
std::string wstringToUtf8(const std::wstring& wstr);
std::wstring utf8ToWstring(const std::string& str);
std::filesystem::path utf8Path(const std::string& utf8Str);
std::string toUtf8Str(const std::filesystem::path& p);
std::filesystem::path makeLongPath(const std::filesystem::path& p);

// PAR3 & JSON functions (v4.2.0)
bool createVolumePar3(const std::string& volumePath, const std::string& parBaseName, int64_t blockSize, int redundancyPercent, std::string& errorMsg);
int verifyVolumePar3(const std::string& volumePath, const std::string& parBaseName, std::vector<std::string>& damagedFiles, std::string& logOutput);
bool restoreVolumePar3(const std::string& volumePath, const std::string& destPath, const std::string& parBaseName, std::string& logOutput);
bool parseIndexJson(const std::string& jsonFilePath, std::vector<PackedVolume>& volumes, std::string& errorMsg);

} // namespace bttb
