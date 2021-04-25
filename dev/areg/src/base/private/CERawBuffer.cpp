/************************************************************************
 * \file        areg/src/base/private/CERawBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Raw Buffer class
 *
 ************************************************************************/

#include "areg/src/base/CERawBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// CERawBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CERawBuffer::CERawBuffer( void )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    ; // do nothing
}

CERawBuffer::CERawBuffer( const unsigned char* buffer, unsigned int dataSize )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  (static_cast<IEByteBuffer &>(self()))
{
    WriteData(buffer, dataSize);
}

CERawBuffer::CERawBuffer( int reservedSize )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    ResizeBuffer(reservedSize, false);
}

CERawBuffer::CERawBuffer( const CERawBuffer& src )
    : CEBufferStreamBase( NEMemory::InvalidBuffer, static_cast<IECursorPosition &>(self()), static_cast<IECursorPosition &>(self()) )
    , CEBufferPosition  ( static_cast<IEByteBuffer &>(self()) )
{
    if (src.IsValid())
    {
        WriteData(src.GetBuffer(), src.GetUsedSize());
    }
}

CERawBuffer::~CERawBuffer( void )
{
    _deleteBuffer();
}

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
const CERawBuffer& CERawBuffer::operator = ( const CERawBuffer &src )
{
    if (this != &src)
    {
        _deleteBuffer();
     
        if (src.IsValid())
        {
            Write(src.GetBuffer(), src.GetUsedSize());
        }
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void CERawBuffer::AddReference( void )
{
    ; // do nothing
}

void CERawBuffer::RemoveReference( void )
{
    _deleteBuffer( );
}

bool CERawBuffer::IsShared( void ) const
{
    return false;
}

void CERawBuffer::_deleteBuffer( void )
{
    if (IsValid())
    {
        delete [] reinterpret_cast<unsigned char *>(mByteBuffer);
    }

    CEBufferPosition::InvalidatePosition();
    mByteBuffer = &NEMemory::InvalidBuffer;
}

bool CERawBuffer::IsSharable( void ) const
{
    return false;
}

void CERawBuffer::InvalidateBuffer( void )
{
    _deleteBuffer();
}

int CERawBuffer::SizeDataOffset(void) const
{
    return sizeof(NEMemory::sBuferHeader);
}

int CERawBuffer::SizeBufferStruct(void) const
{
    return sizeof(NEMemory::sByteBuffer);
}
