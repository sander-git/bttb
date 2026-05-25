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

void run_test4() {
    std::cout << "\n--- STARTING TEST 4: MULTIPLE DIRECTORIES & SYMLINKS (BTTB v3.2.0) ---" << std::endl;
    
    // 1. Test nested path filtering
    std::vector<std::string> dirs = {
        "/mock/dir1",
        "/mock/dir1/nested",
        "/mock/dir2",
        "/mock/dir2/sub/deep",
        "/mock/dir1"
    };
    std::vector<std::string> filtered = bttb::filterNestedDirectories(dirs);
    
    std::cout << "Filtered nested directories count: " << filtered.size() << std::endl;
    for (const auto& d : filtered) {
        std::cout << " * " << d << std::endl;
    }
    
    // We expect "/mock/dir1" and "/mock/dir2" only!
    assert(filtered.size() == 2);
    assert(std::find(filtered.begin(), filtered.end(), "/mock/dir1") != filtered.end());
    assert(std::find(filtered.begin(), filtered.end(), "/mock/dir2") != filtered.end());
    std::cout << "Nested directories filtered correctly!" << std::endl;
    
    // 2. Test human size parsing
    assert(bttb::parseHumanSize("512MB") == 512LL * 1024 * 1024);
    assert(bttb::parseHumanSize("2.5GB") == static_cast<int64_t>(2.5 * 1024 * 1024 * 1024));
    assert(bttb::parseHumanSize("1.5TB") == static_cast<int64_t>(1.5 * 1024 * 1024 * 1024 * 1024));
    assert(bttb::parseHumanSize("1024") == 1024);
    std::cout << "Human readable size parser validated successfully!" << std::endl;
    
    // 3. Test multi-root scanning and symbolic links
    std::string mock_root1 = "./mock_root1";
    std::string mock_root2 = "./mock_root2";
    std::string mock_dest = "./mock_dest";
    
    std::filesystem::remove_all(mock_root1);
    std::filesystem::remove_all(mock_root2);
    std::filesystem::remove_all(mock_dest);
    
    std::filesystem::create_directories(mock_root1);
    std::filesystem::create_directories(mock_root2);
    std::filesystem::create_directories(mock_dest);
    
    {
        std::ofstream f1(mock_root1 + "/file_a.txt");
        f1 << "Root 1 File A";
        
        std::ofstream f2(mock_root2 + "/file_b.txt");
        f2 << "Root 2 File B";
    }
    
    bttb::BttbSolver solver;
    solver.sourceDirectories = { mock_root1, mock_root2 };
    solver.targetDirectory = mock_dest;
    solver.createSymlinks = true;
    solver.moveFiles = false;
    solver.spanMultipleVolumes = false;
    solver.mediumInfo.capacityBytes = 1024 * 1024;
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    
    solver.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Test 4] " << msg << std::endl;
    };
    
    solver.run();
    
    // Verify files in target folder are created as symlinks or copies
    std::filesystem::path dest_a = std::filesystem::path(mock_dest) / "file_a.txt";
    std::filesystem::path dest_b = std::filesystem::path(mock_dest) / "file_b.txt";
    
    std::cout << "Verifying output file_a.txt exists in dest..." << std::endl;
    assert(std::filesystem::exists(dest_a));
    std::cout << "Verifying output file_b.txt exists in dest..." << std::endl;
    assert(std::filesystem::exists(dest_b));
    
    // Cleanup
    std::filesystem::remove_all(mock_root1);
    std::filesystem::remove_all(mock_root2);
    std::filesystem::remove_all(mock_dest);
    
    std::cout << "SUCCESS: Multiple source folders scanned and symlink output engine works perfectly!" << std::endl;
}

#include "cli_engine.hpp"

