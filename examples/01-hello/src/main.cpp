//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     : 1.0.0
// Project     : Hell World
// Copyright   : Aregtech © 2021
// Description : Hello World in AREG SDK
//============================================================================

#include "src/TestCommon.hpp"
#include "areg/src/appbase/CEApplication.hpp"
#include "areg/src/component/NERegistry.hpp"
#include "areg/src/component/CEStubBase.hpp"

class CEHelloWorldService   : public    CEComponent
                            , protected CEStubBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Stub by given component object, which should be already instantiated.
     * \param   masterComp  The master component object, which is initializing service Stub.
     * \note    Before constructor is called, the instance of CEComponent must be already initialized.
     **/
    CEHelloWorldService( CEComponent & masterComp );
    /**
     * \brief   Destructor.
     **/
    virtual ~CEHelloWorldService( void );

};

int main( void )
{
    return 0;
}
