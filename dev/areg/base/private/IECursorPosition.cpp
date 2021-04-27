/************************************************************************
 * \file        areg/base/private/IECursorPosition.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, cursor position interface
 *
 ************************************************************************/
#include "areg/base/IECursorPosition.hpp"

//////////////////////////////////////////////////////////////////////////
// IECursorPosition class implementation
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Indicator of invalid position of cursor
 **/
const unsigned int IECursorPosition::INVALID_CURSOR_POSITION   = static_cast<unsigned int>(-1);

/**
 * \brief   Indicator of cursor start position
 **/
const unsigned int IECursorPosition::START_CURSOR_POSITION     = static_cast<unsigned int>(0);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

IECursorPosition::IECursorPosition( void )
{
    ; // do nothing
}

IECursorPosition::~IECursorPosition( void )
{
    ; // do nothing
}
