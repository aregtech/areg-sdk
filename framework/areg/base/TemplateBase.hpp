#ifndef AREG_BASE_TEMPLATEBASE_HPP
#define AREG_BASE_TEMPLATEBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ETemplateBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, common constants class for templates.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/IOStream.hpp"

//////////////////////////////////////////////////////////////////////////
// ResourceMapImpl class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class template for resource containers; override impl_clean_resource() to customize
 *          cleanup behavior.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT>
class ResourceMapImpl
{
public:
    /**
     * \brief   Called when removing a resource element; override to customize cleanup logic.
     **/
    inline void impl_clean_resource( RESOURCE_KEY & /*Key*/, RESOURCE_OBJECT /*Resource*/ )
    {   }
};

//////////////////////////////////////////////////////////////////////////
// ResourceListMapImpl class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Helper class template providing customizable resource list cleanup and manipulation for
 *          resource containers.
 **/
template <typename RESOURCE_KEY, typename RESOURCE_OBJECT, class ResourceList>
class ResourceListMapImpl
{
public:
    /**
     * \brief   Called when removing all resources. Implement to perform cleanup on the list.
     *
     * \param   Key     The key associated with the resource list.
     * \param   List    The list of resource objects.
     **/
    inline void impl_clean_list( RESOURCE_KEY & Key, ResourceList & List )
    {   }

    /**
     * \brief   Called when adding a resource to the list. Implement to perform custom addition
     *          logic.
     *
     * \param   List        The list of resource objects.
     * \param   Resource    The resource object to add.
     **/
    inline void impl_add_resource( ResourceList & List, RESOURCE_OBJECT Resource )
    {   }

    /**
     * \brief   Called when removing a resource from the list. Implement to perform custom removal
     *          logic.
     *
     * \param   List        The list of resource objects.
     * \param   Resource    The resource object to remove.
     **/
    inline bool impl_remove_resource( ResourceList & List, RESOURCE_OBJECT Resource )
    {
        return false;
    }
};

//////////////////////////////////////////////////////////////////////////
// Constless class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Utility for converting const_iterator to normal iterator without casting.
 **/
template <typename Container>
class Constless
{
public:
    /**
     * \brief   Converts a const_iterator to a normal iterator on a const container.
     *
     * \param   cont    The const container whose iterator should be converted.
     * \param   cit     The const_iterator to convert.
     * \return  Returns the converted iterator.
     **/
    inline const typename Container::iterator iter(const Container& cont, typename Container::const_iterator& cit) const
    {
        return const_cast<Container &>(cont).erase(cit, cit);
    }

    /**
     * \brief   Converts a const_iterator to a normal iterator on a mutable container.
     *
     * \param   cont    The mutable container whose iterator should be converted.
     * \param   cit     The const_iterator to convert.
     * \return  Returns the converted iterator.
     **/
    inline typename Container::iterator iter(Container& cont, typename Container::const_iterator& cit)
    {
        return cont.erase(cit, cit);
    }

    /**
     * \brief   Returns the given iterator unchanged.
     *
     * \param   cont    The container (unused).
     * \param   cit     The iterator to return.
     * \return  Returns the iterator unchanged.
     **/
    inline typename Container::iterator iter(Container& cont, typename Container::iterator& cit)
    {
        return cit;
    }
};

#endif  // AREG_BASE_TEMPLATEBASE_HPP
