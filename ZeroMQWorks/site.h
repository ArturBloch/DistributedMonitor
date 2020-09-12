#pragma once
#include <string>
#include <vector>
#include <zmq.h>
#include <map>
#include "message.h"
#include "messageController.h"
#include <mutex>

class Site {
private:
	int messageIdCounter;
	std::map<std::string, std::vector<Message>> requestMessages;
	std::map<std::string, std::vector<Message>> replyMessages;
	std::map<std::string, Message> myRequests;
	std::mutex mtx;

	void dealTypeMessageToAllSites(std::string objectAddress, MessageType messageType);
	void zmqSendMessage(void* socket, Message message);
	void receiveMessages();
	void processReceivedMessage(Message message);

	bool requestWithPriority(Message message);
	void exitWithoutEntering(std::string objectAddress);
	bool canEnterCriticalSection(std::string objectAddress);

	long getClockTime();

public:
	Site();
	Site(int newPort, std::vector<int> otherPorts);
	~Site();

	int port;
	void* context;
	void* recvSocket;
	std::map<int, void*> peerSockets;

	void initialize(int argSize, char** argv);
	void enter(std::string objectAddress);
	void tryEnter(std::string objectAddress, int ms);
	void wait(std::string objectAddress);
	void exit(std::string objectAddress);
	void doWork();
};