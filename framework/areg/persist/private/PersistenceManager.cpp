/************************************************************************
 * \file        areg/persist/private/PersistenceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

PersistenceManager::PersistenceManager(void)
    : mMapReadOnly  ( )
    , mMapWritable  ( )
{
    ; // nothing
}

PersistenceManager::~PersistenceManager(void)
{
    mMapReadOnly.removeAll();
    mMapWritable.removeAll();
}

bool PersistenceManager::parseReadable(const char * filePath)
{
    mMapReadOnly.removeAll();
    mFileReadOnly = NEString::isEmpty<char>(filePath) == false ? File::getFileFullPath(filePath) : "";
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
    mMapWritable.removeAll();
    mFileWritable = NEString::isEmpty<char>(filePath) == false ? File::getFileFullPath(filePath) : "";
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

int PersistenceManager::getProperty( const char * /*section*/
                                   , const char * /*property*/
                                   , const char * /*position*/
                                   , TEArrayList<Property, const Property &> & /*out_listProperties*/)
{
    return 0;
}
