/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/FileOsCoverageTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of the file object at the edges the system sets: the
 *              environment that names the special folders, a working directory that is gone,
 *              a file size limit, directory paths, and values outside the enumerations.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/base/File.hpp"
#include "areg/base/String.hpp"

#include <string>

#if defined(_POSIX)
    #include <pwd.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <sys/resource.h>
    #include <unistd.h>
#endif  // defined(_POSIX)

namespace
{
    //!< The mode of a new file that is read and written.
    constexpr uint32_t  CREATE_MODE { static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Create) };

    //!< Returns a name in the temporary folder that is unique for the running test.
    areg::String test_path( const char * suffix )
    {
        const std::string name{ std::string( "areg_fileos_" ) + ::testing::UnitTest::GetInstance( )->current_test_info( )->name( ) + suffix };
        return areg::File::make_full_path( areg::File::temp_dir( ), areg::String( name.c_str( ) ) );
    }
}

/**
 * \brief   A size that does not fit behind the cursor is refused and the file is unchanged.
 **/
TEST( FileOsCoverageTest, ensure_size_refuses_a_size_that_overflows )
{
    const areg::String path{ test_path( ".bin" ) };
    areg::File file;
    ASSERT_TRUE( file.open( path, CREATE_MODE ) );
    const uint8_t data[4]{ 1u, 2u, 3u, 4u };
    ASSERT_EQ( file.write( data, sizeof( data ) ), 4u );
    EXPECT_FALSE( file.ensure_size( 0xFFFFFFFFu ) );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_EQ( file.position( ), 4u );
    file.close( );
    EXPECT_TRUE( areg::File::delete_file( path ) );
}

/**
 * \brief   An origin or a folder outside the enumeration moves nothing and names nothing.
 **/
TEST( FileOsCoverageTest, values_outside_the_enumerations_do_nothing )
{
    const areg::String path{ test_path( ".bin" ) };
    areg::File file;
    ASSERT_TRUE( file.open( path, CREATE_MODE ) );
    const uint8_t data[4]{ 1u, 2u, 3u, 4u };
    ASSERT_EQ( file.write( data, sizeof( data ) ), 4u );
    EXPECT_EQ( file.set_position( 0, static_cast<areg::Cursor::SeekOrigin>(77) ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.position( ), 4u );
    file.close( );
    EXPECT_TRUE( areg::File::delete_file( path ) );

    EXPECT_TRUE( areg::File::special_dir( static_cast<areg::File::SpecialFolder>(77) ).is_empty( ) );
}

/**
 * \brief   A name ending with a dot has no extension and keeps the dot.
 **/
TEST( FileOsCoverageTest, name_ending_with_a_dot_has_no_extension )
{
    const areg::String path( "dir/name." );
    EXPECT_EQ( areg::File::file_name( path ), areg::String( "name." ) );
    EXPECT_TRUE( areg::File::file_extension( path ).is_empty( ) );
}

/**
 * \brief   The parent is found without asking for its position.
 **/
TEST( FileOsCoverageTest, find_parent_without_a_position )
{
    EXPECT_TRUE( areg::File::find_parent( "/root/dir/file", nullptr ) );
    EXPECT_FALSE( areg::File::find_parent( "file", nullptr ) );
}

/**
 * \brief   A file opened to write through to the disk keeps its data.
 **/
TEST( FileOsCoverageTest, direct_write_keeps_the_data )
{
    const areg::String path{ test_path( ".bin" ) };
    constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::WriteDirect) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
    areg::File file;
    ASSERT_TRUE( file.open( path, mode ) );
    const uint8_t data[4]{ 4u, 3u, 2u, 1u };
    EXPECT_EQ( file.write( data, sizeof( data ) ), 4u );
    ASSERT_TRUE( file.move_to_begin( ) );
    uint8_t read[4]{ };
    EXPECT_EQ( file.read( read, sizeof( read ) ), 4u );
    EXPECT_EQ( read[3], 1u );
    file.close( );
    EXPECT_TRUE( areg::File::delete_file( path ) );
}

#if defined(_POSIX)

/**
 * \brief   A directory path opened with create makes the missing directory but opens nothing,
 *          also when the directory exists already.
 **/
