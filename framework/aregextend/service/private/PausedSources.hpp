#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_PAUSEDSOURCES_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_PAUSEDSOURCES_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/PausedSources.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the sockets a service receive thread stops reading while the
 *              connection their messages go to keeps more than its cap.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

#include <atomic>
#include <vector>

namespace areg::ext {

/**
 * \brief   The request of a message handler to stop reading the socket that brought the message,
 *          because the connection the message went to is held, see SendBacklog::is_held().
 *          Set and taken on the receive thread that serves the socket.
 **/
struct ReceivePause
{
    SendBacklog *   backlog { nullptr };    //!< The backlog of the held connection, nullptr if none.
    ITEM_ID         target  { 0u };         //!< The held connection.
};

//!< Returns the pause request of the calling thread.
inline ReceivePause & receive_pause() noexcept
{
    static thread_local ReceivePause pause{ };
    return pause;
}

//!< Returns true if the calling receive thread was asked to stop reading its current socket.
inline bool is_receive_paused() noexcept
{
    return (receive_pause().backlog != nullptr);
}

/**
 * \brief   The sockets a receive thread stopped reading because a connection their messages go
 *          to is held. A socket is read again once that connection is released. Used by the
 *          receive thread that serves the sockets only; hold_released() is called by any thread.
 **/
class PausedSources final : public HoldWaiter
{
public:
    //!< The waiter of a receive thread that waits on \a mux.
    explicit PausedSources(areg::SocketMultiplexer & mux);

    ~PausedSources();

    /**
     * \brief   Takes the pause request of the calling thread for \a hSocket.
     * \return  True if the socket is paused and must not be read; false if there was no request
     *          or the connection is released already.
     **/
    bool pause(SOCKETHANDLE hSocket);

    //!< Drops the pause request of the calling thread.
    static inline void discard() noexcept
    {
        receive_pause().backlog = nullptr;
    }

    //!< Returns true if a connection a paused socket waits for may be released.
    [[nodiscard]]
    inline bool has_released() const noexcept
    {
        return mReleased.load(std::memory_order_acquire);
    }

    /**
     * \brief   Calls \a resume(SOCKETHANDLE) for every paused socket whose connection is released,
     *          and forgets it.
     **/
    template<typename ResumeFn>
    inline void resume(ResumeFn && resume)
    {
        mReleased.store(false, std::memory_order_release);
        for (uint32_t i = 0u; i < static_cast<uint32_t>(mSources.size()); )
        {
            const Source source{ mSources[i] };
            if (source.backlog->hold_wait(source.target, *this))
            {
                ++i;
                continue;
            }

            mSources.erase(mSources.begin() + i);
            resume(source.socket);
        }
    }

    //!< Forgets the socket if it is paused.
    void drop(SOCKETHANDLE hSocket) noexcept;

    //!< Forgets every paused socket and every backlog it waits on.
    void clear() noexcept;

    //!< Wakes the receive thread once.
    void hold_released() noexcept override;

private:
    //!< A socket that is not read.
    struct Source
    {
        SOCKETHANDLE    socket;     //!< The socket.
        ITEM_ID         target;     //!< The held connection its message went to.
        SendBacklog *   backlog;    //!< The backlog of that connection.
    };

    areg::SocketMultiplexer &   mMux;       //!< The multiplexer of the receive thread.
    std::vector<Source>         mSources;   //!< The paused sockets.
    std::vector<SendBacklog *>  mBacklogs;  //!< Every backlog this waiter was registered with.
    std::atomic<bool>           mReleased;  //!< True once a connection was released after the last resume().

private:
    PausedSources() = delete;
    AREG_NOCOPY_NOMOVE(PausedSources);
};

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_PAUSEDSOURCES_HPP
