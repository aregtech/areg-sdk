/************************************************************************
 * \file        areg/base/private/FileBuffer.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Memory File object
 *
 ************************************************************************/
#include "areg/base/FileBuffer.hpp"
#include "areg/base/WideString.hpp"

//////////////////////////////////////////////////////////////////////////
// FileBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const unsigned int FileBuffer::BLOCK_SIZE  = (4 * NEMemory::BLOCK_SIZE);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
FileBuffer::FileBuffer( int         mode       /*= (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY)*/
                      , const char* name       /*= NULL*/
                      , int         blockSize  /*= BLOCK_SIZE*/)
    : FileBase    ( )
    , SharedBuffer(blockSize)

    , mIsOpened     (false)
{
    mFileMode = mode & (~FileBase::FOB_ATTACH);  // FOB_MODE_ATTACH
    mFileName = name;
}

FileBuffer::FileBuffer(NEMemory::sByteBuffer & buffer, const char* name /*= NULL*/)
    : FileBase        ( )
    , SharedBuffer    (buffer)

    , mIsOpened         (false)
{
    mFileMode  = (&buffer != &NEMemory::InvalidBuffer) ? FileBase::FO_MODE_ATTACH : FileBase::FO_MODE_INVALID;
    mFileName  = name;
}

FileBuffer::FileBuffer(SharedBuffer & sharedBuffer, const char* name /*= NULL*/)
    : FileBase        ( )
    , SharedBuffer    (sharedBuffer.getByteBuffer())

    , mIsOpened         (false)
{
    mFileMode = sharedBuffer.isValid() ? FileBase::FO_MODE_ATTACH : FileBase::FO_MODE_INVALID;
    mFileName = name;
}

FileBuffer::FileBuffer(const SharedBuffer & sharedBuffer, const char* name /*= NULL*/)
    : FileBase        ( )
    , SharedBuffer    (sharedBuffer)

    , mIsOpened         ( sharedBuffer.isValid() )
{
    mFileMode = FileBase::FO_MODE_ATTACH | FileBase::FO_MODE_READ;
    mFileName = name;
}

FileBuffer::~FileBuffer( void )
{
    mIsOpened   = false;
    mFileMode   = FileBase::FO_MODE_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

inline void FileBuffer::_setName(const char* name)
{
    mFileName = name;
}

bool FileBuffer::open( void )
{
    if ((isOpened() == false) && (mFileMode != FileBase::FO_MODE_INVALID))
    {
        mFileMode = normalizeMode(mFileMode);
        if (isAttachMode() == false)
        {
            resize(mBlockSize, false);
        }
        else
        {
            ASSERT(isValid());
        }

        mIsOpened   = isValid();
    }
    else
    {
        OUTPUT_ERR("File is already opened. Close file.");
    }
    return mIsOpened;
}

bool FileBuffer::open(const char* fileName, unsigned int mode)
{
    bool result = false;
    if (isOpened() == false)
    {
        if (isValid() == false)
            mode &= ~FileBase::FOB_ATTACH;

        mFileMode = mode != FileBase::FO_MODE_INVALID ? mode : mFileMode;
        if (mFileMode != FileBase::FO_MODE_INVALID)
        {
            _setName(fileName);
            result = open();
        }
    }
    else
    {
        OUTPUT_ERR("File is already opened. Close file.");
    }
    return result;
}

void FileBuffer::close( void )
{
    if (isValid())
    {
        if (isForceDelete() || isTemporary() )
            removeReference();
        else if ((isAttachMode() == false) && (isDetachMode() == false))
            removeReference();
        else if (getSizeUsed() == 0)
            removeReference();
        // else, do nothing, either buffer is attached or detach mode is set.
    }

    // keep file name and mode that it can be again reopened.
    // remove 'attached' flag, since the buffer is not valid any mode
    mIsOpened   = false;
    mByteBuffer = &NEMemory::InvalidBuffer;
    mFileMode  &= ~FileBase::FOB_ATTACH;
    BufferPosition::invalidate();
}

unsigned int FileBuffer::read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (isOpened() && canRead())
    {
        result = SharedBuffer::read(buffer, size);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", isOpened() ? "true" : "false", canRead() ? "true" : "false");
    }
    
    return result;
}

