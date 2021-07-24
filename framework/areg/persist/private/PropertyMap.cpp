/************************************************************************
 * \file        areg/persist/private/PropertyMap.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property map object
 ************************************************************************/
#include "areg/persist/private/PropertyMap.hpp"

PropertyMap::PropertyMap(void)
    : TEHashMap<PropertyKey, PropertyValue, const PropertyKey &, const PropertyValue &, PropertyMapImpl>( )
{
}

PropertyMap::~PropertyMap(void)
{
}
