#include "tests.h"
#include <iostream>
#include "site.h"

void runTestA(Site* site)
{
	site->enter("A");
	site->doWork();
	site->wait("A");
	site->doWork();
	site->exit("A");
}

void runTestB(Site* site)
{
	site->enter("A");
	site->doWork();
	site->notifyAll("A");
	site->exit("A");
}

void runTestC(Site* site)
{
	site->enter("C");
	site->doWork();
	site->exit("C");
}
