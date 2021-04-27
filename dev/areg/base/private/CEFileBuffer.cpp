/************************************************************************
 * \file        areg/base/private/CEFileBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Memory File object
 *
 ************************************************************************/
#include "areg/base/CEFileBuffer.hpp"
#include "areg/base/CEWideString.hpp"

//////////////////////////////////////////////////////////////////////////
// CEFileBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const unsigned int CEFileBuffer::BLOCK_SIZE  = (4 * NEMemory::BLOCK_SIZE);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEFileBuffer::CEFileBuffer(  int         mode       /*= (CEFileBase::FO_MODE_WRITE | CEFileBase::FO_MODE_BINARY)*/
                           , const char* name       /*= NULL*/
                           , int         blockSize  /*= BLOCK_SIZE*/)
    : CEFileBase    ( )
    , CESharedBuffer(blockSize)

    , mFileMode     (mode & (~CEFileBase::FOB_ATTACH))  // FOB_MODE_ATTACH
    , mFileName     (name)
    , mIsOpened     (false)
{
}

CEFileBuffer::CEFileBuffer(NEMemory::sByteBuffer & buffer, const char* name /*= NULL*/)
    : CEFileBase        ( )
    , CESharedBuffer    (buffer)

    , mFileMode         (&buffer != &NEMemory::InvalidBuffer ? CEFileBase::FO_MODE_ATTACH : CEFileBase::FO_MODE_INVALID)
    , mFileName         (name)
    , mIsOpened         (false)
{
}

CEFileBuffer::CEFileBuffer(CESharedBuffer & sharedBuffer, const char* name /*= NULL*/)
    : CEFileBase        ( )
    , CESharedBuffer    (sharedBuffer.GetByteBuffer())

    , mFileMode         (sharedBuffer.IsValid() ? CEFileBase::FO_MODE_ATTACH : CEFileBase::FO_MODE_INVALID)
    , mFileName         (name)
    , mIsOpened         (false)
{
}

CEFileBuffer::CEFileBuffer(const CESharedBuffer & sharedBuffer, const char* name /*= NULL*/)
    : CEFileBase        ( )
    , CESharedBuffer    (sharedBuffer)

    , mFileMode         (CEFileBase::FO_MODE_ATTACH | CEFileBase::FO_MODE_READ)
    , mFileName         (name)
    , mIsOpened         ( sharedBuffer.IsValid() )
{
}

CEFileBuffer::~CEFileBuffer( void )
{
    mIsOpened   = false;
    mFileMode   = CEFileBase::FO_MODE_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

inline void CEFileBuffer::SetName(const char* name)
{
    mFileName = name;
}

const char* CEFileBuffer::GetName( void ) const
{
    return static_cast<const char *>(mFileName);
}

int CEFileBuffer::GetMode( void ) const
{
    return mFileMode;
}

bool CEFileBuffer::Open( void )
{
    if ((IsOpened() == false) && (mFileMode != CEFileBase::FO_MODE_INVALID))
    {
        mFileMode = NormalizeMode(mFileMode);
        if (IsAttachMode() == false)
        {
            ResizeBuffer(mBlockSize, false);
        }
        else
        {
            ASSERT(IsValid());
        }

        mIsOpened   = IsValid();
    }
    else
    {
        OUTPUT_ERR("File is already opened. Close file.");
    }
    return mIsOpened;
}

bool CEFileBuffer::Open(const char* fileName, int mode)
{
    bool result = false;
    if (IsOpened() == false)
    {
        if (IsValid() == false)
            mode &= ~CEFileBase::FOB_ATTACH;

        mFileMode = mode != CEFileBase::FO_MODE_INVALID ? mode : mFileMode;
        if (mFileMode != CEFileBase::FO_MODE_INVALID)
        {
            SetName(fileName);
            result = Open();
        }
    }
    else
    {
        OUTPUT_ERR("File is already opened. Close file.");
    }
    return result;
}

void CEFileBuffer::Close( void )
{
    if (IsValid())
    {
        if (IsDeleteForced() || IsTemporary() )
            RemoveReference();
        else if ((IsAttachMode() == false) && (IsDetachMode() == false))
            RemoveReference();
        else if (GetUsedSize() == 0)
            RemoveReference();
        // else, do nothing, either buffer is attached or detach mode is set.
    }

    // keep file name and mode that it can be again reopened.
    // remove 'attached' flag, since the buffer is not valid any mode
    mIsOpened   = false;
    mByteBuffer = &NEMemory::InvalidBuffer;
    mFileMode  &= ~CEFileBase::FOB_ATTACH;
    CEBufferPosition::InvalidatePosition();
}

unsigned int CEFileBuffer::Read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (IsOpened() && CanRead())
    {
        result = CESharedBuffer::Read(buffer, size);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanRead() ? "true" : "false");
    }
    
    return result;
}

