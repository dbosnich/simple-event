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
    Continue = 0, //!< Listener invoked, keep dispatching event.
    Consumed = 1, //!< Listener invoked, stop dispatching event.
    Filtered = 2 //!< Listener filtered, keep dispatching event.
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
    using Callable = std::function<Status(Args...)>;
    using Listener = std::shared_ptr<Callable>;

    [[nodiscard]]
    Listener Register(const Callable& a_callable,
                      const int32_t& a_sortIndex = 0);
    bool Remove(const Listener& a_listener);

    void Dispatch(Args... a_args);

    class Filter
    {
    public:
        using Function = std::function<bool(Args...)>;
        Filter(const Function& a_function,
               const Callable& a_callable);
        Status operator()(Args...) const;

    private:
        Function m_function;
        Callable m_callable;
    };

private:
    std::multimap<int32_t, std::weak_ptr<Callable>> m_listeners;
    std::mutex m_listenersMutex;
};

//--------------------------------------------------------------
//! Registers a callable to invoke when each event is dispatched.
//!
//! \param[in] a_callable A callable object that will be invoked.
//! \param[in] a_sortIndex Order in which to invoke the callable.
//! \return Listener to retain while callable should be invoked.
//!         Release all references to 'deregister' the callable.
//--------------------------------------------------------------
template<class... Args> inline
typename Dispatcher<Args...>::Listener
Dispatcher<Args...>::Register(const Callable& a_callable,
                              const int32_t& a_sortIndex)
{
    // Create the listener and add it to the container.
    std::lock_guard<std::mutex> lock(m_listenersMutex);
    Listener listener = std::make_shared<Callable>(a_callable);
    m_listeners.emplace(std::make_pair(a_sortIndex, listener));

    return listener;
}

//--------------------------------------------------------------
//! Remove a listener so not invoked when events are dispatched.
//!
//! \param[in] a_listener Listener object to stop being invoked.
//! \return True if the listener was removed or false otherwise.
//--------------------------------------------------------------
template<class... Args> inline
bool Dispatcher<Args...>::Remove(const Listener& a_listener)
{
    // Find and remove the listener from the container.
    std::lock_guard<std::mutex> lock(m_listenersMutex);
    const auto& listenersBegin = m_listeners.begin();
    const auto& listenersEnd = m_listeners.end();
    for (auto it = listenersBegin; it != listenersEnd; ++it)
    {
        if (it->second.lock() == a_listener)
        {
            m_listeners.erase(it);
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------
//! Sequentially dispatches an event to all registered listeners.
//! If a listener returns Status::Consumed the dispatch will end,
//! and no remaining (lower priority) listeners shall be invoked.
//!
//! \param[in] a_args Arguments forwarded to each event listener.
//--------------------------------------------------------------
template<class... Args> inline
void Dispatcher<Args...>::Dispatch(Args... a_args)
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
        if (Callable callable = *listener)
        {
            Status status = callable(a_args...);
            if (status == Status::Consumed)
            {
                // Stop sending the event.
                break;
            }
        }
    }
}

//--------------------------------------------------------------
//! Filter objects are essentially event listeners that are only
//! invoked if a filter function with the same args returns true.
//!
//! \param[in] a_function Function to filter all incoming events.
//! \param[in] a_callable Callable to be invoked unless filtered.
//--------------------------------------------------------------
template<class... Args> inline
Dispatcher<Args...>::Filter::Filter(const Function& a_function,
                                    const Callable& a_callable)
    : m_function(a_function)
    , m_callable(a_callable)
{
}

//--------------------------------------------------------------
//! Function call operator which allows the filter object to be
//! registered directly with an event dispatcher as a callable.
//!
//! \param[in] a_args Arguments forwarded to filter and callable.
//--------------------------------------------------------------
template<class... Args> inline
Status Dispatcher<Args...>::Filter::operator()(Args... a_args) const
{
    return (m_callable && m_function && m_function(a_args...)) ?
            m_callable(a_args...) : Status::Filtered;
}

} // namespace Event
} // namespace Simple
