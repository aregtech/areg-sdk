#pragma once
/************************************************************************
 * \file        common\SimpleBitmap.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple 24-bit bitmap image class implementation.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/base/File.hpp"
#include "areg/base/SynchObjects.hpp"
#include "common/NELargeData.hpp"

#include <string>

/**
 * \brief   The simple bitmap object used to generate image, image blocks and RGB pixels.
 *          The current bitmap class is generated gray image, where each pixel is a combination of RBG values,
 *          i.e. 24-bits per pixel bitmap.
 *          Since the application is developed for Windows and Linux system, it is not including
 *          Windows.h file and defines the bitmap structures separately.
 */
class SimpleBitmap
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants.
//////////////////////////////////////////////////////////////////////////
    //! Bits per pixel of image.
    static constexpr uint16_t BIT_COUNT{ 24 };
    static constexpr uint32_t UNCOMPRESSED_RBG{ 0 };

public:
    /**
     * \brief   The Bitmap header info structure.
     */
    struct sBitmapInfoHeader
    {
        //!< Specifies the number of bytes required by the structure.
        uint32_t    bmiSize     { sizeof(sBitmapInfoHeader) };
        //!< Specifies the width of the bitmap, in pixels.
        int32_t     bmiWidth    { 0 };
        //!< Specifies the height of the bitmap, in pixels.
        int32_t     bmiHeight   { 0 };
        //!< Specifies the number of planes for the target device and this value is 1.
        uint16_t    bmiPlanes   { 1 };
        //!< Specifies the number of bits per pixel (bpp) and this value is 24.
        uint16_t    bmiBitCount { BIT_COUNT };
        //!< Specifies compression and this value is UNCOMPRESSED_RBG / BI_RGB to indicate that data is uncompressed.
        uint32_t    bmiCompress { UNCOMPRESSED_RBG };
        //!< Specifies the size, in bytes, of the image.
        uint32_t    bmiSizeImage{ 0 };
        //!< Specifies the horizontal resolution, in pixels per meter.
        int32_t     bmiXPPMeter { 0 };
        //!< Specifies the vertical resolution, in pixels per meter.
        int32_t     bmiYPPMeter { 0 };
        // Specifies the number of color indices and the value is 0 for RGB bitmap image.
        uint32_t    bmiClrUsed  { 0 };
        //!< Specifies the number of color indices that are considered important and the value is 0 to indicate all values are important.
        uint32_t    bmiClrNeed  { 0 };
    };

    /**
     * \brief   This structure contains information about the type, size, and layout of a file that contains a Bitmap.
     */
    struct sBitmapFileHeader
    {
        //!< The file type; must be BM.
        uint16_t    bmfType     { (static_cast<uint16_t>('B') << 8) | (static_cast<uint16_t>('M') << 0) };
        //!< The size, in bytes, of the bitmap file.
        uint32_t    bmfSize     { sizeof(sBitmapFileHeader) + sizeof(sBitmapInfoHeader) };
        //!< Must be 0.
        uint16_t    bmfReserved1{ 0 };
        //!< Must be 0.
        uint16_t    bmfReserved2{ 0 };
        //!< The offset, in bytes, from the beginning of the sBitmapFileHeader structure to the bitmap bits.
        uint32_t    bmfOffBits  { sizeof(sBitmapFileHeader) + sizeof(sBitmapInfoHeader) };
    };

    /**
     * \brief   The bitmap file header structure, bitmap header information and bits aligned to 32-bit value.
     */
    struct sBitmap
    {
        //!< The structure of bitmap file.
        sBitmapFileHeader   bmpFile;
        //!< The structure of bitmap information.
        sBitmapInfoHeader   bmpInfo;
        //!< Bitmap pixels.
        uint32_t            bmpBits[1]{0};
    };

    //!< Default generated image width in pixels.
    static constexpr uint32_t   DEFAULT_WIDTH   = 1024;
    //!< Default generated image height in pixels.
    static constexpr uint32_t   DEFAULT_HEIGHT  = 1024;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor.
