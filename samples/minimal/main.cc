#include <Residue.h>

int main(int argc, char** argv) {
    Residue::setApplicationArgs(argc, argv);

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conf")) {
        try {
            Residue::loadConfiguration(el::Helpers::commandLineArgs()->getParamValue("--conf"));
        } catch (ResidueException& e) {
            std::cout << "Exception (config): " << e.what() << std::endl;
            return 1;
        }
    }

    try {
        Residue::reconnect();
    } catch (ResidueException& e) {
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Connected to client: " << Residue::instance().clientId() << std::endl;
    std::cout << "Server version: " << Residue::instance().serverVersion() << std::endl;

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
