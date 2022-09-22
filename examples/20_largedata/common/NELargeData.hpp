#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/base/SharedBuffer.hpp"

namespace NELargeData
{
    struct sRBG
    {
        uint8_t red{ 0 };
        uint8_t green{ 0 };
        uint8_t blue{ 0 };
    };

    struct sCoord
    {
        uint32_t coordX{ 0 };
        uint32_t coordY{ 0 };
    };

    struct sImageData
    {
        sCoord      imgStartPos{ 0, 0 };
        uint32_t    imgWidth{ 0 };
        uint32_t    imgHeight{ 0 };
        uint32_t    imgRBGLen{ 0 };
        sRBG        imgRGB[1];
    };

    struct sImageBlock
    {
        uint32_t    blockSize{ sizeof( sImageBlock ) };
        uint32_t    frameSeqId{ 0 };
        uint32_t    channelId{ 0 };
        uint32_t    frameWidth{ 0 };
        uint32_t    frameHeight{ 0 };
        sImageData  imageData;
    };

    inline void writeImageBlock( const sImageBlock & imgBlock, SharedBuffer & buffer );

    inline const sImageBlock * getImageBlock( const SharedBuffer & buffer );
}

inline void NELargeData::writeImageBlock( const sImageBlock & imgBlock, SharedBuffer & buffer )
{
    buffer.moveToBegin( );
    buffer.resize( imgBlock.blockSize, false );
    buffer.write( reinterpret_cast<const unsigned char *>(&imgBlock), imgBlock.blockSize );
}

const NELargeData::sImageBlock * NELargeData::getImageBlock( const SharedBuffer & buffer )
{
    const NELargeData::sImageBlock * result = reinterpret_cast<const NELargeData::sImageBlock *>(buffer.getBuffer());
    ASSERT( (result == nullptr) || (result->blockSize <= buffer.getSizeUsed( )) );
    return result;
}
