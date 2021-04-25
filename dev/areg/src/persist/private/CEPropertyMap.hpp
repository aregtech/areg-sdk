#ifndef AREG_PERSIST_PRIVATE_CEPROPERTYMAP_HPP
#define AREG_PERSIST_PRIVATE_CEPROPERTYMAP_HPP
/************************************************************************
 * \file        areg/src/persist/private/CEPropertyMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property map object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/TEHashMap.hpp"
#include "areg/src/persist/CEProperty.hpp"
#include "areg/src/persist/CEPropertyKey.hpp"
#include "areg/src/persist/CEPropertyValue.hpp"

//////////////////////////////////////////////////////////////////////////
// CEPropertyMap class declaration
//////////////////////////////////////////////////////////////////////////
class CEPropertyMapImpl : TEHashMapImpl<const CEPropertyKey &, const CEPropertyValue &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int ImplHashKey( const CEPropertyKey & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   The Property map object, which contains values with keys.
 **/
class CEPropertyMap : protected TEHashMap<CEPropertyKey, CEPropertyValue, const CEPropertyKey &, const CEPropertyValue &, CEPropertyMapImpl>
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The definition of property block saved in map.
     **/
    typedef TEHashMap<CEPropertyKey, CEPropertyValue, const CEPropertyKey &, const CEPropertyValue &, CEPropertyMapImpl>::CEBlock  PropertyBlock;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CEPropertyMap( void );
    /**
     * \brief   Destructor
     **/
    virtual ~CEPropertyMap( void );
};

#endif  // AREG_PERSIST_PRIVATE_CEPROPERTYMAP_HPP
