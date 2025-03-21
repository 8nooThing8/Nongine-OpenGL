#pragma once

#include <vector>

#include <string>

#include "EventSubscriber.hpp"

#include <map>

class PostMaster
{
public:
    PostMaster() = default;
    ~PostMaster() = default;

    void SubscribeToEvent(std::string eventName, FunctionPointer func, GameObject* aGameObject);

    void UnSubscribeFromEvent(std::string eventName, FunctionPointer func, GameObject* aGameObject);

    void InitEvent(std::string command);
    
    PostMaster(const PostMaster& obj) = delete; 

    static PostMaster* GetInstance();

private:
    std::map<std::string, std::vector<EventSubscriber>> subscribers;

    static PostMaster* instance;
};
