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
 * \file            
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, common constants class for templates.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/IOStream.hpp"
namespace areg {

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
    inline void impl_clean_resource( RESOURCE_KEY & /*Key*/, RESOURCE_OBJECT /*Resource*/ ) noexcept
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
    inline void impl_clean_list( RESOURCE_KEY & Key, ResourceList & List ) noexcept
    {   }

    /**
     * \brief   Called when adding a resource to the list. Implement to perform custom addition
     *          logic.
     *
     * \param   List        The list of resource objects.
     * \param   Resource    The resource object to add.
     **/
    inline void impl_add_resource( ResourceList & List, RESOURCE_OBJECT Resource ) noexcept
    {   }

    /**
     * \brief   Called when removing a resource from the list. Implement to perform custom removal
     *          logic.
     *
     * \param   List        The list of resource objects.
     * \param   Resource    The resource object to remove.
     **/
    inline bool impl_remove_resource( ResourceList & List, RESOURCE_OBJECT Resource ) noexcept
    {
        return false;
    }
};

//////////////////////////////////////////////////////////////////////////
// Constless class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Utility for converting a const_iterator to a mutable iterator without undefined
 *          behavior.
 *
 *          The conversion uses the empty-range erase trick: `cont.erase(cit, cit)` erases no
 *          elements (first == last) but returns an iterator to the same position as a mutable
 *          iterator type. This is the only standard-compliant, UB-free approach in C++17 —
 *          there is no std::const_iterator_cast.
 *
 *          Cost: In optimized builds (-O2/-O3) the empty-range erase compiles away to zero
 *          instructions. In debug builds it involves a real call with iterator validity checks.
 *          No better zero-cost alternative exists in C++17 without invoking UB.
 *
 *          Note: MAPPOS / LISTPOS cannot be replaced with const_iterator throughout the
 *          containers, because mutating operations (erase, modify-at-position) always require a
 *          mutable iterator.
 **/
template <typename Container>
class Constless
{
public:
    /**
     * \brief   Converts a const_iterator to a mutable iterator; uses an empty-range erase on a
     *          const-cast of the container. The container is not modified.
     *
     * \param   cont    The const container (const_cast is safe: erase does no work on empty range).
     * \param   cit     The const_iterator to convert.
     * \return  Returns the equivalent mutable iterator.
     **/
    [[nodiscard]]
    inline typename Container::iterator iter(const Container& cont, const typename Container::const_iterator& cit) const noexcept
    {
        return const_cast<Container &>(cont).erase(cit, cit);
    }

    /**
     * \brief   Converts a const_iterator to a mutable iterator on a mutable container.
     *
     * \param   cont    The mutable container.
     * \param   cit     The const_iterator to convert.
     * \return  Returns the equivalent mutable iterator.
     **/
    [[nodiscard]]
    inline typename Container::iterator iter(Container& cont, const typename Container::const_iterator& cit) noexcept
    {
        return cont.erase(cit, cit);
    }

    /**
     * \brief   Returns the given mutable iterator unchanged (no conversion needed).
     *
     * \param   cit     The mutable iterator to return.
     * \return  Returns the iterator unchanged.
     **/
    [[nodiscard]]
    inline constexpr typename Container::iterator iter(Container& /*cont*/, typename Container::iterator cit) noexcept
    {
        return cit;
    }
};

} // namespace areg
#endif  // AREG_BASE_TEMPLATEBASE_HPP
