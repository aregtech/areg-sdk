/************************************************************************
 * \file        areg/base/private/RawBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Raw Buffer class
 *
 ************************************************************************/

#include "areg/base/RawBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// RawBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RawBuffer::RawBuffer( void )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    ; // do nothing
}

RawBuffer::RawBuffer( const unsigned char* buffer, unsigned int dataSize )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  (static_cast<IEByteBuffer &>(self()))
{
    writeData(buffer, dataSize);
}

RawBuffer::RawBuffer( int reservedSize )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    resize(reservedSize > 0 ? static_cast<unsigned int>(reservedSize) : 0, false);
}

RawBuffer::RawBuffer( const RawBuffer& src )
    : BufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , BufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    if (src.isValid())
    {
        writeData(src.getBuffer(), src.getSizeUsed());
    }
}

RawBuffer::~RawBuffer( void )
{
    _deleteBuffer();
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const RawBuffer& RawBuffer::operator = ( const RawBuffer &src )
{
    if (this != &src)
    {
        _deleteBuffer();
     
        if (src.isValid())
        {
            write(src.getBuffer(), src.getSizeUsed());
        }
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void RawBuffer::addReference( void )
{
    ; // do nothing
}

void RawBuffer::removeReference( void )
{
    _deleteBuffer( );
}

bool RawBuffer::isShared( void ) const
{
    return false;
}

void RawBuffer::_deleteBuffer( void )
{
    if (isValid())
    {
        delete [] reinterpret_cast<unsigned char *>(mByteBuffer);
    }

    BufferPosition::invalidate();
    mByteBuffer = &NEMemory::InvalidBuffer;
}

bool RawBuffer::canShare( void ) const
{
    return false;
}

void RawBuffer::invalidate( void )
{
    _deleteBuffer();
}

unsigned int RawBuffer::getDataOffset(void) const
{
    return sizeof(NEMemory::sBuferHeader);
}

unsigned int RawBuffer::getHeaderSize(void) const
{
    return sizeof(NEMemory::sByteBuffer);
}
