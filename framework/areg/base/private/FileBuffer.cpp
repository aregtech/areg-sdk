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
    , mIsOpened     ( sharedBuffer.is_valid() )
{
    mFileMode = static_cast<uint32_t>(FileBase::OpenMode::Attach) | static_cast<uint32_t>(FileBase::OpenMode::Read);
    mFileName = name;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

inline void FileBuffer::_set_name(const char* name)
{
    mFileName = name;
    FileBase::normalize_name(mFileName);
}

bool FileBuffer::open()
{
    if ((is_opened() == false) && (mFileMode != static_cast<uint32_t>(FileBase::OpenMode::Invalid)))
    {
        mFileMode = normalize_mode(mFileMode);
        if (is_attach_mode() == false)
        {
            mSharedBuffer.reserve(mSharedBuffer.block_size(), false);
        }
        else
        {
            ASSERT(is_valid());
        }

        mIsOpened   = is_valid();
    }

    return mIsOpened;
}

bool FileBuffer::open(const String& fileName, uint32_t mode)
{
    bool result = false;
    if (is_opened() == false)
    {
        if (is_valid() == false)
        {
            mode &= ~static_cast<uint32_t>(FileBase::OpenMode::Attach);
        }

        mFileMode = mode != static_cast<uint32_t>(FileBase::OpenMode::Invalid) ? mode : mFileMode;
        if (mFileMode != static_cast<uint32_t>(FileBase::OpenMode::Invalid))
        {
            _set_name(fileName);
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
    if (is_opened() && can_read())
    {
        result = mSharedBuffer.read(buffer, size);
    }
    
    return result;
}

uint32_t FileBuffer::size_readable() const
{
    return (is_opened() ? mSharedBuffer.size_readable() : 0);
}

uint32_t FileBuffer::write(const uint8_t* buffer, uint32_t size)
{
    uint32_t result = 0;

    if (is_opened() && can_write())
    {
        result = mSharedBuffer.write(buffer, size);
    }

    return result;
}

uint32_t FileBuffer::size_writable() const
{
    return (is_opened() ? mSharedBuffer.size_writable() : 0);
}

bool FileBuffer::remove()
{
    mSharedBuffer.invalidate();

    mFileName   = String::empty_string();
    mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
    mIsOpened   = false;

    return true;
}

uint32_t FileBuffer::length() const
{
    return (is_opened() ? mSharedBuffer.size_used() : NEMemory::INVALID_SIZE);
}

bool FileBuffer::is_opened() const
{
    return mIsOpened;
}

uint32_t FileBuffer::reserve(uint32_t newSize)
{
    return (is_opened() ? mSharedBuffer.reserve(newSize, false) : NEMemory::INVALID_SIZE);
}

bool FileBuffer::truncate()
{
    bool result = false;
    if (is_opened() && can_write())
    {
        if (is_attach_mode() == false)
        {
            mSharedBuffer.invalidate();
            result = true;
        }
    }

    return result;
}

uint32_t FileBuffer::set_position( int32_t offset, Cursor::SeekOrigin startAt ) const
{
    return (is_opened() ? mSharedBuffer.set_position(offset, startAt) : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t FileBuffer::position() const
{
    return (is_opened() ? mSharedBuffer.position() : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t FileBuffer::normalize_mode( uint32_t mode ) const
{
    if (mSharedBuffer.is_shared())
    {
        mode |= static_cast<uint32_t>(FileBase::OpenMode::Attach);
    }
    else
    {
        mode &= ~static_cast<uint32_t>(FileBase::OpenMode::Attach);
    }

    return FileBase::normalize_mode(mode);
}

uint32_t FileBuffer::insert_at( const uint8_t* buffer, uint32_t size, uint32_t atPos )
{
    return (is_opened() && can_write() ? mSharedBuffer.insert_at(buffer, size, atPos) : 0);
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
