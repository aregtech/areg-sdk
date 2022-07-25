/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/persist/private/PersistenceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Data persistency manager
 ************************************************************************/
#include "areg/persist/private/PersistenceManager.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/File.hpp"

PersistenceManager & PersistenceManager::getInstance(void)
{
    static PersistenceManager _persistence;
    return _persistence;
}

bool PersistenceManager::parseReadable(const char * filePath)
{
    mMapReadOnly.clear();
    mFileReadOnly = NEString::isEmpty<char>(filePath) == false ? File::getFileFullPath(filePath) : String(String::EmptyString);
    if ( mFileReadOnly.isEmpty() == false )
    {
        File fileConfig( static_cast<const char *>(mFileReadOnly), FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
        if ( fileConfig.open( ) )
        {
            String module = Process::getInstance().getAppName();
            String line;
            Property newProperty;
            while ( fileConfig.readLine(line) > 0 )
            {
                if ( newProperty.parseProperty(line) )
                {
                    if ( (newProperty.getKey().hasModule() == false) || (newProperty.getKey().getModule() == module) )
                    {
                        mMapReadOnly.setAt( static_cast<unsigned int>(newProperty), newProperty );
                        newProperty.resetData();
                    }
                    else
                    {
                        ; // not relevant property, ignore
                    }
                }
                else
                {
                    ; // get next
                }
            }
        }
    }
    return mMapReadOnly.isEmpty() == false;
}

bool PersistenceManager::parseWritable(const char * filePath)
{
    mMapWritable.clear();
    mFileWritable = NEString::isEmpty<char>(filePath) == false ? File::getFileFullPath(filePath) : String(String::EmptyString);
    if ( mFileReadOnly.isEmpty() == false )
    {
        File fileConfig( static_cast<const char *>(mFileReadOnly), FileBase::FO_MODE_EXIST | FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_SHARE_READ );
        if ( fileConfig.open( ) )
        {
            String module = Process::getInstance().getAppName();
            String line;
            Property newProperty;
            while ( fileConfig.readLine(line) > 0 )
            {
                if ( newProperty.parseProperty(line) )
                {
                    if ( (newProperty.getKey().hasModule() == false) || (newProperty.getKey().getModule() == module) )
                    {
                        mMapWritable.setAt( static_cast<unsigned int>(newProperty), newProperty );
                        newProperty.resetData();
                    }
                    else
                    {
                        ; // not relevant property, ignore
                    }
                }
                else
                {
                    ; // get next
                }
            }
        }
    }
    return mMapWritable.isEmpty() == false;
}

int PersistenceManager::getProperty( const String & /*section*/
                                   , const String & /*property*/
                                   , const String & /*position*/
                                   , TEArrayList<Property> & /*out_listProperties*/)
{
    return 0;
}
