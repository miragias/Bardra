#pragma once

#include <vector>
#include <functional>

template <typename... Args>

class Event 
{
    private:
        std::vector<std::function<void(Args...)>> observers;

    public:
        void Subscribe(std::function<void(Args...)> observer) {
            observers.push_back(observer);
        }

        void Invoke(Args... args) {
            for (const auto& observer : observers) {
                observer(args...);
            }
        }
};
