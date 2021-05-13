/************************************************************************
 * \file        areg/base/private/NEBasicShape.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, 
 *              Basic shapes and structures like Point, Size, Rectangle
 *
 ************************************************************************/
#include "areg/base/NEBasicShape.hpp"

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::Point class implementation begin
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Point Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::Point::Point( void )
    : NEBasicShape::sPoint( )
{
    this->x = this->y = 0;
}

NEBasicShape::Point::Point( int x, int y )
    : NEBasicShape::sPoint( )
{
    this->x = x; this->y = y;
}

NEBasicShape::Point::Point( const NEBasicShape::sPoint &src )
    : NEBasicShape::sPoint( )
{
    this->x = src.x; this->y = src.y;
}

NEBasicShape::Point::Point( const IEInStream& stream )
    : NEBasicShape::sPoint( )
{
    stream >> static_cast<NEBasicShape::sPoint &>(*this);
}

NEBasicShape::Point::~Point( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Point operators
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::Point& NEBasicShape::Point::operator = ( const NEBasicShape::sPoint & src )
{
    if (static_cast<const NEBasicShape::sPoint *>(this) != &src)
    {
        this->x = src.x; this->y = src.y;
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::Point class implementation end
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::Size class implementation begin
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Size Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::Size::Size( void )
    : NEBasicShape::sSize( )
{
    this->width = this->height = 0;
}

NEBasicShape::Size::Size( int width, int height )
    : NEBasicShape::sSize( )
{
    this->width = width; this->height = height;
}

NEBasicShape::Size::Size( const NEBasicShape::sSize &src )
    : NEBasicShape::sSize( )
{
    this->width = src.width; this->height = src.height;
}

NEBasicShape::Size::Size( const Size &src )
    : NEBasicShape::sSize( )
{
    this->width = src.width; this->height = src.height;
}

NEBasicShape::Size::Size( const IEInStream& stream )
    : NEBasicShape::sSize( )
{
    stream >> static_cast<NEBasicShape::sSize &>(*this);
}

NEBasicShape::Size::~Size( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Size operator
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::Size& NEBasicShape::Size::operator = ( const NEBasicShape::sSize & src )
{
    if (static_cast<const NEBasicShape::sSize *>(this) != &src)
    {
        this->width = src.width; this->height = src.height;
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::Size class implementation end
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// NEBasicShape::Rect class implementation begin
//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Rect Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
NEBasicShape::Rect::Rect( void )
    : NEBasicShape::sRect( )
{
    this->x = this->y = this->width = this->height = 0;
}

NEBasicShape::Rect::Rect( int x, int y, int width, int height )
    : NEBasicShape::sRect( )
{
    this->x = x; this->y = y; this->width = width; this->height = height;
}

NEBasicShape::Rect::Rect( const NEBasicShape::sRect &src )
    : NEBasicShape::sRect( )
{
    this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
}

NEBasicShape::Rect::Rect( const Rect &src )
    : NEBasicShape::sRect( )
{
    this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
}

NEBasicShape::Rect::Rect( const IEInStream& stream )
    : NEBasicShape::sRect( )
{
    stream >> static_cast<NEBasicShape::sRect &>(*this);
}

NEBasicShape::Rect::~Rect( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NEBasicShape::Rect operator
//////////////////////////////////////////////////////////////////////////
const NEBasicShape::Rect& NEBasicShape::Rect::operator = ( const NEBasicShape::sRect & src )
{
    if (static_cast<const NEBasicShape::sRect *>(this) != &src)
    {
        this->x = src.x; this->y = src.y; this->width = src.width; this->height = src.height;
    }

    return (*this);
}
