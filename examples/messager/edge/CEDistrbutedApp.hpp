// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// CEDistrbutedApp.hpp : main header file for the distrbutedapp application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "edge/res/resource.h"       // main symbols
#include "areg/src/component/NERegistry.hpp"


// CEDistrbutedApp:
// See CEDistrbutedApp.cpp for the implementation of this class
//

class CEDistrbutedApp : public CWinApp
{
public:
    CEDistrbutedApp();

    // Overrides
public:
    virtual BOOL InitInstance();

    // Implementation

    DECLARE_MESSAGE_MAP()
    virtual int ExitInstance( );
};

extern CEDistrbutedApp theApp;