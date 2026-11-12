/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/FileTest.Cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Tests of File object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/Containers.hpp"
#include "areg/logging/areg_log.h"
#include "areg/logging/LoggingDefs.hpp"

#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <string>
#include <filesystem>

#ifdef WINDOWS
    #include <Shlwapi.h>

    #ifdef _MSC_VER
        #pragma comment(lib, "Shlwapi")
    #endif // _MSC_VER
#endif // WINDOWS

/**
 * \brief   Makes sure that the system has no problem to read file from file system.
 *          The test is using STL objects, which suppose to work under any platform.
 **/
TEST( FileTest, stl_file_read )
{
    areg::Application::set_working_directory( nullptr );

    constexpr char fileName[]{ "./config/areg.init" };
    std::error_code err{};
    ASSERT_TRUE( std::filesystem::is_regular_file( fileName, err ) );
    ASSERT_FALSE( static_cast<bool>(err) );

    std::fstream file;
    file.open( fileName, std::ios::in );
    ASSERT_TRUE( file.is_open( ) );

    char buffer[ 1024 ]{ 0 };
    ASSERT_TRUE( file.read( buffer, 1024 ) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    file.close( );
}

/**
 * \brief   Makes sure that the system has no problem to read and write file from file system.
 *          The test is using STL objects, which suppose to work under any platform.
 **/
TEST( FileTest, stl_file_read_write )
{
    areg::Application::set_working_directory( nullptr );

    std::fstream fileRead, fileWrite;
    constexpr char fileNameRead[ ]{ "./config/areg.init" };
    constexpr char fileNameWrite[ ]{ "./write_with_stl.txt" };

    fileRead.open( fileNameRead, std::ios::in );
    ASSERT_TRUE( fileRead.is_open( ) );

    char buffer[ 1024 ]{ 0 };
    ASSERT_TRUE( fileRead.read( buffer, 1024 ) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    fileRead.close( );

    fileWrite.open( fileNameWrite, std::ios::out);
    ASSERT_TRUE( fileWrite.is_open( ) );
    ASSERT_TRUE( fileWrite.write( buffer, 1024 ) );
    fileWrite.close( );

    std::error_code err{};
    ASSERT_TRUE( std::filesystem::is_regular_file( fileNameWrite, err ) );
    ASSERT_FALSE( static_cast<bool>(err) );
}

/**
 * \brief   Makes sure that the system has no problem to read file from file system.
 *          The test is using Win32 API and supposed to work only on Windows system.
 **/
TEST( FileTest, win32_file_read )
{
#if defined(WINDOWS) && !defined(_MINGW)

    areg::Application::set_working_directory( nullptr );

    constexpr char fileName[ ]{ "./config/areg.init" };
    ASSERT_TRUE( PathFileExistsA(fileName) );

    HANDLE hFile = ::CreateFileA( fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, nullptr );
    ASSERT_TRUE( hFile != nullptr );

    char buffer[ 1024 ]{ 0 };
    DWORD dwRead{ 0 };
    ASSERT_TRUE( ::ReadFile( hFile, buffer, 1024, &dwRead, nullptr ) );
    ASSERT_EQ( dwRead, 1024u );
    ASSERT_EQ( buffer[ 0 ], '#' );

    ASSERT_TRUE(::CloseHandle( hFile ));

#endif // defined(WINDOWS) && !swdinws(_MINGW)
}

/**
 * \brief   Makes sure that the system has no problem to read and write file from file system.
 *          The test is using Win32 API and supposed to work only on Windows system.
 **/
TEST( FileTest, win32_file_read_write )
{
#if defined(WINDOWS) && !defined(_MINGW)

    areg::Application::set_working_directory( nullptr );

    constexpr char fileNameRead[ ]{ "./config/areg.init" };
    constexpr char fileNameWrite[ ]{ "./write_with_win32.txt" };

    HANDLE hFileRead = ::CreateFileA( fileNameRead, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, nullptr );
    ASSERT_TRUE( hFileRead != nullptr );

    char buffer[ 1024 ]{ 0 };
    DWORD dwRead{ 0 };
    ASSERT_TRUE( ::ReadFile( hFileRead, buffer, 1024, &dwRead, nullptr ) );
    ASSERT_EQ( dwRead, 1024u );
    ASSERT_EQ( buffer[ 0 ], '#' );
    ASSERT_TRUE( ::CloseHandle( hFileRead ));

    HANDLE hFileWrite = ::CreateFileA( fileNameWrite, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
    ASSERT_TRUE( hFileWrite != nullptr );
    DWORD dwWrite{ 0 };
    ASSERT_TRUE( ::WriteFile( hFileWrite, buffer, 1024, &dwWrite, nullptr ) );
    ASSERT_EQ( dwWrite, 1024u );

    ASSERT_TRUE( ::CloseHandle( hFileWrite ));
    ASSERT_TRUE( PathFileExistsA( fileNameWrite ) );

#endif // WINDOWS
}

/**
 * \brief   The test checks file existence using File API.
 **/
TEST( FileTest, check_file_existence )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileName{ "./config/areg.init" };
    const areg::String fileWrong{ "./config/blah-blah.init" };
    ASSERT_TRUE( areg::File::has_file( fileName.as_string( ) ) );
    ASSERT_FALSE( areg::File::has_file( fileWrong ) );
}

/**
 * \brief   The test checks basic functionality of the file path normalization.
 **/
TEST( FileTest, normalize_file_path_basic )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileName{ "./config/areg.init" };
    areg::String normalized = areg::File::normalize_path( fileName );
    ASSERT_TRUE( normalized.length( ) > fileName.length( ) );
    ASSERT_TRUE( normalized.ends_with( "areg.init" ) );
}

/**
 * \brief   The test checks file open functionality using File object API.
 **/
TEST( FileTest, file_open_basic )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileName{ "./config/areg.init" };
    constexpr unsigned int mode{  static_cast<uint32_t>(areg::File::OpenMode::Read) 
                                | static_cast<uint32_t>(areg::File::OpenMode::Text) 
                                | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };
    areg::File file( fileName, mode );

    ASSERT_TRUE( areg::File::has_file( fileName.as_string( ) ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.position( ), static_cast<uint32_t>(0) );

    file.close( );
}

/**
 * \brief   The test checks creating an empty file by using File object API.
 **/
TEST( FileTest, create_empty_file )
{
    areg::Application::set_working_directory( nullptr );
    const areg::String fileNameWrite{ "./empty_file_areg.txt" };
    constexpr uint32_t modeWrite{ static_cast<uint32_t>(areg::File::OpenMode::Read) 
                                | static_cast<uint32_t>(areg::File::OpenMode::Text) 
                                | static_cast<uint32_t>(areg::File::OpenMode::Create) 
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) 
                                | static_cast<uint32_t>(areg::File::OpenMode::Write) };

    areg::File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    fileWrite.close( );
    ASSERT_TRUE( areg::File::has_file( fileNameWrite ) );
}

/**
 * \brief   The test checks the basic functionality of opening and reading a file by using File object API.
 **/
TEST( FileTest, file_read_basic )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileName{ "./config/areg.init" };
    constexpr uint32_t mode { static_cast<uint32_t>(areg::File::OpenMode::Read)
                            | static_cast<uint32_t>(areg::File::OpenMode::Text) 
                            | static_cast<uint32_t>(areg::File::OpenMode::Exist) 
                            | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };
    areg::File file( fileName, mode );

    ASSERT_TRUE(areg::File::has_file( fileName.as_string( ) ));
    ASSERT_TRUE( file.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(file.read_string( buffer, 1025 ));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );

    file.close( );
}

/**
 * \brief   The test checks the basic functionality of opening, reading and writing a file by using File object API.
 **/
