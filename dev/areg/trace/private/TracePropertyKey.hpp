#ifndef AREG_TRACE_PRIVATE_TRAPROPERTYKEY_HPP
#define AREG_TRACE_PRIVATE_TRAPROPERTYKEY_HPP
/************************************************************************
 * \file        areg/trace/private/TracePropertyKey.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace Property key class. Used when reading or 
 *              saving configuration file. It contains Key value and type
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/trace/private/NELogConfig.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// TracePropertyKey class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging property key object, which is containing
 *          key and module names of the property.
 **/
class TracePropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates property key with empty and invalid data.
     **/
    TracePropertyKey( void );
    /**
     * \brief   Parses given string and initializes property key data.
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    TracePropertyKey( const char * fullKey );
    /**
     * \brief   Copies property key data from given source
     * \param   source  The source of property key to copy data.
     **/
    TracePropertyKey( const TracePropertyKey & source );
    /**
     * \brief   Parses given string and initializes property key data.
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    TracePropertyKey( const String & fullKey );
    /**
     * \brief   Destructor
     **/
    ~TracePropertyKey( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property key data from given source
     * \param   source  The source of property key to copy data.
     **/
    const TracePropertyKey & operator = ( const TracePropertyKey & source );

    /**
     * \brief   Parses given string and initializes property key data
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    inline void operator = ( const String & fullKey );

    /**
     * \brief   Parses given string and initializes property key data
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    inline void operator = ( const char * fullKey );

    /**
     * \brief   Checks equality of two property key objects and returns true
     *          if two objects are equal.
     * \param   other   The property key object to compare.
     **/
    inline bool operator == ( const TracePropertyKey & other ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the string value of property key.
     **/
    inline const String & getKey( void ) const;

    /**
     * \brief   Returns the module name of the property key.
     **/
    inline const String & getModule( void ) const;

    /**
     * \brief   Returns the module data of the property key.
     **/
    inline const String & getModuleData( void ) const;

    /**
     * \brief   Returns the configuration type of the property key.
     * \see     NELogConfig::eLogConfig
     **/
    inline NELogConfig::eLogConfig getLogConfig( void ) const;

    /**
     * \brief   Returns visibility level value of the property key configuration.
     *          There are 2 visibility levels of the property: local and global.
     *              - The global visibility refers to all applications.
     *              - The local visibility refers only the current module.
     * \see     NELogConfig::eConfigKey
     **/
    inline NELogConfig::eConfigKey getConfigKey( void ) const;

    /**
     * \brief   Returns true if the visibility level of the property is local, i.e. refers only to current module
     **/
    inline bool isLocalKey( void ) const;

    /**
     * \brief   Returns true if the visibility level of the property is global, i.e. refers to all applications
     **/
    inline bool isGlobalKey( void ) const;

    /**
     * \brief   Returns true if the key is valid.
     **/
    inline bool isValidKey( void ) const;

    /**
     * \brief   Parses given string and initializes property key data
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    void setKey( const char * fullKey );

    /**
     * \brief   Returns true if the module key is empty or equal to given module name.
     * \param   moduleName  The name of module to check. The parameter is ignored
     *                      if the key data visibility level is global (i.e. no module name is set).
     *                      The parameter cane be NULL or empty if only global visibility should checked.
     * \return  Returns true if property key is set and either module name is empty (i.e. global visibility)
     *          or equal to the given module name.
     **/
    bool isModuleKeySet( const char * moduleName = NULL ) const;

    /**
     * \brief   Returns the syntax name of the property key.
     **/
    const char * getKeySytax( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The key name of the property. Should not be empty.
     **/
    String                  mKeyName;
    /**
     * \brief   The module name of the property. Can be empty if the property key is global.
     **/
    String                  mModuleName;
    /**
     * \brief   The data of module, if set.
     **/
    String                  mModuleData;
    /**
     * \brief   The logging configuration type of the property key
     **/
    NELogConfig::eLogConfig mLogConfig;
    /**
     * \brief   The visibility level of the property key, i.e. local or global.
     **/
    NELogConfig::eConfigKey mConfigKey;
};

//////////////////////////////////////////////////////////////////////////
// TracePropertyKey class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline void TracePropertyKey::operator = ( const String & fullKey )
{
    setKey( fullKey );
}

inline void TracePropertyKey::operator = ( const char * fullKey )
{
    setKey( fullKey );
}

inline bool TracePropertyKey::operator == ( const TracePropertyKey & other ) const
{
    return ( (this == &other) || (mKeyName == other.mKeyName) );
}

inline const String & TracePropertyKey::getKey( void ) const
{
    return mKeyName;
}

inline const String & TracePropertyKey::getModule( void ) const
{
    return mModuleName;
}

inline const String & TracePropertyKey::getModuleData(void) const
{
    return mModuleData;
}

inline NELogConfig::eLogConfig TracePropertyKey::getLogConfig( void ) const
{
    return mLogConfig;
}

inline NELogConfig::eConfigKey TracePropertyKey::getConfigKey( void ) const
{
    return mConfigKey;
}

inline bool TracePropertyKey::isLocalKey( void ) const
{
    return ( mConfigKey == NELogConfig::KeyLocal );
}

inline bool TracePropertyKey::isGlobalKey( void ) const
{
    return ( mConfigKey == NELogConfig::KeyGlobal );
}

inline bool TracePropertyKey::isValidKey( void ) const
{
    return ( mConfigKey != NELogConfig::KeyUndefined );
}

#endif  // AREG_TRACE_PRIVATE_TRAPROPERTYKEY_HPP
