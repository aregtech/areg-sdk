#ifndef AREG_TRACE_PRIVATE_CETRACEPROPERTYKEY_HPP
#define AREG_TRACE_PRIVATE_CETRACEPROPERTYKEY_HPP
/************************************************************************
 * \file        areg/src/trace/private/CETracePropertyKey.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Trace Property key class. Used when reading or 
 *              saving configuration file. It contains Key value and type
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/trace/private/NELogConfig.hpp"
#include "areg/src/base/CEString.hpp"

//////////////////////////////////////////////////////////////////////////
// CETracePropertyKey class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging property key object, which is containing
 *          key and module names of the property.
 **/
class CETracePropertyKey
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates property key with empty and invalid data.
     **/
    CETracePropertyKey( void );
    /**
     * \brief   Parses given string and initializes property key data.
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    CETracePropertyKey( const char * fullKey );
    /**
     * \brief   Copies property key data from given source
     * \param   source  The source of property key to copy data.
     **/
    CETracePropertyKey( const CETracePropertyKey & source );
    /**
     * \brief   Parses given string and initializes property key data.
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    CETracePropertyKey( const CEString & fullKey );
    /**
     * \brief   Destructor
     **/
    ~CETracePropertyKey( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies property key data from given source
     * \param   source  The source of property key to copy data.
     **/
    const CETracePropertyKey & operator = ( const CETracePropertyKey & source );

    /**
     * \brief   Parses given string and initializes property key data
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    inline void operator = ( const CEString & fullKey );

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
    inline bool operator == ( const CETracePropertyKey & other ) const;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the string value of property key.
     **/
    inline const CEString & GetKey( void ) const;

    /**
     * \brief   Returns the module name of the property key.
     **/
    inline const CEString & GetModule( void ) const;

    /**
     * \brief   Returns the module data of the property key.
     **/
    inline const CEString & GetModuleData( void ) const;

    /**
     * \brief   Returns the configuration type of the property key.
     * \see     NELogConfig::eLogConfig
     **/
    inline NELogConfig::eLogConfig GetLogConfig( void ) const;

    /**
     * \brief   Returns visibility level value of the property key configuration.
     *          There are 2 visibility levels of the property: local and global.
     *              - The global visibility refers to all applications.
     *              - The local visibility refers only the current module.
     * \see     NELogConfig::eConfigKey
     **/
    inline NELogConfig::eConfigKey GetConfigKey( void ) const;

    /**
     * \brief   Returns true if the visibility level of the property is local, i.e. refers only to current module
     **/
    inline bool IsLocalKey( void ) const;

    /**
     * \brief   Returns true if the visibility level of the property is global, i.e. refers to all applications
     **/
    inline bool IsGlobalKey( void ) const;

    /**
     * \brief   Returns true if the key is valid.
     **/
    inline bool IsValidKey( void ) const;

    /**
     * \brief   Parses given string and initializes property key data
     * \param   fullKey     The full string of property key to parse and initialize.
     **/
    void SetKey( const char * fullKey );

    /**
     * \brief   Returns true if the module key is empty or equal to given module name.
     * \param   moduleName  The name of module to check. The parameter is ignored
     *                      if the key data visibility level is global (i.e. no module name is set).
     *                      The parameter cane be NULL or empty if only global visibility should checked.
     * \return  Returns true if property key is set and either module name is empty (i.e. global visibility)
     *          or equal to the given module name.
     **/
    bool IsModuleKeySet( const char * moduleName = NULL ) const;

    /**
     * \brief   Returns the syntax name of the property key.
     **/
    const char * GetKeySytax( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The key name of the property. Should not be empty.
     **/
    CEString                mKeyName;
    /**
     * \brief   The module name of the property. Can be empty if the property key is global.
     **/
    CEString                mModuleName;
    /**
     * \brief   The data of module, if set.
     **/
    CEString                mModuleData;
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
// CETracePropertyKey class inline methods implementation
//////////////////////////////////////////////////////////////////////////

inline void CETracePropertyKey::operator = ( const CEString & fullKey )
{   SetKey( fullKey );                                              }

inline void CETracePropertyKey::operator = ( const char * fullKey )
{   SetKey( fullKey );                                              }

inline bool CETracePropertyKey::operator == ( const CETracePropertyKey & other ) const
{   return ( this == &other ? true : mKeyName == other.mKeyName );  }

inline const CEString & CETracePropertyKey::GetKey( void ) const
{   return mKeyName;                                                }

inline const CEString & CETracePropertyKey::GetModule( void ) const
{   return mModuleName;                                             }

inline const CEString & CETracePropertyKey::GetModuleData(void) const
{   return mModuleData;                                             }

inline NELogConfig::eLogConfig CETracePropertyKey::GetLogConfig( void ) const
{   return mLogConfig;                                              }

inline NELogConfig::eConfigKey CETracePropertyKey::GetConfigKey( void ) const
{   return mConfigKey;                                              }

inline bool CETracePropertyKey::IsLocalKey( void ) const
{   return ( mConfigKey == NELogConfig::KeyLocal );                 }

inline bool CETracePropertyKey::IsGlobalKey( void ) const
{   return ( mConfigKey == NELogConfig::KeyGlobal );                }

inline bool CETracePropertyKey::IsValidKey( void ) const
{   return ( mConfigKey != NELogConfig::KeyUndefined );             }

#endif  // AREG_TRACE_PRIVATE_CETRACEPROPERTYKEY_HPP
