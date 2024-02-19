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

}

#endif // SFUI_PARSERS_H_INCLUDED
