//--------------------------------------------------------------
// Copyright (c) David Bosnich <david.bosnich.public@gmail.com>
//
// This code is licensed under the MIT License, a copy of which
// can be found in the license.txt file included at the root of
// this distribution, or at https://opensource.org/licenses/MIT
//--------------------------------------------------------------

#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

//--------------------------------------------------------------
namespace Simple
{
namespace Event
{

//--------------------------------------------------------------
//! Value returned by listener functions that determines whether
//! to continue dispatching an event to lower priority listeners.
//--------------------------------------------------------------
enum class Status
{
    Continue = 0,   //!< Keep dispatching the event.
    Consumed = 1    //!< Stop dispatching the event.
};

//--------------------------------------------------------------
//! Template class that maintains a collection of event listener
//! functions that are invoked each time the event is dispatched.
//!
//! \tparam Args Parameter pack that defines the event signature.
//--------------------------------------------------------------
template<class... Args>
class Dispatcher
{
public:
    using Function = std::function<Status(Args...)>;
    using Listener = std::shared_ptr<Function>;

    [[nodiscard]]
    Listener Register(Function a_function,
                      int32_t a_sortIndex = 0);

    void Dispatch(Args... a_args);

private:
    std::multimap<int32_t, std::weak_ptr<Function>> m_listeners;
    std::mutex m_listenersMutex;
};

//--------------------------------------------------------------
//! Registers a function to invoke when each event is dispatched.
//!
//! \param[in] a_function A function object that will be invoked.
//! \param[in] a_sortIndex Order in which to invoke the function.
//! \return Listener to retain while function should be invoked.
//!         Release all references to 'deregister' the function.
//--------------------------------------------------------------
template<class... Args>
inline typename Dispatcher<Args...>::Listener
Dispatcher<Args...>::Register(Function a_function,
                              int32_t a_sortIndex)
{
    std::lock_guard<std::mutex> lock(m_listenersMutex);

    // Create the listener then store it in a sorted container.
    Listener listener = std::make_shared<Function>(a_function);
    m_listeners.emplace(std::make_pair(a_sortIndex, listener));

    return listener;
}

//--------------------------------------------------------------
//! Sequentially dispatches an event to all registered listeners.
//! If a listener returns Status::Consumed the dispatch will end,
//! and no remaining (lower priority) listeners shall be invoked.
//!
//! \param[in] a_args Arguments forwarded to each event listener.
//--------------------------------------------------------------
template<class... Args>
inline void Dispatcher<Args...>::Dispatch(Args... a_args)
{
    // Gather non-expired listeners.
    std::vector<Listener> listeners;
    {
        std::lock_guard<std::mutex> lock(m_listenersMutex);
        listeners.reserve(m_listeners.size());

        // Iterate over all listeners.
        auto it = m_listeners.begin();
        while (it != m_listeners.end())
        {
            if (Listener listener = it->second.lock())
            {
                // Copy non-expired listeners.
                listeners.push_back(listener);
                ++it;
            }
            else
            {
                // Prune expired listeners.
                it = m_listeners.erase(it);
            }
        }
    }

    // Send the event to each listener.
    for (Listener listener : listeners)
    {
        if (Function func = *listener)
        {
            Status status = func(a_args...);
            if (status == Status::Consumed)
            {
                // Stop sending the event.
                break;
            }
        }
    }
}

} // namespace Event
} // namespace Simple
