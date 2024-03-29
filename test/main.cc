//
//  Residue.h
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present @abumq (Majid Q.)
//
//  See https://github.com/abumq/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include "test.h"
#include "../include/residue.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Default log file: " << ELPP_DEFAULT_LOG_FILE << std::endl;
    std::cout << "Residue SO version " << Residue::version() << std::endl;

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    return ::testing::UnitTest::GetInstance()->Run();
}
