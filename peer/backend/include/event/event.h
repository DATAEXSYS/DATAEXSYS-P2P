#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <any>
#include <chrono>
#include <unordered_map>

class Event {
public:
    // Constructor
    Event(const std::string& type,
          std::any payload = {},
          const std::string& source = "")
        : type(type),
          payload(payload),
          source(source),
          timestamp(std::chrono::steady_clock::now()) {}

    // Getters
    const std::string& getType() const {
        return type;
    }

    const std::any& getPayload() const {
        return payload;
    }

    const std::string& getSource() const {
        return source;
    }

    std::chrono::steady_clock::time_point getTimestamp() const {
        return timestamp;
    }

    // Metadata (optional key-value)
    void setMeta(const std::string& key, const std::any& value) {
        metadata[key] = value;
    }

    std::any getMeta(const std::string& key) const {
        auto it = metadata.find(key);
        if (it != metadata.end()) {
            return it->second;
        }
        return {};
    }

private:
    std::string type;
    std::any payload;
    std::string source;

    std::chrono::steady_clock::time_point timestamp;

    std::unordered_map<std::string, std::any> metadata;
};

#endif