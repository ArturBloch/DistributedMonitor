#pragma once
#include <string>

enum class MessageType { REQUEST, REPLY, REMOVE, OTHER, NOTIFY};

class Message {

public:
    Message();
    int messageId;
    long clockFromProcess;
    int port;
    int responseTo;
    std::string objectAddress;
    MessageType messageType;

    Message(char* fromMessage);
    Message(int messageId, int port, std::string fromMessage);
    Message(int messageId, int responseTo, int port, long clockFromProcess, std::string objectAddress, MessageType messageType);
    bool isRequest();
    bool isReply();
    bool isRemove();
    bool isOther();
    bool isNotify();
    std::string serialize();
    std::string toString();
};