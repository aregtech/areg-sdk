/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/SocketHandover.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, hands accepted sockets to the receive thread that serves them.
 ************************************************************************/
#include "aregextend/service/private/SocketHandover.hpp"

#include "areg/base/SocketMultiplexer.hpp"

namespace areg::ext {

SocketHandover::SocketHandover(areg::SocketMultiplexer & mux) noexcept
    : mMux      ( mux )
    , mPending  ( false )
    , mLock     ( )
    , mAdded    ( )
    , mReleased ( )
{
}

void SocketHandover::add(const areg::SocketAccepted & socket)
{
    {
        Lock lock(mLock);
        mAdded.push_back(socket);
        mPending.store(true, std::memory_order_release);
    }

    mMux.wakeup();
}

void SocketHandover::release(areg::SocketAccepted && socket)
{
    {
        Lock lock(mLock);
        mReleased.push_back(std::move(socket));
        mPending.store(true, std::memory_order_release);
    }

    mMux.wakeup();
}

void SocketHandover::take(std::vector<areg::SocketAccepted> & added, std::vector<areg::SocketAccepted> & released)
{
    Lock lock(mLock);
    mPending.store(false, std::memory_order_relaxed);
    added.swap(mAdded);
    released.swap(mReleased);
}

} // namespace areg::ext
