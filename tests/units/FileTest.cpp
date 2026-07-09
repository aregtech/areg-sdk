/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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
#include "areg/logging/areg_log.h"
#include "areg/logging/LoggingDefs.hpp"

#include <fstream>
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
