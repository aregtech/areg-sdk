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
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Google test dummy file.
 *              Created as a fist file to integrate GTest in AREG
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/trace/GETrace.h"
#include "areg/trace/NETrace.hpp"

#include <fstream>
#include <filesystem>

#ifdef WINDOWS
    #include <shlwapi.h>

    #pragma comment(lib, "Shlwapi.lib")
#endif // WINDOWS


TEST( FileTest, StlFileRead )
{
    Application::setWorkingDirectory( nullptr );

    constexpr char fileName[]{ "./config/log.init" };
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

TEST( FileTest, StlFileReadWrite )
{
    Application::setWorkingDirectory( nullptr );

    std::fstream fileRead, fileWrite;
    constexpr char fileNameRead[ ]{ "./config/log.init" };
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

TEST( FileTest, Win32FileRead )
{
#ifdef WINDOWS

    Application::setWorkingDirectory( nullptr );

    constexpr char fileName[ ]{ "./config/log.init" };
    ASSERT_TRUE( PathFileExistsA(fileName) );

    HANDLE hFile = ::CreateFileA( fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL );
    ASSERT_TRUE( hFile != NULL );

    char buffer[ 1024 ]{ 0 };
    DWORD dwRead{ 0 };
    ASSERT_TRUE( ::ReadFile( hFile, buffer, 1024, &dwRead, NULL ) );
    ASSERT_EQ( dwRead, 1024 );
    ASSERT_EQ( buffer[ 0 ], '#' );

    ASSERT_TRUE(::CloseHandle( hFile ));

#endif // WINDOWS
}

TEST( FileTest, Win32FileReadWrite )
{
#ifdef WINDOWS

    Application::setWorkingDirectory( nullptr );

    constexpr char fileNameRead[ ]{ "./config/log.init" };
    constexpr char fileNameWrite[ ]{ "./write_with_win32.txt" };

    HANDLE hFileRead = ::CreateFileA( fileNameRead, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL );
    ASSERT_TRUE( hFileRead != NULL );

    char buffer[ 1024 ]{ 0 };
    DWORD dwRead{ 0 };
    ASSERT_TRUE( ::ReadFile( hFileRead, buffer, 1024, &dwRead, NULL ) );
    ASSERT_EQ( dwRead, 1024 );
    ASSERT_EQ( buffer[ 0 ], '#' );
    ASSERT_TRUE( ::CloseHandle( hFileRead ));

    HANDLE hFileWrite = ::CreateFileA( fileNameWrite, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    ASSERT_TRUE( hFileWrite != NULL );
    DWORD dwWrite{ 0 };
    ASSERT_TRUE( ::WriteFile( hFileWrite, buffer, 1024, &dwWrite, NULL ) );
    ASSERT_EQ( dwWrite, 1024 );

    ASSERT_TRUE( ::CloseHandle( hFileWrite ));
    ASSERT_TRUE( PathFileExistsA( fileNameWrite ) );

#endif // WINDOWS
}

TEST( FileTest, AregFileCheck )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/log.init" };
    const String fileWrong{ "./config/blah-blah.init" };
    ASSERT_TRUE( File::existFile( fileName.getString( ) ) );
    ASSERT_FALSE( File::existFile( fileWrong ) );
}

TEST( FileTest, AregFileNormalizedPath )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/log.init" };
    String normalized = File::normalizePath( fileName );
    ASSERT_TRUE( normalized.getLength( ) > fileName.getLength( ) );
    ASSERT_TRUE( normalized.endsWith( "log.init" ) );
}

TEST( FileTest, AregFileOpen )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/log.init" };
    constexpr unsigned int mode{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    File file( fileName, mode );

    ASSERT_TRUE( File::existFile( fileName.getString( ) ) );
    ASSERT_TRUE( file.open( ) );
    ASSERT_EQ( file.getPosition( ), 0 );

    file.close( );
}

TEST( FileTest, AregFileCreateEmptyFile )
{
    Application::setWorkingDirectory( nullptr );
    const String fileNameWrite{ "./empty_file_areg.txt" };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    fileWrite.close( );
    ASSERT_TRUE( File::existFile( fileNameWrite ) );
}

TEST( FileTest, AregFileRead )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/log.init" };
    constexpr unsigned int mode{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    File file( fileName, mode );

    ASSERT_TRUE(File::existFile( fileName.getString( ) ));
    ASSERT_TRUE( file.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(file.readString( buffer, 1025 ));
    ASSERT_EQ( dwRead, 1024 );
    ASSERT_EQ( buffer[ 0 ], '#' );

    file.close( );
}

TEST( FileTest, AregFileReadWrite )
{
    Application::setWorkingDirectory( nullptr );

    const String fileNameRead{ "./config/log.init" };
    const String fileNameWrite{ "./write_with_areg.txt" };

    constexpr unsigned int modeRead{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileRead( fileNameRead.getString( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.readString( buffer, 1025 ));
    ASSERT_EQ( dwRead, 1024 );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    ASSERT_TRUE( fileWrite.writeString( buffer ) );

    fileWrite.close( );

    ASSERT_TRUE( File::existFile( fileNameWrite ) );
}

TEST( FileTest, AregCreateFolderCascaded )
{
    Application::setWorkingDirectory( nullptr );

    const String dirPath( "./dir1/dir2/dir3/" );
    ASSERT_FALSE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::createDirCascaded( dirPath ) );
    ASSERT_TRUE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::deleteDir( "./dir1/" ) );
    ASSERT_FALSE( File::existDir( "./dir1/" ) );
}

TEST( FileTest, AregFileReadWriteSubfolder )
{
    Application::setWorkingDirectory( nullptr );

    const String fileNameRead { "./config/log.init" };
    const String fileNameWrite{ "./log/write_with_areg.txt" };

    constexpr unsigned int modeRead { FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_EXIST | FileBase::FO_MODE_SHARE_READ };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileRead( fileNameRead.getString( ), modeRead );
    ASSERT_TRUE( fileRead.open( ) );

    char buffer[ 1025 ]{ 0 };
    uint32_t dwRead = static_cast<uint32_t>(fileRead.readString(buffer, 1025));
    ASSERT_EQ( dwRead, 1024 );
    ASSERT_EQ( buffer[ 0 ], '#' );
    buffer[ 1024 ] = '\0';
    fileRead.close( );

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open() );
    ASSERT_TRUE(fileWrite.writeString(buffer));

    fileWrite.close( );

    ASSERT_TRUE( File::existFile(fileNameWrite) );
}

DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging );
TEST( FileTest, Areg_TraceStartAndStopOnly )
{
    Application::setWorkingDirectory( nullptr );
    ASSERT_TRUE( NETrace::startLogging( "./config/log.init" ) );
    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging );
    } while ( false );

    NETrace::stopLogging( );
}
