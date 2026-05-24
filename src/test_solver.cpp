#include "bttb_logic.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>

int main() {
    std::cout << "Starting automated offline solver verification..." << std::endl;
    
    // Create a temporary mock folder structure
    std::string test_dir = "./mock_test_dir";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    
    // Create mock files of specific sizes:
    // We want to test subset-sum:
    // Medium capacity = 10 sectors (20,480 bytes)
    // Sector size = 2048 bytes
    // File sizes:
    // file1: 2 sectors (4096 bytes)
    // file2: 3 sectors (6144 bytes)
    // file3: 4 sectors (8192 bytes)
    // file4: 5 sectors (10240 bytes)
    // file5: 8 sectors (16384 bytes)
    
    // Optimal subset of size <= 10 sectors is: 2 + 3 + 5 = 10 sectors (or 2 + 8 = 10 sectors).
    // Let's create these files:
    auto create_mock_file = [](const std::string& path, int64_t size) {
        std::ofstream f(path, std::ios::binary);
        std::vector<char> buffer(size, 0);
        f.write(buffer.data(), size);
    };
    
    create_mock_file(test_dir + "/file1.bin", 4096);  // 2 sectors
    create_mock_file(test_dir + "/file2.bin", 6144);  // 3 sectors
    create_mock_file(test_dir + "/file3.bin", 8192);  // 4 sectors
    create_mock_file(test_dir + "/file4.bin", 10240); // 5 sectors
    create_mock_file(test_dir + "/file5.bin", 16384); // 8 sectors
    
    bttb::BttbSolver solver;
    solver.sourceDirectory = test_dir;
    solver.mediumInfo.capacityBytes = 20480; // 10 sectors * 2048 bytes
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    solver.splitDepth = 0;
    solver.skipEmpty = true;
    
    // Capture logs and selection
    std::vector<std::string> selected_files;
    solver.logNotify = [&](const std::string& msg, int type) {
        std::cout << "[Solver Log] " << msg << std::endl;
        if (msg.rfind(" - ", 0) == 0) {
            // Extracts selected items
            selected_files.push_back(msg.substr(3));
        }
    };
    
    solver.run();
    
    // Clean up mock directory
    std::filesystem::remove_all(test_dir);
    
    std::cout << "\nSolver optimal selections:" << std::endl;
    int64_t total_sectors = 0;
    for (const auto& item : selected_files) {
        std::cout << " * " << item << std::endl;
        if (item.find("file1.bin") != std::string::npos) total_sectors += 2;
        if (item.find("file2.bin") != std::string::npos) total_sectors += 3;
        if (item.find("file3.bin") != std::string::npos) total_sectors += 4;
        if (item.find("file4.bin") != std::string::npos) total_sectors += 5;
        if (item.find("file5.bin") != std::string::npos) total_sectors += 8;
    }
    
    std::cout << "Total fitted sectors: " << total_sectors << " (Expected: 10)" << std::endl;
    
    assert(total_sectors == 10);
    std::cout << "SUCCESS: Bin packing subset-sum solver behaves exactly as designed!" << std::endl;
    
    return 0;
}