unsigned int CEFileBuffer::Read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    
    if (IsOpened() && CanRead())
    {
        result = CESharedBuffer::Read(buffer);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanRead() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::Read( CEString & asciiString ) const
{
    unsigned int result = 0;

    if (IsOpened() && CanRead())
    {
        result = CESharedBuffer::Read(asciiString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanRead() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::Read( CEWideString & wideString ) const
{
    unsigned int result = 0;

    if (IsOpened() && CanRead())
    {
        result = CESharedBuffer::Read(wideString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanRead() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::GetReadableSize( void ) const
{
    return (IsOpened() ? CESharedBuffer::GetReadableSize() : 0);
}

unsigned int CEFileBuffer::Write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;

    if (IsOpened() && CanWrite())
    {
        result = CESharedBuffer::Write(buffer, size);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanWrite() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::Write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;

    if (IsOpened() && CanWrite())
    {
        result = CESharedBuffer::Write(buffer);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanWrite() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::Write( const CEString & asciiString )
{
    unsigned int result = 0;

    if (IsOpened() && CanWrite())
    {
        result = CESharedBuffer::Write(asciiString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanWrite() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::Write( const CEWideString & wideString )
{
    unsigned int result = 0;

    if (IsOpened() && CanWrite())
    {
        result = CESharedBuffer::Write(wideString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", IsOpened() ? "true" : "false", CanWrite() ? "true" : "false");
    }

    return result;
}

unsigned int CEFileBuffer::GetWritableSize( void ) const
{
    return (IsOpened() ? CESharedBuffer::GetWritableSize() : 0);
}

bool CEFileBuffer::Remove( void )
{
    RemoveReference();

    mFileName   = CEString::EmptyString;
    mFileMode   = CEFileBase::FO_MODE_INVALID;
    mIsOpened   = false;

    return true;
}

unsigned int CEFileBuffer::GetLength( void ) const
{
    return (IsOpened() ? CESharedBuffer::GetUsedSize() : NEMemory::INVALID_SIZE);
}

bool CEFileBuffer::IsOpened() const
{
    return mIsOpened;
}

unsigned int CEFileBuffer::Reserve(int newSize)
{
    return (IsOpened() ? CESharedBuffer::ResizeBuffer(newSize, false) : NEMemory::INVALID_SIZE);
}

bool CEFileBuffer::PurgeData( void )
{
    bool result = false;
    if (IsOpened() && CanWrite())
    {
        if (IsAttachMode() == false)
        {
            RemoveReference();
            result = true;
        }
    }
    else
    {
        OUTPUT_ERR("The file is not opened or read only, cannot purge data! Close and open file again!");
    }
    return result;
}

unsigned int CEFileBuffer::SetCursorPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{
    return (IsOpened() ? CESharedBuffer::SetCursorPosition(offset, startAt) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int CEFileBuffer::GetCursorPosition( void ) const
{
    return (IsOpened() ? CESharedBuffer::GetCursorPosition() : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int CEFileBuffer::SetPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{
    return SetCursorPosition(offset, startAt);
}

unsigned int CEFileBuffer::GetPosition( void ) const
{
    return GetCursorPosition( );
}

int CEFileBuffer::NormalizeMode( int mode ) const
{
    if (CESharedBuffer::IsShared())
        mode |= CEFileBase::FO_MODE_ATTACH;
    else
        mode &= ~CEFileBase::FOB_ATTACH;
    return CEFileBase::NormalizeMode(mode);
}

unsigned int CEFileBuffer::InsertData( const unsigned char* buffer, unsigned int size, unsigned int insertAt )
{
    return (IsOpened() && CanWrite() ? CESharedBuffer::InsertData(buffer, size, insertAt) : 0);
}

unsigned int CEFileBuffer::Insert( const unsigned char* buffer, unsigned int size, unsigned int insertAt )
{
    return InsertData(buffer, size, insertAt);
}

bool CEFileBuffer::IsEmpty( void ) const
{
    return (IsOpened() ? CESharedBuffer::IsEmpty() : true);
}
