/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/FileBuffer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Memory File object
 *
 ************************************************************************/
#include "areg/base/FileBuffer.hpp"
#include "areg/base/WideString.hpp"

//////////////////////////////////////////////////////////////////////////
// FileBuffer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
FileBuffer::FileBuffer( unsigned int mode       /*= (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY)*/
                      , const char * name       /*= nullptr*/
                      , unsigned int blockSize  /*= BLOCK_SIZE*/)
    : FileBase      ( )

    , mSharedBuffer (blockSize)
    , mIsOpened     (false)
{
    mFileMode = mode & (~ static_cast<unsigned int>(FileBase::FOB_ATTACH) );  // FOB_MODE_ATTACH
    mFileName = name;
}

FileBuffer::FileBuffer(SharedBuffer & sharedBuffer, const char* name /*= nullptr*/)
    : FileBase      ( )

    , mSharedBuffer (sharedBuffer)
    , mIsOpened     (false)
{
    mFileMode = FileBase::FO_MODE_ATTACH;
    mFileName = name;
}

FileBuffer::FileBuffer(const SharedBuffer & sharedBuffer, const char* name /*= nullptr*/)
    : FileBase      ( )

    , mSharedBuffer (sharedBuffer)
    , mIsOpened     ( sharedBuffer.isValid() )
{
    mFileMode = FileBase::FO_MODE_ATTACH | FileBase::FO_MODE_READ;
    mFileName = name;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

inline void FileBuffer::_setName(const char* name)
{
    mFileName = name;
    FileBase::normalizeName(mFileName);
}

bool FileBuffer::open( void )
{
    if ((isOpened() == false) && (mFileMode != FileBase::FO_MODE_INVALID))
    {
        mFileMode = normalizeMode(mFileMode);
        if (isAttachMode() == false)
        {
            mSharedBuffer.reserve(mSharedBuffer.getBlockSize(), false);
        }
        else
        {
            ASSERT(isValid());
        }

        mIsOpened   = isValid();
    }

    return mIsOpened;
}

bool FileBuffer::open(const String& fileName, unsigned int mode)
{
    bool result = false;
    if (isOpened() == false)
    {
        if (isValid() == false)
        {
            mode &= ~FileBase::FOB_ATTACH;
        }

        mFileMode = mode != FileBase::FO_MODE_INVALID ? mode : mFileMode;
        if (mFileMode != FileBase::FO_MODE_INVALID)
        {
            _setName(fileName);
            result = open();
        }
    }

    return result;
}

void FileBuffer::close( void )
{
    // keep file name and mode that it can be again reopened.
    // remove 'attached' flag, since the buffer is not valid any mode
    mIsOpened   = false;
    mFileMode  &= ~FileBase::FOB_ATTACH;
    mSharedBuffer.invalidate();
}

unsigned int FileBuffer::read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (isOpened() && canRead())
    {
        result = mSharedBuffer.read(buffer, size);
    }
    
    return result;
}

unsigned int FileBuffer::getSizeReadable( void ) const
{
    return (isOpened() ? mSharedBuffer.getSizeReadable() : 0);
}

unsigned int FileBuffer::write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;

    if (isOpened() && canWrite())
    {
        result = mSharedBuffer.write(buffer, size);
    }

    return result;
}

unsigned int FileBuffer::getSizeWritable( void ) const
{
    return (isOpened() ? mSharedBuffer.getSizeWritable() : 0);
}

bool FileBuffer::remove( void )
{
    mSharedBuffer.invalidate();

    mFileName   = String::getEmptyString();
    mFileMode   = FileBase::FO_MODE_INVALID;
    mIsOpened   = false;

    return true;
}

unsigned int FileBuffer::getLength( void ) const
{
    return (isOpened() ? mSharedBuffer.getSizeUsed() : NEMemory::INVALID_SIZE);
}

bool FileBuffer::isOpened() const
{
    return mIsOpened;
}

unsigned int FileBuffer::reserve(unsigned int newSize)
{
    return (isOpened() ? mSharedBuffer.reserve(newSize, false) : NEMemory::INVALID_SIZE);
}

bool FileBuffer::truncate( void )
{
    bool result = false;
    if (isOpened() && canWrite())
    {
        if (isAttachMode() == false)
        {
            mSharedBuffer.invalidate();
            result = true;
        }
    }

    return result;
}

unsigned int FileBuffer::setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{
    return (isOpened() ? mSharedBuffer.setPosition(offset, startAt) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int FileBuffer::getPosition( void ) const
{
    return (isOpened() ? mSharedBuffer.getPosition() : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int FileBuffer::normalizeMode( unsigned int mode ) const
{
    if (mSharedBuffer.isShared())
    {
        mode |= FileBase::FO_MODE_ATTACH;
    }
    else
    {
        mode &= ~FileBase::FOB_ATTACH;
    }

    return FileBase::normalizeMode(mode);
}

unsigned int FileBuffer::insertAt( const unsigned char* buffer, unsigned int size, unsigned int atPos )
{
    return (isOpened() && canWrite() ? mSharedBuffer.insertAt(buffer, size, atPos) : 0);
}

unsigned int FileBuffer::read(IEByteBuffer & buffer) const
{
    return FileBase::read(buffer);
}

unsigned int FileBuffer::read(String & ascii) const
{
    return FileBase::read(ascii);
}

unsigned int FileBuffer::read(WideString & wide) const
{
    return FileBase::read(wide);
}

unsigned int FileBuffer::write(const IEByteBuffer & buffer)
{
    return FileBase::write(buffer);
}

unsigned int FileBuffer::write(const String & ascii)
{
    return FileBase::write(ascii);
}

unsigned int FileBuffer::write(const WideString & wide)
{
    return FileBase::write(wide);
}
