#ifndef SFUI_OBSERVER_H_INCLUDED
#define SFUI_OBSERVER_H_INCLUDED

#include <functional>
#include <memory>

namespace sfui {

/**
 * @brief Type definition for an observer token used to automatically manage observer's lifetimes.
 */
struct [[nodiscard]] ObserverToken { std::shared_ptr<void> m_data; };

/**
 * @brief A class that manages a collection of observers.
 *
 * This class provides functionality to add, remove, and invoke a collection of observer handlers.
 * @tparam Handler The type of the observer handler.
 */
template<typename Handler>
class Observers {
public:
    /**
     * @brief Default constructor and destructor for the Observers class.
     */
    Observers() = default;
    ~Observers() = default;

    /**
     * @brief Add an observer handler.
     *
     * Creates a shared pointer to the provided handler and adds it to the list of handlers.
     * @param handler The handler to be added as an observer.
     * @return An observer token that can be used to remove the handler.
     */
    ObserverToken Set(Handler&& handler) {
        auto handler_sp = std::make_shared<Handler>(std::forward<Handler>(handler));
        m_handlers.emplace_back(handler_sp);
        return { handler_sp };
    }

    /**
     * @brief Invoke all observer handlers with given arguments.
     *
     * Invokes all non-expired observer handlers with the provided arguments.
     * @tparam Args The types of arguments to be passed to the observer handlers.
     * @param args The arguments to be passed to the observer handlers.
     * @note All expired handlers will be removed.
     */
    template<typename... Args>
    void Invoke(Args... args) /*const*/ { // method assumed to be const, but it can not be const due to its mutability (it removes expired handlers)
#if 1 // simple and clean version
        for (auto it = m_handlers.begin(); it != m_handlers.end();) {
            if (auto handler_sp = it->lock()) { // Invoke non-expired handlers
                (*handler_sp)(args...);
                ++it;
            } else { // Handler has expired, erase it
                it = m_handlers.erase(it);
            }
        }
#else // optimized version (erase is called only once at the end).
        auto is_handler_expired = [&](const std::weak_ptr<Handler>& handler_wp) {
            if (auto handler_sp = handler_wp.lock()) {
                (*handler_sp)(args...);
                return false;
            }

            return true; // wp is expired
        };

        auto& handlers = const_cast<std::vector<std::weak_ptr<Handler>>&>(m_handlers);
#if __cplusplus >= 202002L // `std::erase_if` support
        std::erase_if(m_handlers, is_handler_expired);
#else // Use erase-remove idiom
        m_handlers.erase(std::remove_if(m_handlers.begin(), m_handlers.end(), is_handler_expired), m_handlers.end());
#endif
#endif
    }

    /**
     * @brief Get the current number of registered observer handlers.
     *
     * Returns the count of observer handlers currently registered with the Observers instance.
     * This count includes both active and expired handlers, as they are still tracked.
     *
     * @return The number of registered observer handlers.
     */
    std::size_t Count() const {
        return m_handlers.size();
    }

private:
    std::vector<std::weak_ptr<Handler>> m_handlers; /**< A collection of observer handlers. */
    // TODO: mutable std::mutex m_mutex; // Consider adding mutex for thread safety if needed.
};

}

#endif // SFUI_OBSERVER_H_INCLUDED
