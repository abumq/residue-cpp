#include <residue/residue.h>

int main(int argc, char** argv) {
    Residue::setInternalLoggingLevel(Residue::InternalLoggingLevel::crazy);
    Residue::setApplicationArgs(argc, argv);

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conf")) {
        try {
            Residue::loadConfiguration(el::Helpers::commandLineArgs()->getParamValue("--conf"));
        } catch (ResidueException& e) {
            std::cout << "Exception (config): " << e.what() << std::endl;
            return 1;
        }
    }

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conn")) {
        try {
            Residue::loadConnection(el::Helpers::commandLineArgs()->getParamValue("--conn"));
        } catch (ResidueException& e) {
            std::cout << "Exception (connection): " << e.what() << std::endl;
            return 1;
        }
    } else {

        try {
            Residue::reconnect();
        } catch (ResidueException& e) {
            std::cout << "Exception: " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cout << "Connected to client: " << Residue::instance().clientId() << std::endl;
    std::cout << "Server version: " << Residue::instance().serverVersion() << std::endl;
    Residue::saveConnection("connection.json");

    while (true) {
        std::wstring input;
        std::cout << "Logger > ";
        std::getline(std::wcin, input);
        if (!input.empty()) {
            CLOG(INFO, "sample-app") << input;
        }
    }

    Residue::disconnect();

    return 0;
}
