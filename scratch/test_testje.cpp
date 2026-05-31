#include <iostream>
#include <string>
#include "bttb_logic.hpp"

int main() {
    std::string err;
    std::cout << "Starting PAR3 creation for testje..." << std::endl;
    bool success = bttb::createVolumePar3("/home/sander/src/antigravity/project1/testje", "testje_par", 2048, 15, err);
    if (success) {
        std::cout << "SUCCESS: PAR3 created successfully!" << std::endl;
    } else {
        std::cout << "FAILURE: " << err << std::endl;
    }
    return success ? 0 : 1;
}