TEST( FileTest, file_read_write_basic )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileNameRead{ "./config/areg.init" };
    const areg::String fileNameWrite{ "./write_with_areg.txt" };

    constexpr uint32_t modeRead { static_cast<uint32_t>(areg::File::OpenMode::Read)
                                | static_cast<uint32_t>(areg::File::OpenMode::Text)
                                | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };
    constexpr uint32_t modeWrite{ static_cast<uint32_t>(areg::File::OpenMode::Read)
                                | static_cast<uint32_t>(areg::File::OpenMode::Text)
                                | static_cast<uint32_t>(areg::File::OpenMode::Create)
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead)
                                | static_cast<uint32_t>(areg::File::OpenMode::Write) };

    areg::File fileRead( fileNameRead.as_string( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.read_string( buffer, 1025 ));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    areg::File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    ASSERT_TRUE( fileWrite.write_string( buffer ) );

    fileWrite.close( );

    ASSERT_TRUE( areg::File::has_file( fileNameWrite ) );
}

/**
 * \brief   The test checks the basic functionality of creating folders cascaded
 *          and deleting them by using File object API.
 **/
TEST( FileTest, create_folder_cascaded )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String dirPath( "./dir1/dir2/dir3/" );
    ASSERT_FALSE( areg::File::has_dir( dirPath ) );
    ASSERT_TRUE( areg::File::create_dir_cascaded( dirPath ) );
    ASSERT_TRUE( areg::File::has_dir( dirPath ) );
    ASSERT_TRUE( areg::File::delete_dir( "./dir1/" ) );
    ASSERT_FALSE( areg::File::has_dir( "./dir1/" ) );
}

/**
 * \brief   The test checks the functionality read an existing file and
 *          wring that into another file located in non-existing folder.
 *          The test will create subfolder and create a file inside.
 **/
TEST( FileTest, file_read_and_write_in_subfolder )
{
    areg::Application::set_working_directory( nullptr );

    const areg::String fileNameRead { "./config/areg.init" };
    const areg::String fileNameWrite{ "./logs/write_with_areg.txt" };

    constexpr uint32_t modeRead { static_cast<uint32_t>(areg::File::OpenMode::Read)
                                | static_cast<uint32_t>(areg::File::OpenMode::Text)
                                | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };

    constexpr uint32_t modeWrite{ static_cast<uint32_t>(areg::File::OpenMode::Read)
                                | static_cast<uint32_t>(areg::File::OpenMode::Text)
                                | static_cast<uint32_t>(areg::File::OpenMode::Create)
                                | static_cast<uint32_t>(areg::File::OpenMode::ShareRead)
                                | static_cast<uint32_t>(areg::File::OpenMode::Write) };

    areg::File fileRead( fileNameRead.as_string( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.read_string(buffer, 1025));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    areg::File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open() );
    ASSERT_TRUE(fileWrite.write_string(buffer));

    fileWrite.close( );

    ASSERT_TRUE( areg::File::has_file(fileNameWrite) );
}

/**
 * \brief   The test checks that is_eof() is false before the end of file and true at the end.
 **/
TEST( FileTest, is_eof_at_end_of_file )
{
    const std::filesystem::path path{ std::filesystem::temp_directory_path() / "areg_is_eof_test.bin" };
    {
        std::ofstream out( path, std::ios::binary | std::ios::trunc );
        out << "0123456789";
    }

    constexpr uint32_t mode { static_cast<uint32_t>(areg::File::OpenMode::Read)
                            | static_cast<uint32_t>(areg::File::OpenMode::Binary)
                            | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                            | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };
    areg::File file( areg::String( path.string().c_str() ), mode );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.length( ), static_cast<uint64_t>(10) );
    ASSERT_FALSE( file.is_eof( ) );

    file.set_position( 0, areg::Cursor::SeekOrigin::End );
    ASSERT_TRUE( file.is_eof( ) );

    file.close( );
    std::filesystem::remove( path );
}

#ifdef POSIX
/**
 * \brief   The test checks that length() returns the size of a sparse file larger than 4 GB.
 **/
TEST( FileTest, length_beyond_4gb )
{
    constexpr uint64_t fileSize{ 5ull * 1024ull * 1024ull * 1024ull };
    const std::filesystem::path path{ std::filesystem::temp_directory_path() / "areg_length_5gb_test.bin" };
    {
        std::ofstream out( path, std::ios::binary | std::ios::trunc );
    }

    std::error_code err;
    std::filesystem::resize_file( path, fileSize, err );
    if (err)
    {
        std::filesystem::remove( path );
        GTEST_SKIP( ) << "Cannot create a sparse file of 5 GB: " << err.message( );
    }

    constexpr uint32_t mode { static_cast<uint32_t>(areg::File::OpenMode::Read)
                            | static_cast<uint32_t>(areg::File::OpenMode::Binary)
                            | static_cast<uint32_t>(areg::File::OpenMode::Exist)
                            | static_cast<uint32_t>(areg::File::OpenMode::ShareRead) };
    areg::File file( areg::String( path.string().c_str() ), mode );
    const bool opened{ file.open( ) };
    const uint64_t length{ opened ? file.length( ) : 0u };
    file.close( );
    std::filesystem::remove( path );

    ASSERT_TRUE( opened );
    ASSERT_EQ( length, fileSize );
}
#endif // POSIX

/**
 * \brief   Every temporary name is a new file that no other call returns.
 **/
TEST( FileTest, temp_name_is_unique )
{
    const areg::String first{ areg::File::temp_name( ) };
    const areg::String second{ areg::File::temp_name( ) };
    const bool firstExists{ areg::File::has_file( first ) };
    const bool secondExists{ areg::File::has_file( second ) };
    areg::File::delete_file( first );
    areg::File::delete_file( second );

    EXPECT_NE( first, second );
    EXPECT_TRUE( firstExists );
    EXPECT_TRUE( secondExists );
}

/**
 * \brief   A file is locked and unlocked, and an exclusive lock needs write access.
 **/
TEST( FileTest, lock_file_needs_matching_access )
{
    const std::filesystem::path path{ std::filesystem::temp_directory_path( ) / "areg_lock_file_test.lock" };
    constexpr uint32_t modeWrite{ static_cast<uint32_t>(areg::File::OpenMode::ShareWrite)
                                | static_cast<uint32_t>(areg::File::OpenMode::OpenAlways) };
    constexpr uint32_t modeRead { static_cast<uint32_t>(areg::File::OpenMode::Read)
                                | static_cast<uint32_t>(areg::File::OpenMode::Exist) };
    {
        areg::File file( areg::String( path.string( ).c_str( ) ), modeWrite );
        ASSERT_TRUE( file.open( ) );
        EXPECT_TRUE( file.lock_file( true ) );
        file.unlock_file( );
        EXPECT_TRUE( file.lock_file( false ) );
        file.unlock_file( );
    }
    {
        areg::File file( areg::String( path.string( ).c_str( ) ), modeRead );
        ASSERT_TRUE( file.open( ) );
        EXPECT_FALSE( file.lock_file( true ) );
        EXPECT_TRUE( file.lock_file( false ) );
        file.unlock_file( );
    }

    std::filesystem::remove( path );
}

//////////////////////////////////////////////////////////////////////////
// Open modes, data access and state of the File object
//////////////////////////////////////////////////////////////////////////

namespace
{
    using OpenMode = areg::File::OpenMode;

    constexpr uint32_t mode_of( OpenMode a )
    {
        return static_cast<uint32_t>(a);
    }

