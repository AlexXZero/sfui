#include "Handlers.h"
#include "Parsers.h"

using namespace sfui;

ComponentHandlers::ComponentHandlers(ComponentBase& parent, const Properties& properties)
    : ComponentGeometry(parent, properties)
{
}

ComponentHandlers::ComponentHandlers(ComponentBase& parent, const nlohmann::json& json)
    : ComponentHandlers(parent, Properties(json))
{
    ParseHandlers(json);
}

void ComponentHandlers::ParseHandlers(const nlohmann::json& json)
{
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
bool ComponentHandlers::HandleEvent(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::TextEntered:
        if (m_textEnterHandlers.Count() > 0 && IsFocused()) {
            m_textEnterHandlers.Invoke(event.text.unicode);
            return true;
        }
        break;

    case sf::Event::KeyPressed:
        if (auto focusedComponent_sp = FocusedComponent(); focusedComponent_sp == nullptr || focusedComponent_sp->m_textEnterHandlers.Count() == 0) {
            if (m_keyPressedHandlers.count(event.key.code) > 0 && m_keyPressedHandlers.at(event.key.code).Count() > 0) {
                m_keyPressedHandlers.at(event.key.code).Invoke();
                return true;
            }
        }
        break;

    case sf::Event::KeyReleased:
        if (auto focusedComponent_sp = FocusedComponent(); focusedComponent_sp == nullptr || focusedComponent_sp->m_textEnterHandlers.Count() == 0) {
            if (m_keyReleasedHandlers.count(event.key.code) > 0 && m_keyReleasedHandlers.at(event.key.code).Count() > 0) {
                m_keyReleasedHandlers.at(event.key.code).Invoke();
                return true;
            }
        }
        break;

    case sf::Event::MouseWheelScrolled:
        break;

    case sf::Event::MouseButtonPressed:
        if (Contains(event.mouseButton.x, event.mouseButton.y)) {
            GainFocus();

            m_mouseClickHandlers.Invoke(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            std::cerr << Name() << ": Press: " <<  event.mouseButton.x << ", " << event.mouseButton.y << std::endl;
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
        if (!Contains(m_mouseOldPosition) && Contains(event.mouseMove.x, event.mouseMove.y)) {
            std::cerr << Name() << ": Enter: " <<  event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
            m_mouseEnterHandlers.Invoke(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseEntered});
        }
        else if (Contains(m_mouseOldPosition) && !Contains(event.mouseMove.x, event.mouseMove.y)) {
            std::cerr << Name() << ": Leave: " <<  event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
            m_mouseLeaveHandlers.Invoke(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseLeft});
        }
        m_mouseOldPosition.first = event.mouseMove.x;
        m_mouseOldPosition.second = event.mouseMove.y;

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

void ComponentHandlers::OnRender(sf::RenderWindow& window)
{
    m_renderHandlers.Invoke(std::ref(window));
}

void ComponentHandlers::OnUpdate()
{
    m_updateHandlers.Invoke();
}

void ComponentHandlers::OnShow()
{
    m_showHandlers.Invoke();
}

void ComponentHandlers::OnHide()
{
    m_hideHandlers.Invoke();
}

void ComponentHandlers::OnEnable()
{
    m_enableHandlers.Invoke();
}

void ComponentHandlers::OnDisable()
{
    m_disableHandlers.Invoke();
}

void ComponentHandlers::OnGainFocus()
{
    m_gainedFocusHandlers.Invoke();
}

void ComponentHandlers::OnLoseFocus()
{
    m_lostFocusHandlers.Invoke();
}

void ComponentHandlers::OnResize()
{
    m_resizeHandlers.Invoke();
}

void ComponentHandlers::OnMove()
{
    m_moveHandlers.Invoke();
}