unsigned int FileBuffer::read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    
    if (isOpened() && canRead())
    {
        result = SharedBuffer::read(buffer);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", isOpened() ? "true" : "false", canRead() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::read( String & asciiString ) const
{
    unsigned int result = 0;

    if (isOpened() && canRead())
    {
        result = SharedBuffer::read(asciiString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", isOpened() ? "true" : "false", canRead() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::read( WideString & wideString ) const
{
    unsigned int result = 0;

    if (isOpened() && canRead())
    {
        result = SharedBuffer::read(wideString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot read data [ %s ], check open mode.", isOpened() ? "true" : "false", canRead() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::getSizeReadable( void ) const
{
    return (isOpened() ? SharedBuffer::getSizeReadable() : 0);
}

unsigned int FileBuffer::write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;

    if (isOpened() && canWrite())
    {
        result = SharedBuffer::write(buffer, size);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", isOpened() ? "true" : "false", canWrite() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;

    if (isOpened() && canWrite())
    {
        result = SharedBuffer::write(buffer);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", isOpened() ? "true" : "false", canWrite() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::write( const String & asciiString )
{
    unsigned int result = 0;

    if (isOpened() && canWrite())
    {
        result = SharedBuffer::write(asciiString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", isOpened() ? "true" : "false", canWrite() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::write( const WideString & wideString )
{
    unsigned int result = 0;

    if (isOpened() && canWrite())
    {
        result = SharedBuffer::write(wideString);
    }
    else
    {
        OUTPUT_ERR("Either file is not opened [ %s ] or cannot write data [ %s ], check open mode.", isOpened() ? "true" : "false", canWrite() ? "true" : "false");
    }

    return result;
}

unsigned int FileBuffer::getSizeWritable( void ) const
{
    return (isOpened() ? SharedBuffer::getSizeWritable() : 0);
}

bool FileBuffer::remove( void )
{
    removeReference();

    mFileName   = String::EmptyString;
    mFileMode   = FileBase::FO_MODE_INVALID;
    mIsOpened   = false;

    return true;
}

unsigned int FileBuffer::getLength( void ) const
{
    return (isOpened() ? SharedBuffer::getSizeUsed() : NEMemory::INVALID_SIZE);
}

bool FileBuffer::isOpened() const
{
    return mIsOpened;
}

unsigned int FileBuffer::reserve(int newSize)
{
    return (isOpened() ? SharedBuffer::resize(newSize, false) : NEMemory::INVALID_SIZE);
}

bool FileBuffer::truncate( void )
{
    bool result = false;
    if (isOpened() && canWrite())
    {
        if (isAttachMode() == false)
        {
            removeReference();
            result = true;
        }
    }
    else
    {
        OUTPUT_ERR("The file is not opened or read only, cannot purge data! Close and open file again!");
    }
    return result;
}

unsigned int FileBuffer::setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{
    return (isOpened() ? SharedBuffer::setPosition(offset, startAt) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int FileBuffer::getPosition( void ) const
{
    return (isOpened() ? SharedBuffer::getPosition() : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int FileBuffer::normalizeMode( unsigned int mode ) const
{
    if (SharedBuffer::isShared())
        mode |= FileBase::FO_MODE_ATTACH;
    else
        mode &= ~FileBase::FOB_ATTACH;
    return FileBase::normalizeMode(mode);
}

unsigned int FileBuffer::insertAt( const unsigned char* buffer, unsigned int size, unsigned int insertAt )
{
    return (isOpened() && canWrite() ? SharedBuffer::insertAt(buffer, size, insertAt) : 0);
}

bool FileBuffer::isEmpty( void ) const
{
    return (isOpened() ? SharedBuffer::isEmpty() : true);
}
