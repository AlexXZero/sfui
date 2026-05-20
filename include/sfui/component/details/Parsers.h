#ifndef SFUI_PARSERS_H_INCLUDED
#define SFUI_PARSERS_H_INCLUDED

#include "ComponentBase.h"
#include "../../ConfigParser.h"
#include <CxxUtils/function_traits.h>
#include <SFML/Graphics.hpp>
#include <functional>
#include <cstdint>
#include <string>

namespace sfui {

// Enables `a || b` as a fallback operator for std::optional values.
// Usage: `using sfui::optional_fallback::operator||;`, then just `a || b` bellow
namespace optional_fallback {
template<typename T>
std::optional<T> operator||(const std::optional<T>& lhs, const std::optional<T>& rhs) {
    return lhs ? lhs : rhs;
}
} // namespace optional_fallback

// SFML types parsers
template<> struct ConfigParser<sf::Color> {
    [[nodiscard]] static sf::Color parse(ConfigView config);
};
template<> struct ConfigParser<sf::Keyboard::Key> {
    [[nodiscard]] static sf::Keyboard::Key parse(ConfigView config);
};
using TextStyle = std::uint32_t; // combination of sf::Text::Style flags
template<> struct ConfigParser<TextStyle> {
    [[nodiscard]] static TextStyle parse(ConfigView config);
};
enum TextAlignment { Left, Right, Center };
template<> struct ConfigParser<TextAlignment> {
    [[nodiscard]] static TextAlignment parse(ConfigView config);
};

using ComponentHandlersParser = std::function<std::function<void()>(ComponentHandlers&, const nlohmann::json&)>;
std::function<void()> ParseComponentAction(ComponentHandlers& component, ConfigView actionConfig);

using CallHandler = std::function<void(ComponentHandlers&)>;
struct RegisterCallHandler_ {
    template<typename F> RegisterCallHandler_(F&& handler, const std::string& handler_name) {
        if constexpr (CxxUtils::arguments_count<F> == 0) {
            Impl([handler = std::forward<F>(handler)](ComponentHandlers&){ handler(); }, handler_name);
        } else {
            using T = std::remove_reference_t<CxxUtils::first_argument_t<F>>;
            static_assert(std::is_base_of_v<ComponentHandlers, T>, "CallHandler must accept a Component or derived type argument!");

            if constexpr (std::is_same_v<T, ComponentHandlers>) {
                Impl(std::forward<F>(handler), handler_name);
            } else {
                Impl([handler = std::forward<F>(handler)](ComponentHandlers& component){ handler(dynamic_cast<T&>(component)); }, handler_name);
            }
        }
    }

private:
    void Impl(CallHandler&& handler, const std::string& handler_name);
};
#define SFUI_GET_COUNT(_1, _2, COUNT, ...) COUNT
#define SFUI_COUNT_ARGS(...) SFUI_GET_COUNT(__VA_ARGS__, 2, 1, 0)
#define RegisterCallHandler(...) SFUI_REGISTER_CALL_HANDLER_IMPL_(SFUI_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_IMPL_(...) SFUI_REGISTER_CALL_HANDLER_IMPL(__VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_IMPL(count, ...) SFUI_REGISTER_CALL_HANDLER_##count(__VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_1(f) RegisterCallHandler_ SFUI_Handler##__LINE__(f, #f)
#define SFUI_REGISTER_CALL_HANDLER_2(f, name) RegisterCallHandler_ SFUI_Handler##__LINE__(f, name)
#define RegisterCallHandlerRuntime(...) SFUI_REGISTER_CALL_HANDLER_RUNTIME_IMPL_(SFUI_COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_RUNTIME_IMPL_(...) SFUI_REGISTER_CALL_HANDLER_RUNTIME_IMPL(__VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_RUNTIME_IMPL(count, ...) SFUI_REGISTER_CALL_HANDLER_RUNTIME_##count(__VA_ARGS__)
#define SFUI_REGISTER_CALL_HANDLER_RUNTIME_1(f) RegisterCallHandler_(f, #f)
#define SFUI_REGISTER_CALL_HANDLER_RUNTIME_2(f, name) RegisterCallHandler_(f, name)

}

#endif // SFUI_PARSERS_H_INCLUDED
