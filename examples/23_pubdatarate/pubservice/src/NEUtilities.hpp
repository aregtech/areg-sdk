/************************************************************************
 * \file        pubservice/src/NEUtilities.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a helper when generate and transfer the image data.
 ************************************************************************/
#pragma once

/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/base/String.hpp"

#include <string_view>

class String;

namespace NEUtilities
{
    /**
     * \brief   Bit settings of option flag.
     *          NEOptions::eOptionFlags
     **/
    enum class eOptionFlags : uint32_t
    {
        /**
         * \brief   No command.
         **/
          CmdNothing        = 0
        /**
         * \brief   The command to change image width
         **/
        , CmdWidth          = 1
        /**
         * \brief   The command to change image height
         **/
        , CmdHeight         = 2
        /**
         * \brief   The command to change lines per image block
         **/
        , CmdLinesPerBlock  = 4
        /**
         * \brief   The command to set image pixel time in nanoseconds.
         **/
        , CmdPixelTime      = 8
        /**
         * \brief   The command to set number of channels.
         **/
        , CmdChannels       = 16
        /**
         * \brief   The command to display information
         **/
        , CmdInformation    = 32
        /**
         * \brief   The command to display help
         **/
        , CmdHelp           = 64
        /**
         * \brief   The command to start large data service
         **/
        , CmdStart          = 128
        /**
         * \brief   The command to stop large data service
         **/
        , CmdStop           = 256
        /**
         * \brief   The command to quit application
         **/
        , CmdQuit           = 512
        /**
         * \brief   The command to display help
         **/
        , Error             = static_cast<uint32_t>(0x80000000)
    };

    //!< Returns the string value of the option flag.
    inline const char * getString(NEUtilities::eOptionFlags flag);

    //! The structure of options command
    struct sOptions
    {
        std::string_view    cmdShort;   //!< The short name of the option command
        std::string_view    cmdLong;    //!< The long name of the option command
        eOptionFlags        cmdValue;   //!< The appropriate value
        uint32_t            minValue;   //!< The minimum value of the command, ignore if zero
        uint32_t            maxValue;   //!< The maximum value of the command, ignore if zero
    };

