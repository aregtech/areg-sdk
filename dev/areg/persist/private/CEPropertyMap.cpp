/************************************************************************
 * \file        areg/persist/private/CEPropertyMap.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Property map object
 ************************************************************************/
#include "areg/persist/private/CEPropertyMap.hpp"

CEPropertyMap::CEPropertyMap(void)
    : TEHashMap<CEPropertyKey, CEPropertyValue, const CEPropertyKey &, const CEPropertyValue &, CEPropertyMapImpl>( )
{
}

CEPropertyMap::~CEPropertyMap(void)
{
}
