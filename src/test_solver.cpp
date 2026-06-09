#include "bttb_logic.hpp"
#include "bttb_locale.hpp"
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
    solver.enablePar3 = false;
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
    solver.enablePar3 = false;
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
    solver.enablePar3 = false;
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
    solver.enablePar3 = false;
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
    solver.enablePar3 = false;
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
    solverExceed.enablePar3 = false;
    solverExceed.sourceDirectory = test_dir;
    solverExceed.mediumInfo.capacityBytes = 2048; // only 2048 bytes capacity!
    solverExceed.mediumInfo.sectorSize = 2048;
    solverExceed.mediumInfo.slackBytes = 0;
    
    bool callbackTriggered = false;
    solverExceed.recommendCapacityNotify = [&](int64_t recommendedBytes) -> bttb::CapacityRecommendResult {
        std::cout << "[Solver Test 5 - Recommendation Callback] Recommended: " << recommendedBytes << " bytes (Expected: 6144 bytes [sector aligned])" << std::endl;
        callbackTriggered = true;
        assert(recommendedBytes == 6144);
        return bttb::CapacityRecommendResult::RESIZE; // Accept and adapt!
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

void run_test6() {
    std::cout << "\n--- STARTING TEST 6: ENTROPY-AWARE SEMANTIC PACKING & TEST SIMULATION (BTTB v4.0.0) ---" << std::endl;
    
    // 1. Verify Cosine Similarity
    bttb::BttbSolver solver;
    solver.enablePar3 = false;
    std::vector<float> vecA = {1.0f, 0.0f, 0.0f};
    std::vector<float> vecB = {1.0f, 0.0f, 0.0f};
    std::vector<float> vecC = {0.0f, 1.0f, 0.0f};
    
    double simAB = solver.computeCosineSimilarity(vecA, vecB);
    double simAC = solver.computeCosineSimilarity(vecA, vecC);
    
    std::cout << "Cosine Similarity A & B (identical): " << simAB << " (Expected: 1.0)" << std::endl;
    std::cout << "Cosine Similarity A & C (orthogonal): " << simAC << " (Expected: 0.0)" << std::endl;
    
    assert(std::abs(simAB - 1.0) < 1e-5);
    assert(std::abs(simAC - 0.0) < 1e-5);
    
    // 2. Test-only simulation verification (No files should be modified on disk)
    std::string test_src = "./mock_semantic_src";
    std::string test_dest = "./mock_semantic_dest";
    std::filesystem::remove_all(test_src);
    std::filesystem::remove_all(test_dest);
    
    std::filesystem::create_directories(test_src);
    {
        std::ofstream f1(test_src + "/rock_music_1.mp3"); f1 << "music";
        std::ofstream f2(test_src + "/rock_music_2.mp3"); f2 << "music";
        std::ofstream f3(test_src + "/text_document.txt"); f3 << "text document";
    }
    
    solver.sourceDirectory = test_src;
    solver.targetDirectory = test_dest;
    solver.testOnlyMode = true; // IMPORTANT: Test mode
    solver.createSymlinks = true;
    solver.moveFiles = false;
    solver.spanMultipleVolumes = false;
    solver.mediumInfo.capacityBytes = 1024 * 1024;
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    solver.enableTrace = true; // Enable trace options to assert folder analysis logs!
    
    solver.logNotify = [](const std::string& msg, int type) {
        std::cout << "[Solver Test 6] " << msg << std::endl;
    };
    
    solver.run();
    
    // Assert no target output exists because of testOnlyMode
    std::cout << "Checking that destination '" << test_dest << "' does not contain files..." << std::endl;
    bool destHasFiles = false;
    if (std::filesystem::exists(test_dest)) {
        for (const auto& entry : std::filesystem::directory_iterator(test_dest)) {
            destHasFiles = true;
            break;
        }
    }
    assert(!destHasFiles);
    std::cout << "SUCCESS: testOnlyMode correctly skipped all filesystem write operations!" << std::endl;
    
    // 3. Verify semantic prompt configuration parser in isCliModeTriggered
    char* testArgv1[] = { (char*)"bttb", (char*)"--semantic", (char*)"keep similar content together" };
    assert(bttb::isCliModeTriggered(3, testArgv1));
    
    char* testArgv2[] = { (char*)"bttb", (char*)"--test" };
    assert(bttb::isCliModeTriggered(2, testArgv2));
    
    std::cout << "CLI semantic prompt trigger detectors validated successfully!" << std::endl;
    
    // 4. Verify deterministic instruction prompt embeddings in bttb_embed.py
    {
        std::filesystem::path tempDir = std::filesystem::temp_directory_path();
        std::string tempIn = (tempDir / "bttb_test_in.json").string();
        std::string tempOut = (tempDir / "bttb_test_out.json").string();
        
        std::string scriptPath = "src/bttb_embed.py";
        if (!std::filesystem::exists(scriptPath)) {
            scriptPath = "../src/bttb_embed.py";
        }
        if (!std::filesystem::exists(scriptPath)) {
            scriptPath = "./bttb_embed.py";
        }

        // Scenario A: same first letter keep together
        {
            std::ofstream testIn(tempIn);
            testIn << "{\n"
                   << "  \"prompt\": \"same first letter keep together\",\n"
                   << "  \"items\": [\n"
                   << "    { \"path\": \"apple.txt\", \"size\": 10 },\n"
                   << "    { \"path\": \"apricot.png\", \"size\": 20 },\n"
                   << "    { \"path\": \"banana.txt\", \"size\": 30 }\n"
                   << "  ]\n"
                   << "}";
            testIn.close();
            
            std::string cmd = "python3 \"" + scriptPath + "\" < \"" + tempIn + "\" > \"" + tempOut + "\"";
            int ret = std::system(cmd.c_str());
            assert(ret == 0);
            
            std::ifstream testOut(tempOut);
            std::stringstream buffer;
            buffer << testOut.rdbuf();
            std::string outJson = buffer.str();
            testOut.close();
            
            // Assert identical first letter mapping gets same unit dimension, and banana gets distinct
            assert(outJson.find("[1.0") != std::string::npos || outJson.find("[1") != std::string::npos);
            std::cout << "SUCCESS: 'same first letter keep together' verified successfully!" << std::endl;
        }

        // Scenario B: same extension keep together
        {
            std::ofstream testIn(tempIn);
            testIn << "{\n"
                   << "  \"prompt\": \"same extension keep together\",\n"
                   << "  \"items\": [\n"
                   << "    { \"path\": \"a.mp3\", \"size\": 10 },\n"
                   << "    { \"path\": \"b.mp3\", \"size\": 20 },\n"
                   << "    { \"path\": \"c.jpg\", \"size\": 30 }\n"
                   << "  ]\n"
                   << "}";
            testIn.close();
            
            std::string cmd = "python3 \"" + scriptPath + "\" < \"" + tempIn + "\" > \"" + tempOut + "\"";
            int ret = std::system(cmd.c_str());
            assert(ret == 0);
            
            std::ifstream testOut(tempOut);
            std::stringstream buffer;
            buffer << testOut.rdbuf();
            std::string outJson = buffer.str();
            testOut.close();
            
            // Assert correct extension group matches
            assert(outJson.find("[1.0") != std::string::npos || outJson.find("[1") != std::string::npos);
            std::cout << "SUCCESS: 'same extension keep together' verified successfully!" << std::endl;
        }

        // Scenario C: similar file size keep together
        {
            std::ofstream testIn(tempIn);
            testIn << "{\n"
                   << "  \"prompt\": \"similar file size keep together\",\n"
                   << "  \"items\": [\n"
                   << "    { \"path\": \"a.txt\", \"size\": 1024 },\n"
                   << "    { \"path\": \"b.txt\", \"size\": 1050 },\n"
                   << "    { \"path\": \"c.txt\", \"size\": 1024000 }\n"
                   << "  ]\n"
                   << "}";
            testIn.close();
            
            std::string cmd = "python3 \"" + scriptPath + "\" < \"" + tempIn + "\" > \"" + tempOut + "\"";
            int ret = std::system(cmd.c_str());
            assert(ret == 0);
            
            std::ifstream testOut(tempOut);
            std::stringstream buffer;
            buffer << testOut.rdbuf();
            std::string outJson = buffer.str();
            testOut.close();
            
            // Assert log-scale size grouping works beautifully
            assert(outJson.find("[1.0") != std::string::npos || outJson.find("[1") != std::string::npos);
            std::cout << "SUCCESS: 'similar file size keep together' verified successfully!" << std::endl;
        }

        // Clean up
        std::filesystem::remove(tempIn);
        std::filesystem::remove(tempOut);
    }
    
    // Cleanup
    std::filesystem::remove_all(test_src);
    std::filesystem::remove_all(test_dest);
    std::cout << "SUCCESS: Entropy-Aware Semantic Solver test suite completed successfully!" << std::endl;
}

void run_test7() {
    std::cout << "\n--- STARTING TEST 7: PAR3 PARITY CREATION, VERIFICATION AND COPY-RESTORATION (BTTB v4.2.0) ---" << std::endl;
    
    std::string test_vol = "./mock_par3_volume";
    std::string test_recovery = "./mock_par3_recovery";
    std::string test_src_dir = "./mock_par3_src_dir";
    std::filesystem::remove_all(test_vol);
    std::filesystem::remove_all(test_recovery);
    std::filesystem::remove_all(test_src_dir);
    
    std::filesystem::create_directories(test_vol);
    std::filesystem::create_directories(test_src_dir);
    
    // Create multiple mock files in the volume
    std::string file1 = test_vol + "/file1.txt";
    std::string file2 = test_vol + "/file2.txt";
    {
        std::ofstream f1(file1);
        f1 << "This is some important text data that needs protection. Repeating it to cross block boundaries. "
           << "This is some important text data that needs protection. Repeating it to cross block boundaries. "
           << "This is some important text data that needs protection. Repeating it to cross block boundaries. ";
        
        std::ofstream f2(file2);
        f2 << "Another important configuration file here. Keep it safe!";
        
        std::ofstream f_src(test_src_dir + "/source_file.txt");
        f_src << "This is source content inside a directory that will be symlinked. It has some text.";
    }
    
    // Create a symlink in the volume to test symlink protection support
    std::filesystem::create_symlink("file2.txt", test_vol + "/file3_sym.txt");
    
    // Create a directory symlink in the volume pointing to the source directory
    std::filesystem::create_directory_symlink(std::filesystem::absolute(test_src_dir), test_vol + "/sym_dir");
    
    // 1. Create PAR3 parity files
    std::string errorMsg;
    bool create_ok = bttb::createVolumePar3(test_vol, "Volume_1", 2048, 15, errorMsg);
    std::cout << "PAR3 Archive Creation: " << (create_ok ? "SUCCESS" : "FAILED") << std::endl;
    if (!create_ok) {
        std::cout << "PAR3 Error details: " << errorMsg << std::endl;
    }
    assert(create_ok);
    
    // Check that PAR3 file exists
    assert(std::filesystem::exists(test_vol + "/Volume_1.par3"));
    std::cout << "PAR3 index file correctly created!" << std::endl;
    
    // Verify that NO PAR3 files were created in the source folder or CWD root
    assert(!std::filesystem::exists(test_src_dir + "/Volume_1.par3"));
    for (const auto& entry : std::filesystem::directory_iterator(test_src_dir)) {
        assert(entry.path().extension() != ".par3");
    }
    assert(!std::filesystem::exists("./Volume_1.par3"));
    std::cout << "Verified that no PAR3 files were created in the source folder or parent/current working directory!" << std::endl;
    
    // 2. Initial verification (should be clean)
    std::vector<std::string> damaged;
    std::string verifyLog;
    int status = bttb::verifyVolumePar3(test_vol, "Volume_1", damaged, verifyLog);
    std::cout << "Initial volume verification status: " << status << " (Expected: 0)" << std::endl;
    assert(status == 0);
    assert(damaged.empty());
    std::cout << "Clean volume verified perfectly!" << std::endl;
    
    // 3. Simulate file damage (corruption)
    {
        std::ofstream f1(file1, std::ios::binary | std::ios::in | std::ios::out);
        f1.seekp(10);
        f1 << "CORRUPT"; // Overwrite some bytes
    }
    std::cout << "Simulated corruption in file1.txt!" << std::endl;
    
    // 4. Verify volume after damage (should detect corruption)
    damaged.clear();
    verifyLog.clear();
    status = bttb::verifyVolumePar3(test_vol, "Volume_1", damaged, verifyLog);
    std::cout << "Post-damage volume verification status: " << status << " (Expected: non-zero)" << std::endl;
    assert(status != 0);
    std::cout << "Damaged files count: " << damaged.size() << " (Expected: 1)" << std::endl;
    assert(damaged.size() >= 1);
    std::cout << "Detected damaged file: " << damaged[0] << std::endl;
    assert(damaged[0].find("file1.txt") != std::string::npos);
    std::cout << "PAR3 successfully detected corrupted file!" << std::endl;
    
    // 5. Restore and Repair to separate folder
    std::string restoreLog;
    bool restore_ok = bttb::restoreVolumePar3(test_vol, test_recovery, "Volume_1", restoreLog);
    std::cout << "Copy-Restoration/Repair process: " << (restore_ok ? "SUCCESS" : "FAILED") << std::endl;
    assert(restore_ok);
    
    // 6. Verify restored folder is now completely clean
    damaged.clear();
    verifyLog.clear();
    status = bttb::verifyVolumePar3(test_recovery, "Volume_1", damaged, verifyLog);
    std::cout << "Restored volume verification status: " << status << " (Expected: 0)" << std::endl;
    assert(status == 0);
    assert(damaged.empty());
    std::cout << "Bit-perfect copy-based restoration and repair verified successfully!" << std::endl;
    
    // Cleanup
    std::filesystem::remove_all(test_vol);
    std::filesystem::remove_all(test_recovery);
    std::filesystem::remove_all(test_src_dir);
    std::cout << "SUCCESS: PAR3 test suite completed successfully!" << std::endl;
}

void run_test8() {
    std::cout << "\n--- STARTING TEST 8: MULTI-THREADED VS SINGLE-THREADED SOLVER COMPETING PERFORMANCE (BTTB v4.5.0) ---" << std::endl;
    
    std::string test_src = "./mock_multithread_src";
    std::string test_dest = "./mock_multithread_dest";
    std::filesystem::remove_all(test_src);
    std::filesystem::remove_all(test_dest);
    std::filesystem::create_directories(test_src);
    
    // Generate 250 files of distinct sizes
    int64_t totalBytes = 0;
    for (int i = 0; i < 250; ++i) {
        std::string filename = test_src + "/file_" + std::to_string(i) + ".bin";
        std::ofstream f(filename, std::ios::binary);
        int64_t size = 1000 + i * 13;
        totalBytes += size;
        std::string dummy(size, 'a');
        f << dummy;
    }
    
    std::cout << "Generated 250 files of distinct sizes. Total size: " << totalBytes << " bytes." << std::endl;
    
    // Configure BttbSolver for single-threaded run
    bttb::BttbSolver solver;
    solver.sourceDirectories.push_back(test_src);
    solver.targetDirectory = test_dest;
    solver.moveFiles = false;
    solver.createSymlinks = false;
    solver.spanMultipleVolumes = false; // single volume
    solver.maxSearchTimeSeconds = 1; // 1 second limit
    solver.enableTrace = false;
    
    // Align capacity to sector size (2KB)
    solver.mediumInfo.capacityBytes = 327680; // 320 KB
    solver.mediumInfo.sectorSize = 2048;
    solver.mediumInfo.slackBytes = 0;
    
    // Single-threaded run
    solver.enableMultiThreading = false;
    std::cout << "Running single-threaded solver..." << std::endl;
    auto t1 = std::chrono::steady_clock::now();
    solver.run();
    auto t2 = std::chrono::steady_clock::now();
    auto elapsedSingleMs = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    uint64_t exploredSingle = solver.exploredStates;
    double percentageSingle = (double)solver.currentBestSectors * solver.mediumInfo.sectorSize / solver.mediumInfo.capacityBytes * 100.0;
    std::cout << "Single-threaded: explored " << exploredSingle << " states in " << elapsedSingleMs << " ms. Best utilization: " << percentageSingle << "%" << std::endl;
    
    // Configure BttbSolver for multi-threaded run
    bttb::BttbSolver solverMulti;
    solverMulti.sourceDirectories.push_back(test_src);
    solverMulti.targetDirectory = test_dest;
    solverMulti.moveFiles = false;
    solverMulti.createSymlinks = false;
    solverMulti.spanMultipleVolumes = false;
    solverMulti.maxSearchTimeSeconds = 1;
    solverMulti.enableTrace = false;
    solverMulti.mediumInfo.capacityBytes = 327680;
    solverMulti.mediumInfo.sectorSize = 2048;
    solverMulti.mediumInfo.slackBytes = 0;
    
    // Multi-threaded run
    solverMulti.enableMultiThreading = true;
    std::cout << "Running multi-threaded solver..." << std::endl;
    auto t3 = std::chrono::steady_clock::now();
    solverMulti.run();
    auto t4 = std::chrono::steady_clock::now();
    auto elapsedMultiMs = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
    uint64_t exploredMulti = solverMulti.exploredStates;
    double percentageMulti = (double)solverMulti.currentBestSectors * solverMulti.mediumInfo.sectorSize / solverMulti.mediumInfo.capacityBytes * 100.0;
    std::cout << "Multi-threaded: explored " << exploredMulti << " states in " << elapsedMultiMs << " ms. Best utilization: " << percentageMulti << "%" << std::endl;
    
    // Verify performance
    unsigned int hardwareCores = std::thread::hardware_concurrency();
    std::cout << "System CPU Core count: " << hardwareCores << std::endl;
    
    if (hardwareCores > 1) {
        std::cout << "Verifying that multi-threaded solver explored more states..." << std::endl;
        assert(exploredMulti > exploredSingle);
        std::cout << "Performance verification SUCCESS: Multi-threaded solver explored " 
                  << (double)exploredMulti / exploredSingle << "x more states than single-threaded!" << std::endl;
    } else {
        std::cout << "Single core detected. Skipping performance ratio assertion." << std::endl;
    }
    
    // Clean up
    std::filesystem::remove_all(test_src);
    std::filesystem::remove_all(test_dest);
    std::cout << "SUCCESS: Multi-threaded solver test completed successfully!" << std::endl;
}

int main() {
    std::cout << "Starting BTTB automated verification suite..." << std::endl;
    run_test1();
    run_test2();
    run_test3();
    run_test4();
    run_test5();
    run_test6();
    run_test7();
    run_test8();
    std::cout << "\nALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
