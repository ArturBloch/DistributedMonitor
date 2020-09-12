#pragma once
#include <vector>
#include "message.h"
#include <map>
#include "zmq.h"

class MessageController {
private:
	std::vector<std::pair<long, Message>> requestQueue;
	std::map<std::string, std::vector<int>> replyReceived;
	void* context;

public:
	MessageController();
	void sendMessage(int port);
	void sendMessages(std::vector<int> port);

};