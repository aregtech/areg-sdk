/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/private/Process.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       The class to handle process. Get process ID, process handle, process name, etc.
 *              OS independent implementation
 ************************************************************************/
#include "areg/base/Process.hpp"

#include "areg/base/File.hpp"
#include <filesystem>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
// Process class implementation
//////////////////////////////////////////////////////////////////////////

Process & Process::getInstance( void )
{
    static Process _theProcess;
    return _theProcess;
}


Process::Process( void )
    : mProcEnv          ( static_cast<Process::eProcEnv>(sizeof(id_type)) )
    , mProcessId        ( Process::UNKNOWN_PROCESS )
    , mProcessHandle    ( nullptr )
    , mAppName          ( )
    , mProcessName      ( )
    , mProcessExt       ( )
    , mProcessPath      ( )
    , mProcessFullPath  ( )
    , mIsInitialized    ( false )
{
    _osInitilize();
}

void Process::_initPaths( const char * fullPath )
{
    ASSERT(fullPath != nullptr);
    OUTPUT_DBG("Initializing data of process [ %s ]", fullPath);

    mProcessFullPath = fullPath;
    std::filesystem::path procPath(mProcessFullPath.getObject());

    if (procPath.empty() != false)
    {
        mProcessPath = procPath.parent_path().empty() ? String::EmptyString : procPath.parent_path().string();
        mProcessName = procPath.filename().empty()    ? String::EmptyString : procPath.filename().string();
        mAppName     = procPath.stem().empty()        ? String::EmptyString : procPath.stem().string();
        mProcessExt  = procPath.extension().empty()   ? String::EmptyString : procPath.extension().string();
    }
}

String Process::getSafeEnvVariable( const char * var ) const
{
    return _osGetEnvVariable( var );
}

