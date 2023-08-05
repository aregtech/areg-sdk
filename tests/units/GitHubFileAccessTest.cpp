/************************************************************************
 * Include files.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"

#include <fstream>
#include <filesystem>

#ifdef WINDOWS
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#endif // WINDOWS


TEST( GitHubFileAccessTest, FileReadWithStl )
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

TEST( GitHubFileAccessTest, FileReadWriteWithStl )
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

TEST( GitHubFileAccessTest, FileReadWithWin32 )
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

TEST( GitHubFileAccessTest, FileReadWriteWithWin32 )
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

TEST( GitHubFileAccessTest, FileCheckFileWithAreg )
{
    Application::setWorkingDirectory( nullptr );

    const String fileName{ "./config/log.init" };
    const String fileWrong{ "./config/blah-blah.init" };
    ASSERT_TRUE( File::existFile( fileName.getString( ) ) );
    ASSERT_FALSE( File::existFile( fileWrong ) );
}

TEST( GitHubFileAccessTest, FileOpenWithAreg )
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

TEST( GitHubFileAccessTest, FileCreateEmptyFileWithAreg )
{
    Application::setWorkingDirectory( nullptr );
    const String fileNameWrite{ "./empty_file_areg.txt" };
    constexpr unsigned int modeWrite{ FileBase::FO_MODE_READ | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_WRITE };

    File fileWrite( fileNameWrite, modeWrite );
    ASSERT_TRUE( fileWrite.open( ) );
    fileWrite.close( );
    ASSERT_TRUE( File::existFile( fileNameWrite ) );
}

TEST( GitHubFileAccessTest, FileReadWithAreg )
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

TEST( GitHubFileAccessTest, FileReadWriteWithAreg )
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

TEST( GitHubFileAccessTest, CreateFolderCascadedWithAreg )
{
    Application::setWorkingDirectory( nullptr );

    const String dirPath( "./dir1/dir2/dir3/" );
    ASSERT_FALSE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::createDirCascaded( dirPath ) );
    ASSERT_TRUE( File::existDir( dirPath ) );
    ASSERT_TRUE( File::deleteDir( "./dir1/" ) );
    ASSERT_FALSE( File::existDir( "./dir1/" ) );
}

TEST( GitHubFileAccessTest, FileReadWriteSubfolderWithAreg )
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