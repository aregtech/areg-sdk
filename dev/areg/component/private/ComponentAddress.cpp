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
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const ThreadAddress & threadAddress )
    : mRoleName     ( ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const ThreadAddress& threadAddress, const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( threadAddress )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const char * roleName )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( DispatcherThread::getCurrentDispatcherThread().getAddress() )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    Component* comp = Component::findComponentByName(roleName);
    if (comp != NULL)
        mThreadAddress = comp->getAddress().getThreadAddress();

    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const char*  roleName, const char * nameThread )
    : mRoleName     ( NEString::isEmpty<char>(roleName) == false ? roleName : ComponentAddress::INVALID_COMPONENT_NAME )
    , mThreadAddress( nameThread != NULL ? DispatcherThread::getDispatcherThread(nameThread).getAddress() : ThreadAddress::INVALID_THREAD_ADDRESS )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mRoleName.truncate(NEUtilities::ITEM_NAMES_MAX_LENGTH);
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::ComponentAddress( const ComponentAddress& src )
    : mRoleName     ( src.mRoleName )
    , mThreadAddress( src.mThreadAddress )
    , mMagicNum     ( src.mMagicNum )
{
    ; // do nothing
}

ComponentAddress::ComponentAddress( const IEInStream & stream )
    : mRoleName     ( stream )
    , mThreadAddress( stream )
    , mMagicNum     ( NEMath::CHECKSUM_IGNORE )
{
    mMagicNum   = ComponentAddress::_magicNumber(*this);
}

ComponentAddress::~ComponentAddress( void )
{
    ; // do nothing
}

bool ComponentAddress::isValid( void ) const
{
    return (mMagicNum != NEMath::CHECKSUM_IGNORE) && mThreadAddress.isValid();
}

String ComponentAddress::convToString(void) const
{
    String result = "";

    result += mRoleName;
    result += NEUtilities::COMPONENT_PATH_SEPARATOR;
    result += ThreadAddress::convAddressToPath(mThreadAddress);

    return result;
}

void ComponentAddress::convFromString(const char * pathComponent, const char** out_nextPart /*= NULL*/)
{
    const char* strSource = pathComponent;

    mRoleName       = String::getSubstring(strSource, NEUtilities::COMPONENT_PATH_SEPARATOR, &strSource);
    mThreadAddress  = ThreadAddress::convPathToAddress(strSource, &strSource);
    mMagicNum       = ComponentAddress::_magicNumber(*this);

    if (out_nextPart != NULL)
        *out_nextPart = strSource;
}

unsigned int ComponentAddress::_magicNumber(const ComponentAddress & addrComp)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if (addrComp.mThreadAddress.isValid() && (addrComp.mRoleName.isEmpty() == false) && (addrComp.mRoleName != ComponentAddress::INVALID_COMPONENT_NAME))
    {
        result = NEMath::crc32Init();
        result = NEMath::crc32Start(result, addrComp.mThreadAddress.getThreadName().getString());
        result = NEMath::crc32Start(result, addrComp.mRoleName.getString());
        result = NEMath::crc32Finish(result);
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
String ComponentAddress::convAddressToPath( const ComponentAddress& componentAddress )
{
    return componentAddress.convToString();
}

ComponentAddress ComponentAddress::convPathToAddress( const char* componentPath, const char** out_nextPart /*= NULL*/ )
{
    ComponentAddress result;
    result.convFromString(componentPath, out_nextPart);
    return result;
}
