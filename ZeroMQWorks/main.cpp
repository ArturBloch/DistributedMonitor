#include <zmq.h>
#include <iostream>
#include "helper.h"
#include "tests.h"
#include "message.h"
#include "site.h"

int main(int argc, char** argv)
{
	Site site;
	std::string arg1(argv[1]);
	if (arg1 == "run" && argc > 3) {
		site.initialize(argc, argv);
	}
	else {
		std::cout << "Wrong initialization" << std::endl;
		return 0;
	}
	std::string answer;
	while (true) {
		std::cin >> answer;
		if (answer == "TESTA"){
			runTestA(&site);
		}
		else if (answer == "TESTB") {
			runTestB(&site);
		}
		else if (answer == "TESTC") {
			runTestC(&site);
		}
	}
}