    //! List of supported commands and options.
    constexpr sOptions  OptionList[]
    {
          {"-w=", "--width=",   eOptionFlags::CmdWidth,         32,     32'768} //! Width in pixels
        , {"-h=", "--height",   eOptionFlags::CmdHeight,        32,     32'768} //! Height in pixels
        , {"-l=", "--lines=",   eOptionFlags::CmdLinesPerBlock,  1,          0} //! Lines per block, maximum is equal to height
        , {"-t=", "--time=" ,   eOptionFlags::CmdPixelTime,      1,    100'000} //! Pixel time in nanoseconds
        , {"-c=", "--channels=",eOptionFlags::CmdChannels,       1,         64} //! Number of channels
        , {"-i" , "--info",     eOptionFlags::CmdInformation,    0,          0} //! Display information
        , {"-h" , "--help",     eOptionFlags::CmdHelp,           0,          0} //! Display help
        , {"-s" , "--start",    eOptionFlags::CmdStart,          0,          0} //! Start large data service
        , {"-p" , "--stop",     eOptionFlags::CmdStop,           0,          0} //! Stop large data service
        , {"-q" , "--quit",     eOptionFlags::CmdQuit,           0,          0} //! Quit application
    };

    //!< Default bits per pixel.
    static constexpr uint32_t BITS_PER_PIXEL    { 24 };
    //!< Default bytes per pixel.
    static constexpr uint32_t BYTES_PER_PIXEL   { 3 };
    //!< Default image width in pixels.
    static constexpr uint32_t IMAGE_WIDTH       { 1024 };
    //!< Default image height in pixels.
    static constexpr uint32_t IMAGE_HEIGHT      { 1024 };
    //!< Default lines per block in count.
    static constexpr uint32_t LINES_PER_BLOCK   { 64 };
    //!< Default pixel dwell time in nanoseconds.
    static constexpr uint32_t DWELL_TIME        { 100 };
    //!< Default image source channels in count.
    static constexpr uint32_t CHANNELS_SOURCE   { 1 };

    //! The option data class
    struct sOptionData
    {
//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
    public:
        //! Option flags, set bitwise
        uint32_t    mFlags      { static_cast<uint32_t>(eOptionFlags::CmdNothing) };
        //! Image width in pixels
        uint32_t    mWidth      { 0 };
        //! Image height in pixels
        uint32_t    mHeight     { 0 };
        //! Lines per image block
        uint32_t    mLines      { 0 };
        //! Pixel time in nanoseconds
        uint32_t    mPixelTime  { 0 };
        //! The number of channels
        uint32_t    mChannels   { 0 };

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Parses option string, which user output on command line.
         * 
         * \param   cmd     The string to parse.
         * \return  Returns true if operation succeeds.
         */
        bool parseCommand(const String& cmd);

        /**
         * \brief   Sets option specific value and indicates which fields are valid.
         * 
         * \param   whichOpt    The option to modify.
         * \param   whichValue  The value to set for the option.
         * \return  Returns true if operation succeeds.
         */
        bool setValue(const sOptions & whichOpt, uint32_t whichValue);

        /**
         * \brief   Normalizes the option structure and takes the values from the give option.
         * 
         * \param   newOptions  The option to consider when normalize.
        */
        const sOptionData& normalizeOptions(const sOptionData& newOptions);

        /**
         * \brief   Returns true if the option has command to print the info.
         */
        inline bool hasPrintInfo(void) const;

        /**
         * \brief   Returns true if the option has command to print help of the application.
         */
        inline bool hasPrintHelp(void) const;

        /**
         * \brief   Returns true if the option has command to start application.
         */
        inline bool hasStart(void) const;

        /**
         * \brief   Returns true if the option has command to stop application.
         */
        inline bool hasStop(void) const;

        /**
         * \brief   Returns true if the option has command to quit application.
         */
        inline bool hasQuit(void) const;

        /**
         * \brief   Returns true if the option had error, like unsupported command.
         */
        inline bool hasError(void) const;

        /**
         * \brief   Based on option values, returns the number of blocks to generate data in 1 second.
         */
        inline uint32_t blocksCount(void) const;

        /**
         * \brief   Based on option values, returns number of pixels per image.
         */
        inline uint32_t pixelsPerImage(void) const;

        /**
         * \brief   Based on option values, returns number of pixels per image block.
         */
        inline uint32_t pixelsPerBlock(void) const;

        /**
         * \brief   Based on option values, return bytes per line.
         */
        inline uint32_t bytesPerLine(void) const;

        /**
         * \brief   Based on option values, return bytes per block.
         */
        inline uint32_t bytesPerBlock(void) const;

        /**
         * \brief   Based on option values, return bytes per image.
         */
        inline uint32_t bytesPerImage(void) const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a line of image.
         */
        inline uint64_t nsPerLine(void) const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a block of image.
         */
        inline uint64_t nsPerBlock(void) const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate an image.
         */
        inline uint64_t nsPerImage(void) const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a block of image.
         * \param   startRowIndex   The beginning of the image pixels.
         * \param   rowCount        The number of row / lines per image block.
         */
        inline uint64_t nsPerBlock(uint32_t startRowIndex, uint32_t rowCount) const;

        /**
         * \brief   The string state of the application.
         */
        inline String getState(void) const;

        /**
         * \brief   Normalizes the options.
         * 
         * \param   newOption   New options as a data source.
         */
        inline void update(const sOptionData& newOption);

    };
}

//////////////////////////////////////////////////////////////////////////
// NEUtilities namespace inline methods.
//////////////////////////////////////////////////////////////////////////
inline bool NEUtilities::sOptionData::hasPrintInfo(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdInformation)) != 0);
}

inline bool NEUtilities::sOptionData::hasPrintHelp(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdHelp)) != 0);
}

inline bool NEUtilities::sOptionData::hasStart(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdStart)) != 0);
}

inline bool NEUtilities::sOptionData::hasStop(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdStop)) != 0);
}

inline bool NEUtilities::sOptionData::hasQuit(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdQuit)) != 0);
}

inline bool NEUtilities::sOptionData::hasError(void) const
{
    return ((mFlags & static_cast<uint32_t>(eOptionFlags::Error)) != 0);
}

inline uint32_t NEUtilities::sOptionData::blocksCount(void) const
{
    if ((mLines != 0) && (mHeight != 0))
    {
        uint32_t blocks = mHeight / mLines;
        return ((mHeight % mLines) != 0 ? blocks + 1 : blocks);
    }
    else
    {
        return 0;
    }
}

inline uint32_t NEUtilities::sOptionData::pixelsPerImage(void) const
{
    return (mWidth * mHeight);
}

