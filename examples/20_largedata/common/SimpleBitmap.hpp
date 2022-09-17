#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/base/File.hpp"

#include <string>

class SimpleBitmap
{
public:
    struct sBitmapInfoHeader
    {
        uint32_t    bmiSize     { sizeof(sBitmapInfoHeader) };
        int32_t     bmiWidth    { 0 };
        int32_t     bmiHeight   { 0 };
        uint16_t    bmiPlanes   { 1 };
        uint16_t    bmiBitCount { 24 };
        uint32_t    bmiCompress { 0 };
        uint32_t    bmiSizeImage{ 0 };
        int32_t     bmiXPPMeter { 0 };
        int32_t     bmiYPPMeter { 0 };
        uint32_t    bmiClrUsed  { 0 };
        uint32_t    bmiClrNeed  { 0 };
    };

    struct sBitmapFileHeader
    {
        uint16_t    bmfType     { (static_cast<uint16_t>('B') << 8) | (static_cast<uint16_t>('M') << 0) };
        uint32_t    bmfSize     { sizeof(sBitmapFileHeader) + sizeof(sBitmapInfoHeader) };
        uint16_t    bmfReserved1{ 0 };
        uint16_t    bmfReserved2{ 0 };
        uint32_t    bmfOffBits  { sizeof(sBitmapFileHeader) + sizeof(sBitmapInfoHeader) };
    };

    struct sBitmap
    {
        sBitmapFileHeader   bmpFile;
        sBitmapInfoHeader   bmpInfo;
        uint32_t            bmpBits[1];
    };

public:
    static constexpr uint32_t   DEFAULT_WIDTH   = 1024;
    static constexpr uint32_t   DEFAULT_HEIGHT  = 1024;

public:
    inline SimpleBitmap(void);
    inline SimpleBitmap(uint32_t width, uint32_t height);
    inline ~SimpleBitmap(void);

public:

    inline bool createBitmap(uint32_t width, uint32_t height);

    inline int32_t getWidth(void) const;

    inline int32_t getHeight(void) const;

    inline int32_t getRowBytes(void) const;

    inline uint8_t* getLine(int32_t line) const;

    inline uint8_t* getPixels(int32_t xCoord, int32_t coordY) const;

    inline bool save(const std::string& fileName);

    inline bool open(const std::string& fileName);

private:
    sBitmap *   mBitmap;
    std::string mFileName;
    
private:
    inline void _allocateBitmap(uint32_t width, uint32_t height);

    inline void _createGreyBitmap(uint32_t width, uint32_t height);

    inline uint8_t* _getData(void) const;

    inline uint32_t _dataSize(uint32_t width, uint32_t height) const;

    inline uint32_t _rowSize(uint32_t width) const;

    inline void _release(void);
};

