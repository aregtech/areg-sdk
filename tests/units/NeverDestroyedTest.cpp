/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/NeverDestroyedTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Unit Tests. The object held in static storage without a destructor.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"

#include "areg/base/private/NeverDestroyed.hpp"

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <thread>
#include <type_traits>

namespace
{
    //!< A type without a user constructor.
    struct PlainData
    {
        uint32_t mFirst;
        uint64_t mSecond;
        char     mText[32];
    };

    //!< A singleton with a private constructor that counts its builds.
    class BuiltOnce
    {
    public:
        static BuiltOnce & instance()
        {
            static areg::NeverDestroyed<BuiltOnce> _instance{ [](void * where) { ::new (where) BuiltOnce(); } };
            return _instance.object();
        }

        static uint32_t build_count()
        {
            return sBuilds;
        }

        int value() const
        {
            return mValue;
        }

    private:
        BuiltOnce()
            : mValue( 42 )
        {
            ++sBuilds;
        }

        int mValue;

        inline static uint32_t sBuilds{ 0u };
    };

    //!< Ends the child with a failure when its destructor runs.
    class DestructorTrap
    {
    public:
        DestructorTrap() = default;

        ~DestructorTrap()
        {
            std::_Exit( areg_child::CHILD_CHECK_FAILED );
        }
    };

    //!< A type aligned above the default alignment.
    struct alignas(64) OverAligned
    {
        char mByte;
    };

    //!< Returns the holder of a destructor trap in static storage.
    areg::NeverDestroyed<DestructorTrap> & trap_holder()
    {
        static areg::NeverDestroyed<DestructorTrap> _holder;
        return _holder;
    }
}

static_assert( std::is_trivially_destructible<areg::NeverDestroyed<DestructorTrap>>::value
             , "A holder must register nothing to run at exit" );
static_assert( std::is_copy_constructible<areg::NeverDestroyed<PlainData>>::value == false
             , "A holder is not copied" );

/**
 * \brief   The default form zeroes a type without a constructor, whatever the storage held before.
 **/
TEST( NeverDestroyedTest, default_form_zeroes_a_type_without_a_constructor )
{
    using Holder = areg::NeverDestroyed<PlainData>;

    alignas(Holder) unsigned char storage[sizeof(Holder)];
    std::memset( storage, 0xA5, sizeof(storage) );

    Holder * holder{ ::new (static_cast<void *>(storage)) Holder() };
    const PlainData & data{ holder->object() };

    EXPECT_EQ( data.mFirst, 0u );
    EXPECT_EQ( data.mSecond, 0u );
    for ( const char symbol : data.mText )
    {
        EXPECT_EQ( symbol, '\0' );
    }
}

/**
 * \brief   The callable form builds a type with a private constructor, once, at one address.
 **/
TEST( NeverDestroyedTest, callable_form_builds_a_private_constructor_once )
{
    BuiltOnce & first{ BuiltOnce::instance() };
    BuiltOnce & second{ BuiltOnce::instance() };

    EXPECT_EQ( &first, &second );
    EXPECT_EQ( first.value(), 42 );
    EXPECT_EQ( BuiltOnce::build_count(), 1u );
}

/**
 * \brief   The object has the alignment of its type.
 **/
TEST( NeverDestroyedTest, object_keeps_the_alignment_of_its_type )
{
    static areg::NeverDestroyed<OverAligned> _aligned;

    EXPECT_EQ( reinterpret_cast<std::uintptr_t>(&_aligned.object()) % alignof(OverAligned), 0u );
}

/**
 * \brief   Threads that race on the first call build the object once and see the same object.
 **/
TEST( NeverDestroyedTest, racing_first_calls_build_the_object_once )
{
    AREG_EXPECT_IN_CHILD(
        static std::atomic<uint32_t> _builds{ 0u };
        struct Racy
        {
            Racy( ) { _builds.fetch_add( 1u ); }
        };

        static std::atomic<bool> _go{ false };
        static std::atomic<const Racy *> _seen[8]{ };
        std::thread threads[8];
        for ( uint32_t index = 0u; index < 8u; ++index )
        {
            threads[index] = std::thread( [index]( )
                {
                    while ( _go.load( ) == false )
                    {
                        std::this_thread::yield( );
                    }

                    static areg::NeverDestroyed<Racy> _holder;
                    _seen[index].store( &_holder.object( ) );
                } );
        }

        _go.store( true );
        for ( auto & thread : threads )
        {
            thread.join( );
        }

        AREG_CHILD_CHECK( _builds.load( ) == 1u );
        for ( const auto & seen : _seen )
        {
            AREG_CHILD_CHECK( seen.load( ) == _seen[0].load( ) );
        }
    );
}

/**
 * \brief   A holder with automatic storage leaves the object alive when it goes out of scope.
 **/
TEST( NeverDestroyedTest, holder_on_the_stack_does_not_run_the_destructor )
{
    AREG_EXPECT_IN_CHILD(
        {
            areg::NeverDestroyed<DestructorTrap> holder;
            static_cast<void>(holder.object( ));
        }
    );
}

/**
 * \brief   Two holders of one type keep their objects apart.
 **/
TEST( NeverDestroyedTest, holders_of_one_type_hold_separate_objects )
{
    static areg::NeverDestroyed<PlainData> _first;
    static areg::NeverDestroyed<PlainData> _second;

    _first.object( ).mFirst = 1u;
    _second.object( ).mFirst = 2u;

    EXPECT_NE( &_first.object( ), &_second.object( ) );
    EXPECT_EQ( _first.object( ).mFirst, 1u );
    EXPECT_EQ( _second.object( ).mFirst, 2u );
}

/**
 * \brief   The destructor of the object does not run when the process exits.
 **/
TEST( NeverDestroyedTest, destructor_does_not_run_at_process_exit )
{
    AREG_EXPECT_IN_CHILD(
        static_cast<void>(trap_holder().object());
    );
}
