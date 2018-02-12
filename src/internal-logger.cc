//
//  internal-logger.cc
//  Residue C++
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
