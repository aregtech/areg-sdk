//============================================================================
// Name        : test-areg-basic.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "areg/appbase/Application.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/File.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/NEMemory.hpp"
#include "src/services/CESystem.hpp"

BEGIN_MODEL("test_areg_basic")

    BEGIN_REGISTER_THREAD( "ThreadSystem" )
        BEGIN_REGISTER_COMPONENT( "CompSystem", CESystem )
            REGISTER_IMPLEMENT_SERVICE( NESystem::ServiceName, 1, 0, 0 )
        END_REGISTER_COMPONENT( "CompSystem" )
    END_REGISTER_THREAD( "ThreadSystem" )

END_MODEL("test_areg_basic")


int main()
{
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
#if 1
	File file("./debug/temp/test.txt", FileBase::FO_MODE_CREATE | FileBase::FO_MODE_WRITE | FileBase::FO_MODE_SHARE_READ | FileBase::FO_MODE_SHARE_WRITE);
	if ( file.open() )
	{
	    OUTPUT_DBG("Create File [ %s ]", file.getName());
	}

	const char * nextPos = NULL;
	const char * lastPos = NULL;
	char fileName[File::MAXIMUM_PATH + 1];
	NEString::copyString<char>(fileName, File::MAXIMUM_PATH + 1, file.getName(), NEString::CountAll );

	OUTPUT_DBG("List of parents for path [ %s ]", fileName);
	for ( int i = 1; File::findParent(fileName, &nextPos, lastPos); ++ i)
	{
	    fileName[nextPos - fileName] = NEString::EndOfString;
	    lastPos = nextPos;
	    OUTPUT_DBG("    >>  %d. %s", i, fileName);
	}

	file.reserve(12);

	file.close();

	File::copyFile(file.getName(), "./temp/data/test1.txt", true);

	String fullPath = File::getFileFullPath("./test.txt");
	OUTPUT_DBG("Full path [ % s]", fullPath.getBuffer());

	NEMemory::sByteBuffer b;
	OUTPUT_DBG("sByteBuffer address [ %u ], size header [ %u ], addr data ptr [ %u ], addr data elem [ %u ], addr with offset [ %u ]"
	            , reinterpret_cast<unsigned char *>(&b)
	            , sizeof(NEMemory::sBuferHeader)
	            , reinterpret_cast<unsigned char *>(&b.bufData)
	            , reinterpret_cast<unsigned char *>(&b.bufData[0])
                , reinterpret_cast<unsigned char *>(&b) + sizeof(NEMemory::sBuferHeader));

	SharedBuffer dummy;
	char temp[16];
	for (int i = 1; i <= 30; ++ i)
	{
	    sprintf(temp, "count %d", i);
	    String str(temp);
	    dummy << str;
	}

	dummy.moveToBegin();

	for (int i = 0; i < 30; ++ i)
	{
	    String str;
	    dummy >> str;
	    printf("%s\n", str.getString());
	}
#endif

	Application::initApplication(true, true, true, true, NULL, NULL);

	Application::loadModel( "test_areg_basic" );

	// Thread::Sleep(3000);

    char ch = getchar();
    if ( ch == '\0' )
    {
        OUTPUT_DBG("main >>> getchar() Received null-terminated symbol.");
    }
    else
    {
        OUTPUT_DBG("main >>> getchar() received [ %c ] symbol.", ch);
    }

	Application::releaseApplication();

	return 0;
}
