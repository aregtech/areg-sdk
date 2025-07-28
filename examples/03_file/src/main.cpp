//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of file objects. It creates 
//               text and binary files, it shows how files can be copied, 
//               use of masks in file names, relative and full paths. 
//               For example, how the mask %time% (time-stamp) or %appname%
//               (process name) can be used.In the areg framework file objects
//               are also streaming objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/String.hpp"

#ifdef  _MSC_VER
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    //!< Write text to the file.
    void writeText( FileBase & file )
    {
        std::cout << std::string( 20, '*' ) << std::endl;

        if ( file.isValid( ) )
        {
            std::cout << "Writing text to file [ " << file.getName( ) << " ] ..." << std::endl;

            // Write texts
            file.write( "!!!Hello World!!!" );
            file << "This is some text.";
            file << "And this one is another part of text.";
        }
        else
        {
            std::cerr << "Invalid file " << file.getName( ) << ". Cannot write text ..." << std::endl;
        }
    }

    //!< Generate and write texts line-by-line
    void writeLines( FileBase & file )
    {
        std::cout << std::string( 20, '*' ) << std::endl;

        if ( file.isValid( ) )
        {
            std::cout << "Writing text to file [ " << file.getName( ) << " ] ..." << std::endl;

            // Write lines.
            file.writeLine( "!!!Hello World!!!" );
            file.writeLine( "This is some text." );
            file.writeLine( "And this one is another part of text." );
        }
        else
        {
            std::cerr << "Invalid file " << file.getName( ) << ". Cannot write line of texts ..." << std::endl;
        }
    }

    //!< Dump the content of the file on console.
    void dumpText( FileBase & file )
    {
        std::cout << std::string( 20, '*' ) << std::endl;

        if ( file.isValid( ) )
        {
            file.moveToBegin( );

            // read the file content
            String text;
            file >> text;

            // dump the content
            std::cout << "BEGIN File [ " << file.getName() << " ] content >>>" << std::endl;
            std::cout << text << std::endl;
            std::cout << "END File [ " << file.getName( ) << " ] content <<<" << std::endl;
        }
        else
        {
            std::cerr << "Invalid file " << file.getName( ) << ". Cannot dump text ..." << std::endl;
        }
    }

} // namespace

//////////////////////////////////////////////////////////////////////////
// Some functionalities of file
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo to show file funtionalities ..." << std::endl;

    // open for read and write text, create if not existing, share for read and write
    constexpr unsigned int mode{ FileBase::FO_MODE_WRITE | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_SHARE_WRITE };

    // Create a text file on file system with relative path name
    File txtFile("./Debug/hello.txt", mode);
    if ( txtFile.open())
    {
        writeText( txtFile );   // Fill file with some text
        dumpText( txtFile );    // Dump on console the file content.
    }

    // Create a file in memory with time-stamp mask in the name
    FileBuffer buffer;
    if (buffer.open("Buffer_%time%", mode))
    {
        writeText(buffer);  // Fill file with some text.
        dumpText(buffer);   // Dump on console the file content.
    }

    // Create a file with the application name as file name mask. Set line-by-line content.
    File lineFile;
    if (lineFile.open( "../../../../temp/%appname%.txt", mode))
    {
        writeLines( lineFile ); // Fill file with line-by-line text.
        dumpText( lineFile );   // Dump on console the file content.
    }

    // Create a binary file, take out the 'text mode' mask
    File binary( "./Debug/binary.dat", mode & (~FileBase::FO_MODE_TEXT) );
    if (binary.open())
    {
        // write data from memory file
        binary.write( buffer.getDataBuffer( ), buffer.getLength());
        dumpText(binary);       // Dump on console the file content.
        binary.reserve( 789 );  // Increase / reserve file space
    }

    // close all files.
    txtFile.close( );
    lineFile.close( );
    buffer.close( );
    binary.close( );

    // Make a copy of files and normalize path by resolving mask in the names.
    String src(File::normalizePath( "./Debug/hello.txt" ));
    String dst(File::normalizePath( "./Debug/copy_%time%.txt" ));

    std::cout << std::string( 20, '.' ) << std::endl;
    std::cout << "Copying file [ " << src << " ] to the file [ " << dst << " ]" << std::endl ;
    File::copyFile( src, dst, true );

    std::cout << "Exit application!" << std::endl;
    return 0;
}
