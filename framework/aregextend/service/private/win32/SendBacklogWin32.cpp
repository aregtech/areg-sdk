/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/win32/SendBacklogWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the writer of a service send thread that never waits for one
 *              socket. Windows part: overlapped writes, completed in the order they started.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/private/SendBacklog.hpp"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <WinSock2.h>
#include <Windows.h>

namespace areg::ext {

namespace
{
    //!< One overlapped write. Its address must not change while the write is in progress.
    struct SendOp
    {
        WSAOVERLAPPED   overlapped  { };    //!< The overlapped structure, no event.
        uint64_t        bytes       { 0u }; //!< The bytes of the write.
    };

    //!< Milliseconds a cancelled write is waited for before its buffers are released anyway.
    constexpr uint32_t  CANCEL_WAIT_MS  { 5'000u };
}

struct SendBacklogOs
{
    std::deque<std::unique_ptr<SendOp>> ops     { };    //!< Writes in progress, in the order they started.
    uint64_t                            inFlight{ 0u }; //!< The bytes of the writes in progress.
};

void SendBacklogOsDelete::operator () (SendBacklogOs * os) const noexcept
{
    delete os;
}

int64_t backlog_os_start( SendBacklogOsPtr & os
                        , SOCKETHANDLE hSocket
                        , const areg::IoBuffer * buffers
                        , uint32_t count
                        , uint64_t maxInFlight
                        , uint64_t & started )
{
    ASSERT((buffers != nullptr) && (count != 0u) && (count <= areg::DEFAULT_DRAIN_LIMIT));

    int64_t completed{ 0 };
    started = 0u;

    uint32_t index{ 0u };
    std::size_t offset{ 0u };
    std::unique_ptr<SendOp> op{ };
    while (index < count)
    {
        if ((os != nullptr) && (os->ops.empty() == false) && (os->inFlight >= maxInFlight))
            break;

        WSABUF wsaBuffers[areg::DEFAULT_DRAIN_LIMIT];
        DWORD bufCount{ 0u };
        uint64_t chunk{ 0u };
        while ((index < count) && (chunk < SendBacklog::WRITE_CHUNK))
        {
            std::size_t length{ buffers[index].size - offset };
            if (chunk + length > SendBacklog::WRITE_CHUNK)
            {
                length = static_cast<std::size_t>(SendBacklog::WRITE_CHUNK - chunk);
            }

            wsaBuffers[bufCount].buf = reinterpret_cast<CHAR *>(const_cast<uint8_t *>(buffers[index].data + offset));
            wsaBuffers[bufCount].len = static_cast<ULONG>(length);
            ++bufCount;
            chunk  += length;
            offset += length;
            if (offset == buffers[index].size)
            {
                offset = 0u;
                ++index;
            }
        }

        if (op == nullptr)
        {
            op = std::make_unique<SendOp>();
        }

        op->overlapped  = WSAOVERLAPPED{ };
        op->bytes       = chunk;

        DWORD sent{ 0u };
        const int result{ ::WSASend(hSocket, wsaBuffers, bufCount, &sent, 0, &op->overlapped, nullptr) };
        if ((result != 0) && (::WSAGetLastError() != WSA_IO_PENDING))
        {
            return -1;
        }

        started += chunk;
        if ((result == 0) && ((os == nullptr) || os->ops.empty()))
        {
            // Completed now, and nothing started before it is still in progress.
            completed += static_cast<int64_t>(chunk);
            continue;
        }

        if (os == nullptr)
        {
            os = SendBacklogOsPtr(new SendBacklogOs());
        }

        os->inFlight += chunk;
        os->ops.push_back(std::move(op));
    }

    return completed;
}

int64_t backlog_os_collect(SendBacklogOs * os, SOCKETHANDLE hSocket)
{
    int64_t completed{ 0 };
    while ((os != nullptr) && (os->ops.empty() == false))
    {
        SendOp & op{ *os->ops.front() };
        if (HasOverlappedIoCompleted(&op.overlapped) == FALSE)
            break;

        DWORD bytes{ 0u };
        DWORD flags{ 0u };
        if ((::WSAGetOverlappedResult(hSocket, &op.overlapped, &bytes, FALSE, &flags) == FALSE) || (static_cast<uint64_t>(bytes) != op.bytes))
        {
            return -1;
        }

        completed    += static_cast<int64_t>(op.bytes);
        os->inFlight -= op.bytes;
        os->ops.pop_front();
    }

    return completed;
}

uint64_t backlog_os_in_flight(const SendBacklogOs * os) noexcept
{
    return (os != nullptr ? os->inFlight : 0u);
}

bool backlog_os_cancel(SendBacklogOsPtr & os, SOCKETHANDLE hSocket) noexcept
{
    if (os == nullptr)
        return true;

    for (const auto & op : os->ops)
    {
        if (HasOverlappedIoCompleted(&op->overlapped) == FALSE)
        {
            static_cast<void>(::CancelIoEx(reinterpret_cast<HANDLE>(hSocket), &op->overlapped));
        }
    }

    // The buffers belong to the operating system until every write is done.
    const ULONGLONG deadline{ ::GetTickCount64() + CANCEL_WAIT_MS };
    bool result{ true };
    for (const auto & op : os->ops)
    {
        while ((HasOverlappedIoCompleted(&op->overlapped) == FALSE) && (::GetTickCount64() < deadline))
        {
            ::Sleep(1);
        }

        result = result && (HasOverlappedIoCompleted(&op->overlapped) != FALSE);
    }

    if (result)
    {
        os.reset();
    }
    else
    {
        static_cast<void>(os.release());
    }

    return result;
}

void backlog_os_wait(SOCKETHANDLE /*hSocket*/, uint32_t timeoutMs) noexcept
{
    ::Sleep(timeoutMs);
}

} // namespace areg::ext

#endif  // _WIN32
