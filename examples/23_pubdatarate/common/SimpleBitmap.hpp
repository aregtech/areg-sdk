#pragma once
/************************************************************************
 * \file        common\SimpleBitmap.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple 24-bit bitmap image class implementation.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/File.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "common/LargeDataDefs.hpp"

#include <algorithm>
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
    struct BitmapInfoHeader
    {
        //!< Specifies the number of bytes required by the structure.
        uint32_t    bmiSize     { sizeof(BitmapInfoHeader) };
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
    struct BitmapFileHeader
    {
        //!< The file type; must be BM.
        uint16_t    bmfType     { (static_cast<uint16_t>('B') << 8) | (static_cast<uint16_t>('M') << 0) };
        //!< The size, in bytes, of the bitmap file.
        uint32_t    bmfSize     { sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) };
        //!< Must be 0.
        uint16_t    bmfReserved1{ 0 };
        //!< Must be 0.
        uint16_t    bmfReserved2{ 0 };
        //!< The offset, in bytes, from the beginning of the BitmapFileHeader structure to the bitmap bits.
        uint32_t    bmfOffBits  { sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) };
    };

    /**
     * \brief   The bitmap file header structure, bitmap header information and bits aligned to 32-bit value.
     */
    struct Bitmap
    {
        //!< The structure of bitmap file.
        BitmapFileHeader   bmpFile;
        //!< The structure of bitmap information.
        BitmapInfoHeader   bmpInfo;
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
    inline SimpleBitmap();

    /**
     * \brief   Constructor to generate image with specified width and height.
     * 
     * \param   width   The width of the generated image in pixels.
     * \param   height  The height of the generated image in pixels.
     */
    inline SimpleBitmap(uint32_t width, uint32_t height);
    //!< Destructor.
    inline ~SimpleBitmap();

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the bitmap image is valid.
     */
    inline bool is_valid() const;

    /**
     * \brief   Creates and generate gray bitmap image with specified width and height in pixels.
     *
     * \param   width   The width of the generated image in pixels.
     * \param   height  The height of the generated image in pixels.
     * \return  Returns true if succeeded to create bitmap image.
     */
    inline bool create_bitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Allocates the space for bitmap image with specified width and height in pixels.
     *          Does not specify gray image data.
     *
     * \param   pix_width   The width of the generated image in pixels.
     * \param   pix_height  The height of the generated image in pixels.
     * \return  Returns true if succeeded to create bitmap image.
     */
    inline bool allocate_bitmap(uint32_t pix_width, uint32_t pix_height);

    /**
     * \brief   Releases the bitmap image.
     */
    inline void release();

    /**
     * \brief   Returns the width in pixels of the image.
     */
    inline uint32_t width() const;

    /**
     * \brief   Returns the height in pixels of the image.
     */
    inline uint32_t height() const;

    /**
     * \brief   Returns number of bytes for 1 row of the image.
     *          The data is aligned to 32-bit.
     */
    inline uint32_t row_bytes() const;

    /**
     * \brief   Returns the pixels starting at the specified line index. 
     *          The line index must be less than the height.
     * 
     * \param   line    The line index.
     */
    inline uint8_t* line(uint32_t line) const;

    /**
     * \brief   Returns the pixels at specified coordinates, where the 
     *          X-coordinate is the column of the bitmap and Y-coordinate is the row (line).
     *          The top left corner of the bitmap is the (0, 0) coordinate.
     * 
     * \param   xCoord  The Y-coordinate of the bitmap, which specifies the column.
     * \param   yCoord  The X-coordinate of the bitmap, which specifies the row / line.
     */
    inline uint8_t* pixels(uint32_t xCoord, uint32_t yCoord) const;

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
    inline bool set_block( const LargeData::ImageBlock& block );

    /**
     * \brief   Returns the block of image starting at the begin of specified row (Y-Coordinate)
     *          and number of lines..
     * 
     * \param   rowIndex    The Y-coordinate of the row index of the image.
     * \param   lines       The number of lines in the image block.
     */
    inline uint32_t block(LargeData::ImageBlock & result, uint32_t rowIndex, uint32_t lines) const;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:
    Bitmap *        mBitmap;    //!< The bitmap
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
    inline void _allocate_bitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Release bitmap to free a space.
     */
    inline void _release();

    /**
     * \brief   Allocates a space for specified image width and height, and fills
     *          Bitmap data with gray RGB values.
     *
     * \param   width   The width of the image to allocate.
     * \param   height  The height of the image to allocate.
     */
    inline void create_gray_bitmap(uint32_t width, uint32_t height);

    /**
     * \brief   Returns the begin of bitmap RBG pixel data.
     */
    inline uint8_t* data() const;

    /**
     * \brief   Returns the size in bytes of space to allocate to create a bitmap image
     *          of specified width and height. The size is aligned to 32-bit
     * 
     * \param   width   The width of the bitmap image in pixels.
     * \param   height  The height of the bitmap image in pixels.
     */
    inline uint32_t data_size(uint32_t width, uint32_t height) const;

    /**
     * \brief   Return the size of pixels for a single line. The size is aligned to 32-bit value.
     * 
     * \param   width   The width of a row.
     */
    inline uint32_t raw_size(uint32_t width) const;
};

