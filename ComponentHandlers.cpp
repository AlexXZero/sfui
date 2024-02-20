#include "ComponentHandlers.h"
#include "ComponentContainer.h"
#include "Parsers.h"

using namespace sfui;

// ComponentHandlers
ComponentHandlers::ComponentHandlers(ComponentContainer& parent, const nlohmann::json& json)
        : ComponentGeometry(parent, json), m_enabled(true), m_visible(true)
{
    // parse optional properties
    if (json.contains("enabled"))   m_enabled   = json["enabled"]   .get<bool>();
    if (json.contains("visible"))   m_visible   = json["visible"]   .get<bool>();
    if (json.contains("onKeyPress")) {
        for (const auto& handler_json: json["onKeyPress"]) {
            if (handler_json.contains("key")) {
                sf::Keyboard::Key key = ParseKey(handler_json["key"]);
                auto handler = ParseComponentHandler(*this, handler_json);
                LinkEvent(OnKeyPress(key, std::move(handler)));
            }
        }
    }
    if (json.contains("onClick")) {
        for (const auto& handler_json: json["onClick"]) {
            // TODO: if (handler_json.contains("button"))
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseLeftClick(std::move(handler)));
        }
    }
    if (json.contains("onRender")) {
        for (const auto& handler_json: json["onRender"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnRender(std::move(handler)));
        }
    }
    if (json.contains("onShow")) {
        for (const auto& handler_json: json["onShow"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnShow(std::move(handler)));
        }
    }
    if (json.contains("onHide")) {
        for (const auto& handler_json: json["onHide"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnHide(std::move(handler)));
        }
    }
    if (json.contains("onEnable")) {
        for (const auto& handler_json: json["onEnable"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnEnable(std::move(handler)));
        }
    }
    if (json.contains("onDisable")) {
        for (const auto& handler_json: json["onDisable"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnDisable(std::move(handler)));
        }
    }
}

#include <iostream>
bool ComponentHandlers::HandleEvent_(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::TextEntered:
        if (m_text_enter_handlers.Count() > 0) {
            m_text_enter_handlers.Invoke(event.text.unicode);
            return true;
        }
        break;

    case sf::Event::KeyPressed:
        if (m_key_pressed_handlers.count(event.key.code) > 0 && m_key_pressed_handlers.at(event.key.code).Count() > 0) {
            m_key_pressed_handlers.at(event.key.code).Invoke();
            return true;
        }
        break;

    case sf::Event::KeyReleased:
        if (m_key_released_handlers.count(event.key.code) > 0 && m_key_released_handlers.at(event.key.code).Count() > 0) {
            m_key_released_handlers.at(event.key.code).Invoke();
            return true;
        }
        break;

    case sf::Event::MouseWheelScrolled:
        break;

    case sf::Event::MouseButtonPressed:
        if (Contains(event.mouseButton.x, event.mouseButton.y) && m_mouse_click_handlers.Count() > 0) {
            m_mouse_click_handlers.Invoke(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            return true;
        }
        if (Contains(event.mouseButton.x, event.mouseButton.y)) {
            std::cerr << Name() << ": Press: " <<  event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
            Parent().BringToFront(dynamic_cast<Component&>(*this));
            //Parent().BringToFront(m_parent["Government"]);
            //Parent().BringToBack(m_parent["Government"]);
            return true;
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (Contains(event.mouseButton.x, event.mouseButton.y)) {
            std::cerr << Name() << ": Release: " <<  event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
            return true;
        }
        break;

    case sf::Event::MouseMoved:
        // Check mouse left and enter events, then remember mouse position for next check
        if (!Contains(m_mouse_old_position) && Contains(event.mouseMove.x, event.mouseMove.y)) {
            std::cerr << Name() << ": Enter: " <<  event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
            m_mouse_enter_handlers.Invoke(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseEntered});
        }
        else if (Contains(m_mouse_old_position) && !Contains(event.mouseMove.x, event.mouseMove.y)) {
            std::cerr << Name() << ": Leave: " <<  event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
            m_mouse_leave_handlers.Invoke(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseLeft});
        }
        m_mouse_old_position.first = event.mouseMove.x;
        m_mouse_old_position.second = event.mouseMove.y;

        if (Contains(event.mouseMove.x, event.mouseMove.y)) {
            std::cerr << Name() << ": Move: " <<  event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
            return true;
        }
        break;

    default:
        // ignore/todo
        break;
    }

    return false;
}
