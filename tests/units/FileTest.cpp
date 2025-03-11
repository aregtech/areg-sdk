/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/FileTest.Cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Tests of File object.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/logging/GELog.h"
#include "areg/logging/NELogging.hpp"

#include <fstream>
#include <filesystem>

#ifdef WINDOWS
    #include <Shlwapi.h>

    #ifdef MS_VISUAL_CPP
        #pragma comment(lib, "Shlwapi.lib")
    #endif  // MS_VISUAL_CPP
#endif // WINDOWS

/**
 * \brief   Makes sure that the system has no problem to read file from file system.
 *          The test is using STL objects, which suppose to work under any platform.
 **/
TEST( FileTest, StlFileRead )
{
    Application::setWorkingDirectory( nullptr );

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
TEST( FileTest, StlFileReadWrite )
{
    Application::setWorkingDirectory( nullptr );

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
TEST( FileTest, Win32FileRead )
{
#ifdef WINDOWS

    Application::setWorkingDirectory( nullptr );

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

#endif // WINDOWS
}

/**
 * \brief   Makes sure that the system has no problem to read and write file from file system.
 *          The test is using Win32 API and supposed to work only on Windows system.
 **/
TEST( FileTest, Win32FileReadWrite )
{
#ifdef WINDOWS

    Application::setWorkingDirectory( nullptr );

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
TEST( FileTest, CheckFileExistence )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/areg.init" };
    const String fileWrong{ "./config/blah-blah.init" };
    ASSERT_TRUE( File::existFile( fileName.getString( ) ) );
    ASSERT_FALSE( File::existFile( fileWrong ) );
}

/**
 * \brief   The test checks basic functionality of the file path normalization.
 **/
TEST( FileTest, NormalizeFilePathBasic )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/areg.init" };
    String normalized = File::normalizePath( fileName );
    ASSERT_TRUE( normalized.getLength( ) > fileName.getLength( ) );
    ASSERT_TRUE( normalized.endsWith( "areg.init" ) );
}

/**
 * \brief   The test checks file open functionality using File object API.
 **/
TEST( FileTest, FileOpenBasic )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/areg.init" };
    constexpr unsigned int mode{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    File file( fileName, mode );

    ASSERT_TRUE( File::existFile( fileName.getString( ) ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.getPosition( ), static_cast<uint32_t>(0) );

    file.close( );
}

/**
 * \brief   The test checks creating an empty file by using File object API.
 **/
TEST( FileTest, CreateEmptyFile )
{
    Application::setWorkingDirectory( nullptr );
    const String fileNameWrite{ "./empty_file_areg.txt" };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    fileWrite.close( );
    ASSERT_TRUE( File::existFile( fileNameWrite ) );
}

/**
 * \brief   The test checks the basic functionality of opening and reading a file by using File object API.
 **/
TEST( FileTest, FileReadBasic )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/areg.init" };
    constexpr unsigned int mode{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    File file( fileName, mode );

    ASSERT_TRUE(File::existFile( fileName.getString( ) ));
    ASSERT_TRUE( file.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(file.readString( buffer, 1025 ));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );

    file.close( );
}

/**
 * \brief   The test checks the basic functionality of opening, reading and writing a file by using File object API.
 **/
TEST( FileTest, FileReadWriteBasic )
{
    Application::setWorkingDirectory( nullptr );

    const String fileNameRead{ "./config/areg.init" };
    const String fileNameWrite{ "./write_with_areg.txt" };

    constexpr unsigned int modeRead{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileRead( fileNameRead.getString( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.readString( buffer, 1025 ));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    ASSERT_TRUE( fileWrite.writeString( buffer ) );

    fileWrite.close( );

    ASSERT_TRUE( File::existFile( fileNameWrite ) );
}

/**
 * \brief   The test checks the basic functionality of creating folders cascaded
 *          and deleting them by using File object API.
 **/
TEST( FileTest, CreateFolderCascaded )
{
    Application::setWorkingDirectory( nullptr );

    const String dirPath( "./dir1/dir2/dir3/" );
    ASSERT_FALSE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::createDirCascaded( dirPath ) );
    ASSERT_TRUE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::deleteDir( "./dir1/" ) );
    ASSERT_FALSE( File::existDir( "./dir1/" ) );
}

/**
 * \brief   The test checks the functionality read an existing file and
 *          wring that into another file located in non-existing folder.
 *          The test will create subfolder and create a file inside.
 **/
TEST( FileTest, FileReadAndWriteInSubfolder )
{
    Application::setWorkingDirectory( nullptr );

    const String fileNameRead { "./config/areg.init" };
    const String fileNameWrite{ "./logs/write_with_areg.txt" };

    constexpr unsigned int modeRead { FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileRead( fileNameRead.getString( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.readString(buffer, 1025));
    ASSERT_EQ( dwRead, static_cast<uint32_t>(1024) );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open() );
    ASSERT_TRUE(fileWrite.writeString(buffer));

    fileWrite.close( );

    ASSERT_TRUE( File::existFile(fileNameWrite) );
}
