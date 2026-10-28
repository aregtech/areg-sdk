/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/PausedSources.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the sockets a service receive thread stops reading while the
 *              connection their messages go to keeps more than its cap.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/private/PausedSources.hpp"

#include <algorithm>

namespace areg::ext {

PausedSources::PausedSources(areg::SocketMultiplexer & mux)
    : HoldWaiter    ( )
    , mMux          ( mux )
    , mSources      ( )
    , mBacklogs     ( )
    , mReleased     ( false )
{
}

PausedSources::~PausedSources()
{
    clear();
}

bool PausedSources::pause(SOCKETHANDLE hSocket)
{
    ReceivePause & request{ receive_pause() };
    SendBacklog * backlog{ request.backlog };
    const ITEM_ID target{ request.target };
    request.backlog = nullptr;
    if ((backlog == nullptr) || (backlog->hold_wait(target, *this) == false))
        return false;

    if (std::find(mBacklogs.begin(), mBacklogs.end(), backlog) == mBacklogs.end())
    {
        mBacklogs.push_back(backlog);
    }

    mSources.push_back(Source{ hSocket, target, backlog });
    return true;
}

void PausedSources::drop(SOCKETHANDLE hSocket) noexcept
{
    for (auto pos = mSources.begin(); pos != mSources.end(); ++pos)
    {
        if (pos->socket == hSocket)
        {
            mSources.erase(pos);
            return;
        }
    }
}

void PausedSources::clear() noexcept
{
    for (SendBacklog * backlog : mBacklogs)
    {
        backlog->hold_cancel(*this);
    }

    mBacklogs.clear();
    mSources.clear();
    mReleased.store(false, std::memory_order_release);
}

void PausedSources::hold_released() noexcept
{
    if (mReleased.exchange(true, std::memory_order_acq_rel) == false)
    {
        mMux.wakeup();
    }
}

} // namespace areg::ext
