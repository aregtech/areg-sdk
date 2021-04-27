/************************************************************************
 * \file        areg/component/private/CEComponentAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Address class implementation.
 *
 ************************************************************************/
#include "areg/component/CEComponentAddress.hpp"

#include "areg/component/CEComponent.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// CEComponentAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The name of invalid component
 **/
const char* const           CEComponentAddress::INVALID_COMPONENT_NAME      = "INVALID_COMPONENT_NAME";
/**
 * \brief   The address of invalid component
 **/
const CEComponentAddress    CEComponentAddress::INVALID_COMPONENT_ADDRESS(CEThreadAddress::INVALID_THREAD_ADDRESS, CEComponentAddress::INVALID_COMPONENT_NAME);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CEComponentAddress::CEComponentAddress( void )
    : mRoleName     ( CEComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( CEThreadAddress::INVALID_THREAD_ADDRESS )
{
    ; // do nothing
}

CEComponentAddress::CEComponentAddress( const CEThreadAddress & threadAddress )
    : mRoleName     ( CEComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
{
    ; // do nothing
}

CEComponentAddress::CEComponentAddress( const CEThreadAddress& threadAddress, const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : CEComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
{
    mRoleName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

CEComponentAddress::CEComponentAddress( const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : CEComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( CEDispatcherThread::GetCurrentDispatcherThread().GetThreadAddress() )
{
    mRoleName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    CEComponent* comp = CEComponent::FindComponentByName(roleName);
    if (comp != NULL)
        mThreadAddress = comp->GetComponentAddress().GetThreadAddress();
}

CEComponentAddress::CEComponentAddress( const char*  roleName, const char * nameThread )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : CEComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( nameThread != NULL ? CEDispatcherThread::GetDispatcherThread(nameThread).GetThreadAddress() : CEThreadAddress::INVALID_THREAD_ADDRESS )
{
    mRoleName.Truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

CEComponentAddress::CEComponentAddress( const CEComponentAddress& src )
    : mRoleName     ( src.mRoleName )
    , mThreadAddress( src.mThreadAddress )
{
    ; // do nothing
}

CEComponentAddress::CEComponentAddress( const IEInStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
{
    ;   // this
}

CEComponentAddress::~CEComponentAddress( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////

CEComponentAddress::operator unsigned int ( void ) const
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( IsValid() )
    {
        result = NEMath::Crc32Start( ~static_cast<unsigned int>(mThreadAddress), mRoleName.GetBuffer() );
        result = NEMath::Crc32Finish(result);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
CEString CEComponentAddress::ConvertAddressToPath( const CEComponentAddress& componentAddress )
{
    CEString result = "";
    
    result += componentAddress.mRoleName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += CEThreadAddress::ConvertAddressToPath(componentAddress.mThreadAddress);

    return result;
}

CEComponentAddress CEComponentAddress::ConvertPathToAddress( const char* componentPath, const char** out_nextPart /*= NULL*/ )
{
    CEComponentAddress result;
    const char* strSource = componentPath;
    if (out_nextPart != NULL)
        *out_nextPart = componentPath;

    result.mRoleName     = CEString::GetSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mThreadAddress= CEThreadAddress::ConvertPathToAddress(strSource, &strSource);

    if (result.IsValid())
    {
        if (out_nextPart != NULL)
            *out_nextPart = strSource;
    }

    return result;
}
