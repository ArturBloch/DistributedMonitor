#pragma once
#include "message.h"

namespace Helper {
    std::string enum_to_string(MessageType messageType);
    MessageType string_to_enum(std::string messageType);
};