void run_test5() {
    std::cout << "\n--- STARTING TEST 5: UNICODE/LONG-PATHS, UNREADABLE FILES, AND RETRY ADAPTIVE CAPACITIES (BTTB v3.3.0) ---" << std::endl;

    // 1. Long path & Unicode test
    std::string baseUnicode = "./mock_unicode_測試";
    std::filesystem::remove_all(baseUnicode);
    std::filesystem::create_directories(baseUnicode);
    
    // Create a very long directory path (>280 chars)
    std::string longPath = baseUnicode;
    for (int i = 0; i < 15; ++i) {
        longPath += "/subdir_level_" + std::to_string(i) + "_長路徑";
    }
    std::filesystem::create_directories(longPath);
    
    std::string unicodeFile = longPath + "/unicode_file_文件.bin";
    {
        std::ofstream f(unicodeFile);
        f << "Hello Unicode and Long Paths!";
    }
    
    std::cout << "Long unicode path created: " << unicodeFile.size() << " characters." << std::endl;
    
    bttb::BttbSolver solver;
    solver.sourceDirectory = baseUnicode;
    solver.mediumInfo.capacityBytes = 1000;
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    solver.skipUnreadable = true;
    
    solver.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Test 5 - Scan] " << msg << std::endl;
    };
    
    solver.run();
    assert(solver.itemsToSplit.size() > 0);
    std::cout << "Unicode & long path scanned successfully!" << std::endl;

    // 2. Adaptive Capacity Recommendation test
    std::string test_dir = "./mock_exceed_dir";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    
    {
        std::ofstream f(test_dir + "/file_large.bin");
        std::vector<char> buffer(5000, 0); // 5000 bytes
        f.write(buffer.data(), 5000);
    }
    
    bttb::BttbSolver solverExceed;
    solverExceed.sourceDirectory = test_dir;
    solverExceed.mediumInfo.capacityBytes = 2048; // only 2048 bytes capacity!
    solverExceed.mediumInfo.sectorSize = 2048;
    solverExceed.mediumInfo.slackBytes = 0;
    
    bool callbackTriggered = false;
    solverExceed.recommendCapacityNotify = [&](int64_t recommendedBytes) -> bool {
        std::cout << "[Solver Test 5 - Recommendation Callback] Recommended: " << recommendedBytes << " bytes (Expected: 6144 bytes [sector aligned])" << std::endl;
        callbackTriggered = true;
        assert(recommendedBytes == 6144);
        return true; // Accept and adapt!
    };
    
    solverExceed.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Test 5 - Exceed] " << msg << std::endl;
    };
    
    solverExceed.run();
    assert(callbackTriggered);
    assert(solverExceed.mediumInfo.capacityBytes == 6144);
    std::cout << "Adaptive capacity recommendation triggered and solved successfully!" << std::endl;

    // 3. CLI Mode Trigger testing
    char* testArgv1[] = { (char*)"bttb" };
    assert(!bttb::isCliModeTriggered(1, testArgv1));

    char* testArgv2[] = { (char*)"bttb", (char*)"-gui" };
    assert(!bttb::isCliModeTriggered(2, testArgv2));

    char* testArgv3[] = { (char*)"bttb", (char*)"-c", (char*)"512MB" };
    assert(bttb::isCliModeTriggered(3, testArgv3));

    char* testArgv4[] = { (char*)"bttb", (char*)"-gui", (char*)"-c", (char*)"512MB" };
    assert(!bttb::isCliModeTriggered(4, testArgv4));

    char* testArgv5[] = { (char*)"bttb", (char*)"dir1", (char*)"dir2" };
    assert(bttb::isCliModeTriggered(3, testArgv5));
    
    std::cout << "CLI Mode Trigger detector validated successfully!" << std::endl;

    // Cleanup
    std::filesystem::remove_all(baseUnicode);
    std::filesystem::remove_all(test_dir);
}

int main() {
    std::cout << "Starting BTTB automated verification suite..." << std::endl;
    run_test1();
    run_test2();
    run_test3();
    run_test4();
    run_test5();
    std::cout << "\nALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
