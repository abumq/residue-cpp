//
//  dispatcher.h
//  Residue C++
//
//  Official C++ client library for Residue logging server
//
//  Copyright (C) 2017-present @abumq (Majid Q.)
//
//  See https://github.com/abumq/residue-cpp/blob/master/LICENSE
//  for licensing information
//

#ifndef RESIDUE_DISPATCHER_H
#define RESIDUE_DISPATCHER_H

#include "log.h"
#include "include/residue.h"

class ResidueDispatcher : public el::base::DefaultLogDispatchCallback // we override default log dispatcher rather than create new one
{
public:
    ResidueDispatcher() : m_residue(nullptr), m_data(nullptr) {}

    inline void setResidue(Residue* residue) { m_residue = residue; }
protected:
    void handle(const el::LogDispatchData* data) noexcept override;
private:
    Residue* m_residue;
    const el::LogDispatchData* m_data;
    std::string m_host;
};

#endif /* RESIDUE_DISPATCHER_H */