inline SimpleBitmap::SimpleBitmap(void)
    : mBitmap   (nullptr)
    , mFileName ( )
{
    _createGreyBitmap(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

inline SimpleBitmap::SimpleBitmap(uint32_t width, uint32_t height)
    : mBitmap   (nullptr)
    , mFileName ( )
{
    _createGreyBitmap(width, height);
}

inline SimpleBitmap::~SimpleBitmap(void)
{
    _release();
}

inline bool SimpleBitmap::createBitmap(uint32_t width, uint32_t height)
{
    _release();
    _createGreyBitmap(width, height);
    return (mBitmap != nullptr);
}

inline int32_t SimpleBitmap::getWidth(void) const
{
    return (mBitmap != nullptr ? mBitmap->bmpInfo.bmiWidth : 0);
}

inline int32_t SimpleBitmap::getHeight(void) const
{
    return (mBitmap != nullptr ? mBitmap->bmpInfo.bmiHeight : 0);
}

inline int32_t SimpleBitmap::getRowBytes(void) const
{
    return (mBitmap != nullptr ? _rowSize(mBitmap->bmpInfo.bmiWidth) : 0);
}

inline uint8_t* SimpleBitmap::getLine(int32_t line) const
{
    uint8_t* result = nullptr;
    if (mBitmap != nullptr)
    {
        uint32_t offset = static_cast<uint32_t>(line) * _rowSize(mBitmap->bmpInfo.bmiWidth);
        result = _getData() + offset;
    }

    return result;
}

inline uint8_t* SimpleBitmap::getPixels(int32_t xCoord, int32_t yCoord) const
{
    uint8_t* result = nullptr;
    if ((mBitmap != nullptr) && (xCoord >= 0) && (yCoord >= 0))
    {
        uint32_t offset = static_cast<uint32_t>(yCoord) * _rowSize(mBitmap->bmpInfo.bmiWidth);
        uint32_t col    = static_cast<uint32_t>(xCoord) * (mBitmap->bmpInfo.bmiBitCount / 8);
        result = _getData() + offset + col;
    }

    return result;
}

inline bool SimpleBitmap::save(const std::string& fileName)
{
    bool result{ false };
    if (mBitmap != nullptr)
    {
        File file(fileName, FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY);
        if (file.open() && (file.write(reinterpret_cast<uint8_t*>(mBitmap), mBitmap->bmpFile.bmfSize) == mBitmap->bmpFile.bmfSize))
        {
            result = true;
            mFileName = fileName;
        }
    }

    return result;
}

inline bool SimpleBitmap::open(const std::string& fileName)
{
    bool result{ false };

    _release();
    File file(fileName, FileBase::FO_MODE_READ | FileBase::FO_MODE_BINARY);
    if (file.open())
    {
        sBitmapFileHeader bmf;
        if (file.read(reinterpret_cast<uint8_t*>(&bmf), sizeof(sBitmapFileHeader)) == sizeof(sBitmapFileHeader))
        {
            uint32_t size   = bmf.bmfSize;
            uint8_t* buffer = DEBUG_NEW uint8_t[size];
            file.moveToBegin();
            if (file.read(buffer, size) == size)
            {
                mBitmap = reinterpret_cast<sBitmap*>(buffer);
                result = true;
            }
            else
            {
                delete[] buffer;
            }
        }
    }

    return result;
}

inline void SimpleBitmap::_allocateBitmap(uint32_t width, uint32_t height)
{
    uint32_t dataSize = _dataSize(width, height);
    uint32_t fileSize = dataSize + sizeof(sBitmap);
    uint8_t* buffer = DEBUG_NEW uint8_t[fileSize];
    if (buffer != nullptr)
    {
        mBitmap = new(buffer)sBitmap;
        mBitmap->bmpFile.bmfSize    = fileSize;
        mBitmap->bmpInfo.bmiSize    = dataSize;
        mBitmap->bmpInfo.bmiWidth   = static_cast<int32_t>(width);
        mBitmap->bmpInfo.bmiHeight  = static_cast<int32_t>(height);
    }
}

inline void SimpleBitmap::_createGreyBitmap(uint32_t width, uint32_t height)
{
    _allocateBitmap(width, height);
    if (mBitmap != nullptr)
    {
        uint8_t grey = 0;
        uint32_t row = _rowSize(width);
        uint8_t* pix = reinterpret_cast<uint8_t*>(&mBitmap->bmpBits[0]);
        for (int32_t i = 0; i < height; ++i)
        {
            uint32_t offset = static_cast<uint32_t>(i * row);
            uint8_t* dst = pix + offset;
            ::memset(dst, grey++, row);
        }
    }
}

inline uint8_t* SimpleBitmap::_getData(void) const
{
    return (mBitmap != nullptr ? reinterpret_cast<uint8_t *>(&mBitmap->bmpBits[0]) : nullptr);
}

inline uint32_t SimpleBitmap::_rowSize(uint32_t width) const
{
    // return ((((width * 24) + 31) & ~31) >> 3);
    return ((((width * 24) + 31) / 32) * 4);
}

inline uint32_t SimpleBitmap::_dataSize(uint32_t width, uint32_t height) const
{
    return (_rowSize(width) * height);
}

inline void SimpleBitmap::_release(void)
{
    uint8_t* data = reinterpret_cast<uint8_t*>(mBitmap);
    delete[] data;
    mBitmap = nullptr;
}
