/************************************************************************
 * \file        pubservice/src/UtilityDefs.hpp
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
     *          NEOptions::OptionFlag
     **/
    enum class OptionFlag : uint32_t
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
    inline const char * as_string(NEUtilities::OptionFlag flag);

    //! The structure of options command
    struct OptionSpec
    {
        std::string_view    cmdShort;   //!< The int16_t name of the option command
        std::string_view    cmdLong;    //!< The long name of the option command
        OptionFlag        cmdValue;   //!< The appropriate value
        uint32_t            minValue;   //!< The minimum value of the command, ignore if zero
        uint32_t            maxValue;   //!< The maximum value of the command, ignore if zero
    };

    //! List of supported commands and options.
    constexpr OptionSpec  OptionSpecs[]
    {
          {"-w=", "--width=",   OptionFlag::CmdWidth,         32,     32'768} //! Width in pixels
        , {"-h=", "--height",   OptionFlag::CmdHeight,        32,     32'768} //! Height in pixels
        , {"-l=", "--lines=",   OptionFlag::CmdLinesPerBlock,  1,          0} //! Lines per block, maximum is equal to height
        , {"-t=", "--time=" ,   OptionFlag::CmdPixelTime,      1,    100'000} //! Pixel time in nanoseconds
        , {"-c=", "--channels=",OptionFlag::CmdChannels,       1,         64} //! Number of channels
        , {"-i" , "--info",     OptionFlag::CmdInformation,    0,          0} //! Display information
        , {"-h" , "--help",     OptionFlag::CmdHelp,           0,          0} //! Display help
        , {"-s" , "--start",    OptionFlag::CmdStart,          0,          0} //! Start large data service
        , {"-p" , "--stop",     OptionFlag::CmdStop,           0,          0} //! Stop large data service
        , {"-q" , "--quit",     OptionFlag::CmdQuit,           0,          0} //! Quit application
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
    struct OptionValues
    {
//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
    public:
        //! Option flags, set bitwise
        uint32_t    mFlags      { static_cast<uint32_t>(OptionFlag::CmdNothing) };
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
        bool setValue(const OptionSpec & whichOpt, uint32_t whichValue);

        /**
         * \brief   Normalizes the option structure and takes the values from the give option.
         * 
         * \param   newOptions  The option to consider when normalize.
        */
        const OptionValues& normalizeOptions(const OptionValues& newOptions);

        /**
         * \brief   Returns true if the option has command to print the info.
         */
        inline bool hasPrintInfo() const;

        /**
         * \brief   Returns true if the option has command to print help of the application.
         */
        inline bool hasPrintHelp() const;

        /**
         * \brief   Returns true if the option has command to start application.
         */
        inline bool hasStart() const;

        /**
         * \brief   Returns true if the option has command to stop application.
         */
        inline bool hasStop() const;

        /**
         * \brief   Returns true if the option has command to quit application.
         */
        inline bool hasQuit() const;

        /**
         * \brief   Returns true if the option had error, like unsupported command.
         */
        inline bool hasError() const;

        /**
         * \brief   Based on option values, returns the number of blocks to generate data in 1 second.
         */
        inline uint32_t blocksCount() const;

        /**
         * \brief   Based on option values, returns number of pixels per image.
         */
        inline uint32_t pixelsPerImage() const;

        /**
         * \brief   Based on option values, returns number of pixels per image block.
         */
        inline uint32_t pixelsPerBlock() const;

        /**
         * \brief   Based on option values, return bytes per line.
         */
        inline uint32_t bytesPerLine() const;

        /**
         * \brief   Based on option values, return bytes per block.
         */
        inline uint32_t bytesPerBlock() const;

        /**
         * \brief   Based on option values, return bytes per image.
         */
        inline uint32_t bytesPerImage() const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a line of image.
         */
        inline uint64_t nsPerLine() const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a block of image.
         */
        inline uint64_t nsPerBlock() const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate an image.
         */
        inline uint64_t nsPerImage() const;

        /**
         * \brief   Based on option values, return duration in nanoseconds to generate a block of image.
         * \param   startRowIndex   The beginning of the image pixels.
         * \param   rowCount        The number of row / lines per image block.
         */
        inline uint64_t nsPerBlock(uint32_t startRowIndex, uint32_t rowCount) const;

        /**
         * \brief   The string state of the application.
         */
        inline String getState() const;

        /**
         * \brief   Normalizes the options.
         * 
         * \param   newOption   New options as a data source.
         */
        inline void update(const OptionValues& newOption);

    };
}

//////////////////////////////////////////////////////////////////////////
// NEUtilities namespace inline methods.
//////////////////////////////////////////////////////////////////////////
inline bool NEUtilities::OptionValues::hasPrintInfo() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::CmdInformation)) != 0);
}

inline bool NEUtilities::OptionValues::hasPrintHelp() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::CmdHelp)) != 0);
}

inline bool NEUtilities::OptionValues::hasStart() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::CmdStart)) != 0);
}

