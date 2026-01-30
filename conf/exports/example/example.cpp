// example.cpp : Checks availability and compilation with 'areg' library.
//

#include <iostream>
#include "areg/base/String.hpp"

int main()
{
	String hello("Hello from Areg SDK ...");
	std::cout << hello.getData() << std::endl;

	hello = "\'areg.init\' location -> ";
	hello += AREG_SHARE_INIT; // <== 'areg.init' file location
	std::cout << hello.getData() << std::endl;

	return 0;
}
