#include <Residue.h>

int main(int argc, char** argv) {
    Residue::setApplicationArgs(argc, argv);

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conf")) {
        Residue::loadConfiguration(el::Helpers::commandLineArgs()->getParamValue("--conf"));
    }

    Residue::connect();

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
