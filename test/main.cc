//
//  Residue.h
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present Muflihun Labs
//
//  Author: @abumusamq
//
//  https://muflihun.com
//  https://muflihun.github.io/residue
//  https://github.com/muflihun/residue-cpp
//
//  See https://github.com/muflihun/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include "test.h"
#include "../include/Residue.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    std::cout << "Default log file: " << ELPP_DEFAULT_LOG_FILE << std::endl;
    std::cout << "Residue SO version " << Residue::version() << std::endl;

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);

    return ::testing::UnitTest::GetInstance()->Run();
}
