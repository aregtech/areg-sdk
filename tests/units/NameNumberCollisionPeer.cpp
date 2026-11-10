/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 *
 * \file        units/NameNumberCollisionPeer.cpp
 * \brief       An event of an anonymous namespace with the same names as one in
 *              NameNumberCollisionTest.cpp, used by its tests.
 ************************************************************************/
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventTemplate.hpp"

#include <atomic>

namespace
{
    struct AnonStatus
    {
        int32_t value;
    };

    AREG_DECLARE_EVENT(AnonStatus, AnonStatusEvent, AnonStatusConsumer);

    class PeerConsumer final : public AnonStatusConsumer
    {
    public:
        std::atomic<int32_t> mCalls{ 0 };

    protected:
        void process_event(const AnonStatus & /*data*/) override
        {
            mCalls.fetch_add(1);
        }
    };

    PeerConsumer & _peer_consumer()
    {
        static PeerConsumer _consumer;
        return _consumer;
    }
}

uint32_t nc_peer_event_id()
{
    return AnonStatusEvent::CLASS_ID;
}

bool nc_peer_listen(areg::DispatcherThread & dispatcher)
{
    return AnonStatusEvent::add_listener(_peer_consumer(), dispatcher);
}

bool nc_peer_forget(areg::DispatcherThread & dispatcher)
{
    return AnonStatusEvent::remove_listener(_peer_consumer(), dispatcher);
}

int32_t nc_peer_calls()
{
    return _peer_consumer().mCalls.load();
}
