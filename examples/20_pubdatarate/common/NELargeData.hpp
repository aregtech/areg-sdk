#pragma once
/************************************************************************
 * \file        common\NELargeData.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains basic constants and objects that are
 *              shared between components.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/base/IEIOStream.hpp"

#include <memory>
#include <string_view>
#include <utility>
#include <string.h>

/**
 * \brief   The namespace to define large data project specific structures and constants.
 */
namespace NELargeData
{
    //!< The role name of the service
    constexpr std::string_view  ServiceRoleName { "LargeDataService" };

    //!< 1 second timeout used to output data rate.
    constexpr uint32_t           TIMER_TIMEOUT  { 1'000u };

    //!< The RGB structure of 1 pixel data.
    struct sRBG
    {
        uint8_t red     { 0 }; //!< The red value of a pixel.
        uint8_t green   { 0 }; //!< The green value of a pixel.
        uint8_t blue    { 0 }; //!< The blue value of a pixel.
    };

    //!< Coordinates.
    struct sCoord
    {
        uint32_t coordX { 0 }; //!< X-coordinate.
        uint32_t coordY { 0 }; //!< Y-coordinate.
    };

    //!< The structure of an image data.
    struct sImageData
    {
        //!< Specifies the (x, y) starting position coordinate of an image data
        sCoord      imgStartPos { 0, 0 };
        //!< Specifies  the width of an image data.
        uint32_t    imgWidth    { 0 };
        //!< Specifies the height of an image data.
        uint32_t    imgHeight   { 0 };
        //!< Specifies the length in bytes of an image pixels.
        uint32_t    imgRBGLen   { 0 };
        //!< The following is array of pixels. Each pixel is a combination of RGB values.
        sRBG        imgRGB[1];
    };

    //!< The structure of an image block.
    struct sImageBlock
    {
        //!< The total size of an image block
        uint32_t    blockSize   { sizeof( sImageBlock ) };
        //!< The frame ID of image block. Blocks with same ID belong to same image frame.
        uint32_t    frameSeqId  { 0 };
        //!< The image data source ID.
        uint32_t    channelId   { 0 };
        //!< Specifies the total width of the image frame.
        uint32_t    frameWidth  { 0 };
        //!< Specifies the total height of the image frame.
        uint32_t    frameHeight { 0 };
        //!< The image data structure followed by array of pixels.
        sImageData  imageData;
    };

    //!< The image block class, which is a wrapper of image block structure.
    class ImageBlock
    {
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor.
//////////////////////////////////////////////////////////////////////////
    public:
        inline ImageBlock(void);
        inline ImageBlock(ImageBlock&& src) noexcept;
        inline ~ImageBlock(void);

//////////////////////////////////////////////////////////////////////////
// Attributes, operations and operators.
//////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Assigning move operator of the image block.
         * 
         * \param   src     The source of an image block.
         */
        inline ImageBlock& operator = (ImageBlock&& src) noexcept;

        /**
         * \brief   Allocates space and initializes image block of specified size.
         * 
         * \param   blockSize   The total size of the image block.
         * \return  If succeeded, returns valid pointer of image block structure.
         *          Otherwise, returns null pointer.
         */
        inline sImageBlock* initialize(uint32_t blockSize);
        
        /**
         * \brief   Updates the channel ID and frame ID of the image block and returns
         *          pointer to the image block structure.
         * 
         * \param   channelId   The channel ID to set for image block.
         * \param   sequenceNr  The frame ID to set for image block
         * \return  Return pointer to image block.
         */
        inline const sImageBlock* getBlock(uint32_t channelId, uint32_t sequenceNr);

        /**
         * \brief   Return pointer to the image block structure.
         */
        inline const sImageBlock* getBlock(void) const;

        /**
         * \brief   Sets the image block data.
         */
        inline void setBlock(sImageBlock* block);

        /**
         * \brief   Releases the image block.
         */
        inline void release(void);

        /**
         * \brief   Return the total size of the image block.
         */
        inline uint32_t getSize(void) const;

        /**
         * \brief   Returns true if the image block is empty.
         */
        inline bool isEmpty(void) const;

        /**
         * \brief   Sets the channel ID and the frame ID of the image block.
         */
        inline void setIds(uint32_t channelId, uint32_t frameId);

        /**
         * \brief   The streaming operators to serialize image block into the streaming buffer.
         */
        friend inline IEOutStream& operator << (IEOutStream& stream, const NELargeData::ImageBlock& output);

        /**
         * \brief   Initializes image block from the streaming buffer.
         */
        friend inline const IEInStream& operator >> (const IEInStream& stream, NELargeData::ImageBlock& input);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
    private:
        //!< The image block.
        sImageBlock *   mBlock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    private:
        ImageBlock(const ImageBlock& /*src*/) = delete;
        ImageBlock& operator = (const ImageBlock & /*src*/) = delete;
    };

    /**
     * \brief   The streaming operators to serialize image block into the streaming buffer.
     */
    inline IEOutStream& operator << (IEOutStream& stream, const NELargeData::ImageBlock& output);

    /**
     * \brief   Initializes image block from the streaming buffer.
     */
    inline const IEInStream& operator >> (const IEInStream& stream, NELargeData::ImageBlock& input);

}

//////////////////////////////////////////////////////////////////////////
// NELargeData specific inline methods.
//////////////////////////////////////////////////////////////////////////
inline NELargeData::ImageBlock::ImageBlock(void)
    : mBlock    (nullptr)
{
}

inline NELargeData::ImageBlock::ImageBlock(NELargeData::ImageBlock&& src) noexcept
    : mBlock    (std::move(src.mBlock))
{
    src.mBlock = nullptr;
}

inline NELargeData::ImageBlock::~ImageBlock(void)
{
    release();
}

inline NELargeData::ImageBlock& NELargeData::ImageBlock::operator = (NELargeData::ImageBlock&& src) noexcept
{
    if (this != &src)
    {
        release();
        mBlock = std::move(src.mBlock);
        src.mBlock = nullptr;
    }

    return (*this);
}

inline NELargeData::sImageBlock* NELargeData::ImageBlock::initialize(uint32_t blockSize)
{
    release();
    uint8_t* data = blockSize != 0 ? DEBUG_NEW uint8_t[blockSize] : nullptr;
    if (data != nullptr)
    {
        ::memset(data, 0x00, blockSize);
        mBlock = new(data) NELargeData::sImageBlock;
        mBlock->blockSize = blockSize;
    }

    return mBlock;
}

inline const NELargeData::sImageBlock* NELargeData::ImageBlock::getBlock(uint32_t channelId, uint32_t sequenceNr)
{
    if (mBlock != nullptr)
    {
        mBlock->channelId = channelId;
        mBlock->frameSeqId = sequenceNr;
    }

    return mBlock;
}

inline const NELargeData::sImageBlock* NELargeData::ImageBlock::getBlock(void) const
{
    return mBlock;
}

inline void NELargeData::ImageBlock::setBlock(sImageBlock* block)
{
    release();
    mBlock = block;
}

inline void NELargeData::ImageBlock::release(void)
{
    if (mBlock != nullptr)
    {
        delete[] reinterpret_cast<uint8_t*>(mBlock);
        mBlock = nullptr;
    }
}

inline uint32_t NELargeData::ImageBlock::getSize(void) const
{
    return (mBlock != nullptr ? mBlock->blockSize : 0);
}

inline bool NELargeData::ImageBlock::isEmpty(void) const
{
    return (mBlock != nullptr) && (mBlock->blockSize != 0);
}

inline void NELargeData::ImageBlock::setIds(uint32_t channelId, uint32_t frameId)
{
    if (mBlock != nullptr)
    {
        mBlock->channelId = channelId;
        mBlock->frameSeqId = frameId;
    }
}

inline IEOutStream& NELargeData::operator << (IEOutStream& stream, const NELargeData::ImageBlock& output)
{
    uint32_t size{ output.getSize() };
    if (size != 0)
    {
        uint8_t* data = reinterpret_cast<uint8_t*>(output.mBlock);
        stream.write(data, size);
    }
    else
    {
        stream << size;
    }

    return stream;
}

inline const IEInStream& NELargeData::operator >> (const IEInStream& stream, NELargeData::ImageBlock& input)
{
    uint32_t size{ 0 };
    stream >> size;
    uint8_t* data = size != 0 ? DEBUG_NEW uint8_t[size] : nullptr;

    if (data != nullptr)
    {
        NELargeData::sImageBlock* block = new(data) NELargeData::sImageBlock;
        block->blockSize = size;
        uint32_t skip = sizeof(block->blockSize);
        size -= skip;
        data += skip;
        stream.read(data, size);

        input.setBlock(block);
    }
    else
    {
        input.release();
    }

    return stream;
}