TEST( FileOsCoverageTest, directory_path_opened_with_create_makes_the_directory_and_opens_nothing )
{
    const areg::String dir{ test_path( "_dir" ) };
    areg::String path( dir );
    path += "/";
    constexpr uint32_t mode{ static_cast<uint32_t>(areg::File::OpenMode::Read) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
    {
        areg::File file;
        EXPECT_FALSE( file.open( path, mode ) );
        EXPECT_TRUE( areg::File::has_dir( dir ) );
    }
    {
        areg::File file;
        EXPECT_FALSE( file.open( path, mode ) );
        EXPECT_FALSE( file.is_opened( ) );
    }

    EXPECT_TRUE( areg::File::delete_dir( dir ) );
}

/**
 * \brief   A temporary name is composed of the folder and the prefix when no temporary file can
 *          be created there.
 **/
TEST( FileOsCoverageTest, temp_name_is_composed_when_no_temporary_file_can_be_made )
{
    const std::string longPrefix( areg::File::MAXIMUM_PATH + 8u, 'p' );
    const areg::String longName{ areg::File::temp_name( areg::String( longPrefix.c_str( ) ), false, true ) };
    EXPECT_EQ( areg::File::name_with_extension( longName ), areg::String( longPrefix.c_str( ) ) );

    const areg::String missing( "areg_fileos_missing_dir/name" );
    const areg::String inMissing{ areg::File::temp_name( missing, false, true ) };
    EXPECT_EQ( areg::File::name_with_extension( inMissing ), areg::String( "name" ) );
    EXPECT_FALSE( areg::File::has_file( inMissing ) );
}

/**
 * \brief   The home folder comes from the user database without HOME, and the temporary folder
 *          from TMPDIR, TMP or TEMP in this order.
 **/
TEST( FileOsCoverageTest, special_folders_follow_the_environment )
{
    AREG_EXPECT_IN_CHILD(
        AREG_CHILD_CHECK( ::unsetenv( "HOME" ) == 0 );
        const struct passwd * user{ ::getpwuid( ::getuid( ) ) };
        if ( user != nullptr )
        {
            AREG_CHILD_CHECK( areg::File::special_dir( areg::File::SpecialFolder::UserHome ) == areg::String( user->pw_dir ) );
        }

        AREG_CHILD_CHECK( ::unsetenv( "TMPDIR" ) == 0 );
        AREG_CHILD_CHECK( ::setenv( "TMP" , "/areg_tmp" , 1 ) == 0 );
        AREG_CHILD_CHECK( areg::File::special_dir( areg::File::SpecialFolder::Temp ) == areg::String( "/areg_tmp" ) );
        AREG_CHILD_CHECK( ::unsetenv( "TMP" ) == 0 );
        AREG_CHILD_CHECK( ::setenv( "TEMP" , "/areg_temp" , 1 ) == 0 );
        AREG_CHILD_CHECK( areg::File::special_dir( areg::File::SpecialFolder::Temp ) == areg::String( "/areg_temp" ) );
        AREG_CHILD_CHECK( ::unsetenv( "TEMP" ) == 0 );
        AREG_CHILD_CHECK( areg::File::special_dir( areg::File::SpecialFolder::Temp ) == areg::String( "~/tmp" ) );
    );
}

/**
 * \brief   An empty home folder makes the user mask the current directory.
 **/
TEST( FileOsCoverageTest, empty_home_makes_the_user_mask_the_current_directory )
{
    AREG_EXPECT_IN_CHILD(
        AREG_CHILD_CHECK( ::setenv( "HOME" , "" , 1 ) == 0 );
        AREG_CHILD_CHECK( areg::File::special_dir( areg::File::SpecialFolder::UserHome ).is_empty( ) );
        AREG_CHILD_CHECK( areg::File::normalize_path( areg::String( "%user%/name.txt" ) ) == areg::File::make_full_path( areg::File::current_dir( ) , areg::String( "name.txt" ) ) );
    );
}

#if !defined(__CYGWIN__)
/**
 * \brief   Without a working directory the path helpers return the path they were given.
 **/
TEST( FileOsCoverageTest, path_helpers_keep_the_path_when_the_working_directory_is_gone )
{
    AREG_EXPECT_IN_CHILD(
        const areg::String saved{ areg::File::current_dir( ) };
        const areg::String gone{ test_path( "_cwd" ) };
        AREG_CHILD_CHECK( areg::File::create_dir( gone ) );
        AREG_CHILD_CHECK( areg::File::set_current_dir( gone ) );
        AREG_CHILD_CHECK( ::rmdir( gone.as_string( ) ) == 0 );

        AREG_CHILD_CHECK( areg::File::current_dir( ).is_empty( ) );
        AREG_CHILD_CHECK( areg::File::file_full_path( areg::String( "name.txt" ) ) == areg::String( "name.txt" ) );
        AREG_CHILD_CHECK( areg::File::make_full_path( areg::String( "dir" ) , areg::String( "name.txt" ) ) == areg::String( "dir/name.txt" ) );
        AREG_CHILD_CHECK( areg::File::normalize_path( areg::String( "dir/name.txt" ) ) == areg::String( "dir/name.txt" ) );

        AREG_CHILD_CHECK( areg::File::set_current_dir( saved ) );
        AREG_CHILD_CHECK( areg::File::current_dir( ) == saved );
    );
}

/**
 * \brief   A write or a growth beyond the file size limit fails and leaves the file as it was;
 *          a write within the limit still works.
 **/
TEST( FileOsCoverageTest, write_beyond_the_file_size_limit_changes_nothing )
{
    AREG_EXPECT_IN_CHILD(
        const areg::String path{ test_path( ".bin" ) };
        areg::File file;
        AREG_CHILD_CHECK( file.open( path , CREATE_MODE ) );
        struct rlimit saved{ };
        AREG_CHILD_CHECK( ::getrlimit( RLIMIT_FSIZE , &saved ) == 0 );
        struct rlimit limit{ saved };
        limit.rlim_cur = 64u;
        AREG_CHILD_CHECK( ::signal( SIGXFSZ , SIG_IGN ) != SIG_ERR );
        AREG_CHILD_CHECK( ::setrlimit( RLIMIT_FSIZE , &limit ) == 0 );

        uint8_t data[128]{ };
        data[0] = 1u;
        const uint32_t written{ file.write( data , sizeof( data ) ) };
        const uint32_t afterWrite{ file.position( ) };
        const uint32_t reserved{ file.reserve( 1024u ) };
        const bool ensured{ file.ensure_size( 1024u ) };
        const uint32_t small{ file.write( data , 16u ) };
        AREG_CHILD_CHECK( ::setrlimit( RLIMIT_FSIZE , &saved ) == 0 );

        AREG_CHILD_CHECK( written == 0u );
        AREG_CHILD_CHECK( afterWrite == 0u );
        AREG_CHILD_CHECK( reserved == areg::Cursor::INVALID_CURSOR_POSITION );
        AREG_CHILD_CHECK( ensured == false );
        AREG_CHILD_CHECK( small == 16u );
        AREG_CHILD_CHECK( file.length( ) <= 64u );
        file.close( );
        AREG_CHILD_CHECK( areg::File::delete_file( path ) );
    );
}
#endif  // !defined(__CYGWIN__)

#endif  // defined(_POSIX)
