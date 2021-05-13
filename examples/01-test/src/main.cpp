//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     : 1.0.0
// Project     : Hell World
// Copyright   : Aregtech © 2021
// Description : Hello World in AREG SDK
//============================================================================

#include "src/TestCommon.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/NERegistry.hpp"
#include "areg/component/StubBase.hpp"

class HelloWorldService : public    Component
                        , protected StubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of Component must be already initialized.
     **/
    HelloWorldService( Component & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~HelloWorldService( void );

};

int main( void )
{
    return 0;
}