    constexpr uint32_t mode_of( OpenMode a, OpenMode b )
    {
        return (static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr uint32_t mode_of( OpenMode a, OpenMode b, OpenMode c )
    {
        return (static_cast<uint32_t>(a) | static_cast<uint32_t>(b) | static_cast<uint32_t>(c));
    }

    //!< A directory in the system temp folder, unique for the running test, deleted with the object.
    class TempFolder
    {
    public:
        TempFolder( void )
            : mDir  ( std::filesystem::temp_directory_path( ) / (std::string("areg_file_") + ::testing::UnitTest::GetInstance( )->current_test_info( )->name( )) )
        {
            std::error_code err;
            std::filesystem::remove_all( mDir, err );
            std::filesystem::create_directories( mDir, err );
        }

        ~TempFolder( void )
        {
            std::error_code err;
            std::filesystem::remove_all( mDir, err );
        }

        areg::String path( const char * name ) const
        {
            return areg::String( (mDir / name).string( ).c_str( ) );
        }

        const std::filesystem::path & dir( void ) const
        {
            return mDir;
        }

    private:
        std::filesystem::path   mDir;
    };

    std::string file_content( const std::filesystem::path & path )
    {
        std::ifstream in( path, std::ios::binary );
        return std::string( std::istreambuf_iterator<char>( in ), std::istreambuf_iterator<char>( ) );
    }

    void put_content( const areg::String & path, const std::string & content )
    {
        std::ofstream out( path.data( ), std::ios::binary | std::ios::trunc );
        out << content;
    }
}

/**
 * \brief   Opening an existing file with the Exist flag fails if the file is missing and leaves no file.
 **/
TEST( FileTest, open_missing_file_with_exist_flag_fails )
{
    TempFolder tmp;
    areg::File file( tmp.path( "missing.bin" ), mode_of( OpenMode::Read, OpenMode::Exist ) );
    EXPECT_FALSE( file.open( ) );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "missing.bin" ) ) );
}

/**
 * \brief   Opening a file without the name or with an empty name fails.
 **/
TEST( FileTest, open_without_name_fails )
{
    areg::File file;
    EXPECT_FALSE( file.open( ) );
    EXPECT_FALSE( file.open( areg::String( ), mode_of( OpenMode::OpenAlways ) ) );
    EXPECT_FALSE( file.is_opened( ) );
}

/**
 * \brief   OpenAlways creates a missing file and keeps the data of an existing file.
 **/
TEST( FileTest, open_always_creates_and_keeps_data )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "always.bin" ) };
    {
        areg::File file( path, mode_of( OpenMode::OpenAlways, OpenMode::Binary ) );
        ASSERT_TRUE( file.open( ) );
        EXPECT_TRUE( file.can_write( ) );
        EXPECT_EQ( file.write( reinterpret_cast<const uint8_t *>("12345"), 5 ), 5u );
    }

    ASSERT_EQ( file_content( tmp.dir( ) / "always.bin" ), "12345" );
    areg::File again( path, mode_of( OpenMode::OpenAlways, OpenMode::Binary ) );
    ASSERT_TRUE( again.open( ) );
    EXPECT_EQ( again.length( ), 5u );
}

/**
 * \brief   The Create flag makes an existing file empty.
 **/
TEST( FileTest, create_flag_truncates_existing_file )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "create.bin" ) };
    put_content( path, "old content" );

    areg::File file( path, mode_of( OpenMode::Create, OpenMode::Write, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_EQ( file.write( reinterpret_cast<const uint8_t *>("new"), 3 ), 3u );
    file.close( );
    EXPECT_EQ( file_content( tmp.dir( ) / "create.bin" ), "new" );
}

/**
 * \brief   Truncate mode empties an existing file; Write mode keeps the data and writes from the begin.
 **/
TEST( FileTest, truncate_and_write_modes_differ_in_existing_data )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "modes.bin" ) };

    put_content( path, "abcdefgh" );
    {
        areg::File file( path, mode_of( OpenMode::Write, OpenMode::Exist, OpenMode::Binary ) );
        ASSERT_TRUE( file.open( ) );
        EXPECT_EQ( file.length( ), 8u );
        EXPECT_EQ( file.write( reinterpret_cast<const uint8_t *>("XY"), 2 ), 2u );
    }
    EXPECT_EQ( file_content( tmp.dir( ) / "modes.bin" ), "XYcdefgh" );

    {
        areg::File file( path, mode_of( OpenMode::Truncate, OpenMode::Exist, OpenMode::Binary ) );
        ASSERT_TRUE( file.open( ) );
        EXPECT_EQ( file.length( ), 0u );
    }
    EXPECT_EQ( file_content( tmp.dir( ) / "modes.bin" ), "" );
}

/**
 * \brief   A file opened for reading only does not write; the data is not changed.
 **/
TEST( FileTest, read_only_mode_rejects_write )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "readonly.bin" ) };
    put_content( path, "abcd" );

    areg::File file( path, mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.can_read( ) );
    EXPECT_FALSE( file.can_write( ) );
    EXPECT_EQ( file.write( reinterpret_cast<const uint8_t *>("zz"), 2 ), 0u );
    EXPECT_FALSE( file.truncate( ) );
    EXPECT_EQ( file.reserve( 100 ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_FALSE( file.ensure_size( 100 ) );
    EXPECT_FALSE( file.lock_file( true ) );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_EQ( file_content( tmp.dir( ) / "readonly.bin" ), "abcd" );
}

/**
 * \brief   A directory cannot be opened as a file for writing.
 **/
TEST( FileTest, open_directory_for_writing_fails )
{
    TempFolder tmp;
    areg::File file( areg::String( tmp.dir( ).string( ).c_str( ) ), mode_of( OpenMode::OpenAlways, OpenMode::Binary ) );
    EXPECT_FALSE( file.open( ) );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_TRUE( areg::File::has_dir( areg::String( tmp.dir( ).string( ).c_str( ) ) ) );
}

#ifdef POSIX
/**
 * \brief   A file without the write permission is not opened for writing.
 **/
TEST( FileTest, open_file_without_write_permission_fails )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "locked.bin" ) };
    put_content( path, "abcd" );
    std::error_code err;
    std::filesystem::permissions( path.data( ), std::filesystem::perms::owner_read, std::filesystem::perm_options::replace, err );
    ASSERT_FALSE( static_cast<bool>(err) );

    // a privileged user ignores the permission
    std::ofstream probe( path.data( ), std::ios::binary | std::ios::app );
    if ( probe.is_open( ) )
    {
        probe.close( );
        GTEST_SKIP( ) << "The file permissions are not enforced for this user.";
    }

    areg::File write( path, mode_of( OpenMode::Write, OpenMode::Exist, OpenMode::Binary ) );
    EXPECT_FALSE( write.open( ) );

    areg::File read( path, mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) );
    EXPECT_TRUE( read.open( ) );
}
#endif // POSIX

/**
 * \brief   Written data is read back after the cursor moves to the begin; the cursor follows read and write.
 **/
TEST( FileTest, write_seek_read_round_trip )
{
    TempFolder tmp;
    areg::File file( tmp.path( "data.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_EQ( file.position( ), 0u );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("0123456789"), 10 ), 10u );
    EXPECT_EQ( file.position( ), 10u );
    EXPECT_EQ( file.length( ), 10u );

    EXPECT_EQ( file.set_position( 2, areg::Cursor::SeekOrigin::Begin ), 2u );
    EXPECT_EQ( file.set_position( 3, areg::Cursor::SeekOrigin::Current ), 5u );
    EXPECT_EQ( file.set_position( -1, areg::Cursor::SeekOrigin::End ), 9u );

    ASSERT_TRUE( file.move_to_begin( ) );
    char text[ 16 ]{ };
    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 4 ), 4u );
    EXPECT_STREQ( text, "0123" );
    EXPECT_EQ( file.position( ), 4u );

    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 16 ), 6u );
    EXPECT_TRUE( file.is_eof( ) );
    EXPECT_EQ( file.read( reinterpret_cast<uint8_t *>(text), 16 ), 0u );
}

