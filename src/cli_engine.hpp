#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "bttb_logic.hpp"

namespace bttb {

inline void printCliHelp(const char* prog) {
    std::cout << "==================================================\n";
    std::cout << "         Burn to the Brim (BTTB) CLI Engine      \n";
    std::cout << "==================================================\n";
    std::cout << "Usage: " << prog << " [options] <source_folder1> [source_folder2] ...\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help                 Show this help manual\n";
    std::cout << "  -t, --target <path>        Specify target directory for copy/move/symlink\n";
    std::cout << "  -c, --capacity <size>      Medium size (e.g. 650MB, 4.7GB, 512MB, 256GB, 1.5TB)\n";
    std::cout << "                             Default: CD (650MB)\n";
    std::cout << "  -s, --sector <bytes>       Cluster/sector alignment in bytes (default: 2048 for CD/DVD, 4096 for USB)\n";
    std::cout << "  -k, --slack <size>         Allowed slack tolerance space (e.g. 2.5MB, 2048B)\n";
    std::cout << "  -d, --depth <level>        Directories splitting waterlevel depth (default: 0)\n";
    std::cout << "  -m, --move                 Move fitted files instead of copying\n";
    std::cout << "  -l, --symlink              Create symbolic links in target folder (Default)\n";
    std::cout << "  --span                     Span fitted items recursively across multiple volumes\n";
    std::cout << "  --no-skip-empty            Do not skip empty files or folders\n";
    std::cout << "  --no-skip-unreadable       Abort scanner if unreadable files are encountered\n";
    std::cout << "  --trace                    Show verbose branching backtracking diagnostics\n";
    std::cout << "  --time <seconds>           Kombinatoric search time limit in seconds (default: 10s)\n";
    std::cout << "  --rules <patterns>         Semicolon-separated glob matching groupings (e.g., \"*.mp3;*.wav\")\n";
    std::cout << "  --semantic <prompt>        Enable MiniLM entropy-aware semantic packing with a prompt\n";
    std::cout << "  --test                     Run solver calculations in interactive simulation test mode (no disk writes)\n";
    std::cout << "  -gui, --gui                Force loading the GUI interface\n";
}

inline int runCliEngine(int argc, char* argv[]) {
    BttbSolver solver;
    
    // Default options
    solver.mediumInfo.capacityBytes = 681574400; // CD 650MB
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    solver.createSymlinks = true;
    solver.moveFiles = false;
    
    std::vector<std::string> rawSources;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printCliHelp(argv[0]);
            return 0;
        } else if ((arg == "-t" || arg == "--target") && i + 1 < argc) {
            solver.targetDirectory = argv[++i];
        } else if ((arg == "-c" || arg == "--capacity") && i + 1 < argc) {
            std::string capStr = argv[++i];
            solver.mediumInfo.capacityBytes = parseHumanSize(capStr);
            // Default sector size to 4096 if capacity is large (> 8GB) suggesting USB/filesystem
            if (solver.mediumInfo.capacityBytes > 8000000000LL) {
                solver.mediumInfo.sectorSize = 4096;
            }
        } else if ((arg == "-s" || arg == "--sector") && i + 1 < argc) {
            solver.mediumInfo.sectorSize = std::stoll(argv[++i]);
        } else if ((arg == "-k" || arg == "--slack") && i + 1 < argc) {
            solver.mediumInfo.slackBytes = parseHumanSize(argv[++i]);
        } else if ((arg == "-d" || arg == "--depth") && i + 1 < argc) {
            solver.splitDepth = std::stoi(argv[++i]);
        } else if (arg == "-m" || arg == "--move") {
            solver.moveFiles = true;
            solver.createSymlinks = false;
        } else if (arg == "-l" || arg == "--symlink") {
            solver.createSymlinks = true;
            solver.moveFiles = false;
        } else if (arg == "--span") {
            solver.spanMultipleVolumes = true;
        } else if (arg == "--no-skip-empty") {
            solver.skipEmpty = false;
        } else if (arg == "--no-skip-unreadable") {
            solver.skipUnreadable = false;
        } else if (arg == "--trace") {
            solver.enableTrace = true;
        } else if (arg == "--semantic" && i + 1 < argc) {
            solver.semanticPrompt = argv[++i];
            solver.enableSemanticPacking = true;
        } else if (arg == "--test") {
            solver.testOnlyMode = true;
        } else if (arg == "--time" && i + 1 < argc) {
            solver.maxSearchTimeSeconds = std::stoi(argv[++i]);
        } else if (arg == "--rules" && i + 1 < argc) {
            std::string rulesStr = argv[++i];
            std::string temp = "";
            for (char c : rulesStr) {
                if (c == ';') {
                    if (!temp.empty()) {
                        GroupRule r;
                        r.patternString = temp;
                        r.matchFiles = true;
                        r.matchFolders = true;
                        r.isRegex = false;
                        r.compiledRegex = globToRegex(temp);
                        solver.groupingRules.push_back(r);
                        temp = "";
                    }
                } else {
                    temp += c;
                }
            }
            if (!temp.empty()) {
                GroupRule r;
                r.patternString = temp;
                r.matchFiles = true;
                r.matchFolders = true;
                r.isRegex = false;
                r.compiledRegex = globToRegex(temp);
                solver.groupingRules.push_back(r);
            }
        } else if (arg == "-gui" || arg == "--gui") {
            // Handled outside
        } else {
            // Assume positional source directory
            if (!arg.empty() && arg[0] != '-') {
                rawSources.push_back(arg);
            }
        }
    }
    
    if (rawSources.empty()) {
        std::cerr << "Error: No source directories specified.\n\n";
        printCliHelp(argv[0]);
        return 1;
    }
    
    // Semicolon join sources
    solver.sourceDirectories = rawSources;
    if (rawSources.size() == 1) {
        solver.sourceDirectory = rawSources[0];
    } else {
        std::string joined = "";
        for (size_t i = 0; i < rawSources.size(); ++i) {
            if (i > 0) joined += ";";
            joined += rawSources[i];
        }
        solver.sourceDirectory = joined;
    }
    
    // Set standard CLI notifications
    solver.logNotify = [](const std::string& msg, int type) {
        std::string prefix = "[INFO]      ";
        if (type == 1)      prefix = "\033[1;32m[SUCCESS]\033[0m   ";
        else if (type == 2) prefix = "\033[1;31m[ERROR]\033[0m     ";
        else if (type == 3) prefix = "\033[1;36m[IMPORTANT]\033[0m ";
        std::cout << prefix << msg << std::endl;
    };
    
    // Terminal recommended capacity query
    solver.recommendCapacityNotify = [](int64_t recommendedBytes) -> CapacityRecommendResult {
        double recMB = (double)recommendedBytes / (1024.0 * 1024.0);
        std::cout << "\n\033[1;33m[WARNING] Minimum volume size recommended: " 
                  << recMB << " MB (" << recommendedBytes << " bytes) based on maximum file size scanned.\033[0m" << std::endl;
        std::cout << "Choose action:\n"
                  << "  [r] Resize: Automatically adapt the volume size and retry\n"
                  << "  [s] Skip: Skip any larger files and continue\n"
                  << "  [c] Cancel: Abort solving\n"
                  << "Choice (r/s/c): " << std::flush;
        std::string answer;
        std::getline(std::cin, answer);
        if (answer == "r" || answer == "R" || answer == "resize" || answer == "RESIZE") {
            return CapacityRecommendResult::RESIZE;
        } else if (answer == "s" || answer == "S" || answer == "skip" || answer == "SKIP") {
            return CapacityRecommendResult::SKIP_LARGER;
        } else {
            return CapacityRecommendResult::CANCEL;
        }
    };
    
    std::cout << "Starting solver process in CLI mode...\n";
    solver.run();
    return 0;
}

// Determines if argv has any CLI-specific flags
inline bool isCliModeTriggered(int argc, char* argv[]) {
    bool hasGuiFlag = false;
    bool hasCliOption = false;
    int dirCount = 0;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-gui" || arg == "--gui") {
            hasGuiFlag = true;
        } else if (arg == "-h" || arg == "--help" || arg == "-t" || arg == "--target" ||
                   arg == "-c" || arg == "--capacity" || arg == "-s" || arg == "--sector" ||
                   arg == "-k" || arg == "--slack" || arg == "-d" || arg == "--depth" ||
                   arg == "-m" || arg == "--move" || arg == "-l" || arg == "--symlink" ||
                   arg == "--span" || arg == "--no-skip-empty" || arg == "--no-skip-unreadable" ||
                   arg == "--trace" || arg == "--time" || arg == "--rules" ||
                   arg == "--semantic" || arg == "--test") {
            hasCliOption = true;
        } else if (!arg.empty() && arg[0] != '-') {
            dirCount++;
        }
    }
    
    if (hasGuiFlag) return false;
    if (hasCliOption) return true;
    if (dirCount > 1) return true; // multiple source directories implies CLI mode
    
    return false; // single source directory with no other CLI options defaults to GUI mode
}

} // namespace bttb
