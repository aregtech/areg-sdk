/************************************************************************
 * \file        areg/src/base/private/NEBasicShape.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, 
 *              Basic shapes and structures like Point, Size, Rectangle
 *
 ************************************************************************/
#include "areg/src/base/NEBasicShape.hpp"

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::CEPoint class implementation begin
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CEPoint Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::CEPoint::CEPoint( void )
    : NEBasicShape::sPoint( )
{
    this->x = this->y = 0;
}

NEBasicShape::CEPoint::CEPoint( int x, int y )
    : NEBasicShape::sPoint( )
{
    this->x = x; this->y = y;
}

NEBasicShape::CEPoint::CEPoint( const NEBasicShape::sPoint &src )
    : NEBasicShape::sPoint( )
{
    this->x = src.x; this->y = src.y;
}

NEBasicShape::CEPoint::CEPoint( const IEInStream& stream )
    : NEBasicShape::sPoint( )
{
    stream >> static_cast<NEBasicShape::sPoint &>(*this);
}

NEBasicShape::CEPoint::~CEPoint( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CEPoint operators
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::CEPoint& NEBasicShape::CEPoint::operator = ( const NEBasicShape::sPoint & src )
{
    if (static_cast<const NEBasicShape::sPoint *>(this) != &src)
    {
        this->x = src.x; this->y = src.y;
    }
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::CEPoint class implementation end
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::CESize class implementation begin
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CESize Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::CESize::CESize( void )
    : NEBasicShape::sSize( )
{
    this->width = this->height = 0;
}

NEBasicShape::CESize::CESize( int width, int height )
    : NEBasicShape::sSize( )
{
    this->width = width; this->height = height;
}

NEBasicShape::CESize::CESize( const NEBasicShape::sSize &src )
    : NEBasicShape::sSize( )
{
    this->width = src.width; this->height = src.height;
}

NEBasicShape::CESize::CESize( const CESize &src )
    : NEBasicShape::sSize( )
{
    this->width = src.width; this->height = src.height;
}

NEBasicShape::CESize::CESize( const IEInStream& stream )
    : NEBasicShape::sSize( )
{
    stream >> static_cast<NEBasicShape::sSize &>(*this);
}

NEBasicShape::CESize::~CESize( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CESize operator
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::CESize& NEBasicShape::CESize::operator = ( const NEBasicShape::sSize & src )
{
    if (static_cast<const NEBasicShape::sSize *>(this) != &src)
    {
        this->width = src.width; this->height = src.height;
    }
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::CESize class implementation end
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::CERect class implementation begin
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CERect Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::CERect::CERect( void )
    : NEBasicShape::sRect( )
{
    this->x = this->y = this->width = this->height = 0;
}

NEBasicShape::CERect::CERect( int x, int y, int width, int height )
    : NEBasicShape::sRect( )
{
    this->x = x; this->y = y; this->width = width; this->height = height;
}

NEBasicShape::CERect::CERect( const NEBasicShape::sRect &src )
    : NEBasicShape::sRect( )
{
    this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
}

NEBasicShape::CERect::CERect( const CERect &src )
    : NEBasicShape::sRect( )
{
    this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
}

NEBasicShape::CERect::CERect( const IEInStream& stream )
    : NEBasicShape::sRect( )
{
    stream >> static_cast<NEBasicShape::sRect &>(*this);
}

NEBasicShape::CERect::~CERect( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::CERect operator
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::CERect& NEBasicShape::CERect::operator = ( const NEBasicShape::sRect & src )
{
    if (static_cast<const NEBasicShape::sRect *>(this) != &src)
    {
        this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
    }
    return (*this);
}
