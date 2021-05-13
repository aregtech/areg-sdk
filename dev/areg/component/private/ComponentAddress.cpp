/************************************************************************
 * \file        areg/component/private/ComponentAddress.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Address class implementation.
 *
 ************************************************************************/
#include "areg/component/ComponentAddress.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// ComponentAddress class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The name of invalid component
 **/
const char* const           ComponentAddress::INVALID_COMPONENT_NAME      = "INVALID_COMPONENT_NAME";
/**
 * \brief   The address of invalid component
 **/
const ComponentAddress    ComponentAddress::INVALID_COMPONENT_ADDRESS(ThreadAddress::INVALID_THREAD_ADDRESS, ComponentAddress::INVALID_COMPONENT_NAME);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentAddress::ComponentAddress( void )
    : mRoleName     ( ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( ThreadAddress::INVALID_THREAD_ADDRESS )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress )
    : mRoleName     ( ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const ThreadAddress& threadAddress, const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

ComponentAddress::ComponentAddress( const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( DispatcherThread::getCurrentDispatcherThread().getAddress() )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::findComponentByName(roleName);
    if (comp != NULL)
        mThreadAddress = comp->getAddress().getThreadAddress();
}

ComponentAddress::ComponentAddress( const char*  roleName, const char * nameThread )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( nameThread != NULL ? DispatcherThread::getDispatcherThread(nameThread).getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
}

ComponentAddress::ComponentAddress( const ComponentAddress& src )
    : mRoleName     ( src.mRoleName )
    , mThreadAddress( src.mThreadAddress )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const IEInStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
{
    ;   // this
}

ComponentAddress::~ComponentAddress( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////

ComponentAddress::operator unsigned int ( void ) const
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( isValid() )
    {
        result = NEMath::crc32Start( ~static_cast<unsigned int>(mThreadAddress), mRoleName.getString() );
        result = NEMath::crc32Finish(result);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ComponentAddress::convAddressToPath( const ComponentAddress& componentAddress )
{
    String result = "";
    
    result += componentAddress.mRoleName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += ThreadAddress::convAddressToPath(componentAddress.mThreadAddress);

    return result;
}

ComponentAddress ComponentAddress::convPathToAddress( const char* componentPath, const char** out_nextPart /*= NULL*/ )
{
    ComponentAddress result;
    const char* strSource = componentPath;
    if (out_nextPart != NULL)
        *out_nextPart = componentPath;

    result.mRoleName     = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    result.mThreadAddress= ThreadAddress::convPathToAddress(strSource, &strSource);

    if (result.isValid())
    {
        if (out_nextPart != NULL)
            *out_nextPart = strSource;
    }

    return result;
}
