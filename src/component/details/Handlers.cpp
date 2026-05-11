#include "component/details/Handlers.h"
#include "component/details/Parsers.h"

using namespace sfui;

ComponentHandlers::ComponentHandlers(ComponentBase& parent, const Properties& properties)
    : ComponentGeometry(parent, properties)
{
}

ComponentHandlers::ComponentHandlers(ComponentBase& parent, const nlohmann::json& json)
    : ComponentGeometry(parent, Properties(json))
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
    if (json.contains("onMouseEnter")) {
        for (const auto& handler_json: json["onMouseEnter"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseEnter(std::move(handler)));
        }
    }
    if (json.contains("onMouseLeave")) {
        for (const auto& handler_json: json["onMouseLeave"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseLeave(std::move(handler)));
        }
    }
    if (json.contains("onClick")) {
        for (const auto& handler_json: json["onClick"]) {
            // TODO: if (handler_json.contains("button"))
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseLeftClick(std::move(handler)));
        }
    }
    if (json.contains("onButtonPress")) {
        for (const auto& handler_json: json["onButtonPress"]) {
            // TODO: if (handler_json.contains("button"))
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseButtonPress(std::move(handler)));
        }
    }
    if (json.contains("onButtonRelease")) {
        for (const auto& handler_json: json["onButtonRelease"]) {
            // TODO: if (handler_json.contains("button"))
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseButtonRelease(std::move(handler)));
        }
    }
    if (json.contains("onScrollUp")) {
        for (const auto& handler_json: json["onScrollUp"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseScrollUp(std::move(handler)));
        }
    }
    if (json.contains("onScrollDown")) {
        for (const auto& handler_json: json["onScrollDown"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnMouseScrollDown(std::move(handler)));
        }
    }
    if (json.contains("onRender")) {
        for (const auto& handler_json: json["onRender"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnRender(std::move(handler)));
        }
    }
    if (json.contains("onUpdate")) {
        for (const auto& handler_json: json["onUpdate"]) {
            auto handler = ParseComponentHandler(*this, handler_json);
            LinkEvent(OnUpdate(std::move(handler)));
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

bool ComponentHandlers::HandleEvent(const sf::Event& event)
{
    switch (event.type) {
    case sf::Event::TextEntered:
        if (m_textEnterHandlers.count() > 0 && IsFocused()) {
            m_textEnterHandlers.notify(event.text.unicode);
            return true;
        }
        break;

    case sf::Event::KeyPressed:
        if (auto focusedComponent_sp = FocusedComponent(); focusedComponent_sp == nullptr || focusedComponent_sp->m_textEnterHandlers.count() == 0) {
            if (m_keyPressHandlers.count(event.key.code) > 0 && m_keyPressHandlers.at(event.key.code).count() > 0) {
                m_keyPressHandlers.at(event.key.code).notify();
                return true;
            }
        }
        break;

    case sf::Event::KeyReleased:
        if (auto focusedComponent_sp = FocusedComponent(); focusedComponent_sp == nullptr || focusedComponent_sp->m_textEnterHandlers.count() == 0) {
            if (m_keyReleaseHandlers.count(event.key.code) > 0 && m_keyReleaseHandlers.at(event.key.code).count() > 0) {
                m_keyReleaseHandlers.at(event.key.code).notify();
                return true;
            }
        }
        break;

    case sf::Event::MouseWheelScrolled:
        if (Contains(m_mouseOldPosition.first, m_mouseOldPosition.second)) {
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                if (event.mouseWheelScroll.delta > 0) {
                    m_mouseScrollUpHandlers.notify(/*m_mouseOldPosition.first, m_mouseOldPosition.second, event.mouseWheelScroll.delta*/);
                } else if (event.mouseWheelScroll.delta < 0) {
                    m_mouseScrollDownHandlers.notify(/*m_mouseOldPosition.first, m_mouseOldPosition.second, event.mouseWheelScroll.delta*/);
                }
            }
            return true;
        }
        break;

    case sf::Event::MouseButtonPressed:
        if (Contains(event.mouseButton.x, event.mouseButton.y)) {
            GainFocus();

            m_mouseButtonPressHandlers.notify(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            return true;
        }
        break;

    case sf::Event::MouseButtonReleased:
        if (Contains(event.mouseButton.x, event.mouseButton.y)) {
            m_mouseButtonReleaseHandlers.notify(event.mouseButton.button, event.mouseButton.x, event.mouseButton.y);
            return true;
        }
        break;

    case sf::Event::MouseMoved:
        // Check mouse left and enter events, then remember mouse position for next check
        if (!Contains(m_mouseOldPosition) && Contains(event.mouseMove.x, event.mouseMove.y)) {
            m_mouseEnterHandlers.notify(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseEntered});
        }
        else if (Contains(m_mouseOldPosition) && !Contains(event.mouseMove.x, event.mouseMove.y)) {
            m_mouseLeaveHandlers.notify(event.mouseMove.x, event.mouseMove.y);
            //HandleEvent_({sf::Event::MouseLeft});
        }
        m_mouseOldPosition.first = event.mouseMove.x;
        m_mouseOldPosition.second = event.mouseMove.y;

        if (Contains(event.mouseMove.x, event.mouseMove.y)) {
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
    m_renderHandlers.notify(std::ref(window));
}

void ComponentHandlers::OnUpdate()
{
    m_updateHandlers.notify();
}

void ComponentHandlers::OnShow()
{
    m_showHandlers.notify();
}

void ComponentHandlers::OnHide()
{
    m_hideHandlers.notify();
}

void ComponentHandlers::OnEnable()
{
    m_enableHandlers.notify();
}

void ComponentHandlers::OnDisable()
{
    m_disableHandlers.notify();
}

void ComponentHandlers::OnGainFocus()
{
    m_gainedFocusHandlers.notify();
}

void ComponentHandlers::OnLoseFocus()
{
    m_lostFocusHandlers.notify();
}

void ComponentHandlers::OnResize()
{
    m_resizeHandlers.notify();
}

void ComponentHandlers::OnMove()
{
    m_moveHandlers.notify();
}