//////////////////////////////////////////////////////////////////////////
// SimpleBitmap class inline methods implementation.
//////////////////////////////////////////////////////////////////////////
inline SimpleBitmap::SimpleBitmap()
    : mBitmap   (nullptr)
    , mFileName ( )
    , mFrameId  ( 0xFFFFFFFFu )
    , mChannelId( -1 )
{
    create_gray_bitmap(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

inline SimpleBitmap::SimpleBitmap(uint32_t width, uint32_t height)
    : mBitmap   (nullptr)
    , mFileName ( )
    , mFrameId  ( 0xFFFFFFFFu )
    , mChannelId( -1 )
{
    create_gray_bitmap(width, height);
}

inline SimpleBitmap::~SimpleBitmap()
{
    _release();
}

inline bool SimpleBitmap::is_valid() const
{
    return (mBitmap != nullptr);
}

inline bool SimpleBitmap::create_bitmap(uint32_t width, uint32_t height)
{
    _release();
    create_gray_bitmap(width, height);
    return (mBitmap != nullptr);
}

inline bool SimpleBitmap::allocate_bitmap(uint32_t pix_width, uint32_t pix_height)
{
    bool result{ true };
    if ((pix_width != width()) || (pix_height != height()))
    {
        _release();
        _allocate_bitmap(pix_width, pix_height);
        result = is_valid();
    }

    return result;
}

inline void SimpleBitmap::release()
{
    _release();
}

inline uint32_t SimpleBitmap::width() const
{
    return static_cast<uint32_t>(mBitmap != nullptr ? mBitmap->bmpInfo.bmiWidth : 0);
}

inline uint32_t SimpleBitmap::height() const
{
    return static_cast<uint32_t>(mBitmap != nullptr ? mBitmap->bmpInfo.bmiHeight : 0);
}

inline uint32_t SimpleBitmap::row_bytes() const
{
    return mBitmap != nullptr ? raw_size(static_cast<uint32_t>(mBitmap->bmpInfo.bmiWidth)) : 0u;
}

inline uint8_t* SimpleBitmap::line(uint32_t line) const
{
    uint8_t* result = nullptr;
    if (mBitmap != nullptr)
    {
        uint32_t offset = static_cast<uint32_t>(line) * raw_size(static_cast<uint32_t>(mBitmap->bmpInfo.bmiWidth));
        result = data() + offset;
    }

    return result;
}

inline uint8_t* SimpleBitmap::pixels(uint32_t xCoord, uint32_t yCoord) const
{
    uint8_t* result = nullptr;
    // if ((mBitmap != nullptr) && (xCoord != 0) && (yCoord != 0))
    if (mBitmap != nullptr)
    {
        uint32_t offset = yCoord * raw_size(static_cast<uint32_t>(mBitmap->bmpInfo.bmiWidth));
        uint32_t col    = xCoord * (mBitmap->bmpInfo.bmiBitCount / 8);
        result = data() + offset + col;
    }

    return result;
}

inline bool SimpleBitmap::save(const std::string& fileName)
{
    bool result{ false };
    if (mBitmap != nullptr)
    {
        areg::File file(fileName, static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Binary));
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
    areg::File file(fileName, static_cast<uint32_t>(areg::File::OpenMode::Read) | static_cast<uint32_t>(areg::File::OpenMode::Binary));
    if (file.open())
    {
        BitmapFileHeader bmf;
        if (file.read(reinterpret_cast<uint8_t*>(&bmf), sizeof(BitmapFileHeader)) == sizeof(BitmapFileHeader))
        {
            uint32_t size   = bmf.bmfSize;
            uint8_t* buffer = DEBUG_NEW uint8_t[size];
            file.move_to_begin();
            if (file.read(buffer, size) == size)
            {
                mBitmap = reinterpret_cast<Bitmap*>(buffer);
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

inline bool SimpleBitmap::set_block( const LargeData::ImageBlock & block )
{
    if (block.is_empty())
        return false;

    const LargeData::RawImageBlock* imgBlock = block.block();

    if ((mChannelId != -1) && (mChannelId != static_cast<int32_t>(imgBlock->channelId)))
        return false; // wrong source

    if ( (width( ) != imgBlock->frameWidth) || (height( ) != imgBlock->frameHeight) )
    {
        _release( );
        _allocate_bitmap( imgBlock->frameWidth, imgBlock->frameHeight );
    }

    uint8_t * pix = pixels( static_cast<uint32_t>(imgBlock->imageData.imgStartPos.posX), static_cast<uint32_t>(imgBlock->imageData.imgStartPos.posY) );
    ::memcpy(pix, reinterpret_cast<const uint8_t *>(imgBlock->imageData.imgRGB), imgBlock->imageData.imgRBGLen );
    mChannelId  = static_cast<int32_t>(imgBlock->channelId);
    mFrameId    = imgBlock->frameSeqId;

    return true;
}

inline uint32_t SimpleBitmap::block(LargeData::ImageBlock& result, uint32_t rowIndex, uint32_t lines) const
{
    if ((mBitmap == nullptr) || (mBitmap->bmpInfo.bmiSize == 0))
        return 0u;

    uint32_t pix_width = width();
    uint32_t pix_height = height();
    uint32_t remain = pix_height - rowIndex;
    lines = std::min(lines, remain);
    uint32_t sizePixels = data_size(pix_width, lines);
    uint32_t sizeBlock = sizePixels + sizeof(LargeData::RawImageBlock);
    LargeData::RawImageBlock* block = result.initialize(sizeBlock);

    block->blockSize = sizeBlock;
    block->channelId = 0;
    block->frameSeqId = 0;
    block->frameHeight = pix_height;
    block->frameWidth = pix_width;
    block->imageData.imgHeight = lines;
    block->imageData.imgRBGLen = sizePixels;
    block->imageData.imgStartPos = areg::Coord{ 0, static_cast<int32_t>(rowIndex) };
    block->imageData.imgWidth = pix_width;

    areg::copy_elems<uint8_t>(reinterpret_cast<uint8_t*>(block->imageData.imgRGB), pixels(0, rowIndex), sizePixels);
    return sizeBlock;
}

inline void SimpleBitmap::_allocate_bitmap(uint32_t width, uint32_t height)
{
    uint32_t dataSize = data_size(width, height);
    uint32_t fileSize = dataSize + sizeof(Bitmap);
    uint8_t* buffer = DEBUG_NEW uint8_t[fileSize];
    if (buffer != nullptr)
    {
        mBitmap = new(buffer)Bitmap;
        mBitmap->bmpFile.bmfSize    = fileSize;
        mBitmap->bmpInfo.bmiSize    = dataSize;
        mBitmap->bmpInfo.bmiWidth   = static_cast<int32_t>(width);
        mBitmap->bmpInfo.bmiHeight  = static_cast<int32_t>(height);
    }
}

inline void SimpleBitmap::create_gray_bitmap(uint32_t width, uint32_t height)
{
    _allocate_bitmap(width, height);
    if (mBitmap != nullptr)
    {
        uint8_t grey = 0;
        uint32_t row = raw_size(width);
        uint8_t* pix = reinterpret_cast<uint8_t*>(&mBitmap->bmpBits[0]);
        for (uint32_t i = 0; i < height; ++i)
        {
            uint32_t offset = static_cast<uint32_t>(i * row);
            uint8_t* dst = pix + offset;
            ::memset(dst, grey++, row);
        }
    }
}

inline uint8_t* SimpleBitmap::data() const
{
    return (mBitmap != nullptr ? reinterpret_cast<uint8_t *>(&mBitmap->bmpBits[0]) : nullptr);
}

inline uint32_t SimpleBitmap::raw_size(uint32_t width) const
{
    // return ((((width * 24) + 31) & ~31) >> 3);
    return ((((width * BIT_COUNT) + 31) / 32) * 4);
}

inline uint32_t SimpleBitmap::data_size(uint32_t width, uint32_t height) const
{
    return (raw_size(width) * height);
}

inline void SimpleBitmap::_release()
{
    uint8_t* data = reinterpret_cast<uint8_t*>(mBitmap);
    delete[] data;
    mBitmap = nullptr;
}
