/************************************************************************
 * \file        areg/base/private/CERuntimeClassID.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Runtime Class ID
 *              This class contains information of Runtime Class ID.
 *              Every Runtime Class contains class ID to identify and
 *              cast class using ID. As an ID of Runtime class used class name.
 *
 ************************************************************************/
#include "areg/base/CERuntimeClassID.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/NEMath.hpp"

//////////////////////////////////////////////////////////////////////////
// CERuntimeClassID class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
const char* const CERuntimeClassID::BAD_CLASS_ID = "_BAD_RUNTIME_CLASS_ID_";

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
CERuntimeClassID::CERuntimeClassID( void )
    : mClassName    (BAD_CLASS_ID)
{
    ; // do nothing
}

CERuntimeClassID::CERuntimeClassID( const char * className )
    : mClassName    (NEString::isEmpty<char>(className) == false ? className : BAD_CLASS_ID)
{
    ; // do nothing
}

CERuntimeClassID::CERuntimeClassID( const CERuntimeClassID & src )
    : mClassName    (src.mClassName)
{
    ASSERT(src.mClassName.IsEmpty() == false);
}

CERuntimeClassID::~CERuntimeClassID( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

CERuntimeClassID::operator unsigned int( void ) const
{
    ASSERT(mClassName.IsEmpty() == false);
    return static_cast<unsigned int>( NEMath::Crc32Calculate( static_cast<const char *>(mClassName.GetBuffer())) );
}

void CERuntimeClassID::SetName( const char* className )
{
    mClassName = (NEString::isEmpty<char>(className) == false ? className : BAD_CLASS_ID);
}
