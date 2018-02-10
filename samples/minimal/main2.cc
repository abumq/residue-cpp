#include <Residue.h>

int main(int argc, char** argv) {
    Residue::setApplicationArgs(argc, argv);

    if (el::Helpers::commandLineArgs()->hasParamWithValue("--conf")) {
        Residue::loadConfiguration(el::Helpers::commandLineArgs()->getParamValue("--conf"));
    }

    Residue::reconnect();

    system("date");
    int count = 100;
    for (int i = 1; i <= count; ++i) {
        LOG(INFO) << "Message #" << i << " from residue sample";
    }
    std::cout << "Already finished the logger - just flushing it to the server." << std::endl;

    system("date");

    Residue::disconnect();

    return 0;
}
