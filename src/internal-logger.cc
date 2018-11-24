//
//  internal-logger.cc
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present Zuhd Web Services
//  Copyright (C) 2017-present @abumusamq
//
//  https://muflihun.com/
//  https://zuhd.org
//  https://github.com/zuhd-org/residue-cpp/
//
//  See https://github.com/zuhd-org/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include "internal-logger.h"

InternalLogger::~InternalLogger()
{
    if (m_enabled) {
        std::string levelStr;
        switch (m_level) {
        case crazy:
            levelStr = "CRAZY";
            break;
        case debug:
            levelStr = "DEBUG";
            break;
        case error:
            levelStr = "ERROR";
            break;
        case warning:
            levelStr = "WARNING";
            break;
        case info:
            levelStr = "INFO";
            break;
        default:
        case none:
            break;
        }

        std::cout << "[" << levelStr << "]: " << m_stream.str() << std::endl;
    }
}
