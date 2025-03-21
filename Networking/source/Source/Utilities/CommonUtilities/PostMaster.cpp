#include "PostMaster.h"
#include <stdio.h>

#include <iostream>


PostMaster* PostMaster::GetInstance()
{
    if (!instance)
    {
        instance = new PostMaster();
    }
        
    return instance;
}

void PostMaster::SubscribeToEvent(std::string eventName, FunctionPointer func, GameObject* aGameObject)
{
    EventSubscriber eventSubscriber;

    eventSubscriber.myFunc = func;
    eventSubscriber.myGameObject = aGameObject;

    subscribers[eventName].push_back(eventSubscriber);
}

void PostMaster::UnSubscribeFromEvent(std::string eventName, FunctionPointer func, GameObject* aGameObject)
{
    for (int i = 0; i < subscribers[eventName].size(); i++)
    {
        if (func == subscribers[eventName][i].myFunc && aGameObject == subscribers[eventName][i].myGameObject)
        {
            for (int j = i; j < subscribers[eventName].size() - 1; j++)
            {
                subscribers[eventName][j] = subscribers[eventName][j + 1];
            }
            
            subscribers[eventName].pop_back();

            break;
        }
    }
}

void PostMaster::InitEvent(std::string command)
{
    for (auto& subscriber : subscribers[command])
    {
        (*subscriber.myGameObject.*subscriber.myFunc)();
    }
}