//////////////////////////////////////////////////////////////////////////
public:
    //!< Default constructor, generates an image with default width and height.
    inline SimpleBitmap(void);

    /**
     * \brief   Constructor to generate image with specified width and height.
     * 
     * \param   width   The width of the generated image in pixels.
     * \param   height  The height of the generated image in pixels.
     */
    inline SimpleBitmap(uint32_t width, uint32_t height);
    //!< Destructor.
    inline ~SimpleBitmap(void);

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the bitmap image is valid.
     */
    inline bool isValid(void) const;

    /**
     * \brief   Creates and generate gray bitmap image with specified width and height in pixels.
     *
     * \param   width   The width of the generated image in pixels.
     * \param   height  The height of the generated image in pixels.
     * \return  Returns true if succeeded to create bitmap image.
     */
    inline bool createBitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Allocates the space for bitmap image with specified width and height in pixels.
     *          Does not specify gray image data.
     *
     * \param   width   The width of the generated image in pixels.
     * \param   height  The height of the generated image in pixels.
     * \return  Returns true if succeeded to create bitmap image.
     */
    inline bool allocateBitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Releases the bitmap image.
     */
    inline void release(void);

    /**
     * \brief   Returns the width in pixels of the image.
     */
    inline uint32_t getWidth(void) const;

    /**
     * \brief   Returns the height in pixels of the image.
     */
    inline uint32_t getHeight(void) const;

    /**
     * \brief   Returns number of bytes for 1 row of the image.
     *          The data is aligned to 32-bit.
     */
    inline uint32_t getRowBytes(void) const;

    /**
     * \brief   Returns the pixels starting at the specified line index. 
     *          The line index must be less than the height.
     * 
     * \param   line    The line index.
     */
    inline uint8_t* getLine(int32_t line) const;

    /**
     * \brief   Returns the pixels at specified coordinates, where the 
     *          X-coordinate is the column of the bitmap and Y-coordinate is the row (line).
     *          The top left corner of the bitmap is the (0, 0) coordinate.
     * 
     * \param   xCoord  The Y-coordinate of the bitmap, which specifies the column.
     * \param   yCoord  The X-coordinate of the bitmap, which specifies the row / line.
     */
    inline uint8_t* getPixels(int32_t xCoord, int32_t yCoord) const;

    /**
     * /brief   Saves the bitmap into the specified file.
     *          The saved bitmap is a 24-bit RGB image.
     * 
     * \param   fileName    The file path to save.
     * \return  Returns true if operation succeeded.
     */
    inline bool save(const std::string& fileName);

    /**
     * \brief   Opens the specified bitmap file and creates bitmap.
     *          The bitmap image of the specified file must be 24-bit RGB image with no compression.
     * 
     * \param   fileName    The file path to open.
     * \return  Returns true if operation succeeded.
     */
    inline bool open(const std::string& fileName);

    /**
     * \brief   Sets the specified image block in the bitmap. The image block is a chunk of an image, where it
     *          is specified width and height, the start of coordinates (normally, beginning of the line),
     *          the number of lines in the image block and the RGB pixels. If the width and height of actual bitmap
     *          differ from width and height of the image block, the bitmap image is released and the new space is
     *          allocated.
     * 
     * \param   block   The block of image data, which contains information of the width, height, coordinates of the
     *                  start of the image block, number of lines in the block and the RBG pixels.
     * \return  Returns true if operation succeeded.
     */
    inline bool setBlock( const NELargeData::ImageBlock& block );

    /**
     * \brief   Returns the block of image starting at the begin of specified row (Y-Coordinate)
     *          and number of lines..
     * 
     * \param   rowIndex    The Y-coordinate of the row index of the image.
     * \param   lines       The number of lines in the image block.
     */
    inline NELargeData::ImageBlock getBlock(uint32_t rowIndex, uint32_t lines) const;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    sBitmap *       mBitmap;    //!< The bitmap
    std::string     mFileName;  //!< The name of file, valid only if open bitmap or save.
    uint32_t        mFrameId;   //!< The ID of the frame.
    int32_t         mChannelId; //!< Image data source channel.
    
//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Allocates a space for specified image width and height so that
     *          it is ready to receive and fill image data, like image block.
     * 
     * \param   width   The width of the image to allocate.
     * \param   height  The height of the image to allocate.
     */
    inline void _allocateBitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Allocates a space for specified image width and height, and fills
     *          Bitmap data with gray RGB values.
     *
     * \param   width   The width of the image to allocate.
     * \param   height  The height of the image to allocate.
     */
    inline void _createGrayBitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Returns the begin of bitmap RBG pixel data.
     */
    inline uint8_t* _getData(void) const;

    /**
     * \brief   Returns the size in bytes of space to allocate to create a bitmap image
     *          of specified width and height. The size is aligned to 32-bit
     * 
     * \param   width   The width of the bitmap image in pixels.
     * \param   height  The height of the bitmap image in pixels.
     */
    inline uint32_t _dataSize(uint32_t width, uint32_t height) const;

    /**
     * \brief   Return the size of pixels for a single line. The size is aligned to 32-bit value.
     * 
     * \param   width   The width of a row.
     */
    inline uint32_t _rowSize(uint32_t width) const;

    /**
     * \brief   Release bitmap to free a space.
     */
    inline void _release(void);
};

