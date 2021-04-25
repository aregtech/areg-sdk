//============================================================================
// Name        : test-areg-basic.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "areg/src/appbase/CEApplication.hpp"
#include "areg/src/component/NERegistry.hpp"
#include "areg/src/component/CEComponentLoader.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/base/CEFile.hpp"
#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/NEMemory.hpp"
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
	CEFile file("./debug/temp/test.txt", CEFileBase::FO_MODE_CREATE | CEFileBase::FO_MODE_WRITE | CEFileBase::FO_MODE_SHARE_READ | CEFileBase::FO_MODE_SHARE_WRITE);
	if ( file.Open() )
	{
	    OUTPUT_DBG("Create File [ %s ]", file.GetName());
	}

	const char * nextPos = NULL;
	const char * lastPos = NULL;
	char fileName[CEFile::MAXIMUM_PATH + 1];
	NEString::copyString<char>(fileName, CEFile::MAXIMUM_PATH + 1, file.GetName(), NEString::CountAll );

	OUTPUT_DBG("List of parents for path [ %s ]", fileName);
	for ( int i = 1; CEFile::FindParent(fileName, &nextPos, lastPos); ++ i)
	{
	    fileName[nextPos - fileName] = NEString::EndOfString;
	    lastPos = nextPos;
	    OUTPUT_DBG("    >>  %d. %s", i, fileName);
	}

	file.Reserve(12);

	file.Close();

	CEFile::FileCopy(file.GetName(), "./temp/data/test1.txt", true);

	CEString fullPath = CEFile::GetFileFullPath("./test.txt");
	OUTPUT_DBG("Full path [ % s]", fullPath.GetBuffer());

	NEMemory::sByteBuffer b;
	OUTPUT_DBG("sByteBuffer address [ %u ], size header [ %u ], addr data ptr [ %u ], addr data elem [ %u ], addr with offset [ %u ]"
	            , reinterpret_cast<unsigned char *>(&b)
	            , sizeof(NEMemory::sBuferHeader)
	            , reinterpret_cast<unsigned char *>(&b.bufData)
	            , reinterpret_cast<unsigned char *>(&b.bufData[0])
                , reinterpret_cast<unsigned char *>(&b) + sizeof(NEMemory::sBuferHeader));

	CESharedBuffer dummy;
	char temp[16];
	for (int i = 1; i <= 30; ++ i)
	{
	    sprintf(temp, "count %d", i);
	    CEString str(temp);
	    dummy << str;
	}

	dummy.MoveToBeginOfData();

	for (int i = 0; i < 30; ++ i)
	{
	    CEString str;
	    dummy >> str;
	    printf("%s\n", str.GetBuffer());
	}
#endif

	CEApplication::Initialize(true, true, true, true, NULL, NULL);

	CEApplication::StartModel( "test_areg_basic" );

	CEThread::Sleep(3000);

    char ch = getchar();
    if ( ch == '\0' )
    {
        OUTPUT_DBG("main >>> getchar() Received null-terminated symbol.");
    }
    else
    {
        OUTPUT_DBG("main >>> getchar() received [ %c ] symbol.", ch);
    }

	CEApplication::Release();

	return 0;
}
