#include "component/details/Handlers.h"
#include "component/details/Parsers.h"

using namespace sfui;

ComponentHandlers::ComponentHandlers(ComponentBase& parent, const Properties& properties)
    : ComponentGeometry(parent, properties)
{
}

ComponentHandlers::ComponentHandlers(ComponentBase& parent, ConfigView config)
    : ComponentGeometry(parent, Properties(config))
{
    ParseHandlers(config);
}

void ComponentHandlers::ParseHandlers(ConfigView config)
{
    for (const auto& actionConfig : config.array("onKeyPress")) {
        auto key = actionConfig.required<sf::Keyboard::Key>("key");
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnKeyPress(key, std::move(action)));
    }
    for (const auto& actionConfig : config.array("onMouseEnter")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseEnter(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onMouseLeave")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseLeave(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onClick")) {
        // TODO: auto button = config.required<sf::Mouse::Button>("button");
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseLeftClick(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onButtonPress")) {
        // TODO: auto button = config.required<sf::Mouse::Button>("button");
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseButtonPress(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onButtonRelease")) {
        // TODO: auto button = config.required<sf::Mouse::Button>("button");
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseButtonRelease(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onScrollUp")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseScrollUp(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onScrollDown")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnMouseScrollDown(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onRender")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnRender(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onUpdate")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnUpdate(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onShow")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnShow(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onHide")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnHide(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onEnable")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnEnable(std::move(action)));
    }
    for (const auto& actionConfig : config.array("onDisable")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnDisable(std::move(action)));
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
