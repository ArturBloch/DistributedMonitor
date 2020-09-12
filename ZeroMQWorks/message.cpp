#include "message.h"
#include "helper.h"

Message::Message()
{
}

Message::Message(char* fromMessage)
{
    std::string newMessage = std::string(fromMessage);
    std::string delimiter = ";";
    messageId = std::stoi(newMessage.substr(0, newMessage.find(delimiter)));
    newMessage.erase(0, newMessage.find(delimiter) + delimiter.length());
    responseTo = std::stoi(newMessage.substr(0, newMessage.find(delimiter)));
    newMessage.erase(0, newMessage.find(delimiter) + delimiter.length());
    port = std::stoi(newMessage.substr(0, newMessage.find(delimiter)));
    newMessage.erase(0, newMessage.find(delimiter) + delimiter.length());
    clockFromProcess = std::stol(newMessage.substr(0, newMessage.find(delimiter)));
    newMessage.erase(0, newMessage.find(delimiter) + delimiter.length());
    objectAddress = newMessage.substr(0, newMessage.find(delimiter));
    newMessage.erase(0, newMessage.find(delimiter) + delimiter.length());
    messageType = Helper::string_to_enum(newMessage.substr(0, newMessage.find(delimiter)));
}

Message::Message(int _messageId, int _port, std::string fromMessage)
{
    messageId = _messageId;
    objectAddress = fromMessage;
    port = _port;
    clockFromProcess = -1;
    messageType = MessageType::OTHER;
    responseTo = -1;
}

Message::Message(int _messageId, int _responseTo, int _port, long _clockFromProcess, std::string _objectAddress, MessageType _messageType)
    : messageId(_messageId), responseTo(_responseTo), port(_port), clockFromProcess(_clockFromProcess), objectAddress(_objectAddress), messageType(_messageType) {}

bool Message::isRequest()
{
    return Message::messageType == MessageType::REQUEST;
}

bool Message::isReply()
{
    return Message::messageType == MessageType::REPLY;
}

bool Message::isRemove()
{
    return Message::messageType == MessageType::REMOVE;
}

bool Message::isOther()
{
    return Message::messageType == MessageType::OTHER;
}

std::string Message::serialize()
{
    return std::to_string(messageId) + ";" + std::to_string(responseTo) + ";" + std::to_string(port) + ";" + std::to_string(clockFromProcess) + ";" + objectAddress + ";" + Helper::enum_to_string(messageType);
}

std::string Message::toString() {
    return "\n  MESSAGE_ID: " + std::to_string(messageId) + "\n  RESPONSE_TO: " + std::to_string(responseTo) + "\n  PORT: " + std::to_string(port) + "\n  CLOCK_TIME: " + std::to_string(clockFromProcess) + "\n  OBJECT_ADDRESS: " + objectAddress + "\n  MESSAGE_TYPE: " + Helper::enum_to_string(messageType);
}
