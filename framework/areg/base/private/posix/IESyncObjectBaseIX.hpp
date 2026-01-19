#ifndef AREG_BASE_PRIVATE_POSIX_IESYNCOBJECTBASEIX_HPP
#define AREG_BASE_PRIVATE_POSIX_IESYNCOBJECTBASEIX_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/IESyncObjectBaseIX.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, POSIX based Synchronization object base.
 *
 ************************************************************************/

 /************************************************************************
  * Includes
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#if  defined(_POSIX) || defined(POSIX)

#include "areg/base/private/posix/NESyncTypesIX.hpp"
#include "areg/base/IESyncObject.hpp"
#include "areg/base/String.hpp"

//////////////////////////////////////////////////////////////////////////
// Declared classes and hierarchies.
//////////////////////////////////////////////////////////////////////////
class IESyncObjectBaseIX;
    class MutexIX;
        class WaitableBaseIX;
            class WaitableEventIX;
            class WaitableMutexIX;

//////////////////////////////////////////////////////////////////////////
// IESyncObjectBaseIX class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   POSIX specific Synchronization object base class.
 *          All synchronization objects should inherit from this class.
 **/
class IESyncObjectBaseIX
{
//////////////////////////////////////////////////////////////////////////
// Constants, types and static members.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The length of synchronization object name, including null-termination symbol.
     **/
    static constexpr int    _MAX_NAME_LENGTH    { 32 };

//////////////////////////////////////////////////////////////////////////
// Protected constructor.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initialization constructor.
     * \param   syncType    The type of synchronization object.
     * \param   asciiName   The name of synchronization object.
     **/
    IESyncObjectBaseIX( NESyncTypesIX::eSyncObject syncType, const char* asciiName = nullptr );

//////////////////////////////////////////////////////////////////////////
// Public destructor.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Destructor.
     **/
    virtual ~IESyncObjectBaseIX( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IESyncObjectBaseIX attributes.
/************************************************************************/
    
    /**
     * \brief   Returns the type of synchronization object.
     **/
    inline NESyncTypesIX::eSyncObject getSyncType( void ) const;

    /**
     * \brief   Returns synchronization object name.
     **/
    inline const String & getName( void ) const;

/************************************************************************/
// IESyncObjectBaseIX overrides.
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
    const NESyncTypesIX::eSyncObject  mSyncType;

    /**
     * \brief   The name of synchronization object.
     **/
    String  mSyncName;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    IESyncObjectBaseIX( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( IESyncObjectBaseIX );
};


//////////////////////////////////////////////////////////////////////////
// IESyncObjectBaseIX class inline implementation
//////////////////////////////////////////////////////////////////////////
inline NESyncTypesIX::eSyncObject IESyncObjectBaseIX::getSyncType( void ) const
{
    return mSyncType;
}

inline const String & IESyncObjectBaseIX::getName( void ) const
{
    return mSyncName;
}

#endif //  defined(_POSIX) || defined(POSIX)

#endif // AREG_BASE_PRIVATE_POSIX_IESYNCOBJECTBASEIX_HPP
