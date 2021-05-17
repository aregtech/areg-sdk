//============================================================================
// Name        : 03-file.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include <iostream>

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS


//////////////////////////////////////////////////////////////////////////
// Some functionalities of file
//////////////////////////////////////////////////////////////////////////
int main()
{
    const char * aText  = "!!!Hello World!!!";
    const char * aLine1 = "This is a line of text.";
    const char * aLine2 = "And another line of text.";

    const char * fPath1 = "./debug/file1.txt";
    const char * fPath2 = "./debug/copy_%time%.txt";
    const char * fPath3 = "../../../../temp/%appname%.txt";

/************************************************************************/
// Initialization
/************************************************************************/

    String txtFile, txtBuff, txtTemp;

    // open for read and write text, create if not existing, share for read and write
    unsigned int mode = FileBase::FO_MODE_WRITE | FileBase::FO_MODE_TEXT | FileBase::FO_MODE_CREATE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_SHARE_WRITE;

    File file(fPath1, mode );
    FileBuffer buffer;
    File temp(fPath3, mode);

    /************************************************************************/
    // File
    /************************************************************************/
    file.open();
    OUTPUT_DBG("[ %s ] to create file [ %s ]", file.isValid() ? "SUCCEDED" : "FAILED", file.getName());

    if (file.isValid())
    {
        const char * nextPos = NULL;
        const char * lastPos = NULL;
        char * fileName = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
        NEString::copyString<char>(fileName, File::MAXIMUM_PATH + 1, file.getName(), NEString::CountAll );

        // List parent folders.
        OUTPUT_DBG("List of parents for path [ %s ]", fileName);
        for ( int i = 1; File::findParent(fileName, &nextPos, lastPos); ++ i)
        {
            fileName[nextPos - fileName] = NEString::EndOfString;
            lastPos = nextPos;
            OUTPUT_DBG("    >>  %d. %s", i, fileName);
        }

        // write data
        file.writeString(aText);
        file << String(aLine1);
        file << aLine2;

        file.moveToBegin();
        file.read( txtFile );   // read content

        OUTPUT_DBG("BEGIN File [ %s ] content >>>", file.getName());
        std::cout << txtFile.getString() << std::endl;
        OUTPUT_DBG("END File [ %s ] content <<<", file.getName());

        delete [] fileName;
    }

    /************************************************************************/
    // FileBuffer
    /************************************************************************/
    buffer.open("FileBuffer_%time%", mode);
    OUTPUT_DBG("[ %s ] to create file [ %s ]", buffer.isValid() ? "SUCCEDED" : "FAILED", buffer.getName());
    if (buffer.isValid())
    {
        FileBuffer & stream = static_cast<FileBuffer &>(buffer);
        stream << String(aText);
        stream.writeString(aLine1);
        stream << aLine2;

        stream.moveToBegin();
        stream >> txtBuff;

        OUTPUT_DBG("BEGIN File [ %s ] content >>>", buffer.getName());
        std::cout << txtBuff.getString() << std::endl;
        OUTPUT_DBG("END File [ %s ] content <<<", buffer.getName());
    }

    OUTPUT_DBG("2 file objects have [ %s ] content of data.", txtFile == txtBuff ? "identical" : "different");

    if (temp.open())
    {
        temp.writeLine(aText);
        temp.writeLine(aLine1);
        temp.writeLine(aLine2);

        temp.moveToBegin();
        temp.read(txtTemp);

        OUTPUT_DBG("BEGIN File [ %s ] content >>>", temp.getName());
        std::cout << txtTemp.getString() << std::endl;
        OUTPUT_DBG("END File [ %s ] content <<<", temp.getName());
    }

    /************************************************************************/
    // Close files
    /************************************************************************/
    String fullPath(File::getFileFullPath(fPath1));
    file.close();
    buffer.close();
    temp.close();

    /************************************************************************/
    // Make a copy of file
    /************************************************************************/
    // normalize file paths
    String src = File::normalizePath(fPath1);
    String dst = File::normalizePath(fPath2);
    OUTPUT_DBG("Copying file [ %s ] to [ %s ]", src.getString(), dst.getString());
    File::copyFile(src, dst, true);

	return 0;
}
