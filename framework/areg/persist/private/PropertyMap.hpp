#ifndef AREG_PERSIST_PRIVATE_PROPERTYMAP_HPP
#define AREG_PERSIST_PRIVATE_PROPERTYMAP_HPP
/************************************************************************
 * \file        areg/persist/private/PropertyMap.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property map object
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/TEHashMap.hpp"
#include "areg/persist/Property.hpp"
#include "areg/persist/PropertyKey.hpp"
#include "areg/persist/PropertyValue.hpp"

//////////////////////////////////////////////////////////////////////////
// PropertyMap class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Property map helper class implementation
 **/
class PropertyMapImpl : TEHashMapImpl<const PropertyKey &, const PropertyValue &>
{
public:
    /**
     * \brief   Called to calculate the 32-bit hash key value.
     * \ param  Key     The object to calculate 32-bit hash key.
     * \return  Returns 32-bit hash key value.
     **/
    inline unsigned int implHashKey( const PropertyKey & Key ) const
    {
        return static_cast<unsigned int>(Key);
    }
};

/**
 * \brief   The Property map object, which contains values with keys.
 **/
class PropertyMap : protected TEHashMap<PropertyKey, PropertyValue, const PropertyKey &, const PropertyValue &, PropertyMapImpl>
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The definition of property block saved in map.
     **/
    typedef TEHashMap<PropertyKey, PropertyValue, const PropertyKey &, const PropertyValue &, PropertyMapImpl>::Block  PropertyBlock;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    PropertyMap( void );
    /**
     * \brief   Destructor
     **/
    virtual ~PropertyMap( void );
};

#endif  // AREG_PERSIST_PRIVATE_PROPERTYMAP_HPP
