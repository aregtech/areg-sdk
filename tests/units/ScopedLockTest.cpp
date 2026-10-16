/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ScopedLockTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of ScopedLock and of the containers that lock with it.
 *              Every container operation takes its lock and releases it exactly once, never
 *              releases a lock it does not hold, and keeps the data consistent under contention.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/ResourceListMap.hpp"
#include "areg/base/ResourceMap.hpp"
#include "areg/base/Stack.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>
#include <thread>
#include <type_traits>
#include <vector>

namespace
{
    //!< A recursive lock that counts its calls and every unlock by a thread that does not hold it.
    class CountingLock final : public areg::Lockable
    {
    public:
        CountingLock( )
            : areg::Lockable( areg::SyncObject::SyncKind::SoSpinlock )
        {
        }

        bool lock( uint32_t /*timeout*/ ) final
        {
            static_cast<void>(mInner.lock( areg::WAIT_INFINITE ));
            mLocks.fetch_add( 1u, std::memory_order_relaxed );
            return true;
        }

        bool unlock( ) final
        {
            if ( mInner.is_held_by_current( ) == false )
            {
                mStrayUnlocks.fetch_add( 1u, std::memory_order_relaxed );
                return false;
            }

            mUnlocks.fetch_add( 1u, std::memory_order_relaxed );
            return mInner.unlock( );
        }

        bool try_lock( ) final
        {
            return mInner.try_lock( ) ? lock( areg::DO_NOT_WAIT ) : false;
        }

        //!< Returns true if the calling thread holds the lock.
        bool is_held_by_current( ) const noexcept
        {
            return mInner.is_held_by_current( );
        }

        //!< Resets the counters; called while nobody holds the lock.
        void reset( )
        {
            mLocks          = 0u;
            mUnlocks        = 0u;
            mStrayUnlocks   = 0u;
        }

        areg::SpinLock          mInner;
        std::atomic<uint32_t>   mLocks          { 0u };
        std::atomic<uint32_t>   mUnlocks        { 0u };
        std::atomic<uint32_t>   mStrayUnlocks   { 0u };
    };

    //!< Checks that the operation just done took the lock, released it as often, and left it free.
    void expect_balanced( CountingLock & sync, const char * operation )
    {
        EXPECT_GT( sync.mLocks.load( ), 0u ) << operation << " did not lock";
        EXPECT_EQ( sync.mLocks.load( ), sync.mUnlocks.load( ) ) << operation;
        EXPECT_EQ( sync.mStrayUnlocks.load( ), 0u ) << operation << " unlocked without holding the lock";
        EXPECT_FALSE( sync.is_held_by_current( ) ) << operation << " left the lock held";
        sync.reset( );
    }

    //!< A stack locked by a CountingLock, with access to the lock.
    template <typename VALUE>
    class CountingStack : public areg::StackBase<VALUE, CountingLock>
    {
    public:
        CountingLock & sync( ) const noexcept
        {
            return static_cast<CountingLock &>(areg::StackBase<VALUE, CountingLock>::lockable( ));
        }
    };

    struct Item
    {
        int value { 0 };
    };

    constexpr uint32_t  THREADS     { 4u };
    constexpr uint32_t  PER_THREAD  { 20000u };
}

/**
 * \brief   ScopedLock takes the type of the object it locks, so the calls are direct.
 **/
TEST( ScopedLockTest, DeducesTheConcreteType )
{
    areg::SpinLock spin;
    areg::NolockSyncObject nolock;
    areg::ScopedLock spinLock( spin );
    areg::ScopedLock noLock( nolock );

    static_assert( std::is_same_v<decltype(spinLock), areg::ScopedLock<areg::SpinLock>>, "SpinLock is deduced" );
    static_assert( std::is_same_v<decltype(noLock), areg::ScopedLock<areg::NolockSyncObject>>, "NolockSyncObject is deduced" );
    EXPECT_TRUE( spin.is_held_by_current( ) );
}

/**
 * \brief   ScopedLock locks once on construction and unlocks once on destruction, also nested.
 **/
TEST( ScopedLockTest, LocksAndUnlocksOnce )
{
    CountingLock sync;
    {
        areg::ScopedLock outer( sync );
        EXPECT_TRUE( sync.is_held_by_current( ) );
        {
            areg::ScopedLock inner( sync );
            EXPECT_EQ( sync.mLocks.load( ), 2u );
        }

        EXPECT_TRUE( sync.is_held_by_current( ) );
        EXPECT_EQ( sync.mUnlocks.load( ), 1u );
    }

    expect_balanced( sync, "nested ScopedLock" );
}

