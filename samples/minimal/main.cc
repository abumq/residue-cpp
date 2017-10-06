#include <Residue.h>

int main(int argc, char** argv) {
    Residue::setApplicationArgs(argc, argv);

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conf")) {
        Residue::loadConfiguration(el::Helpers::commandLineArgs()->getParamValue("--conf"));
    }

    try {
        Residue::reconnect();
    } catch (ResidueException& e) {
        std::cout << "exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Connected to client: " << Residue::instance().clientId() << std::endl;
    std::cout << "Server version: " << Residue::instance().serverVersion() << std::endl;
    std::cout << "Server licensee: " << Residue::instance().licensee() << std::endl;

    while (true) {
        std::wstring input;
        std::cout << "Logger > ";
        std::getline(std::wcin, input);
        if (!input.empty()) {
            LOG(INFO) << input;
        }
    }

    Residue::disconnect();

    return 0;
}
