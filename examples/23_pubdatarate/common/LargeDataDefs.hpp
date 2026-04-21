#pragma once
/************************************************************************
 * \file        common\LargeDataDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains basic constants and objects that are
 *              shared between components.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/IOStream.hpp"
#include "areg/base/MathDefs.hpp"

#include <memory>
#include <string_view>
#include <utility>
#include <string.h>

/**
 * \brief   The namespace to define large data project specific structures and constants.
 */
namespace LargeData
{
    //!< The role name of the service
    constexpr std::string_view  ServiceRoleName { "LargeDataService" };

    //!< 1 second timeout used to output data rate.
    constexpr uint32_t           TIMER_TIMEOUT  { 1'000u };

    //!< The RGB structure of 1 pixel data.
    struct RBG
    {
        uint8_t red     { 0 }; //!< The red value of a pixel.
        uint8_t green   { 0 }; //!< The green value of a pixel.
        uint8_t blue    { 0 }; //!< The blue value of a pixel.
    };

    //!< The structure of an image data.
    struct ImageData
    {
        //!< Specifies the (x, y) starting position coordinate of an image data
        areg::Coord   imgStartPos { 0, 0 };
        //!< Specifies  the width of an image data.
        uint32_t        imgWidth    { 0 };
        //!< Specifies the height of an image data.
        uint32_t        imgHeight   { 0 };
        //!< Specifies the length in bytes of an image pixels.
        uint32_t        imgRBGLen   { 0 };
        //!< The following is array of pixels. Each pixel is a combination of RGB values.
        RBG            imgRGB[1];
    };

    //!< The structure of an image block.
    struct RawImageBlock
    {
        //!< The total size of an image block
        uint32_t    blockSize   { sizeof(RawImageBlock) };
        //!< The frame ID of image block. Blocks with same ID belong to same image frame.
        uint32_t    frameSeqId  { 0 };
        //!< The image data source ID.
        uint32_t    channelId   { 0 };
        //!< Specifies the total width of the image frame.
        uint32_t    frameWidth  { 0 };
        //!< Specifies the total height of the image frame.
        uint32_t    frameHeight { 0 };
        //!< The image data structure followed by array of pixels.
        ImageData  imageData;
    };

    //!< The image block class, which is a wrapper of image block structure.
    class ImageBlock
    {
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor.
//////////////////////////////////////////////////////////////////////////
    public:
        inline ImageBlock();
        inline ImageBlock(ImageBlock&& src) noexcept;
        inline ~ImageBlock();

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
        inline RawImageBlock* initialize(uint32_t blockSize);
        
        /**
         * \brief   Updates the channel ID and frame ID of the image block and returns
         *          pointer to the image block structure.
         * 
         * \param   channelId   The channel ID to set for image block.
         * \param   sequenceNr  The frame ID to set for image block
         * \return  Return pointer to image block.
         */
        inline const RawImageBlock* block(uint32_t channelId, uint32_t sequenceNr);

        /**
         * \brief   Return pointer to the image block structure.
         */
        inline const RawImageBlock* block() const;

        /**
         * \brief   Sets the image block data.
         */
        inline void set_block(RawImageBlock* block);

        /**
         * \brief   Releases the image block.
         */
        inline void release();

        /**
         * \brief   Return the total size of the image block.
         */
        inline uint32_t getSize() const;

        /**
         * \brief   Returns true if the image block is empty.
         */
        inline bool isEmpty() const;

        /**
         * \brief   Sets the channel ID and the frame ID of the image block.
         */
        inline void setIds(uint32_t channelId, uint32_t frameId);

        /**
         * \brief   Updates only the frame sequence ID. Use in the hot send loop when
         *          the channel ID was pre-stamped at initialisation time.
         *
         * \param   frameId     The frame sequence number to stamp on this block.
         */
        inline void set_frame_id(uint32_t frameId);

        /**
         * \brief   The streaming operators to serialize image block into the streaming buffer.
         */
        friend inline areg::OutStream& operator << (areg::OutStream& stream, const ImageBlock& output);

        /**
         * \brief   Initializes image block from the streaming buffer.
         */
        friend inline const areg::InStream& operator >> (const areg::InStream& stream, ImageBlock& input);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
    private:
        //!< The image block.
        RawImageBlock *   mBlock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
    private:
        ImageBlock(const ImageBlock& /*src*/) = delete;
        ImageBlock& operator = (const ImageBlock & /*src*/) = delete;
    };

    inline areg::OutStream& operator << (areg::OutStream& stream, const LargeData::ImageBlock& output)
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

    inline const areg::InStream& operator >> (const areg::InStream& stream, LargeData::ImageBlock& input)
    {
        uint32_t size{ 0 };
        stream >> size;
        uint8_t* data = size != 0 ? DEBUG_NEW uint8_t[size] : nullptr;

        if (data != nullptr)
        {
            LargeData::RawImageBlock* block = new(data) LargeData::RawImageBlock;
            block->blockSize = size;
            uint32_t skip = sizeof(block->blockSize);
            size -= skip;
            data += skip;
            stream.read(data, size);

            input.set_block(block);
        }
        else
        {
            input.release();
        }

        return stream;
    }
}

//////////////////////////////////////////////////////////////////////////
// LargeData specific inline methods.
//////////////////////////////////////////////////////////////////////////
inline LargeData::ImageBlock::ImageBlock()
    : mBlock    (nullptr)
{
}

inline LargeData::ImageBlock::ImageBlock(LargeData::ImageBlock&& src) noexcept
    : mBlock    (std::move(src.mBlock))
{
    src.mBlock = nullptr;
}

inline LargeData::ImageBlock::~ImageBlock()
{
    release();
}

inline LargeData::ImageBlock& LargeData::ImageBlock::operator = (LargeData::ImageBlock&& src) noexcept
{
    if (this != &src)
    {
        release();
        mBlock = std::move(src.mBlock);
        src.mBlock = nullptr;
    }

    return (*this);
}

inline LargeData::RawImageBlock* LargeData::ImageBlock::initialize(uint32_t blockSize)
{
    release();
    uint8_t* data = blockSize != 0 ? DEBUG_NEW uint8_t[blockSize] : nullptr;
    if (data != nullptr)
    {
        ::memset(data, 0x00, blockSize);
        mBlock = new(data) LargeData::RawImageBlock;
        mBlock->blockSize = blockSize;
    }

    return mBlock;
}

inline const LargeData::RawImageBlock* LargeData::ImageBlock::block(uint32_t channelId, uint32_t sequenceNr)
{
    if (mBlock != nullptr)
    {
        mBlock->channelId = channelId;
        mBlock->frameSeqId = sequenceNr;
    }

    return mBlock;
}

inline const LargeData::RawImageBlock* LargeData::ImageBlock::block() const
{
    return mBlock;
}

inline void LargeData::ImageBlock::set_block(RawImageBlock* block)
{
    release();
    mBlock = block;
}

inline void LargeData::ImageBlock::release()
{
    if (mBlock != nullptr)
    {
        delete[] reinterpret_cast<uint8_t*>(mBlock);
        mBlock = nullptr;
    }
}

inline uint32_t LargeData::ImageBlock::getSize() const
{
    return (mBlock != nullptr ? mBlock->blockSize : 0);
}

inline bool LargeData::ImageBlock::isEmpty() const
{
    return (mBlock == nullptr) || (mBlock->blockSize == 0);
}

inline void LargeData::ImageBlock::setIds(uint32_t channelId, uint32_t frameId)
{
    if (mBlock != nullptr)
    {
        mBlock->channelId = channelId;
        mBlock->frameSeqId = frameId;
    }
}

inline void LargeData::ImageBlock::set_frame_id(uint32_t frameId)
{
    if (mBlock != nullptr)
    {
        mBlock->frameSeqId = frameId;
    }
}
