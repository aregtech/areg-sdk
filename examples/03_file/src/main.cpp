//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
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
#include <iostream>
#include <string>

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS


//!< Write some text to the file.
static void writeText(FileBase & file)
{
    std::cout << std::string( 20, '*' ) << std::endl;

    if (file.isValid())
    {
        printf( "Writing text to file [ %s ] ...\n", file.getName( ).getString() );
        // Write some texts.
        file.write( String( "!!!Hello World!!!" ) );
        file << "This is some text.";
        file << "And this one is another part of text.";
    }
    else
    {
        std::cerr << "Invalid file " << file.getName().getString() << ". Cannot write text ..." << std::endl;
    }
}

static void writeLines(FileBase & file)
{
    std::cout << std::string( 20, '*' ) << std::endl;

    if ( file.isValid( ) )
    {
        printf( "Writing lines of text to file [ %s ] ...\n", file.getName( ).getString() );
        // Write some texts.
        file.writeLine( String( "!!!Hello World!!!" ) );
        file.writeLine( "This is some text." );
        file.writeLine( "And this one is another part of text." );
    }
    else
    {
        std::cerr << "Invalid file " << file.getName( ).getString() << ". Cannot write text ..." << std::endl;
    }
}

//!< Dump the content of the file.
static void dumpText(FileBase & file)
{
    std::cout << std::string( 20, '*' ) << std::endl;

    if (file.isValid())
    {
        // read the file content
        String text;
        file.moveToBegin();
        file >> text;

        // dump the content
        printf( "BEGIN File [ %s ] content >>>\n", file.getName( ).getString( ) );
        std::cout << text.getString( ) << std::endl;
        printf( "END   File [ %s ] content <<<\n", file.getName( ).getString( ) );
    }
    else
    {
        std::cerr << "Invalid file " << file.getName( ).getString() << ". Cannot dump text ..." << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////
// Some functionalities of file
//////////////////////////////////////////////////////////////////////////
int main()
{
    // open for read and write text, create if not existing, share for read and write
    unsigned int mode = FileBase::FO_MODE_WRITE | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_SHARE_WRITE;

    // Create a normal text file on file system
    // The file name has relative path to the text file
    File file("./Debug/hello.txt", mode);
    if (file.open())
    {
        // Write a text to the file
        writeText(file);
        // output what has written.
        dumpText(file);
    }

    // Create memory file (i.e. buffer in memory, which acts like a file).
    // The file name has a time-stamp mask in the name.
    FileBuffer buffer;
    if (buffer.open("Buffer_%time%", mode))
    {
        // write a text
        writeText(buffer);
        // dump file content
        dumpText(buffer);
    }

    file.close();
    // Create file and write text line-by-line
    // The file name has application (process name) mask in the name.
    if (file.open( "../../../../temp/%appname%.txt", mode))
    {
        // write lines of texts.
        writeLines(file);
        // dumpp file content
        dumpText(file);
    }

    // Create binary file
    File binary( "./Debug/binary.dat", mode & (~FileBase::FO_MODE_TEXT) );
    if (binary.open())
    {
        // get buffer object of file buffer and write to binary.
        binary.write( buffer.getDataBuffer( ), buffer.getLength());
        // dump the content
        dumpText(binary);

        // increase / reserve file space
        binary.reserve( 789 );
    }

    // close all files.
    file.close( );
    buffer.close( );
    binary.close( );

    // Make a copy of file
    // normalize file paths i.e. get file path and resolve masks in the name.
    String src = File::normalizePath( "./Debug/hello.txt" );
    String dst = File::normalizePath( "./Debug/copy_%time%.txt" );
    printf( "\nCopying file [ %s ] to [ %s ]\n", src.getString( ), dst.getString( ) );
    File::copyFile( src, dst, true );

    return 0;
}
