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
		site.initialize(argc, argv);
		std::cout << "Connect to sockets manually by typing CONNECT" << std::endl;
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
		else if (answer == "CONNECT") {
			std::cout << "type port number you want to connect to " << std::endl;
			std::cin >> answer;
			site.connectTo(stoi(answer));
		}
	}
}