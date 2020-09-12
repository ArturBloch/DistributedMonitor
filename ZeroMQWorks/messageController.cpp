#include "MessageController.h"
#include "zmq.h"

MessageController::MessageController()
{
	context = zmq_ctx_new();
	
}

void MessageController::sendMessage(int port)
{
}
