//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : This project demonstrates use of file objects. It creates 
//               text and binary files, it shows how files can be copied, 
//               use of masks in file names, relative and full paths. 
//               For example, how the mask %time% (time-stamp) or %appname%
//               (process name) can be used.In the areg framework file objects
//               are also streaming objects.
//============================================================================

#include "areg/base/areg_global.h"
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
    void print_separator(char ch = '*', int32_t count = 20)
    {
        std::cout << std::string(static_cast<size_t>(count), ch) << std::endl;
    }

    //!< Write text to the file
    void write_text(areg::FileBase & file)
    {
        print_separator();
        if (!file.is_valid())
        {
            std::cerr << "Invalid file " << file.name() << ". Cannot write text ..." << std::endl;
            return;
        }

        std::cout << "Writing text to file [" << file.name() << "] ..." << std::endl;
        file.write("!!!Hello World!!!");
        file << "This is some text." << "And this one is another part of text.";
    }

    //!< Write lines to the file
    void write_lines(areg::FileBase & file)
    {
        print_separator();
        if (!file.is_valid())
        {
            std::cerr << "Invalid file " << file.name() << ". Cannot write lines ..." << std::endl;
            return;
        }

        std::cout << "Writing text to file [" << file.name() << "] ..." << std::endl;
        file.write_line("!!!Hello World!!!");
        file.write_line("This is some text.");
        file.write_line("And this one is another part of text.");
    }

    //!< Dump file content to console
    void dump_text(areg::FileBase & file)
    {
        print_separator();
        if (!file.is_valid())
        {
            std::cerr << "Invalid file " << file.name() << ". Cannot dump text ..." << std::endl;
            return;
        }

        file.move_to_begin();
        areg::String text;
        file >> text;

        std::cout << "BEGIN File [" << file.name() << "] content >>>" << std::endl;
        std::cout << text << std::endl;
        std::cout << "END File [" << file.name() << "] content <<<" << std::endl;
    }

} // namespace

//////////////////////////////////////////////////////////////////////////
// File functionalities demo
//////////////////////////////////////////////////////////////////////////
int main()
{
    std::cout << "Demo to show file functionalities ..." << std::endl;

    constexpr uint32_t mode = static_cast<uint32_t>(areg::FileBase::OpenMode::Write)
                            | static_cast<uint32_t>(areg::FileBase::OpenMode::Text)
                            | static_cast<uint32_t>(areg::FileBase::OpenMode::Create)
                            | static_cast<uint32_t>(areg::FileBase::OpenMode::ShareRead)
                            | static_cast<uint32_t>(areg::FileBase::OpenMode::ShareWrite);
    // Create a text file on file system
    areg::File txtFile("./Debug/hello.txt", mode);
    if (txtFile.open())
    {
        write_text(txtFile);
        dump_text(txtFile);
    }

    // In-memory file with timestamp mask
    areg::FileBuffer buffer;
    if (buffer.open("Buffer_%time%", mode))
    {
        write_text(buffer);
        dump_text(buffer);
    }

    // File with appname mask, write line-by-line
    areg::File lineFile;
    if (lineFile.open("./temp/%appname%.txt", mode))
    {
        write_lines(lineFile);
        dump_text(lineFile);
    }

    // Binary file (text mode removed)
    areg::File binary("./Debug/binary.dat", mode & ~static_cast<uint32_t>(areg::FileBase::OpenMode::Text));
    if (binary.open())
    {
        binary.write(buffer.data_buffer(), buffer.length());
        dump_text(binary);
        binary.reserve(789);
    }

    // Close all files
    txtFile.close();
    lineFile.close();
    buffer.close();
    binary.close();

    // Copy and normalize paths
    areg::String src = areg::File::normalize_path("./Debug/hello.txt");
    areg::String dst = areg::File::normalize_path("./Debug/copy_%time%.txt");

    print_separator('.', 20);
    std::cout << "Copying file [" << src << "] to [" << dst << "]" << std::endl;
    areg::File::copy_file(src, dst, true);

    std::cout << "Exit application!" << std::endl;
    return 0;
}