/**
 * \brief   Every operation of StackBase locks and unlocks its synchronization object in balance.
 **/
TEST( ScopedLockTest, StackOperationsAreBalanced )
{
    CountingStack<int> stack;
    CountingLock & sync{ stack.sync( ) };

    static_cast<void>(stack.push_last( 1 ));                expect_balanced( sync, "push_last" );
    static_cast<void>(stack.push_last( 2 ));                expect_balanced( sync, "push_last" );
    EXPECT_EQ( stack.size( ), 2u );                         expect_balanced( sync, "size" );
    EXPECT_FALSE( stack.is_empty( ) );                      expect_balanced( sync, "is_empty" );
    EXPECT_TRUE( stack.contains( 2 ) );                     expect_balanced( sync, "contains" );
    static_cast<void>(stack.find( 1 ));                     expect_balanced( sync, "find" );
    EXPECT_EQ( stack.pop_first( ), 1 );                     expect_balanced( sync, "pop_first" );

    CountingStack<int> copy;
    CountingLock & copySync{ copy.sync( ) };
    copy = stack;                                           expect_balanced( sync, "copy source" );
    expect_balanced( copySync, "copy target" );
    EXPECT_EQ( copy.size( ), 1u );                          expect_balanced( copySync, "size" );

    stack.clear( );                                         expect_balanced( sync, "clear" );
}

/**
 * \brief   Every operation of ResourceMapBase locks and unlocks its synchronization object in balance.
 **/
TEST( ScopedLockTest, ResourceMapOperationsAreBalanced )
{
    using CountingMap = areg::ResourceMapBase<int, Item *, CountingLock>;
    CountingMap map;
    CountingLock & sync{ static_cast<CountingLock &>(map.lockable( )) };
    Item first{ 1 }, second{ 2 };

    map.register_resource_object( 1, &first );              expect_balanced( sync, "register_resource_object" );
    map.register_resource_object( 2, &second );             expect_balanced( sync, "register_resource_object" );
    EXPECT_TRUE( map.exist( 1 ) );                          expect_balanced( sync, "exist" );
    EXPECT_EQ( map.find_resource_object( 2 ), &second );    expect_balanced( sync, "find_resource_object" );
    EXPECT_EQ( map.unregister_resource_object( 1 ), &first ); expect_balanced( sync, "unregister_resource_object" );
    EXPECT_FALSE( map.is_empty( ) );                        expect_balanced( sync, "is_empty" );
    map.remove_all_resources( );                            expect_balanced( sync, "remove_all_resources" );
    EXPECT_TRUE( map.is_empty( ) );                         expect_balanced( sync, "is_empty" );
}

/**
 * \brief   Every operation of ResourceListMapBase locks and unlocks its synchronization object in balance.
 **/
TEST( ScopedLockTest, ResourceListMapOperationsAreBalanced )
{
    using CountingListMap = areg::ResourceListMapBase<int, Item *, CountingLock>;
    CountingListMap map;
    CountingLock & sync{ static_cast<CountingLock &>(map.lockable( )) };
    Item first{ 1 }, second{ 2 };

    map.register_resource_object( 7, &first );             expect_balanced( sync, "register_resource_object" );
    map.register_resource_object( 7, &second );            expect_balanced( sync, "register_resource_object" );
    EXPECT_TRUE( map.exist( 7 ) );                          expect_balanced( sync, "exist" );
    const auto * list{ map.find_resource( 7 ) };            expect_balanced( sync, "find_resource" );
    EXPECT_NE( list, nullptr );
    map.unregister_resource_object( 7, &first );           expect_balanced( sync, "unregister_resource_object" );
    map.remove_all_resources( );                            expect_balanced( sync, "remove_all_resources" );
    EXPECT_TRUE( map.is_empty( ) );                         expect_balanced( sync, "is_empty" );
}

/**
 * \brief   Producers and consumers share a stack locked with the counting lock: every value
 *          arrives once, and no thread ever releases a lock it does not hold.
 **/
