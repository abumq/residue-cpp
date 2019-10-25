//
//  dispatcher.cc
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present Amrayn Web Services
//  Copyright (C) 2017-present @abumusamq
//
//  https://muflihun.com/
//  https://amrayn.com
//  https://github.com/amrayn/residue-cpp/
//
//  See https://github.com/amrayn/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#include "dispatcher.h"
#include "internal-logger.h"

void ResidueDispatcher::handle(const el::LogDispatchData* data) noexcept
{
    m_data = data;

    if (data->logMessage()->logger()->id() == RESIDUE_LOGGER_ID) {
        el::base::DefaultLogDispatchCallback::handle(m_data);
    } else {
        std::time_t t = std::time(nullptr);
        std::tm* nowTm;
        if (m_residue->m_utc) {
            nowTm = std::gmtime(&t);
        } else {
            nowTm = std::localtime(&t);
        }
        time_t now;
        if (nowTm != nullptr) {
            now = mktime(nowTm);
            now += m_residue->m_timeOffset;
            now *= 1000;
        } else {
            InternalLogger(InternalLogger::error) << "Unable to create time. Using second method to send local time instead.";
            now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
        }
        m_residue->addToQueue(std::move(Residue::RawRequest
                                              {
                                                    static_cast<unsigned long>(now),
                                                    el::Helpers::getThreadName(),
                                                    m_data->logMessage()->logger()->id(),
                                                    m_data->logMessage()->message(),
                                                    m_data->logMessage()->file(),
                                                    m_data->logMessage()->line(),
                                                    m_data->logMessage()->func(),
                                                    static_cast<unsigned int>(m_data->logMessage()->level()),
                                                    m_data->logMessage()->verboseLevel()
                                              }
                                        )
                              );
    }
}