//////////////////////////////////////////////////////////////////////////
// SimpleBitmap class inline methods implementation.
//////////////////////////////////////////////////////////////////////////
inline SimpleBitmap::SimpleBitmap(void)
    : mBitmap   (nullptr)
    , mFileName ( )
    , mFrameId  ( 0xFFFFFFFFu )
    , mChannelId( -1 )
{
    _createGrayBitmap(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

inline SimpleBitmap::SimpleBitmap(uint32_t width, uint32_t height)
    : mBitmap   (nullptr)
    , mFileName ( )
    , mFrameId  ( 0xFFFFFFFFu )
    , mChannelId( -1 )
{
    _createGrayBitmap(width, height);
}

inline SimpleBitmap::~SimpleBitmap(void)
{
    _release();
}

inline bool SimpleBitmap::isValid(void) const
{
    return (mBitmap != nullptr);
}

inline bool SimpleBitmap::createBitmap(uint32_t width, uint32_t height)
{
    _release();
    _createGrayBitmap(width, height);
    return (mBitmap != nullptr);
}

inline bool SimpleBitmap::allocateBitmap(uint32_t width, uint32_t height)
{
    bool result{ true };
    if ((width != getWidth()) || (height != getHeight()))
    {
        _release();
        _allocateBitmap(width, height);
        result = isValid();
    }

    return result;
}

inline void SimpleBitmap::release(void)
{
    _release();
}

inline uint32_t SimpleBitmap::getWidth(void) const
{
    return static_cast<uint32_t>(mBitmap != nullptr ? mBitmap->bmpInfo.bmiWidth : 0);
}

inline uint32_t SimpleBitmap::getHeight(void) const
{
    return static_cast<uint32_t>(mBitmap != nullptr ? mBitmap->bmpInfo.bmiHeight : 0);
}

inline uint32_t SimpleBitmap::getRowBytes(void) const
{
    return static_cast<uint32_t>(mBitmap != nullptr ? _rowSize(mBitmap->bmpInfo.bmiWidth) : 0);
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

inline bool SimpleBitmap::setBlock( const NELargeData::ImageBlock & block )
{
    if (block.isEmpty())
        return false;

    const NELargeData::sImageBlock* imgBlock = block.getBlock();

    if ((mChannelId != -1) && (mChannelId != static_cast<int32_t>(imgBlock->channelId)))
        return false; // wrong source

    if ( (getWidth( ) != imgBlock->frameWidth) || (getHeight( ) != imgBlock->frameHeight) )
    {
        _release( );
        _allocateBitmap( imgBlock->frameWidth, imgBlock->frameHeight );
    }

    uint8_t * pixels = getPixels( imgBlock->imageData.imgStartPos.coordX, imgBlock->imageData.imgStartPos.coordY );
    ::memcpy( pixels, reinterpret_cast<const uint8_t *>(imgBlock->imageData.imgRGB), imgBlock->imageData.imgRBGLen );
    mChannelId  = imgBlock->channelId;
    mFrameId    = imgBlock->frameSeqId;

    return true;
}

inline NELargeData::ImageBlock SimpleBitmap::getBlock(uint32_t rowIndex, uint32_t lines) const
{
    NELargeData::ImageBlock result;

    if ((mBitmap == nullptr) || (mBitmap->bmpInfo.bmiSize == 0))
        return result;

    uint32_t width = getWidth();
    uint32_t height = getHeight();
    uint32_t remain = height - rowIndex;
    lines = MACRO_MIN(lines, remain);
    uint32_t sizePixels = _dataSize(width, lines);
    uint32_t sizeBlock = sizePixels + sizeof(NELargeData::sImageBlock);
    NELargeData::sImageBlock* block = result.initialize(sizeBlock);

    block->blockSize = sizeBlock;
    block->channelId = 0;
    block->frameSeqId = 0;
    block->frameHeight = height;
    block->frameWidth = width;
    block->imageData.imgHeight = lines;
    block->imageData.imgRBGLen = sizePixels;
    block->imageData.imgStartPos = NELargeData::sCoord{ 0, rowIndex };
    block->imageData.imgWidth = width;

    ::memcpy(reinterpret_cast<uint8_t*>(block->imageData.imgRGB), getPixels(0, rowIndex), sizePixels);

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

inline void SimpleBitmap::_createGrayBitmap(uint32_t width, uint32_t height)
{
    _allocateBitmap(width, height);
    if (mBitmap != nullptr)
    {
        uint8_t grey = 0;
        uint32_t row = _rowSize(width);
        uint8_t* pix = reinterpret_cast<uint8_t*>(&mBitmap->bmpBits[0]);
        for (uint32_t i = 0; i < height; ++i)
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
    return ((((width * BIT_COUNT) + 31) / 32) * 4);
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
