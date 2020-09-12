#include "site.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <chrono>
#include "message.h"
#include <map>
#include "helper.h"
#include <thread>

void Site::dealTypeMessageToAllSites(std::string objectAddress, MessageType messageType)
{
	mtx.lock();
	long messageId = messageIdCounter;
	Message message = Message(messageId, -1, port, getClockTime(), objectAddress, messageType);
	myRequests[objectAddress] = message; // add the message to your own vector as well
	for (std::map<int, void*>::iterator it = peerSockets.begin(); it != peerSockets.end(); ++it) { // send the messages to all other sites
		char* buffer = new char[255];
		zmqSendMessage(it->second, message);
		memset(buffer, 0, 255);
		zmq_recv(it->second, buffer, 255, 0);
		Message message = Message(buffer);
		processReceivedMessage(message);
	}
	mtx.unlock();
}

void Site::zmqSendMessage(void* socket, Message message)
{
	zmq_send(socket, message.serialize().c_str(), 255, 0);
	messageIdCounter++;
}

Site::Site()
{
	context = zmq_ctx_new();
	messageIdCounter = 0;
}

Site::Site(int newPort, std::vector<int> otherPorts)
{
}

Site::~Site()
{
	zmq_close(recvSocket);
	for (std::map<int, void*>::iterator it = peerSockets.begin(); it != peerSockets.end(); ++it) {
		zmq_close(it->second);
	}
	zmq_ctx_destroy(context);
}

void Site::initialize(int argSize, char** argv)
{
	port = atoi(argv[2]);
	context = zmq_ctx_new();
	recvSocket = zmq_socket(context, ZMQ_REP);
	std::string siteRecvSocket = "tcp://127.0.0.1:" + std::to_string(port);
	std::cout << "Listening on " << siteRecvSocket << std::endl;
	int rc = zmq_bind(recvSocket, siteRecvSocket.c_str());
	assert(rc == 0);

	for (int i = 3; i < argSize; i++) {
		int peerPortNumber = atoi(argv[i]);
		void* sendSocket = zmq_socket(context, ZMQ_REQ);
		std::string siteSendSocket = "tcp://127.0.0.1:" + std::to_string(peerPortNumber);
		std::cout << "Connected to " << peerPortNumber << std::endl;
		int rc = zmq_connect(sendSocket, siteSendSocket.c_str());
		peerSockets[peerPortNumber] = sendSocket;
		assert(rc == 0);
	}

	std::thread receiveThread(&Site::receiveMessages, this);
	receiveThread.detach();
}

