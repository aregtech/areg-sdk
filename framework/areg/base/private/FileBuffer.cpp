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
FileBuffer::FileBuffer( uint32_t mode       /*= (static_cast<uint32_t>(FileBase::OpenMode::Write) | static_cast<uint32_t>(FileBase::OpenMode::Binary))*/
                      , const char * name       /*= nullptr*/
                      , uint32_t blockSize  /*= BLOCK_SIZE*/)
    : FileBase      ( )

    , mSharedBuffer (blockSize)
    , mIsOpened     (false)
{
    mFileMode = mode & (~static_cast<uint32_t>(FileBase::OpenMode::Attach) );  // FOB_MODE_ATTACH
    mFileName = name;
}

FileBuffer::FileBuffer(SharedBuffer & sharedBuffer, const char* name /*= nullptr*/)
    : FileBase      ( )

    , mSharedBuffer (sharedBuffer)
    , mIsOpened     (false)
{
    mFileMode = static_cast<uint32_t>(FileBase::OpenMode::Attach);
    mFileName = name;
}

FileBuffer::FileBuffer(const SharedBuffer & sharedBuffer, const char* name /*= nullptr*/)
    : FileBase      ( )

    , mSharedBuffer (sharedBuffer)
    , mIsOpened     ( sharedBuffer.isValid() )
{
    mFileMode = static_cast<uint32_t>(FileBase::OpenMode::Attach) | static_cast<uint32_t>(FileBase::OpenMode::Read);
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

bool FileBuffer::open()
{
    if ((isOpened() == false) && (mFileMode != static_cast<uint32_t>(FileBase::OpenMode::Invalid)))
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

bool FileBuffer::open(const String& fileName, uint32_t mode)
{
    bool result = false;
    if (isOpened() == false)
    {
        if (isValid() == false)
        {
            mode &= ~static_cast<uint32_t>(FileBase::OpenMode::Attach);
        }

        mFileMode = mode != static_cast<uint32_t>(FileBase::OpenMode::Invalid) ? mode : mFileMode;
        if (mFileMode != static_cast<uint32_t>(FileBase::OpenMode::Invalid))
        {
            _setName(fileName);
            result = open();
        }
    }

    return result;
}

void FileBuffer::close()
{
    // keep file name and mode that it can be again reopened.
    // remove 'attached' flag, since the buffer is not valid any mode
    mIsOpened   = false;
    mFileMode  &= ~static_cast<uint32_t>(FileBase::OpenMode::Attach);
    mSharedBuffer.invalidate();
}

uint32_t FileBuffer::read(uint8_t* buffer, uint32_t size) const
{
    uint32_t result = 0;
    if (isOpened() && canRead())
    {
        result = mSharedBuffer.read(buffer, size);
    }
    
    return result;
}

uint32_t FileBuffer::getSizeReadable() const
{
    return (isOpened() ? mSharedBuffer.getSizeReadable() : 0);
}

uint32_t FileBuffer::write(const uint8_t* buffer, uint32_t size)
{
    uint32_t result = 0;

    if (isOpened() && canWrite())
    {
        result = mSharedBuffer.write(buffer, size);
    }

    return result;
}

uint32_t FileBuffer::getSizeWritable() const
{
    return (isOpened() ? mSharedBuffer.getSizeWritable() : 0);
}

bool FileBuffer::remove()
{
    mSharedBuffer.invalidate();

    mFileName   = String::getEmptyString();
    mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
    mIsOpened   = false;

    return true;
}

uint32_t FileBuffer::getLength() const
{
    return (isOpened() ? mSharedBuffer.getSizeUsed() : areg::INVALID_SIZE);
}

bool FileBuffer::isOpened() const
{
    return mIsOpened;
}

uint32_t FileBuffer::reserve(uint32_t newSize)
{
    return (isOpened() ? mSharedBuffer.reserve(newSize, false) : areg::INVALID_SIZE);
}

bool FileBuffer::truncate()
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

uint32_t FileBuffer::setPosition( int32_t offset, Cursor::SeekOrigin startAt ) const
{
    return (isOpened() ? mSharedBuffer.setPosition(offset, startAt) : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t FileBuffer::getPosition() const
{
    return (isOpened() ? mSharedBuffer.getPosition() : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t FileBuffer::normalizeMode( uint32_t mode ) const
{
    if (mSharedBuffer.isShared())
    {
        mode |= static_cast<uint32_t>(FileBase::OpenMode::Attach);
    }
    else
    {
        mode &= ~static_cast<uint32_t>(FileBase::OpenMode::Attach);
    }

    return FileBase::normalizeMode(mode);
}

uint32_t FileBuffer::insertAt( const uint8_t* buffer, uint32_t size, uint32_t atPos )
{
    return (isOpened() && canWrite() ? mSharedBuffer.insertAt(buffer, size, atPos) : 0);
}

uint32_t FileBuffer::read(ByteBuffer & buffer) const
{
    return FileBase::read(buffer);
}

uint32_t FileBuffer::read(String & ascii) const
{
    return FileBase::read(ascii);
}

uint32_t FileBuffer::read(WideString & wide) const
{
    return FileBase::read(wide);
}

uint32_t FileBuffer::write(const ByteBuffer & buffer)
{
    return FileBase::write(buffer);
}

uint32_t FileBuffer::write(const String & ascii)
{
    return FileBase::write(ascii);
}

uint32_t FileBuffer::write(const WideString & wide)
{
    return FileBase::write(wide);
}
