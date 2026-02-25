//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of file objects. It creates 
//               text and binary files, it shows how files can be copied, 
//               use of masks in file names, relative and full paths. 
//               For example, how the mask %time% (time-stamp) or %appname%
//               (process name) can be used.In the areg framework file objects
//               are also streaming objects.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/File.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/String.hpp"

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    //!< Print a separator line
    void printSeparator(char ch = '*', int32_t count = 20)
    {
        std::cout << std::string(count, ch) << std::endl;
    }

    //!< Write text to the file
    void writeText(FileBase & file)
    {
        printSeparator();
        if (!file.isValid())
        {
            std::cerr << "Invalid file " << file.getName() << ". Cannot write text ..." << std::endl;
            return;
        }

        std::cout << "Writing text to file [" << file.getName() << "] ..." << std::endl;
        file.write("!!!Hello World!!!");
        file << "This is some text." << "And this one is another part of text.";
    }

    //!< Write lines to the file
    void write_lines(FileBase & file)
    {
        printSeparator();
        if (!file.isValid())
        {
            std::cerr << "Invalid file " << file.getName() << ". Cannot write lines ..." << std::endl;
            return;
        }

        std::cout << "Writing text to file [" << file.getName() << "] ..." << std::endl;
        file.write_line("!!!Hello World!!!");
        file.write_line("This is some text.");
        file.write_line("And this one is another part of text.");
    }

    //!< Dump file content to console
    void dumpText(FileBase & file)
    {
        printSeparator();
        if (!file.isValid())
        {
            std::cerr << "Invalid file " << file.getName() << ". Cannot dump text ..." << std::endl;
            return;
        }

        file.moveToBegin();
        String text;
        file >> text;

        std::cout << "BEGIN File [" << file.getName() << "] content >>>" << std::endl;
        std::cout << text << std::endl;
        std::cout << "END File [" << file.getName() << "] content <<<" << std::endl;
    }

} // namespace

//////////////////////////////////////////////////////////////////////////
// File functionalities demo
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo to show file functionalities ..." << std::endl;

    constexpr uint32_t mode = static_cast<uint32_t>(FileBase::OpenMode::Write)
                            | static_cast<uint32_t>(FileBase::OpenMode::Text)
                            | static_cast<uint32_t>(FileBase::OpenMode::Create)
                            | static_cast<uint32_t>(FileBase::OpenMode::ShareRead)
                            | static_cast<uint32_t>(FileBase::OpenMode::ShareWrite);
    // Create a text file on file system
    File txtFile("./Debug/hello.txt", mode);
    if (txtFile.open())
    {
        writeText(txtFile);
        dumpText(txtFile);
    }

    // In-memory file with timestamp mask
    FileBuffer buffer;
    if (buffer.open("Buffer_%time%", mode))
    {
        writeText(buffer);
        dumpText(buffer);
    }

    // File with appname mask, write line-by-line
    File lineFile;
    if (lineFile.open("../../../../temp/%appname%.txt", mode))
    {
        write_lines(lineFile);
        dumpText(lineFile);
    }

    // Binary file (text mode removed)
    File binary("./Debug/binary.dat", mode & ~static_cast<uint32_t>(FileBase::OpenMode::Text));
    if (binary.open())
    {
        binary.write(buffer.getDataBuffer(), buffer.getLength());
        dumpText(binary);
        binary.reserve(789);
    }

    // Close all files
    txtFile.close();
    lineFile.close();
    buffer.close();
    binary.close();

    // Copy and normalize paths
    String src = File::normalizePath("./Debug/hello.txt");
    String dst = File::normalizePath("./Debug/copy_%time%.txt");

    printSeparator('.', 20);
    std::cout << "Copying file [" << src << "] to [" << dst << "]" << std::endl;
    File::copyFile(src, dst, true);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