void Site::enter(std::string objectAddress)
{
	std::cout << "\nTrying to enter " << objectAddress << std::endl;
	dealTypeMessageToAllSites(objectAddress, MessageType::REQUEST);
	while (!canEnterCriticalSection(objectAddress)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void Site::tryEnter(std::string objectAddress, int ms)
{
	std::cout << "\nTrying to enter " << objectAddress << " for specified " << ms << " miliseconds" <<  std::endl;
	dealTypeMessageToAllSites(objectAddress, MessageType::REQUEST);
	auto start = std::chrono::steady_clock::now();
	auto end = std::chrono::steady_clock::now();
	auto timeSpentTrying = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	while (!canEnterCriticalSection(objectAddress) && timeSpentTrying < ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		end = std::chrono::steady_clock::now();
		timeSpentTrying = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	}
	if (canEnterCriticalSection(objectAddress)) {
		std::cout << "\nManaged to acquire the lock " << objectAddress << std::endl;
		exit(objectAddress);
	}
	else {
		std::cout << "\nDidn't manage to acquire the lock " << objectAddress << std::endl;
		exitWithoutEntering(objectAddress);
	}
}

void Site::wait(std::string objectAddress) {
	exit(objectAddress);
	enter(objectAddress);
}

void Site::exit(std::string objectAddress) {
	mtx.lock();
	std::cout << "Exiting critical section" << objectAddress << std::endl;
	myRequests.erase(objectAddress);
	for (std::vector<Message>::iterator it = requestMessages[objectAddress].begin(); it != requestMessages[objectAddress].end(); ++it) {
		Message replyMessage = Message(messageIdCounter, it->messageId, port, getClockTime(), objectAddress, MessageType::REPLY);
		zmqSendMessage(peerSockets[it->port], replyMessage);
		char* buffer = new char[255];
		memset(buffer, 0, 255);
		zmq_recv(peerSockets[it->port], buffer, 255, 0);
	}
	requestMessages[objectAddress].clear();
	replyMessages[objectAddress].clear();
	std::cout << "Exited critical section " << objectAddress << std::endl;
	mtx.unlock();
}

bool Site::canEnterCriticalSection(std::string objectAddress)
{
	mtx.lock();
	if (myRequests.find(objectAddress) == myRequests.end() || replyMessages[objectAddress].size() != peerSockets.size()) {
		std::cout << "Waiting on " << objectAddress << " " << replyMessages[objectAddress].size() << "/" << peerSockets.size() << std::endl;
		mtx.unlock();
		return false;
	}
	std::cout << "Entered " << objectAddress << " " << replyMessages[objectAddress].size() << "/" << peerSockets.size() << std::endl;
	mtx.unlock();
	return true;
}

void Site::doWork() {
	std::cout << "Doing work\n" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

void Site::receiveMessages()
{
	char* buffer = new char[255];
	std::cout << "Receiving function started on a different thread" << std::endl;
	while (true) {
		memset(buffer, 0, 255);
		if (zmq_recv(recvSocket, buffer, 255, 0) > -1) {
			mtx.lock();
			Message message = Message(buffer);
			processReceivedMessage(message);
			mtx.unlock();
		}
	}
}

void Site::processReceivedMessage(Message message)
{
	if (message.isOther()) {
		zmqSendMessage(recvSocket, Message(messageIdCounter, port, "OK"));
		return;
	}
	if (message.isRequest()) { 
		if (requestWithPriority(message)) { // if incoming request with priority then return REPLY immediately
			Message replyMessage = Message(messageIdCounter, message.messageId, port, getClockTime(), message.objectAddress, MessageType::REPLY);
			zmqSendMessage(recvSocket, replyMessage);
		}
		else { // request doesn't have priority so add to request list
			zmqSendMessage(recvSocket, Message(messageIdCounter, port, "OK"));
			requestMessages[message.objectAddress].push_back(message);
		}
	}
	else if (message.isReply()) { // add replies to the appropriate list
		zmqSendMessage(recvSocket, Message(messageIdCounter, port, "OK"));
		if (myRequests[message.objectAddress].messageId == message.responseTo) {
			replyMessages[message.objectAddress].push_back(message);
		}
	}
	else if (message.isRemove()) {
		zmqSendMessage(recvSocket, Message(messageIdCounter, port, "OK"));
		std::vector<Message> messageList = requestMessages[message.objectAddress];
		auto it = messageList.begin();
		while (it != messageList.end())
		{
			if (it->port == message.port && it->messageId == message.responseTo) {
				it = messageList.erase(it);
			}
			else {
				++it;
			}
		}
	}
}

bool Site::requestWithPriority(Message message) {
	std::map<std::string, Message>::iterator myRequest = myRequests.find(message.objectAddress);
	if (myRequest == myRequests.end()) return true;
	if (myRequest->second.clockFromProcess > message.clockFromProcess) return true;
	return false;
}

void Site::exitWithoutEntering(std::string objectAddress)
{
	std::cout << "Removing messages about entering " << objectAddress << " from the system " << std::endl;
	mtx.lock();
	for (std::vector<Message>::iterator it = requestMessages[objectAddress].begin(); it != requestMessages[objectAddress].end(); ++it) {
		Message replyMessage = Message(messageIdCounter, it->messageId, port, getClockTime(), objectAddress, MessageType::REPLY);
		zmqSendMessage(peerSockets[it->port], replyMessage);
		char* buffer = new char[255];
		memset(buffer, 0, 255);
		zmq_recv(peerSockets[it->port], buffer, 255, 0);
	}
	myRequests.erase(objectAddress);
	mtx.unlock();
	dealTypeMessageToAllSites(objectAddress, MessageType::REMOVE);
	requestMessages[objectAddress].clear();
	replyMessages[objectAddress].clear();
	std::cout << "Aborted the operation of entering " << objectAddress << std::endl;
}

long Site::getClockTime()
{	
	auto duration = std::chrono::system_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}
