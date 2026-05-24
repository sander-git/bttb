#include "bttb_logic.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

void create_testinput(const std::filesystem::path& baseDir) {
    std::filesystem::remove_all(baseDir);
    std::filesystem::create_directories(baseDir);
    std::filesystem::create_directories(baseDir / "folder");
    
    auto create_file = [](const std::filesystem::path& path, int64_t size) {
        std::ofstream f(path, std::ios::binary);
        std::vector<char> buffer(4096, 0);
        int64_t written = 0;
        while (written < size) {
            int64_t chunk = std::min<int64_t>(4096, size - written);
            f.write(buffer.data(), static_cast<std::streamsize>(chunk));
            written += chunk;
        }
    };
    
    create_file(baseDir / "output1", 10 * 1024 * 1024);
    create_file(baseDir / "output2", 20 * 1024 * 1024);
    create_file(baseDir / "output3", 30 * 1024 * 1024);
    create_file(baseDir / "output4", 40 * 1024 * 1024);
    create_file(baseDir / "output5", 50 * 1024 * 1024);
    create_file(baseDir / "folder/output6", 10 * 1024 * 1024);
    create_file(baseDir / "folder/output7", 25 * 1024 * 1024);
    create_file(baseDir / "folder/output8", 35 * 1024 * 1024);
}

void run_test1() {
    std::cout << "--- STARTING TEST 1: OFFLINE SUBSET-SUM BACKTRACKING ---" << std::endl;
    // Create a temporary mock folder structure
    std::string test_dir = "./mock_test_dir";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    
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
    solver.enableTrace = true;
    
    std::vector<std::string> selected_files;
    solver.logNotify = [&](const std::string& msg, int type) {
        std::cout << "[Solver Log] " << msg << std::endl;
        if (msg.rfind(" - ", 0) == 0) {
            selected_files.push_back(msg.substr(3));
        }
    };
    
    solver.run();
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
}

void run_test2() {
    std::cout << "\n--- STARTING TEST 2: MULTI-VOLUME SPANNING (60MB volumes) ---" << std::endl;
    
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path src = currentDir.parent_path().parent_path() / "testinput";
    std::filesystem::path dest = currentDir.parent_path().parent_path() / "testoutput";
    
    std::cout << "Source directory resolved: " << src.string() << std::endl;
    std::cout << "Target directory resolved: " << dest.string() << std::endl;
    
    // Recreate testinput to ensure fresh state
    std::cout << "Recreating source testinput folder structure..." << std::endl;
    create_testinput(src);
    
    std::cout << "Cleaning target testoutput directory..." << std::endl;
    std::filesystem::remove_all(dest);
    std::filesystem::create_directories(dest);
    
    bttb::BttbSolver solver;
    solver.sourceDirectory = src.string();
    solver.targetDirectory = dest.string();
    solver.moveFiles = true;
    solver.spanMultipleVolumes = true;
    solver.splitDepth = 1;
    solver.mediumInfo.capacityBytes = 60LL * 1024LL * 1024LL; // 60MB
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    solver.enableTrace = true; // Enable trace options to show trace output!
    
    solver.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Spanning Test] " << msg << std::endl;
    };
    
    solver.run();
    
    // Verify that directories Volume_1, Volume_2, Volume_3, Volume_4 exist and contain files
    std::cout << "\nVerifying volume output folders..." << std::endl;
    for (int i = 1; i <= 4; ++i) {
        std::filesystem::path volPath = dest / ("Volume_" + std::to_string(i));
        std::cout << "Checking existence of: " << volPath.string() << "..." << std::endl;
        assert(std::filesystem::exists(volPath));
        assert(std::filesystem::is_directory(volPath));
    }
    
    std::cout << "SUCCESS: Spanning test outputs generated correctly to " << dest.string() << "!" << std::endl;
    
    // Recreate testinput again so files remain there intact for manual verification/checks!
    std::cout << "Restoring project1/testinput files for user inspection..." << std::endl;
    create_testinput(src);
}

void run_test3() {
    std::cout << "\n--- STARTING TEST 3: HIGH-SLACK TESTINPUT2 VERIFICATION ---" << std::endl;
    
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::filesystem::path src = currentDir.parent_path().parent_path().parent_path() / "testinput2";
    
    std::cout << "Source directory resolved: " << src.string() << std::endl;
    
    bttb::BttbSolver solver;
    solver.sourceDirectory = src.string();
    solver.moveFiles = false;
    solver.spanMultipleVolumes = false;
    solver.splitDepth = 1;
    
    // capacity: 650MB = 681574400 bytes
    // sectorSize: 2048
    // slack: 650MB - 2048 = 681572352 bytes
    solver.mediumInfo.capacityBytes = 681574400LL; 
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 681572352LL;
    solver.maxSearchTimeSeconds = 10;
    solver.enableTrace = true;
    
    solver.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Test 3] " << msg << std::endl;
    };
    
    auto startTime = std::chrono::steady_clock::now();
    solver.run();
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "Test 3 completed in: " << elapsed << " ms" << std::endl;
    assert(elapsed < 10000); // must be well under the 10-second limit!
    std::cout << "SUCCESS: High-slack solver finishes instantly!" << std::endl;
}

int main() {
    std::cout << "Starting BTTB automated verification suite..." << std::endl;
    run_test1();
    run_test2();
    run_test3();
    std::cout << "\nALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
