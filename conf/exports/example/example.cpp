// example.cpp : Checks availability and compilation with 'areg' library.
//

#include <iostream>
#include "areg/base/String.hpp"
#include "areg/appbase/AppDefs.hpp"

int main()
{
	areg::String hello("Hello from Areg SDK ...");
	std::cout << hello.data() << std::endl;

	hello = "\'areg.init\' location -> ";
	hello += areg::DEFAULT_CONFIG_FILE; // <== 'areg.init' file location
	std::cout << hello.data() << std::endl;

	return 0;
}
