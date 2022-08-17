/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Statistics.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing statistics.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Statistics.hpp"
#include "areg/base/NECommon.hpp"

Statistics::Statistics(void)
    : mVerbose      ( false )
    , mStatSent     ( )
    , mStatReceived ( )
    , mLock         (false)
{
}

void Statistics::initialize(bool verbose)
{
    mVerbose = verbose;
    if (verbose)
    {
        Timestamp now = Clock::now();
        sCoord curPos = _getCursorCurrentPos();

        mStatSent.statLastUpdate = now;
        mStatSent.statDataSize = 0;
        mStatSent.statPosition.posX = 0;
        mStatSent.statPosition.posY = curPos.posY + 0;

        mStatReceived.statLastUpdate = now;
        mStatReceived.statDataSize = 0;
        mStatReceived.statPosition.posX = 0;
        mStatReceived.statPosition.posY = curPos.posY + 1;
    }
}

void Statistics::setVerbose(bool verbose)
{
    mVerbose = verbose;
}

void Statistics::sentBytes(uint32_t bytesSent)
{
    if (mVerbose == false)
        return; // do nothing

    if (bytesSent != 0)
    {
        mStatSent.statDataSize += bytesSent;

        Timestamp now = Clock::now();
        std::chrono::duration<double, std::milli> duration = now - mStatSent.statLastUpdate;
        uint32_t elapsed = static_cast<uint32_t>(duration.count());
        if (elapsed >= NECommon::TIMEOUT_1_SEC)
        {
            Lock lock(mLock);

            sCoord curPos = _getCursorCurrentPos();
            _outputSendBytes(elapsed);
            _setCursorCurrentPos(curPos);

            mStatSent.statDataSize = 0;
            mStatSent.statLastUpdate = now;
        }
    }
    else
    {
        Lock lock(mLock);

        sCoord curPos = _getCursorCurrentPos();
        _outputSendBytes(0);
        _setCursorCurrentPos(curPos);
    }
}

void Statistics::receivedBytes(uint32_t bytesReceived)
{
    if (mVerbose == false)
        return; // do nothing
    
    if (bytesReceived != 0)
    {
        mStatReceived.statDataSize += bytesReceived;

        Timestamp now = Clock::now();
        std::chrono::duration<double, std::milli> duration = now - mStatReceived.statLastUpdate;
        uint32_t elapsed = static_cast<uint32_t>(duration.count());
        if (elapsed >= NECommon::TIMEOUT_1_SEC)
        {

            Lock lock(mLock);

            sCoord curPos = _getCursorCurrentPos();
            _outputReceiveBytes(elapsed);
            _setCursorCurrentPos(curPos);

            mStatReceived.statDataSize = 0;
            mStatReceived.statLastUpdate = now;
        }
    }
    else
    {
        Lock lock(mLock);

        sCoord curPos = _getCursorCurrentPos();
        _outputReceiveBytes(0);
        _setCursorCurrentPos(curPos);
    }
}

void Statistics::_outputSendBytes(uint32_t duration)
{
    _setCursorCurrentPos(mStatSent.statPosition);

    if (duration == 0)
    {
        printf("\rSend data with the rate: ___ B/s");
    }
    else
    {
        float rate = static_cast<float>(mStatSent.statDataSize) / duration;
        if (static_cast<uint32_t>(rate) > ONE_MEGABYTE)
        {
            rate /= ONE_MEGABYTE;
            printf("\rSend data with the rate: %.02f MB/s", rate);
        }
        else if (static_cast<uint32_t>(rate) > ONE_KILOBYTE)
        {
            rate /= ONE_KILOBYTE;
            printf("\rSend data with the rate: %.02f KB/s", rate);
        }
        else
        {
            printf("\rSend data with the rate: %.02f B/s", rate);
        }
    }
}

void Statistics::_outputReceiveBytes(uint32_t duration)
{
    _setCursorCurrentPos(mStatReceived.statPosition);

    if (duration == 0)
    {
        printf("\rRecv data with the rate: ___ B/s");
    }
    else
    {
        float rate = static_cast<float>(mStatReceived.statDataSize) / duration;
        if (static_cast<uint32_t>(rate) > ONE_MEGABYTE)
        {
            rate /= ONE_MEGABYTE;
            printf("\rRecv data with the rate: %.02f MB/s", rate);
        }
        else if (static_cast<uint32_t>(rate) > ONE_KILOBYTE)
        {
            rate /= ONE_KILOBYTE;
            printf("\rRecv data with the rate: %.02f KB/s", rate);
        }
        else
        {
            printf("\rRecv data with the rate: %.02f B/s", rate);
        }
    }
}