inline uint32_t NEUtilities::sOptionData::pixelsPerBlock(void) const
{
    return (mWidth * mLines);
}

inline uint32_t NEUtilities::sOptionData::bytesPerLine(void) const
{
    return ((((mWidth * BITS_PER_PIXEL) + 31) / 32) * 4);
}

inline uint32_t NEUtilities::sOptionData::bytesPerBlock(void) const
{
    return (bytesPerLine() * mLines);
}

inline uint32_t NEUtilities::sOptionData::bytesPerImage(void) const
{
    return (bytesPerLine() * mHeight);
}

inline uint64_t NEUtilities::sOptionData::nsPerLine(void) const
{
    return static_cast<uint64_t>(mPixelTime) * mWidth;
}

inline uint64_t NEUtilities::sOptionData::nsPerBlock(void) const
{
    return static_cast<uint64_t>(mPixelTime) * pixelsPerBlock();
}

inline uint64_t NEUtilities::sOptionData::nsPerImage(void) const
{
    return static_cast<uint64_t>(mPixelTime) * pixelsPerImage();
}

inline uint64_t NEUtilities::sOptionData::nsPerBlock(uint32_t startRowIndex, uint32_t rowCount) const
{
    if ((startRowIndex + rowCount) > mHeight)
        rowCount = mHeight - startRowIndex;

    return nsPerLine() * rowCount;
}

inline String NEUtilities::sOptionData::getState(void) const
{
    if (hasStart())
        return String("STARTED");
    else
        return String("STOPPED");
}

inline void NEUtilities::sOptionData::update(const sOptionData& newOption)
{
    mFlags = static_cast<uint32_t>(eOptionFlags::CmdNothing);

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdWidth)) != 0)
    {
        mWidth = newOption.mWidth;
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdWidth);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdHeight)) != 0)
    {
        mHeight = newOption.mHeight;
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdHeight);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdLinesPerBlock)) != 0)
    {
        mLines = newOption.mLines;
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdLinesPerBlock);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdPixelTime)) != 0)
    {
        mPixelTime = newOption.mPixelTime;
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdPixelTime);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdChannels)) != 0)
    {
        mChannels = newOption.mChannels;
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdChannels);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdStart)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(eOptionFlags::CmdStop);
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdStart);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdStop)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(eOptionFlags::CmdStart);
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdStop);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(eOptionFlags::CmdQuit)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(eOptionFlags::CmdStart);
        mFlags &= ~static_cast<uint32_t>(eOptionFlags::CmdStop);
        mFlags |= static_cast<uint32_t>(eOptionFlags::CmdQuit);
    }
}

inline const char * NEUtilities::getString(NEUtilities::eOptionFlags flag)
{
    switch (flag)
    {
    case NEUtilities::eOptionFlags::CmdNothing:
        return "NEUtilities::eOptionFlags::CmdNothing";
    case NEUtilities::eOptionFlags::CmdWidth:
        return "NEUtilities::eOptionFlags::CmdWidth";
    case NEUtilities::eOptionFlags::CmdHeight:
        return "NEUtilities::eOptionFlags::CmdHeight";
    case NEUtilities::eOptionFlags::CmdLinesPerBlock:
        return "NEUtilities::eOptionFlags::CmdLinesPerBlock";
    case NEUtilities::eOptionFlags::CmdPixelTime:
        return "NEUtilities::eOptionFlags::CmdPixelTime";
    case NEUtilities::eOptionFlags::CmdChannels:
        return "NEUtilities::eOptionFlags::CmdChannels";
    case NEUtilities::eOptionFlags::CmdInformation:
        return "NEUtilities::eOptionFlags::CmdInformation";
    case NEUtilities::eOptionFlags::CmdHelp:
        return "NEUtilities::eOptionFlags::CmdHelp";
    case NEUtilities::eOptionFlags::CmdStart:
        return "NEUtilities::eOptionFlags::CmdStart";
    case NEUtilities::eOptionFlags::CmdStop:
        return "NEUtilities::eOptionFlags::CmdStop";
    case NEUtilities::eOptionFlags::CmdQuit:
        return "NEUtilities::eOptionFlags::CmdQuit";
    case NEUtilities::eOptionFlags::Error:
        return "NEUtilities::eOptionFlags::Error";
    
    default:
        return "ERR: Invalid NEUtilities::eOptionFlags value!";
    }
}
