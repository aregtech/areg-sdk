#pragma once

#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"

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

    struct sImageBlock
    {
        uint32_t    imgBlockLen{ 0 };
        sCoord      imgStartPos{ 0 };
        uint32_t    imgEndPos{ 0 };
        uint32_t    imgWidth{ 0 };
        uint32_t    imgRBGLen{ 0 };
        sRBG        imgRGB[1];
    };
}

inline IEOutStream& operator << (IEOutStream& stream, const NELargeData::sCoord & imgCoord)
{
    stream.write(reinterpret_cast<const uint8_t *>(&imgCoord), sizeof(NELargeData::sCoord));
    return stream;
}

inline IEOutStream& operator << (IEOutStream& stream, const NELargeData::sImageBlock & imgBlock)
{
    stream  << imgBlock.imgBlockLen 
            << imgBlock.imgStartPos 
            << imgBlock.imgEndPos
            << imgBlock.imgWidth
            << imgBlock.imgRBGLen;

    stream.write(reinterpret_cast<const uint8_t*>(imgBlock.imgRGB), imgBlock.imgRBGLen);
    return stream;
}

inline const IEInStream& operator >> (const IEInStream& stream, NELargeData::sCoord& imgCoord)
{
    stream.read(reinterpret_cast<uint8_t*>(&imgCoord), sizeof(NELargeData::sCoord));
    return stream;
}

inline const IEInStream& operator >> (const IEInStream& stream, NELargeData::sImageBlock& imgBlock)
{

}