/**
 * \brief   Reading or writing with a null buffer or zero size does nothing.
 **/
TEST( FileTest, read_write_invalid_arguments_do_nothing )
{
    TempFolder tmp;
    areg::File file( tmp.path( "args.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    uint8_t byte{ 1 };
    EXPECT_EQ( file.write( nullptr, 4 ), 0u );
    EXPECT_EQ( file.write( &byte, 0 ), 0u );
    EXPECT_EQ( file.length( ), 0u );
    ASSERT_EQ( file.write( &byte, 1 ), 1u );
    ASSERT_TRUE( file.move_to_begin( ) );
    EXPECT_EQ( file.read( nullptr, 4 ), 0u );
    EXPECT_EQ( file.read( &byte, 0 ), 0u );
    EXPECT_EQ( file.position( ), 0u );
}

/**
 * \brief   After close the object is closed and every operation reports a failure; closing twice is safe.
 **/
TEST( FileTest, closed_file_reports_failures_and_double_close_is_safe )
{
    TempFolder tmp;
    areg::File file( tmp.path( "closed.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("ab"), 2 ), 2u );

    file.close( );
    file.close( );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_FALSE( file.is_valid( ) );

    uint8_t byte{ 0 };
    EXPECT_EQ( file.position( ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_EQ( file.reserve( 8 ), areg::Cursor::INVALID_CURSOR_POSITION );
    EXPECT_FALSE( file.truncate( ) );
    EXPECT_FALSE( file.ensure_size( 8 ) );
    EXPECT_FALSE( file.lock_file( false ) );
    EXPECT_FALSE( file.remove( ) );
    EXPECT_EQ( file.write( &byte, 1 ), 0u );
    EXPECT_EQ( file.read( &byte, 1 ), 0u );
    file.flush( );
    file.unlock_file( );

    EXPECT_EQ( file_content( tmp.dir( ) / "closed.bin" ), "ab" );
}

/**
 * \brief   A second open() of an opened file reports success and keeps the same file; open(name, mode) of an
 *          opened file fails.
 **/
TEST( FileTest, open_of_opened_file )
{
    TempFolder tmp;
    areg::File file( tmp.path( "twice.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("ab"), 2 ), 2u );
    EXPECT_TRUE( file.open( ) );
    EXPECT_EQ( file.position( ), 2u );
    EXPECT_FALSE( file.open( tmp.path( "other.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "other.bin" ) ) );
}

/**
 * \brief   open(name, mode) opens the file by the name and the mode; after close the object opens another file.
 **/
TEST( FileTest, open_with_name_and_mode_reopens_other_file )
{
    TempFolder tmp;
    put_content( tmp.path( "first.bin" ), "11" );
    put_content( tmp.path( "second.bin" ), "2222" );

    areg::File file;
    ASSERT_TRUE( file.open( tmp.path( "first.bin" ), mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) ) );
    EXPECT_EQ( file.length( ), 2u );
    file.close( );
    ASSERT_TRUE( file.open( tmp.path( "second.bin" ), mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) ) );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_TRUE( file.name( ).ends_with( "second.bin" ) );
}

/**
 * \brief   truncate() makes the file empty and keeps it opened for writing.
 **/
TEST( FileTest, truncate_empties_the_file )
{
    TempFolder tmp;
    areg::File file( tmp.path( "trunc.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("abcdef"), 6 ), 6u );
    ASSERT_TRUE( file.truncate( ) );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_TRUE( file.is_opened( ) );
    EXPECT_EQ( file_content( tmp.dir( ) / "trunc.bin" ), "" );
}

/**
 * \brief   reserve() grows the file zero filled keeping the cursor; shrinking moves the cursor to the end;
 *          zero size empties the file.
 **/
TEST( FileTest, reserve_grows_and_shrinks )
{
    TempFolder tmp;
    areg::File file( tmp.path( "reserve.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("abcd"), 4 ), 4u );

    EXPECT_EQ( file.reserve( 16 ), 4u );
    EXPECT_EQ( file.length( ), 16u );
    EXPECT_EQ( file.position( ), 4u );

    EXPECT_EQ( file.reserve( 2 ), 2u );
    EXPECT_EQ( file.length( ), 2u );
    EXPECT_EQ( file.position( ), 2u );

    EXPECT_EQ( file.reserve( 0 ), 0u );
    EXPECT_EQ( file.length( ), 0u );
    EXPECT_EQ( file.position( ), 0u );
}

/**
 * \brief   ensure_size() leaves the file as it is when the free space is enough.
 **/
TEST( FileTest, ensure_size_keeps_file_when_space_is_enough )
{
    TempFolder tmp;
    areg::File file( tmp.path( "ensure.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("abcd"), 4 ), 4u );
    ASSERT_TRUE( file.move_to_begin( ) );

    EXPECT_TRUE( file.ensure_size( 2 ) );
    EXPECT_EQ( file.length( ), 4u );
}

/**
 * \brief   ensure_size() extends the file and reports true when the free space after the cursor is not enough.
 **/
TEST( FileTest, ensure_size_extends_when_needed )
{
    TempFolder tmp;
    areg::File file( tmp.path( "ensure.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("abcd"), 4 ), 4u );
    ASSERT_TRUE( file.move_to_begin( ) );

    EXPECT_TRUE( file.ensure_size( 10 ) );
    EXPECT_EQ( file.length( ), 10u );
    EXPECT_EQ( file.position( ), 0u );
}

/**
 * \brief   ensure_size() refuses a size past the 32-bit file position and leaves the file as it is.
 **/
TEST( FileTest, ensure_size_refuses_a_size_past_the_position_limit )
{
    TempFolder tmp;
    areg::File file( tmp.path( "ensure.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>("abcd"), 4 ), 4u );

    EXPECT_FALSE( file.ensure_size( (std::numeric_limits<uint32_t>::max)( ) ) );
    EXPECT_EQ( file.length( ), 4u );
    EXPECT_EQ( file.position( ), 4u );
}

/**
 * \brief   A last dot followed by a character that is not a letter or a digit starts no extension.
 **/
TEST( FileTest, dot_before_a_non_alphanumeric_starts_no_extension )
{
    const areg::String path( "folder/archive.-v2" );
    EXPECT_STREQ( areg::File::file_name( path ).as_string( ), "archive.-v2" );
    EXPECT_TRUE( areg::File::file_extension( path ).is_empty( ) );
}

/**
 * \brief   resize_and_fill() sets the size of the file and fills the new space with the byte.
 **/
TEST( FileTest, resize_and_fill_sets_bytes )
{
    TempFolder tmp;
    areg::File file( tmp.path( "fill.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    file.resize_and_fill( 8, 0xAB );
    EXPECT_EQ( file.length( ), 8u );
    file.close( );
    EXPECT_EQ( file_content( tmp.dir( ) / "fill.bin" ), std::string( 8, static_cast<char>(0xAB) ) );
}

/**
 * \brief   remove() deletes the file from the disk and resets the object.
 **/
TEST( FileTest, remove_deletes_the_file )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "remove.bin" ) };
    areg::File file( path, mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( areg::File::has_file( path ) );

    EXPECT_TRUE( file.remove( ) );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_FALSE( areg::File::has_file( path ) );
    EXPECT_TRUE( file.name( ).is_empty( ) );
    EXPECT_EQ( file.mode( ), mode_of( OpenMode::Invalid ) );
}

/**
 * \brief   A file opened with the Delete flag disappears when it is closed.
 **/
TEST( FileTest, delete_flag_removes_file_on_close )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "delete.bin" ) };
    {
        areg::File file( path, mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Delete ) );
        ASSERT_TRUE( file.open( ) );
        EXPECT_TRUE( file.is_force_delete( ) );
        EXPECT_TRUE( areg::File::has_file( path ) );
        file.close( );
        EXPECT_FALSE( areg::File::has_file( path ) );
    }
}

#ifdef POSIX
/**
 * \brief   A temporary file is created from a name template and gets the unique name.
 **/
TEST( FileTest, create_temp_file_gets_unique_name )
{
    TempFolder tmp;
    const areg::String pattern{ tmp.path( "tmpXXXXXX" ) };
    areg::File file( pattern, mode_of( OpenMode::CreateTemp ) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.is_temporary( ) );
    EXPECT_NE( file.name( ), pattern );
    EXPECT_FALSE( file.name( ).is_valid_position( file.name( ).find_first( "XXXXXX" ) ) );
    EXPECT_TRUE( areg::File::has_file( file.name( ) ) );
    EXPECT_TRUE( file.remove( ) );
}
#endif // POSIX

/**
 * \brief   Data written through the typed functions is stored in the file and read back after the file is reopened.
 **/
TEST( FileTest, typed_values_survive_reopen )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "typed.bin" ) };
    {
        areg::File file( path, mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
        ASSERT_TRUE( file.open( ) );
        ASSERT_TRUE( file.write_int( 1234 ) );
        ASSERT_TRUE( file.write_double( 6.25 ) );
        ASSERT_TRUE( file.write_string( "name" ) );
        ASSERT_EQ( file.write( areg::String( "text" ) ), 4u + 4u );
    }

    areg::File file( path, mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    int32_t iv{ 0 };
    double dv{ 0 };
    EXPECT_TRUE( file.read_int( iv ) );
    EXPECT_TRUE( file.read_double( dv ) );
    EXPECT_EQ( iv, 1234 );
    EXPECT_EQ( dv, 6.25 );
    char name[ 16 ]{ };
    EXPECT_GE( file.read_string( name, 16 ), 4 );
    EXPECT_STREQ( name, "name" );
}

/**
 * \brief   A shared buffer is stored in a file with its size.
 **/
TEST( FileTest, shared_buffer_is_stored_with_its_size )
{
    TempFolder tmp;
    areg::File file( tmp.path( "objects.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );

    areg::SharedBuffer source;
    ASSERT_EQ( source.write( reinterpret_cast<const uint8_t *>("payload"), 7 ), 7u );
    EXPECT_EQ( file.write( source ), 7u + sizeof(int32_t) );
    EXPECT_EQ( file.length( ), 7u + sizeof(int32_t) );
    ASSERT_TRUE( file.move_to_begin( ) );
    int32_t size{ 0 };
    ASSERT_TRUE( file.read_int( size ) );
    EXPECT_EQ( size, 7 );
}

/**
 * \brief   A shared buffer stored in a file is read back with the same content.
 **/
TEST( FileTest, shared_buffer_round_trip )
{
    TempFolder tmp;
    areg::File file( tmp.path( "objects.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );

    areg::SharedBuffer source;
    ASSERT_EQ( source.write( reinterpret_cast<const uint8_t *>("payload"), 7 ), 7u );
    ASSERT_EQ( file.write( source ), 7u + sizeof(int32_t) );
    ASSERT_TRUE( file.move_to_begin( ) );

    areg::SharedBuffer result;
    EXPECT_EQ( file.read( result ), 7u + sizeof(int32_t) );
    ASSERT_EQ( result.size_used( ), 7u );
    EXPECT_EQ( std::memcmp( result.buffer( ), "payload", 7 ), 0 );
}

/**
 * \brief   A string object is stored in a file and read back.
 **/
TEST( FileTest, string_object_round_trip )
{
    TempFolder tmp;
    areg::File file( tmp.path( "string.bin" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    const areg::String ascii( "ascii" );
    EXPECT_EQ( file.write( ascii ), 4u + 5u );
    const areg::WideString wide( L"wide" );
    EXPECT_EQ( file.write( wide ), 4u + 4u * 2u );

    ASSERT_TRUE( file.move_to_begin( ) );
    areg::String back;
    areg::WideString wideBack;
    EXPECT_EQ( file.read( back ), 4u + 5u );
    EXPECT_EQ( file.read( wideBack ), 4u + 4u * 2u );
    EXPECT_STREQ( back.as_string( ), "ascii" );
    EXPECT_EQ( wideBack, wide );
}

/**
 * \brief   Lines are written to a text file and read back one by one with every line ending.
 **/
TEST( FileTest, text_lines_round_trip )
{
    TempFolder tmp;
    const areg::String path{ tmp.path( "lines.txt" ) };
    put_content( path, "first\nsecond\r\n\nlast" );

    areg::File file( path, mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Text ) );
    ASSERT_TRUE( file.open( ) );
    areg::String line;
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "first" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "second" );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_TRUE( line.is_empty( ) );
    EXPECT_GT( file.read_line( line ), 0 );
    EXPECT_STREQ( line.as_string( ), "last" );
    EXPECT_EQ( file.read_line( line ), 0 );
}

/**
 * \brief   write_line() adds the line end, search finds the text in the file.
 **/
TEST( FileTest, write_line_and_search_text )
{
    TempFolder tmp;
    areg::File file( tmp.path( "search.txt" ), mode_of( OpenMode::Write, OpenMode::Create, OpenMode::Text ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_TRUE( file.write_line( "alpha" ) );
    ASSERT_TRUE( file.write_line( "Beta" ) );
    ASSERT_TRUE( file.write_line( areg::String( "gamma" ) ) );
    file.close( );
    EXPECT_EQ( file_content( tmp.dir( ) / "search.txt" ), "alpha\nBeta\ngamma\n" );

    areg::File reader( tmp.path( "search.txt" ), mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Text ) );
    ASSERT_TRUE( reader.open( ) );
    EXPECT_EQ( reader.search_text( 0, "beta", false ), 6u );
    EXPECT_EQ( reader.search_text( 0, "beta", true ), areg::Cursor::INVALID_CURSOR_POSITION );
    const uint8_t pattern[ 5 ]{ 'g', 'a', 'm', 'm', 'a' };
    EXPECT_EQ( reader.search_data( 0, pattern, 5 ), 11u );
}

//////////////////////////////////////////////////////////////////////////
// Static helpers
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   File name, extension and directory are split from a path.
 **/
TEST( FileTest, path_split_into_name_extension_directory )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string dir{ std::string( 1, sep ) + "a" + sep + "b" };
    const areg::String path{ (dir + sep + "name.tar.gz").c_str( ) };

    EXPECT_STREQ( areg::File::name_with_extension( path ).as_string( ), "name.tar.gz" );
    EXPECT_STREQ( areg::File::file_extension( path ).as_string( ), ".gz" );
    EXPECT_STREQ( areg::File::file_name( path ).as_string( ), "name.tar" );
    EXPECT_STREQ( areg::File::file_directory( path ).as_string( ), dir.c_str( ) );

    const areg::String noExtension{ (dir + sep + "plain").c_str( ) };
    EXPECT_STREQ( areg::File::name_with_extension( noExtension ).as_string( ), "plain" );
    EXPECT_TRUE( areg::File::file_extension( noExtension ).is_empty( ) );
    EXPECT_STREQ( areg::File::file_name( noExtension ).as_string( ), "plain" );
    EXPECT_TRUE( areg::File::file_directory( areg::String( "plain" ) ).is_empty( ) );
}

/**
 * \brief   A file name without any directory is a name; it is split into name and extension as any path.
 **/
TEST( FileTest, path_split_of_bare_file_name )
{
    EXPECT_STREQ( areg::File::name_with_extension( areg::String( "plain" ) ).as_string( ), "plain" );
    EXPECT_STREQ( areg::File::file_name( areg::String( "plain" ) ).as_string( ), "plain" );
    EXPECT_STREQ( areg::File::name_with_extension( areg::String( "name.c" ) ).as_string( ), "name.c" );
    EXPECT_STREQ( areg::File::file_name( areg::String( "name.c" ) ).as_string( ), "name" );
    EXPECT_STREQ( areg::File::file_extension( areg::String( "name.c" ) ).as_string( ), ".c" );
}

/**
 * \brief   A directory path or an empty path has no file name and no extension.
 **/
TEST( FileTest, path_split_of_directory_and_empty_path )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const areg::String directory{ (std::string("a") + sep + "b" + sep).c_str( ) };
    EXPECT_TRUE( areg::File::name_with_extension( directory ).is_empty( ) );
    EXPECT_TRUE( areg::File::file_extension( directory ).is_empty( ) );
    EXPECT_TRUE( areg::File::file_name( directory ).is_empty( ) );

    const areg::String empty;
    EXPECT_TRUE( areg::File::name_with_extension( empty ).is_empty( ) );
    EXPECT_TRUE( areg::File::file_extension( empty ).is_empty( ) );
    EXPECT_TRUE( areg::File::file_name( empty ).is_empty( ) );
    EXPECT_TRUE( areg::File::file_directory( empty ).is_empty( ) );
}

/**
 * \brief   A dot that is not followed by an alphanumeric symbol does not start an extension.
 **/
TEST( FileTest, extension_needs_alphanumeric_symbol )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string dir{ std::string( 1, sep ) + "d" + sep };
    EXPECT_TRUE( areg::File::file_extension( areg::String( (dir + "name.").c_str( ) ) ).is_empty( ) );
    EXPECT_STREQ( areg::File::file_name( areg::String( (dir + "name.").c_str( ) ) ).as_string( ), "name." );
    EXPECT_STREQ( areg::File::file_extension( areg::String( (dir + "name.c").c_str( ) ) ).as_string( ), ".c" );
    EXPECT_TRUE( areg::File::file_extension( areg::String( "name." ) ).is_empty( ) );
    EXPECT_STREQ( areg::File::file_name( areg::String( "name." ) ).as_string( ), "name." );
    EXPECT_STREQ( areg::File::file_name( areg::String( "a.b." ) ).as_string( ), "a.b." );
    EXPECT_STREQ( areg::File::file_name( areg::String( "." ) ).as_string( ), "." );
    EXPECT_TRUE( areg::File::file_extension( areg::String( "." ) ).is_empty( ) );
}

/**
 * \brief   The parent directory name is found in a path.
 **/
TEST( FileTest, find_parent_locates_the_parent )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string path{ std::string( 1, sep ) + "one" + sep + "two" + sep + "three" };

    const char * next{ nullptr };
    EXPECT_TRUE( areg::File::find_parent( path.c_str( ), &next ) );
    ASSERT_NE( next, nullptr );
    EXPECT_EQ( static_cast<size_t>(next - path.c_str( )), path.rfind( sep ) );

    EXPECT_FALSE( areg::File::find_parent( nullptr, &next ) );
    EXPECT_FALSE( areg::File::find_parent( "", &next ) );
    EXPECT_FALSE( areg::File::find_parent( "name", &next ) );
    EXPECT_TRUE( areg::File::parent_dir( areg::String( "name" ) ).is_empty( ) );
}

/**
 * \brief   parent_dir() returns the path without its last component.
 **/
TEST( FileTest, parent_dir_is_the_path_without_last_component )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string parent{ std::string( 1, sep ) + "one" + sep + "two" };
    const std::string path{ parent + sep + "three" };
    EXPECT_STREQ( areg::File::parent_dir( areg::String( path.c_str( ) ) ).as_string( ), parent.c_str( ) );
    EXPECT_STREQ( areg::File::parent_dir( areg::String( (path + sep).c_str( ) ) ).as_string( ), parent.c_str( ) );

    const std::string relative{ std::string( "one" ) + sep + "two" };
    EXPECT_STREQ( areg::File::parent_dir( areg::String( relative.c_str( ) ) ).as_string( ), "one" );
}

/**
 * \brief   find_parent() with the position of a previous call walks up to the next parent.
 **/
TEST( FileTest, find_parent_walks_up_from_last_position )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string path{ std::string( "root" ) + sep + "one" + sep + "two" + sep + "three" };

    const char * next{ nullptr };
    ASSERT_TRUE( areg::File::find_parent( path.c_str( ), &next ) );
    EXPECT_EQ( static_cast<size_t>(next - path.c_str( )), path.rfind( sep ) );

    const char * upper{ nullptr };
    ASSERT_TRUE( areg::File::find_parent( path.c_str( ), &upper, next ) );
    EXPECT_EQ( static_cast<size_t>(upper - path.c_str( )), path.find( sep, 5 ) );

    ASSERT_TRUE( areg::File::find_parent( path.c_str( ), &next, upper ) );
    EXPECT_EQ( static_cast<size_t>(next - path.c_str( )), path.find( sep ) );

    EXPECT_FALSE( areg::File::find_parent( path.c_str( ), &upper, next ) );
    EXPECT_EQ( upper, nullptr );
    EXPECT_FALSE( areg::File::find_parent( path.c_str( ), &upper, path.c_str( ) ) );
}

/**
 * \brief   is_path_separator() accepts the separators of the platform and nothing else.
 **/
TEST( FileTest, is_path_separator_knows_the_platform_separators )
{
    static_assert( areg::File::is_path_separator( areg::File::PATH_SEPARATOR ), "the preferred separator" );
    EXPECT_TRUE( areg::File::is_path_separator( '/' ) );
#ifdef _WIN32
    EXPECT_TRUE( areg::File::is_path_separator( '\\' ) );
#else   // _WIN32
    EXPECT_FALSE( areg::File::is_path_separator( '\\' ) );
#endif  // _WIN32
    EXPECT_FALSE( areg::File::is_path_separator( '.' ) );
    EXPECT_FALSE( areg::File::is_path_separator( ':' ) );
    EXPECT_FALSE( areg::File::is_path_separator( 'a' ) );
    EXPECT_FALSE( areg::File::is_path_separator( '\0' ) );
}

/**
 * \brief   last_path_separator() finds the last separator within the length, or returns -1.
 **/
TEST( FileTest, last_path_separator_searches_within_the_length )
{
    static_assert( areg::File::last_path_separator( "a/b/c", 5 ) == 3, "the last separator" );
    static_assert( areg::File::last_path_separator( "abc", 3 ) == -1, "no separator" );

    const char path[]{ "/one/two/three" };
    EXPECT_EQ( areg::File::last_path_separator( path, 14 ), 8 );
    EXPECT_EQ( areg::File::last_path_separator( path, 8 ), 4 );
    EXPECT_EQ( areg::File::last_path_separator( path, 9 ), 8 );
    EXPECT_EQ( areg::File::last_path_separator( path, 4 ), 0 );
    EXPECT_EQ( areg::File::last_path_separator( path, 1 ), 0 );
    EXPECT_EQ( areg::File::last_path_separator( path, 0 ), -1 );
    EXPECT_EQ( areg::File::last_path_separator( nullptr, 0 ), -1 );
    EXPECT_EQ( areg::File::last_path_separator( nullptr, -5 ), -1 );

#ifdef _WIN32
    const char mixed[]{ "C:\\one/two\\three" };
    EXPECT_EQ( areg::File::last_path_separator( mixed, 16 ), 10 );
    EXPECT_EQ( areg::File::last_path_separator( mixed, 10 ), 6 );
    EXPECT_EQ( areg::File::last_path_separator( mixed, 6 ), 2 );
#else   // _WIN32
    const char mixed[]{ "/one\\two" };
    EXPECT_EQ( areg::File::last_path_separator( mixed, 8 ), 0 );
#endif  // _WIN32
}

/**
 * \brief   The path helpers accept a path written with '/' on every platform.
 **/
TEST( FileTest, path_helpers_accept_the_unix_separator )
{
    const areg::String path( "dir/sub/name.ext" );
    EXPECT_EQ( areg::File::name_with_extension( path ), areg::String( "name.ext" ) );
    EXPECT_EQ( areg::File::file_name( path ), areg::String( "name" ) );
    EXPECT_EQ( areg::File::file_extension( path ), areg::String( ".ext" ) );
    EXPECT_EQ( areg::File::file_directory( path ), areg::String( "dir/sub" ) );
    EXPECT_EQ( areg::File::parent_dir( path ), areg::String( "dir/sub" ) );
    EXPECT_TRUE( areg::File::name_with_extension( areg::String( "dir/sub/" ) ).is_empty( ) );
    EXPECT_EQ( areg::File::parent_dir( areg::String( "dir/sub/" ) ), areg::String( "dir" ) );

#ifdef _WIN32
    const areg::String mixed( "C:\\dir/sub\\name.ext" );
    EXPECT_EQ( areg::File::name_with_extension( mixed ), areg::String( "name.ext" ) );
    EXPECT_EQ( areg::File::file_directory( mixed ), areg::String( "C:\\dir/sub" ) );
    EXPECT_EQ( areg::File::parent_dir( mixed ), areg::String( "C:\\dir/sub" ) );
    EXPECT_TRUE( areg::File::name_with_extension( areg::String( "C:\\dir/" ) ).is_empty( ) );
#endif  // _WIN32
}

/**
 * \brief   split_path() appends the components for both separators, skips empty components,
 *          and keeps the entries that are already in the list.
 **/
TEST( FileTest, split_path_appends_components )
{
    areg::StringList list;
    EXPECT_EQ( areg::File::split_path( areg::String( "/a//b\\c/" ), list ), 3 );
    ASSERT_EQ( list.size( ), 3u );
    EXPECT_STREQ( list.first_entry( ).as_string( ), "a" );
    EXPECT_STREQ( list.last_entry( ).as_string( ), "c" );

    EXPECT_EQ( areg::File::split_path( areg::String( "x" ), list ), 1 );
    EXPECT_EQ( list.size( ), 4u );
    EXPECT_EQ( areg::File::split_path( areg::String( "" ), list ), 0 );
    EXPECT_EQ( areg::File::split_path( areg::String( "///" ), list ), 0 );
    EXPECT_EQ( list.size( ), 4u );
}

/**
 * \brief   make_full_path() joins the directory and the file name to an absolute path.
 **/
TEST( FileTest, make_full_path_joins_components )
{
    TempFolder tmp;
    const areg::String full{ areg::File::make_full_path( areg::String( tmp.dir( ).string( ).c_str( ) ), areg::String( "file.txt" ) ) };
    EXPECT_EQ( std::filesystem::path( full.data( ) ), tmp.dir( ) / "file.txt" );

    const areg::String relative{ areg::File::make_full_path( areg::String( "dir" ), areg::String( "file.txt" ) ) };
    EXPECT_TRUE( std::filesystem::path( relative.data( ) ).is_absolute( ) );
    EXPECT_TRUE( relative.ends_with( "file.txt" ) );
}

/**
 * \brief   file_full_path() makes a relative path absolute and returns an empty string for an empty path.
 **/
TEST( FileTest, file_full_path_is_absolute )
{
    const areg::String full{ areg::File::file_full_path( areg::String( "some/relative.txt" ) ) };
    EXPECT_TRUE( std::filesystem::path( full.data( ) ).is_absolute( ) );
    EXPECT_TRUE( full.ends_with( "relative.txt" ) );
    EXPECT_TRUE( areg::File::file_full_path( areg::String( ) ).is_empty( ) );
}

/**
 * \brief   normalize_path() replaces the masks, makes the path absolute, and returns empty for an empty path.
 **/
TEST( FileTest, normalize_path_expands_masks )
{
    EXPECT_TRUE( areg::File::normalize_path( areg::String( ) ).is_empty( ) );

    const areg::String user{ areg::File::normalize_path( areg::String( "%user%/areg_data.txt" ) ) };
    EXPECT_FALSE( user.is_valid_position( user.find_first( "%user%" ) ) );
    EXPECT_TRUE( user.ends_with( "areg_data.txt" ) );
    EXPECT_TRUE( std::filesystem::path( user.data( ) ).is_absolute( ) );

    const areg::String stamped{ areg::File::normalize_path( areg::String( "log_%time%_%time%.txt" ) ) };
    EXPECT_FALSE( stamped.is_valid_position( stamped.find_first( "%time%" ) ) );
    EXPECT_TRUE( stamped.ends_with( ".txt" ) );

    const areg::String named{ areg::File::normalize_path( areg::String( "%appname%.cfg" ) ) };
    EXPECT_FALSE( named.is_valid_position( named.find_first( "%appname%" ) ) );
    EXPECT_TRUE( named.ends_with( ".cfg" ) );
}

/**
 * \brief   normalize_path() resolves the . and .. entries of the path.
 **/
TEST( FileTest, normalize_path_resolves_dot_entries )
{
    constexpr char sep{ areg::File::PATH_SEPARATOR };
    const std::string base{ areg::File::current_dir( ).data( ) };
    const std::string messy{ std::string( "one" ) + sep + "two" + sep + ".." + sep + "." + sep + "three.txt" };
    const areg::String normalized{ areg::File::normalize_path( areg::String( messy.c_str( ) ) ) };
    const std::string expected{ (std::filesystem::path( base ) / "one" / "three.txt").string( ) };
    EXPECT_EQ( std::string( normalized.data( ) ), expected );
}

/**
 * \brief   The special folders are not empty and the user folder is an existing directory.
 **/
TEST( FileTest, special_dir_returns_existing_folders )
{
    const areg::String home{ areg::File::special_dir( areg::File::SpecialFolder::UserHome ) };
    EXPECT_FALSE( home.is_empty( ) );
    EXPECT_TRUE( areg::File::has_dir( home ) );
    EXPECT_FALSE( areg::File::special_dir( areg::File::SpecialFolder::Personal ).is_empty( ) );
    EXPECT_FALSE( areg::File::special_dir( areg::File::SpecialFolder::AppData ).is_empty( ) );
    EXPECT_FALSE( areg::File::special_dir( areg::File::SpecialFolder::Temp ).is_empty( ) );
    EXPECT_FALSE( areg::File::executable_dir( ).is_empty( ) );
}

/**
 * \brief   Directories are created, cascaded creation is safe to repeat, and an existing file is not a directory.
 **/
TEST( FileTest, create_and_delete_directories )
{
    TempFolder tmp;
    const areg::String single{ tmp.path( "single" ) };
    EXPECT_TRUE( areg::File::create_dir( single ) );
    EXPECT_TRUE( areg::File::has_dir( single ) );
    EXPECT_FALSE( areg::File::create_dir( single ) );
    EXPECT_FALSE( areg::File::create_dir( tmp.path( "absent/child" ) ) );
    EXPECT_FALSE( areg::File::create_dir( areg::String( ) ) );

    const areg::String nested{ tmp.path( "n1/n2/n3" ) };
    EXPECT_TRUE( areg::File::create_dir_cascaded( nested ) );
    EXPECT_TRUE( areg::File::create_dir_cascaded( nested ) );
    EXPECT_TRUE( areg::File::has_dir( nested ) );
    EXPECT_FALSE( areg::File::create_dir_cascaded( areg::String( ) ) );

    put_content( tmp.path( "n1/n2/file.txt" ), "x" );
    EXPECT_FALSE( areg::File::has_dir( tmp.path( "n1/n2/file.txt" ) ) );
    EXPECT_FALSE( areg::File::create_dir_cascaded( tmp.path( "n1/n2/file.txt/sub" ) ) );

    EXPECT_TRUE( areg::File::delete_dir( tmp.path( "n1" ) ) );
    EXPECT_FALSE( areg::File::has_dir( tmp.path( "n1" ) ) );
    EXPECT_FALSE( areg::File::delete_dir( areg::String( ) ) );
    EXPECT_FALSE( areg::File::delete_dir( tmp.path( "n1" ) ) );
}

/**
 * \brief   has_file() and has_dir() are exclusive; empty and missing paths are neither.
 **/
TEST( FileTest, has_file_and_has_dir_are_exclusive )
{
    TempFolder tmp;
    put_content( tmp.path( "f.txt" ), "x" );
    EXPECT_TRUE( areg::File::has_file( tmp.path( "f.txt" ) ) );
    EXPECT_FALSE( areg::File::has_dir( tmp.path( "f.txt" ) ) );
    EXPECT_TRUE( areg::File::has_dir( areg::String( tmp.dir( ).string( ).c_str( ) ) ) );
    EXPECT_FALSE( areg::File::has_file( areg::String( tmp.dir( ).string( ).c_str( ) ) ) );
    EXPECT_FALSE( areg::File::has_file( areg::String( ) ) );
    EXPECT_FALSE( areg::File::has_dir( areg::String( ) ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "none.txt" ) ) );
}

/**
 * \brief   A file is copied; an existing target is replaced only if the copy is forced.
 **/
TEST( FileTest, copy_file_respects_force_flag )
{
    TempFolder tmp;
    put_content( tmp.path( "src.txt" ), "source" );
    put_content( tmp.path( "dst.txt" ), "target" );

    EXPECT_FALSE( areg::File::copy_file( tmp.path( "src.txt" ), tmp.path( "dst.txt" ), false ) );
    EXPECT_EQ( file_content( tmp.dir( ) / "dst.txt" ), "target" );
    EXPECT_TRUE( areg::File::copy_file( tmp.path( "src.txt" ), tmp.path( "dst.txt" ), true ) );
    EXPECT_EQ( file_content( tmp.dir( ) / "dst.txt" ), "source" );
    EXPECT_TRUE( areg::File::copy_file( tmp.path( "src.txt" ), tmp.path( "new.txt" ), false ) );
    EXPECT_EQ( file_content( tmp.dir( ) / "new.txt" ), "source" );
    EXPECT_EQ( file_content( tmp.dir( ) / "src.txt" ), "source" );
}

/**
 * \brief   copy_file() with an empty or missing path fails and creates nothing.
 **/
TEST( FileTest, copy_file_invalid_paths_fail )
{
    TempFolder tmp;
    put_content( tmp.path( "src.txt" ), "source" );
    EXPECT_FALSE( areg::File::copy_file( areg::String( ), tmp.path( "a.txt" ), true ) );
    EXPECT_FALSE( areg::File::copy_file( tmp.path( "src.txt" ), areg::String( ), true ) );
    EXPECT_FALSE( areg::File::copy_file( tmp.path( "absent.txt" ), tmp.path( "a.txt" ), true ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "a.txt" ) ) );
}

/**
 * \brief   A file is moved: the source disappears and the target holds the data.
 **/
TEST( FileTest, move_file_moves_data )
{
    TempFolder tmp;
    put_content( tmp.path( "from.txt" ), "payload" );
    EXPECT_TRUE( areg::File::move_file( tmp.path( "from.txt" ), tmp.path( "to.txt" ) ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "from.txt" ) ) );
    EXPECT_EQ( file_content( tmp.dir( ) / "to.txt" ), "payload" );

    EXPECT_FALSE( areg::File::move_file( tmp.path( "from.txt" ), tmp.path( "other.txt" ) ) );
    EXPECT_FALSE( areg::File::move_file( areg::String( ), tmp.path( "other.txt" ) ) );
    EXPECT_FALSE( areg::File::move_file( tmp.path( "to.txt" ), areg::String( ) ) );
    EXPECT_TRUE( areg::File::has_file( tmp.path( "to.txt" ) ) );
}

/**
 * \brief   delete_file() removes a file once; a missing or empty path fails.
 **/
TEST( FileTest, delete_file_reports_result )
{
    TempFolder tmp;
    put_content( tmp.path( "gone.txt" ), "x" );
    EXPECT_TRUE( areg::File::delete_file( tmp.path( "gone.txt" ) ) );
    EXPECT_FALSE( areg::File::has_file( tmp.path( "gone.txt" ) ) );
    EXPECT_FALSE( areg::File::delete_file( tmp.path( "gone.txt" ) ) );
    EXPECT_FALSE( areg::File::delete_file( areg::String( ) ) );
}

/**
 * \brief   The current directory is changed and restored; an empty or missing directory is rejected.
 **/
TEST( FileTest, set_current_dir_changes_and_restores )
{
    TempFolder tmp;
    const areg::String before{ areg::File::current_dir( ) };
    EXPECT_FALSE( areg::File::set_current_dir( areg::String( ) ) );
    EXPECT_FALSE( areg::File::set_current_dir( tmp.path( "missing" ) ) );
    EXPECT_EQ( areg::File::current_dir( ), before );

    ASSERT_TRUE( areg::File::set_current_dir( areg::String( tmp.dir( ).string( ).c_str( ) ) ) );
    std::error_code err;
    EXPECT_TRUE( std::filesystem::equivalent( areg::File::current_dir( ).data( ), tmp.dir( ), err ) );
    EXPECT_TRUE( areg::File::set_current_dir( before ) );
    EXPECT_EQ( areg::File::current_dir( ), before );
}

/**
 * \brief   temp_name() with a prefix is in the temp folder and starts with the prefix; without the temp
 *          folder it creates no file; the unique names differ.
 **/
TEST( FileTest, temp_name_variants )
{
    const areg::String inTemp{ areg::File::temp_name( areg::String( "areg_prefix" ), true, true ) };
    EXPECT_TRUE( areg::File::has_file( inTemp ) );
    EXPECT_TRUE( areg::File::file_name( inTemp ).starts_with( "areg_prefix" ) );
    areg::File::delete_file( inTemp );

    const areg::String local{ areg::File::temp_name( areg::String( "areg_local" ), false, false ) };
    EXPECT_FALSE( areg::File::has_file( local ) );
    EXPECT_TRUE( local.ends_with( "areg_local" ) );

    const areg::String first{ areg::File::temp_name( areg::String( "areg_u" ), true, false ) };
    EXPECT_TRUE( first.find_first( "areg_u" ) != areg::INVALID_POS );

    const areg::String byProcess{ areg::File::temp_name( areg::String( ), true, true ) };
    EXPECT_TRUE( areg::File::has_file( byProcess ) );
    areg::File::delete_file( byProcess );
}

/**
 * \brief   A shared lock is taken on a file opened only for reading and released.
 **/
TEST( FileTest, shared_read_lock_on_read_only_handle )
{
    TempFolder tmp;
    put_content( tmp.path( "lock.bin" ), "x" );
    areg::File file( tmp.path( "lock.bin" ), mode_of( OpenMode::Read, OpenMode::Exist, OpenMode::Binary ) );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.lock_file( false ) );
    file.unlock_file( );
}