TEST( ScopedLockTest, StackUnderContentionIsConsistent )
{
    CountingStack<uint32_t> stack;
    CountingLock & sync{ stack.sync( ) };
    std::atomic<uint64_t> popped{ 0u };
    std::atomic<uint64_t> sum{ 0u };

    std::vector<std::thread> threads;
    for ( uint32_t t = 0u; t < THREADS; ++ t )
    {
        threads.emplace_back( [&stack, t]( )
            {
                for ( uint32_t i = 0u; i < PER_THREAD; ++ i )
                    static_cast<void>(stack.push_last( t * PER_THREAD + i ));
            } );

        threads.emplace_back( [&stack, &popped, &sum]( )
            {
                while ( popped.load( ) < THREADS * PER_THREAD )
                {
                    stack.lock( );
                    if ( stack.is_empty( ) == false )
                    {
                        sum += stack.pop_first( );
                        ++ popped;
                    }

                    stack.unlock( );
                }
            } );
    }

    for ( std::thread & thread : threads )
        thread.join( );

    const uint64_t count{ static_cast<uint64_t>(THREADS) * PER_THREAD };
    EXPECT_EQ( popped.load( ), count );
    EXPECT_EQ( sum.load( ), count * ( count - 1u ) / 2u );
    EXPECT_TRUE( stack.is_empty( ) );
    EXPECT_EQ( sync.mStrayUnlocks.load( ), 0u );
    EXPECT_EQ( sync.mLocks.load( ), sync.mUnlocks.load( ) );
}

/**
 * \brief   ConcurrentStack, the SpinLock stack the framework uses, under the same contention.
 **/
TEST( ScopedLockTest, ConcurrentStackUnderContentionIsConsistent )
{
    areg::ConcurrentStack<uint32_t> stack;
    std::atomic<uint64_t> popped{ 0u };
    std::atomic<uint64_t> sum{ 0u };

    std::vector<std::thread> threads;
    for ( uint32_t t = 0u; t < THREADS; ++ t )
    {
        threads.emplace_back( [&stack, t]( )
            {
                for ( uint32_t i = 0u; i < PER_THREAD; ++ i )
                    static_cast<void>(stack.push_last( t * PER_THREAD + i ));
            } );

        threads.emplace_back( [&stack, &popped, &sum]( )
            {
                while ( popped.load( ) < THREADS * PER_THREAD )
                {
                    stack.lock( );
                    if ( stack.is_empty( ) == false )
                    {
                        sum += stack.pop_first( );
                        ++ popped;
                    }

                    stack.unlock( );
                }
            } );
    }

    for ( std::thread & thread : threads )
        thread.join( );

    const uint64_t count{ static_cast<uint64_t>(THREADS) * PER_THREAD };
    EXPECT_EQ( popped.load( ), count );
    EXPECT_EQ( sum.load( ), count * ( count - 1u ) / 2u );
    EXPECT_TRUE( stack.is_empty( ) );
}

/**
 * \brief   Threads register, find and unregister their own keys in one ConcurrentResourceMap.
 **/
TEST( ScopedLockTest, ConcurrentResourceMapUnderContentionIsConsistent )
{
    areg::ConcurrentResourceMap<uint32_t, Item *> map;
    std::vector<Item> items( THREADS );
    std::atomic<uint32_t> misses{ 0u };

    std::vector<std::thread> threads;
    for ( uint32_t t = 0u; t < THREADS; ++ t )
    {
        threads.emplace_back( [&map, &items, &misses, t]( )
            {
                Item * item{ &items[t] };
                for ( uint32_t i = 0u; i < PER_THREAD; ++ i )
                {
                    const uint32_t key{ t * PER_THREAD + i };
                    map.register_resource_object( key, item );
                    if ( map.find_resource_object( key ) != item )
                        ++ misses;
                    if ( map.unregister_resource_object( key ) != item )
                        ++ misses;
                }
            } );
    }

    for ( std::thread & thread : threads )
        thread.join( );

    EXPECT_EQ( misses.load( ), 0u );
    EXPECT_TRUE( map.is_empty( ) );
}

/**
 * \brief   A container that locks with Mutex is free for every thread after its construction:
 *          a default constructed Mutex is not owned by the thread that created it.
 **/
TEST( ScopedLockTest, ContainerWithMutexIsFreeForOtherThreads )
{
    areg::Mutex mutex;
    EXPECT_FALSE( mutex.is_locked( ) );

    areg::ResourceMapBase<int, Item *, areg::Mutex> map;
    bool locked{ false };
    std::thread other( [&map, &locked]( )
        {
            locked = map.lockable( ).lock( 500u );
            if ( locked )
                static_cast<void>(map.lockable( ).unlock( ));
        } );

    other.join( );
    EXPECT_TRUE( locked ) << "the container's Mutex is held by the thread that created it";
}
