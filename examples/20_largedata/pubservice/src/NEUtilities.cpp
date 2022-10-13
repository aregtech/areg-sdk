/************************************************************************
 * \file        pubservice/src/NEUtilities.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file is a helper when generate and transfer the image data.
 ************************************************************************/

#include "pubservice/src/NEUtilities.hpp"

#include "areg/base/Containers.hpp"
#include "areg/base/String.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(examples_20_pubservice_NEUtilities_sOptionData_parseCommand);
DEF_TRACE_SCOPE(examples_20_pubservice_NEUtilities_sOptionData_setValue);

bool NEUtilities::sOptionData::parseCommand(const String& cmd)
{
    static constexpr std::string_view   _delimiter{ " " };

    TRACE_SCOPE(examples_20_pubservice_NEUtilities_sOptionData_parseCommand);
    TRACE_DBG("Parsing command [ $s ]", cmd.getString());

    Tokenizer tokens(cmd, _delimiter, false);
    const StringArray& list = tokens.getList();
    mFlags = static_cast<uint32_t>(eOptionFlags::CmdNothing);

    for (uint32_t i = 0; i < list.getSize(); ++i)
    {
        String entry(list[i]);
        entry.trimAll();

        bool found{ false };
        for (const auto& opt : OptionList)
        {
            if (entry.startsWith(opt.cmdShort))
            {
                entry.substring(static_cast<NEString::CharPos>(opt.cmdShort.length()));
                found = true;
            }
            else if (entry.startsWith(opt.cmdLong))
            {
                entry.substring(static_cast<NEString::CharPos>(opt.cmdLong.length()));
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
            mFlags = static_cast<uint32_t>(eOptionFlags::Error);
            break;
        }
    }

    return (hasError() == false);
}

bool NEUtilities::sOptionData::setValue(const sOptions& whichOpt, uint32_t whichValue)
{
    TRACE_SCOPE(examples_20_pubservice_NEUtilities_sOptionData_setValue);
    TRACE_DBG("Validating the option command [ %s ] with values [ %u ], the allowed min = [ %u ] and max = [ %u ]"
                    , NEUtilities::getString(whichOpt.cmdValue)
                    , whichValue
                    , whichOpt.minValue
                    , whichOpt.maxValue);

    if (((mFlags & static_cast<uint32_t>(whichOpt.cmdValue)) != 0    ) ||
        ((whichOpt.minValue != 0) && (whichValue < whichOpt.minValue)) ||
        ((whichOpt.maxValue != 0) && (whichValue > whichOpt.maxValue)))
    {
        mFlags = static_cast<uint32_t>(eOptionFlags::Error);
        return false;
    }

    bool result{ true };
    switch (whichOpt.cmdValue)
    {
    case eOptionFlags::CmdWidth:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mWidth = whichValue;
        break;

    case eOptionFlags::CmdHeight:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mHeight = whichValue;
        break;

    case eOptionFlags::CmdLinesPerBlock:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mLines = whichValue;
        break;

    case eOptionFlags::CmdPixelTime:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mPixelTime = whichValue;
        break;

    case eOptionFlags::CmdChannels:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        mChannels = whichValue;
        break;

    case eOptionFlags::CmdInformation:
    case eOptionFlags::CmdHelp:
    case eOptionFlags::CmdStart:
    case eOptionFlags::CmdStop:
    case eOptionFlags::CmdQuit:
        mFlags |= static_cast<uint32_t>(whichOpt.cmdValue);
        break;

    default:
        mFlags = static_cast<uint32_t>(eOptionFlags::Error);
        result = false;
        break;
    }

    return result;
}

const NEUtilities::sOptionData& NEUtilities::sOptionData::normalizeOptions(const sOptionData& oldOptions)
{
    if ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdWidth)) == 0)
    {
        mWidth = oldOptions.mWidth;
    }

    if ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdHeight)) == 0)
    {
        mHeight = oldOptions.mHeight;
    }

    if ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdLinesPerBlock)) == 0)
    {
        mLines = oldOptions.mLines;
    }

    if ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdPixelTime)) == 0)
    {
        mPixelTime = oldOptions.mPixelTime;
    }

    if ((mFlags & static_cast<uint32_t>(eOptionFlags::CmdChannels)) == 0)
    {
        mChannels = oldOptions.mChannels;
    }

    return (*this);
}
