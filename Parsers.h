#ifndef SFUI_PARSERS_H_INCLUDED
#define SFUI_PARSERS_H_INCLUDED

#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <cstdint>
#include <string>
#include "Component.h"
#include "ComponentContainer.h"

namespace sfui {

sf::Color ParseColor(const nlohmann::json& json);
sf::Keyboard::Key ParseKey(const nlohmann::json& json);

using ComponentParser = std::function<void(ComponentContainer&, const nlohmann::json&)>;
void SetComponentParser(std::string key, ComponentParser parser);
void ParseComponents(ComponentContainer& parent, const nlohmann::json& json);
template <typename Component> constexpr auto MakeComponentParser = [](ComponentContainer& parent, const nlohmann::json& json){ parent.Emplace<Component>(json); };

using ComponentHandlersParser = std::function<std::function<void()>(ComponentHandlers&, const nlohmann::json&)>;
std::function<void()> ParseComponentHandler(ComponentHandlers& component, const nlohmann::json& json);

using CallHandler = std::function<void(Component&)>;
struct RegisterCallHandler_ { RegisterCallHandler_(CallHandler&& handler, const std::string& handler_name); };
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
