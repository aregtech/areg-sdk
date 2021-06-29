#ifndef AREG_PERSIST_PRIVATE_PERSISTENMANAGER_HPP
#define AREG_PERSIST_PRIVATE_PERSISTENMANAGER_HPP
/************************************************************************
 * \file        areg/persist/private/PersistenceManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Persistence manager to save and read data
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/persist/Property.hpp"
#include "areg/base/EContainers.hpp"


//////////////////////////////////////////////////////////////////////////
// PersistenceManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The Persistence manager to read and write data into the storage.
 *          Each saved data is property, which consists of key and value pairs.
 **/
class PersistenceManager
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The map of saved properties.
     **/
    typedef TEIntegerHashMapImpl<const Property &>                      DataMapImpl;
    typedef TEIntegerHashMap<Property, const Property &, DataMapImpl>   DataMap;

//////////////////////////////////////////////////////////////////////////
// Hidden constructor / destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Hidden constructor
     **/
    PersistenceManager( void );
    /**
     * \brief   Destructor
     **/
    ~PersistenceManager( void );

//////////////////////////////////////////////////////////////////////////
// Static public
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns instance of persistence manager, which is a singleton object.
     **/
    static PersistenceManager & getInstance( void );

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
    bool parseReadable( const char * filePath );

    /**
     * \brief   Reads persistence data saved in given file, initializes properties.
     *          The file is opened for reading and writing, data can be updated.
     * \param   filePath    Relative or absolute path to persistent file with data to read and update.
     * \return  Returns true if successfully opened file and parsed data.
     **/
    bool parseWritable( const char * filePath );

    /**
     * \brief   Searches appropriate section of data in persistence map, finds position and reads all property values in the given array list.
     * \param   section     The name of section to search in persistence map.
     * \param   position    The name of position at section to get the values
     * \return  Returns number of properties in the list.
     **/
    int getProperty( const char * section, const char * property, const char * position, TEArrayList<Property, const Property &> & out_listProperties );

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
    String      mFileReadOnly;
    /**
     * \brief   The name of file opened for read-write
     **/
    String      mFileWritable;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PersistenceManager( const PersistenceManager & /*src*/ );
    const PersistenceManager & operator = ( const PersistenceManager & /*src*/ );
};

#endif  // AREG_PERSIST_PRIVATE_PERSISTENMANAGER_HPP
