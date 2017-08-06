//
//  main.cc
//  Residue C++ Client Tests
//
//  Copyright © 2017 Muflihun Labs
//

#include "test.h"
#include <Residue.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Default log file: " << ELPP_DEFAULT_LOG_FILE << std::endl;
    std::cout << "Residue SO version " << Residue::version() << std::endl;

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    return ::testing::UnitTest::GetInstance()->Run();
}
