#ifndef SFUI_PARSERS_H_INCLUDED
#define SFUI_PARSERS_H_INCLUDED

#include "ComponentBase.h"
#include <CxxUtils/function_traits.h>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <cstdint>
#include <string>

namespace sfui {

sf::Color ParseColor(const nlohmann::json& json);
sf::Keyboard::Key ParseKey(const nlohmann::json& json);

using ComponentParser = std::function<void(ComponentBase&, const nlohmann::json&)>;
void SetComponentParser(std::string key, ComponentParser parser);
void ParseComponents(ComponentBase& parent, const nlohmann::json& json);
template <typename Component> constexpr auto MakeComponentParser = [](ComponentBase& parent, const nlohmann::json& json){ parent.Emplace<Component>(json); };

using ComponentHandlersParser = std::function<std::function<void()>(ComponentHandlers&, const nlohmann::json&)>;
std::function<void()> ParseComponentHandler(ComponentHandlers& component, const nlohmann::json& json);

using CallHandler = std::function<void(Component&)>;
struct RegisterCallHandler_ {
    template<typename F> RegisterCallHandler_(F&& handler, const std::string& handler_name) {
        if constexpr (CxxUtils::arguments_count<F> == 0) {
            Impl([handler = std::forward<F>(handler)](Component&){ handler(); }, handler_name);
        } else {
            using T = std::remove_reference_t<CxxUtils::first_argument_t<F>>;
            static_assert(std::is_base_of_v<Component, T>, "CallHandler must accept a Component or derived type argument!");

            if constexpr (std::is_same_v<T, Component>) {
                Impl(std::forward<F>(handler), handler_name);
            } else {
                Impl([handler = std::forward<F>(handler)](Component& component){ handler(dynamic_cast<T&>(component)); }, handler_name);
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
