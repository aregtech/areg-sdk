/************************************************************************
 * \file        areg/src/persist/private/CEPersistenceManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Data persistency manager
 ************************************************************************/
#include "areg/src/persist/private/CEPersistenceManager.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEFile.hpp"

CEPersistenceManager & CEPersistenceManager::GetPersistenceManager(void)
{
    static CEPersistenceManager _persistence;
    return _persistence;
}

CEPersistenceManager::CEPersistenceManager(void)
    : mMapReadOnly  ( )
    , mMapWritable  ( )
{
    ; // nothing
}

CEPersistenceManager::~CEPersistenceManager(void)
{
    mMapReadOnly.RemoveAll();
    mMapWritable.RemoveAll();
}

bool CEPersistenceManager::ParseReadable(const char * filePath)
{
    mMapReadOnly.RemoveAll();
    mFileReadOnly = NEString::isEmpty<char>(filePath) == false ? CEFile::GetFileFullPath(filePath) : "";
    if ( mFileReadOnly.IsEmpty() == false )
    {
        CEFile fileConfig( static_cast<const char *>(mFileReadOnly), CEFileBase::FO_MODE_EXIST | CEFileBase::FO_MODE_READ | CEFileBase::FO_MODE_TEXT | CEFileBase::FO_MODE_SHARE_READ );
        if ( fileConfig.Open( ) )
        {
            CEString module = CEProcess::GetProcess().GetAppName();
            CEString line;
            CEProperty newProperty;
            while ( fileConfig.ReadLine(line) > 0 )
            {
                if ( newProperty.ParseProperty(line) )
                {
                    if ( (newProperty.GetKey().HasModule() == false) || (newProperty.GetKey().GetModule() == module) )
                    {
                        mMapReadOnly.SetKey( static_cast<unsigned int>(newProperty), newProperty );
                        newProperty.ResetData();
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
    return mMapReadOnly.IsEmpty() == false;
}

bool CEPersistenceManager::ParseWritable(const char * filePath)
{
    mMapWritable.RemoveAll();
    mFileWritable = NEString::isEmpty<char>(filePath) == false ? CEFile::GetFileFullPath(filePath) : "";
    if ( mFileReadOnly.IsEmpty() == false )
    {
        CEFile fileConfig( static_cast<const char *>(mFileReadOnly), CEFileBase::FO_MODE_EXIST | CEFileBase::FO_MODE_READ | CEFileBase::FO_MODE_TEXT | CEFileBase::FO_MODE_SHARE_READ );
        if ( fileConfig.Open( ) )
        {
            CEString module = CEProcess::GetProcess().GetAppName();
            CEString line;
            CEProperty newProperty;
            while ( fileConfig.ReadLine(line) > 0 )
            {
                if ( newProperty.ParseProperty(line) )
                {
                    if ( (newProperty.GetKey().HasModule() == false) || (newProperty.GetKey().GetModule() == module) )
                    {
                        mMapWritable.SetKey( static_cast<unsigned int>(newProperty), newProperty );
                        newProperty.ResetData();
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
    return mMapWritable.IsEmpty() == false;
}

int CEPersistenceManager::GetProperty(  const char * /*section*/
                                      , const char * /*property*/
                                      , const char * /*position*/
                                      , TEArrayList<CEProperty, const CEProperty &> & /*out_listProperties*/)
{
    return 0;
}
