#ifndef AREG_BASE_ETEMPLATEBASE_HPP
#define AREG_BASE_ETEMPLATEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/ETemplateBase.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, common constants class for templates.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/IEIOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// TEResourceMapImpl class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The class template for resource containers that save objects associated with the key.
 *          The methods of class are called when clean-up resource element.
 *          Make own implementation if it differs from default implementation.
 * \tparam  RESOURCE_KEY    The type of key to access resource object.
 * \tparam  RESOURCE_OBJECT The type of resource object stored in the map.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT>
class TEResourceMapImpl
{
public:
    /**
     * \brief	Called when all resources are removed.
     **/
    inline void implCleanResource( RESOURCE_KEY & /*Key*/, RESOURCE_OBJECT /*Resource*/ )
    {   }
};

//////////////////////////////////////////////////////////////////////////
// TEResourceListMapImpl class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The class template for resource containers that save list of resources associated with the key.
 *          The methods of class are called when add or remove list, or when remove resource from the list to 
 *          make additional cleanup operation such as free memory.
 *          Make own implementation if it differs from default implementation.
 * \tparam  RESOURCE_KEY    The type of key to access the list of resource objects.
 * \tparam  RESOURCE_OBJECT The type of resource object stored in the resource list.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList>
class TEResourceListMapImpl
{
public:
    /**
     * \brief	Called when all resources are removed.
     *          This function is called from RemoveAllResources() for every single
     *          resource being unregistered.
     * \param	Key	    The Key value of resource.
     * \param	List    The list of resource objects.
     **/
    inline void implCleanResourceList( RESOURCE_KEY & Key, ResourceList & List )
    {   }

    /**
     * \brief	Called when need to add resource object to the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to add to the list.
     **/
    inline void implAddResource( ResourceList & List, RESOURCE_OBJECT Resource )
    {   }

    /**
     * \brief	Called when need to remove resource object from the list.
     * \param	List        The list of resource objects.
     * \param   Resource    The resource object to remove from the list.
     **/
    inline bool implRemoveResource( ResourceList & List, RESOURCE_OBJECT Resource )
    {
        return false;
    }
};

//////////////////////////////////////////////////////////////////////////
// Constless class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Converts the const_iterator type random access iterator into normal iterator.
 * \tparam  Container   The container object type to convert.
 */
template <typename Container>
class Constless
{
public:
    /**
     * \brief   Converts the given const_iterator type into normal iterator type during run-time without casting.
     * \param   cont    The container object, which const_iterator should be converted.
     * \param   cit     The const_iterator object to convert
     * \return  Returns converted iterator type object.
     */
    inline const typename Container::iterator iter(const Container& cont, typename Container::const_iterator& cit) const
    {
        return const_cast<Container &>(cont).erase(cit, cit);
    }

    inline typename Container::iterator iter(Container& cont, typename Container::const_iterator& cit)
    {
        return cont.erase(cit, cit);
    }

    inline typename Container::iterator iter(Container& cont, typename Container::iterator& cit)
    {
        return cit;
    }
};

#endif  // AREG_BASE_ETEMPLATEBASE_HPP
