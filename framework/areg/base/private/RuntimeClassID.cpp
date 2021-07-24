/************************************************************************
 * \file        areg/base/private/RuntimeClassID.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Runtime Class ID
 *              This class contains information of Runtime Class ID.
 *              Every Runtime Class contains class ID to identify and
 *              cast class using ID. As an ID of Runtime class used class name.
 *
 ************************************************************************/
#include "areg/base/RuntimeClassID.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// RuntimeClassID class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
const char* const RuntimeClassID::BAD_CLASS_ID = "_BAD_RUNTIME_CLASS_ID_";

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
RuntimeClassID::RuntimeClassID( void )
    : mClassName(BAD_CLASS_ID)
    , mMagicNum (NEMath::CHECKSUM_IGNORE)
{
    ; // do nothing
}

RuntimeClassID::RuntimeClassID( const char * className )
    : mClassName(BAD_CLASS_ID)
    , mMagicNum (NEMath::CHECKSUM_IGNORE)
{
    if (NEString::isEmpty<char>(className) == false)
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32Calculate(className);
    }
}

RuntimeClassID::RuntimeClassID( const RuntimeClassID & src )
    : mClassName(src.mClassName)
    , mMagicNum (src.mMagicNum)
{
    ASSERT(src.mClassName.isEmpty() == false);
}

RuntimeClassID::~RuntimeClassID( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void RuntimeClassID::setName( const char* className )
{
    if (NEString::isEmpty<char>(className))
    {
        mClassName  = RuntimeClassID::BAD_CLASS_ID;
        mMagicNum      = NEMath::CHECKSUM_IGNORE;
    }
    else
    {
        mClassName  = className;
        mMagicNum   = NEMath::crc32Calculate(className);
    }
}
