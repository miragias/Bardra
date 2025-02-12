#pragma once
#include "stdafx.h"

#include <unordered_map>
#include <functional>

template <typename... Args>
class Event {
private:
    std::unordered_map<size_t, std::function<void(Args...)>> observers;
    size_t nextID = 0;  // Unique ID generator

public:
    using ListenerID = size_t;

    // Subscribe returns an ID that can be used to unsubscribe
    ListenerID Subscribe(std::function<void(Args...)> observer) 
    {
        size_t id = nextID++;
        observers[id] = observer;
        return id;
    }

    // Unsubscribe using the ID
    void Unsubscribe(ListenerID id) 
    {
        observers.erase(id);
    }

    // Invoke all listeners
    void Invoke(Args... args) 
    {
        for (auto it = observers.begin(); it != observers.end(); ++it) 
        {
            it->second(args...);  // it->second is the callback function
        }
    }
};