inline bool NEUtilities::OptionValues::hasStop() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::CmdStop)) != 0);
}

inline bool NEUtilities::OptionValues::hasQuit() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::CmdQuit)) != 0);
}

inline bool NEUtilities::OptionValues::hasError() const
{
    return ((mFlags & static_cast<uint32_t>(OptionFlag::Error)) != 0);
}

inline uint32_t NEUtilities::OptionValues::blocksCount() const
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

inline uint32_t NEUtilities::OptionValues::pixelsPerImage() const
{
    return (mWidth * mHeight);
}

inline uint32_t NEUtilities::OptionValues::pixelsPerBlock() const
{
    return (mWidth * mLines);
}

inline uint32_t NEUtilities::OptionValues::bytesPerLine() const
{
    return ((((mWidth * BITS_PER_PIXEL) + 31) / 32) * 4);
}

inline uint32_t NEUtilities::OptionValues::bytesPerBlock() const
{
    return (bytesPerLine() * mLines);
}

inline uint32_t NEUtilities::OptionValues::bytesPerImage() const
{
    return (bytesPerLine() * mHeight);
}

inline uint64_t NEUtilities::OptionValues::nsPerLine() const
{
    return static_cast<uint64_t>(mPixelTime) * mWidth;
}

inline uint64_t NEUtilities::OptionValues::nsPerBlock() const
{
    return static_cast<uint64_t>(mPixelTime) * pixelsPerBlock();
}

inline uint64_t NEUtilities::OptionValues::nsPerImage() const
{
    return static_cast<uint64_t>(mPixelTime) * pixelsPerImage();
}

inline uint64_t NEUtilities::OptionValues::nsPerBlock(uint32_t startRowIndex, uint32_t rowCount) const
{
    if ((startRowIndex + rowCount) > mHeight)
        rowCount = mHeight - startRowIndex;

    return nsPerLine() * rowCount;
}

inline String NEUtilities::OptionValues::getState() const
{
    if (hasStart())
        return String("STARTED");
    else
        return String("STOPPED");
}

inline void NEUtilities::OptionValues::update(const OptionValues& newOption)
{
    mFlags = static_cast<uint32_t>(OptionFlag::CmdNothing);

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdWidth)) != 0)
    {
        mWidth = newOption.mWidth;
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdWidth);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdHeight)) != 0)
    {
        mHeight = newOption.mHeight;
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdHeight);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdLinesPerBlock)) != 0)
    {
        mLines = newOption.mLines;
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdLinesPerBlock);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdPixelTime)) != 0)
    {
        mPixelTime = newOption.mPixelTime;
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdPixelTime);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdChannels)) != 0)
    {
        mChannels = newOption.mChannels;
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdChannels);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdStart)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(OptionFlag::CmdStop);
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdStart);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdStop)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(OptionFlag::CmdStart);
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdStop);
    }

    if ((newOption.mFlags & static_cast<uint32_t>(OptionFlag::CmdQuit)) != 0)
    {
        mFlags &= ~static_cast<uint32_t>(OptionFlag::CmdStart);
        mFlags &= ~static_cast<uint32_t>(OptionFlag::CmdStop);
        mFlags |= static_cast<uint32_t>(OptionFlag::CmdQuit);
    }
}

inline const char * NEUtilities::as_string(NEUtilities::OptionFlag flag)
{
    switch (flag)
    {
    case NEUtilities::OptionFlag::CmdNothing:
        return "NEUtilities::OptionFlag::CmdNothing";
    case NEUtilities::OptionFlag::CmdWidth:
        return "NEUtilities::OptionFlag::CmdWidth";
    case NEUtilities::OptionFlag::CmdHeight:
        return "NEUtilities::OptionFlag::CmdHeight";
    case NEUtilities::OptionFlag::CmdLinesPerBlock:
        return "NEUtilities::OptionFlag::CmdLinesPerBlock";
    case NEUtilities::OptionFlag::CmdPixelTime:
        return "NEUtilities::OptionFlag::CmdPixelTime";
    case NEUtilities::OptionFlag::CmdChannels:
        return "NEUtilities::OptionFlag::CmdChannels";
    case NEUtilities::OptionFlag::CmdInformation:
        return "NEUtilities::OptionFlag::CmdInformation";
    case NEUtilities::OptionFlag::CmdHelp:
        return "NEUtilities::OptionFlag::CmdHelp";
    case NEUtilities::OptionFlag::CmdStart:
        return "NEUtilities::OptionFlag::CmdStart";
    case NEUtilities::OptionFlag::CmdStop:
        return "NEUtilities::OptionFlag::CmdStop";
    case NEUtilities::OptionFlag::CmdQuit:
        return "NEUtilities::OptionFlag::CmdQuit";
    case NEUtilities::OptionFlag::Error:
        return "NEUtilities::OptionFlag::Error";
    
    default:
        return "ERR: Invalid NEUtilities::OptionFlag value!";
    }
}
