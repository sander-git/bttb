#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include "bttb_logic.hpp"

// Utility helper to read clean lines
std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

// Spin indicator for background solver wait
void show_spinner(const std::atomic<bool>& running) {
    const char spinner[] = {'|', '/', '-', '\\'};
    int i = 0;
    while (running) {
        std::cout << "\rSolving... " << spinner[i] << std::flush;
        i = (i + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
    std::cout << "\r                  \r" << std::flush;
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "         Burn to the Brim (Windows Port)         " << std::endl;
    std::cout << "==================================================" << std::endl << std::endl;
    
    bttb::BttbSolver solver;
    
    // 1. Source Folder
    while (true) {
        std::string src = read_line("Enter Source Folder path: ");
        if (src.empty()) {
            std::cout << "Error: Source folder cannot be empty." << std::endl;
            continue;
        }
        if (!std::filesystem::exists(src)) {
            std::cout << "Error: Folder does not exist." << std::endl;
            continue;
        }
        solver.sourceDirectory = src;
        break;
    }
    
    // 2. Target Folder & Move Choice
    std::string move_choice = read_line("Move fitted files to a target directory? (y/n): ");
    if (move_choice == "y" || move_choice == "Y") {
        solver.moveFiles = true;
        while (true) {
            std::string dest = read_line("Enter Target Folder path: ");
            if (dest.empty()) {
                std::cout << "Error: Target folder cannot be empty." << std::endl;
                continue;
            }
            solver.targetDirectory = dest;
            break;
        }
    } else {
        solver.moveFiles = false;
    }
    
    // 3. Media Selection
    std::cout << "\nSelect Storage Medium:" << std::endl;
    std::cout << " 1. CD (650 MB)" << std::endl;
    std::cout << " 2. CD (700 MB)" << std::endl;
    std::cout << " 3. DVD (4.7 GB)" << std::endl;
    std::cout << " 4. DVD DL (8.5 GB)" << std::endl;
    std::cout << " 5. Custom Capacity" << std::endl;
    
    int choice = 0;
    while (true) {
        std::string s_choice = read_line("Enter choice (1-5): ");
        try {
            choice = std::stoi(s_choice);
            if (choice >= 1 && choice <= 5) break;
        } catch (...) {}
        std::cout << "Invalid choice. Please enter 1-5." << std::endl;
    }
    
    if (choice == 1) {
        solver.mediumInfo.capacityBytes = 681574400;
        solver.mediumInfo.sectorSize = 2048;
    } else if (choice == 2) {
        solver.mediumInfo.capacityBytes = 734003200;
        solver.mediumInfo.sectorSize = 2048;
    } else if (choice == 3) {
        solver.mediumInfo.capacityBytes = 4700000000;
        solver.mediumInfo.sectorSize = 2048;
    } else if (choice == 4) {
        solver.mediumInfo.capacityBytes = 8500000000;
        solver.mediumInfo.sectorSize = 2048;
    } else {
        std::string cap = read_line("Enter custom capacity in Bytes: ");
        solver.mediumInfo.capacityBytes = std::stoll(cap);
        std::string clus = read_line("Enter cluster size in Bytes (default 2048): ");
        solver.mediumInfo.sectorSize = clus.empty() ? 2048 : std::stoll(clus);
    }
    
    // Slack Bytes
    std::string slack = read_line("Enter allowable slack capacity in Bytes (default 0): ");
    solver.mediumInfo.slackBytes = slack.empty() ? 0 : std::stoll(slack);
    
    // 4. Split Depth
    std::string depth = read_line("Enter Directory Split Depth (default 0): ");
    solver.splitDepth = depth.empty() ? 0 : std::stoi(depth);
    
    // 5. Skip Empty
    std::string empty = read_line("Skip empty files and folders? (y/n, default y): ");
    solver.skipEmpty = !(empty == "n" || empty == "N");
    
    // 6. Grouping Rules
    std::cout << "\nEnter file grouping rules (e.g. *.mp3, press Enter on empty line to finish):" << std::endl;
    while (true) {
        std::string pattern = read_line(" * Add pattern: ");
        if (pattern.empty()) break;
        
        bttb::GroupRule rule;
        rule.patternString = pattern;
        rule.matchFiles = true;
        rule.matchFolders = true;
        rule.isRegex = false;
        rule.compiledRegex = bttb::globToRegex(pattern);
        solver.groupingRules.push_back(rule);
    }
    
    // Setup standard console log output
    solver.logNotify = [](const std::string& msg, int type) {
        std::string prefix = "[INFO]      ";
        if (type == 1)      prefix = "[SUCCESS]   ";
        else if (type == 2) prefix = "[ERROR]     ";
        else if (type == 3) prefix = "[IMPORTANT] ";
        std::cout << prefix << msg << std::endl;
    };
    
    std::cout << "\nStarting bin-packing solving thread..." << std::endl;
    
    std::atomic<bool> solver_running{true};
    
    // Launch background thread resembling GTK thread model
    std::jthread solver_thread([&]() {
        solver.run();
        solver_running = false;
    });
    
    // Launch spinner to track progress
    show_spinner(solver_running);
    
    if (solver_thread.joinable()) {
        solver_thread.join();
    }
    
    std::cout << "\n==================================================" << std::endl;
    std::cout << "         Processing Finished. Press Enter.        " << std::endl;
    std::cout << "==================================================" << std::endl;
    std::string exit_wait;
    std::getline(std::cin, exit_wait);
    
    return 0;
}
