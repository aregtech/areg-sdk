#ifndef AREG_PERSIST_PRIVATE_CEPERSISTENCEMANAGER_HPP
#define AREG_PERSIST_PRIVATE_CEPERSISTENCEMANAGER_HPP
/************************************************************************
 * \file        areg/src/persist/private/CEPersistenceManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Persistence manager to save and read data
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/persist/CEProperty.hpp"
#include "areg/src/base/EContainers.hpp"


//////////////////////////////////////////////////////////////////////////
// CEPersistenceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Persistence manager to read and write data into the storage.
 *          Each saved data is property, which consists of key and value pairs.
 **/
class CEPersistenceManager
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The map of saved properties.
     **/
    typedef TEIntegerHashMapImpl<const CEProperty &>                        DataMapImpl;
    typedef TEIntegerHashMap<CEProperty, const CEProperty &, DataMapImpl>   DataMap;

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Hidden constructor
     **/
    CEPersistenceManager( void );
    /**
     * \brief   Destructor
     **/
    ~CEPersistenceManager( void );

//////////////////////////////////////////////////////////////////////////
// Static public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns instance of persistence manager, which is a singleton object.
     **/
    static CEPersistenceManager & GetPersistenceManager( void );

//////////////////////////////////////////////////////////////////////////
// Properties and operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Reads persistence data saved in given file, initializes properties.
     *          The file is opened only for reading and no data can be updated.
     * \param   filePath    Relative or absolute path to persistent file with data to read.
     * \return  Returns true if successfully opened file and parsed data.
     **/
    bool ParseReadable( const char * filePath );

    /**
     * \brief   Reads persistence data saved in given file, initializes properties.
     *          The file is opened for reading and writing, data can be updated.
     * \param   filePath    Relative or absolute path to persistent file with data to read and update.
     * \return  Returns true if successfully opened file and parsed data.
     **/
    bool ParseWritable( const char * filePath );

    /**
     * \brief   Searches appropriate section of data in persistence map, finds position and reads all property values in the given array list.
     * \param   section     The name of section to search in persistence map.
     * \param   position    The name of position at section to get the values
     * \return  Returns number of properties in the list.
     **/
    int GetProperty( const char * section, const char * property, const char * position, TEArrayList<CEProperty, const CEProperty &> & out_listProperties );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Map of read only persisted properties
     **/
    DataMap     mMapReadOnly;
    /**
     * \brief   Map of read-write persisted properties
     **/
    DataMap     mMapWritable;
    /**
     * \brief   The name of file opened for read-only
     **/
    CEString    mFileReadOnly;
    /**
     * \brief   The name of file opened for read-write
     **/
    CEString    mFileWritable;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CEPersistenceManager( const CEPersistenceManager & /*src*/ );
    const CEPersistenceManager & operator = ( const CEPersistenceManager & /*src*/ );
};

#endif  // AREG_PERSIST_PRIVATE_CEPERSISTENCEMANAGER_HPP
