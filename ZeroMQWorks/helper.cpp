#include <chrono>
#include <iostream>
#include "helper.h"
#include <ctime>
#include "message.h"

namespace Helper {
    std::string Helper::enum_to_string(MessageType messageType)
    {
        switch (messageType) {
        case MessageType::REQUEST:
            return "REQUEST";
        case MessageType::REPLY:
            return "REPLY";
        case MessageType::REMOVE:
            return "REMOVE";
        case MessageType::OTHER:
            return "OTHER";
        }
    }

    MessageType Helper::string_to_enum(std::string messageType)
    {
        if (messageType == "REQUEST") {
            return MessageType::REQUEST;
        }
        else if (messageType == "REPLY") {
            return MessageType::REPLY;
        }
        else if (messageType == "REMOVE"){
            return MessageType::REMOVE;
        } else {
            return MessageType::OTHER;
        }
        return MessageType();
    }
}