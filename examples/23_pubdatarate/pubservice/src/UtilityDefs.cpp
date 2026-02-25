/************************************************************************
 * \file        pubservice/src/UtilityDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file is a helper when generate and transfer the image data.
 ************************************************************************/

#include "pubservice/src/UtilityDefs.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/String.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(examples_23_pubservice_NEUtilities_sOptionData_parseCommand);
DEF_LOG_SCOPE(examples_23_pubservice_NEUtilities_sOptionData_setValue);

bool areg::OptionValues::parseCommand(const areg::String& cmd)
{
    static constexpr std::string_view   _delimiter{ " " };

    LOG_SCOPE(examples_23_pubservice_NEUtilities_sOptionData_parseCommand);
    LOG_DBG("Parsing command [ $s ]", cmd.getString());

    Tokenizer tokens(cmd, _delimiter, false);
    const StringArray& list = tokens.getList();
    mFlags = static_cast<uint32_t>(OptionFlag::CmdNothing);

    for (uint32_t i = 0; i < list.getSize(); ++i)
    {
        areg::String entry(list[i]);
        entry.trimAll();

        bool found{ false };
        for (const auto& opt : OptionSpecs)
        {
            if (entry.startsWith(opt.cmdShort))
            {
                entry.substring(static_cast<areg::CharPos>(opt.cmdShort.length()));
                found = true;
            }
            else if (entry.startsWith(opt.cmdLong))
            {
                entry.substring(static_cast<areg::CharPos>(opt.cmdLong.length()));
                found = true;
            }

            if (found)
            {
                uint32_t value = entry.isEmpty() == false ? entry.toUInt32() : 0;
                setValue(opt, value);
                break;
            }
        }

        if (found == false)
        {
            mFlags = static_cast<uint32_t>(OptionFlag::Error);
            break;
        }
    }

    return (hasError() == false);
}

bool areg::OptionValues::setValue(const OptionSpec& whichOpt, uint32_t whichValue)
{
    LOG_SCOPE(examples_23_pubservice_NEUtilities_sOptionData_setValue);
    LOG_DBG("Validating the option command [ %s ] with values [ %u ], the allowed min = [ %u ] and max = [ %u ]"
                    , areg::getString(whichOpt.cmdValue)
                    , whichValue
                    , whichOpt.minValue
                    , whichOpt.maxValue);

    if (((mFlags & static_cast<uint32_t>(whichOpt.cmdValue)) != 0    ) ||
        ((whichOpt.minValue != 0) && (whichValue < whichOpt.minValue)) ||
        ((whichOpt.maxValue != 0) && (whichValue > whichOpt.maxValue)))
    {
        mFlags = static_cast<uint32_t>(OptionFlag::Error);
        return false;
    }

    bool result{ true };
    switch (whichOpt.cmdValue)
    {
    case OptionFlag::CmdWidth:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mWidth = whichValue;
        break;

    case OptionFlag::CmdHeight:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mHeight = whichValue;
        break;

    case OptionFlag::CmdLinesPerBlock:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mLines = whichValue;
        break;

    case OptionFlag::CmdPixelTime:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mPixelTime = whichValue;
        break;

    case OptionFlag::CmdChannels:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mChannels = whichValue;
        break;

    case OptionFlag::CmdInformation:
    case OptionFlag::CmdHelp:
    case OptionFlag::CmdStart:
    case OptionFlag::CmdStop:
    case OptionFlag::CmdQuit:
    case OptionFlag::CmdNothing:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        break;

    case OptionFlag::Error:
    default:
        mFlags = static_cast<uint32_t>(OptionFlag::Error);
        result = false;
        break;
    }

    return result;
}

const areg::OptionValues& areg::OptionValues::normalizeOptions(const OptionValues& oldOptions)
{
    if ((mFlags & static_cast<uint32_t>(OptionFlag::CmdWidth)) == 0)
    {
        mWidth = oldOptions.mWidth;
    }

    if ((mFlags & static_cast<uint32_t>(OptionFlag::CmdHeight)) == 0)
    {
        mHeight = oldOptions.mHeight;
    }

    if ((mFlags & static_cast<uint32_t>(OptionFlag::CmdLinesPerBlock)) == 0)
    {
        mLines = oldOptions.mLines;
    }

    if ((mFlags & static_cast<uint32_t>(OptionFlag::CmdPixelTime)) == 0)
    {
        mPixelTime = oldOptions.mPixelTime;
    }

    if ((mFlags & static_cast<uint32_t>(OptionFlag::CmdChannels)) == 0)
    {
        mChannels = oldOptions.mChannels;
    }

    return (*this);
}
