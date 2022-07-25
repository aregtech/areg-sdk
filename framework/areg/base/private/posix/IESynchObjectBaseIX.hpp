#ifndef AREG_BASE_PRIVATE_POSIX_IESYNCHOBJECTBASEIX_HPP
#define AREG_BASE_PRIVATE_POSIX_IESYNCHOBJECTBASEIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/IESynchObjectBaseIX.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/NESynchTypesIX.hpp"
#include "areg/base/IESynchObject.hpp"

//////////////////////////////////////////////////////////////////////////
// Declared classes and hierarchies.
//////////////////////////////////////////////////////////////////////////
class IESynchObjectBaseIX;
    class MutexIX;
        class WaitableBaseIX;
            class WaitableEventIX;
            class WaitableMutexIX;

//////////////////////////////////////////////////////////////////////////
// IESynchObjectBaseIX class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX specific Synchronization object base class.
 *          All synchronization objects should inherit from this class.
 **/
class IESynchObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constants, types and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The length of synchronization object name, including null-termination symbol.
     **/
    static const int    _MAX_NAME_LENGTH    = 32;

//////////////////////////////////////////////////////////////////////////
// Protected constructor.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     * \param   synchType   The type of synchronization object.
     * \param   asciiName   The name of synchronization object.
     **/
    IESynchObjectBaseIX( NESynchTypesIX::eSynchObject synchType, const char* asciiName = nullptr );

//////////////////////////////////////////////////////////////////////////
// Public destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IESynchObjectBaseIX( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IESynchObjectBaseIX attributes.
/************************************************************************/
    
    /**
     * \brief   Returns the type of synchronization object.
     **/
    inline NESynchTypesIX::eSynchObject getSynchType( void ) const;

    /**
     * \brief   Returns synchronization object name.
     **/
    inline const String & getName( void ) const;

/************************************************************************/
// IESynchObjectBaseIX overrides.
/************************************************************************/
    
    /**
     * \brief   Returns true if synchronization object is valid.
     **/
    virtual bool isValid( void ) const = 0;

    /**
     * \brief   Triggered when synchronization object is going to be deleted.
     *          This should free all resources.
     **/
    virtual void freeResources( void ) = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   The type of synchronization object.
     **/
    const NESynchTypesIX::eSynchObject  mSynchType;

    /**
     * \brief   The name of synchronization object.
     **/
    String  mSynchName;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    IESynchObjectBaseIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IESynchObjectBaseIX );
};


//////////////////////////////////////////////////////////////////////////
// IESynchObjectBaseIX class inline implementation
//////////////////////////////////////////////////////////////////////////
inline NESynchTypesIX::eSynchObject IESynchObjectBaseIX::getSynchType( void ) const
{
    return mSynchType;
}

inline const String & IESynchObjectBaseIX::getName( void ) const
{
    return mSynchName;
}

#endif //  defined(_POSIX) || defined(POSIX)

#endif // AREG_BASE_PRIVATE_POSIX_IESYNCHOBJECTBASEIX_HPP
