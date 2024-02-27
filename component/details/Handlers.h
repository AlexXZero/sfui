#ifndef SFUI_COMPONENT_HANDLERS_H_INCLUDED
#define SFUI_COMPONENT_HANDLERS_H_INCLUDED

#include "Geometry.h"
#include "../../utils/Observer.h"
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

namespace sfui {

class ComponentHandlers : public ComponentGeometry {
    using ResizeHandler = std::function<void(std::uint16_t /*new_width*/, std::uint16_t /*new_width*/)>;
    using MoveHandler = std::function<void(std::int16_t /*new_x*/, std::int16_t /*new_y*/)>;
    using ShowHandler = std::function<void(bool /*state*/)>;
    using EnableHandler = std::function<void(bool /*state*/)>;
    using RenderHandler = std::function<void()>;
    using KeyPressHandler = std::function<void()>;
    using KeyReleaseHandler = std::function<void()>;
    using TextEnterHandler = std::function<void(std::uint32_t /*unicode*/)>;
    using MouseMove = std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>;
    using MouseMoveHandler = std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>;
    using MouseEnterHandler = std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>;
    using MouseLeaveHandler = std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>;
    using MouseClickHandler = std::function<void(sf::Mouse::Button /*button*/, std::int16_t /*x*/, std::int16_t /*y*/)>;
public:
    ComponentHandlers(Component& parent, const nlohmann::json& json);
    virtual ~ComponentHandlers() = default;

    // Handlers
    ObserverToken OnResize(ResizeHandler&& handler) { return m_resize_handlers.Set(std::forward<ResizeHandler>(handler)); }
    ObserverToken OnMove(MoveHandler&& handler) { return m_move_handlers.Set(std::forward<MoveHandler>(handler)); }
    ObserverToken OnShow(ShowHandler&& handler) { return m_show_handlers.Set(std::forward<ShowHandler>(handler)); }
    ObserverToken OnShow(std::function<void()>&& handler) { return OnShow([handler](bool state){ if (state) { handler(); } }); }
    ObserverToken OnHide(std::function<void()>&& handler) { return OnShow([handler](bool state){ if (!state) { handler(); } }); }
    ObserverToken OnEnable(EnableHandler&& handler) { return m_enable_handlers.Set(std::forward<EnableHandler>(handler)); }
    ObserverToken OnEnable(std::function<void()>&& handler) { return OnEnable([handler](bool state){ if (state) { handler(); } }); }
    ObserverToken OnDisable(std::function<void()>&& handler) { return OnEnable([handler](bool state){ if (!state) { handler(); } }); }
    ObserverToken OnRender(RenderHandler&& handler) { return m_render_handlers.Set(std::forward<RenderHandler>(handler)); }
    ObserverToken OnKeyPress(sf::Keyboard::Key key, KeyPressHandler&& handler) {
        if (m_key_pressed_handlers.count(key) == 0) {
            m_key_pressed_handlers.insert({key, Observers<KeyPressHandler>()});
        }
        return m_key_pressed_handlers.at(key).Set(std::forward<KeyPressHandler>(handler));
    }
    ObserverToken OnMouseMove(MouseMoveHandler&& handler) { return m_mouse_move_handlers.Set(std::forward<MouseMoveHandler>(handler)); }
    ObserverToken OnMouseMove(std::function<void()>&& handler) { return OnMouseMove([handler](std::int16_t, std::int16_t){ handler(); }); }
    ObserverToken OnMouseEnter(MouseEnterHandler&& handler) { return m_mouse_enter_handlers.Set(std::forward<MouseEnterHandler>(handler)); }
    ObserverToken OnMouseEnter(std::function<void()>&& handler) { return OnMouseEnter([handler](std::int16_t, std::int16_t){ handler(); }); }
    ObserverToken OnMouseLeave(MouseLeaveHandler&& handler) { return m_mouse_leave_handlers.Set(std::forward<MouseLeaveHandler>(handler)); }
    ObserverToken OnMouseLeave(std::function<void()>&& handler) { return OnMouseLeave([handler](std::int16_t, std::int16_t){ handler(); }); }
    ObserverToken OnMouseClick(MouseClickHandler&& handler) { return m_mouse_click_handlers.Set(std::forward<MouseClickHandler>(handler)); }
    ObserverToken OnMouseClick(std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>&& handler) { return OnMouseClick([handler](sf::Mouse::Button, std::int16_t x, std::int16_t y){ handler(x, y); }); }
    ObserverToken OnMouseClick(std::function<void()>&& handler) { return OnMouseClick([handler](sf::Mouse::Button, std::int16_t, std::int16_t){ handler(); }); }
    ObserverToken OnMouseLeftClick(MouseClickHandler&& handler) { return OnMouseClick([handler](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ if (button == sf::Mouse::Button::Left) handler(button, x, y); }); }
    ObserverToken OnMouseLeftClick(std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>&& handler) { return OnMouseLeftClick([handler](sf::Mouse::Button, std::int16_t x, std::int16_t y){ handler(x, y); }); }
    ObserverToken OnMouseLeftClick(std::function<void()>&& handler) { return OnMouseLeftClick([handler](sf::Mouse::Button, std::int16_t, std::int16_t){ handler(); }); }

    bool IsEnabled() const { return m_enabled; }
    void Enable(bool state = true) { m_enabled = state; m_enable_handlers.Invoke(state); }
    void Disable() { Enable(false); }

    bool IsVisible() const { return m_visible; }
    void Show(bool state = true) { m_visible = state; m_show_handlers.Invoke(state); }
    void Hide() { Show(false); }

protected:
    void LinkEvent(ObserverToken observer_token) { m_observers.push_back(observer_token); }
    void HandleRender() { m_render_handlers.Invoke(); }

private:
    friend class ComponentContainer;
    virtual bool HandleEvent_(const sf::Event& event);

private:
    bool m_enabled;
    bool m_visible;
    std::vector<ObserverToken> m_observers;
    Observers<ResizeHandler> m_resize_handlers;
    Observers<MoveHandler> m_move_handlers;
    Observers<ShowHandler> m_show_handlers;
    Observers<EnableHandler> m_enable_handlers;
    Observers<RenderHandler> m_render_handlers;
    Observers<MouseMoveHandler> m_mouse_move_handlers;
    Observers<MouseEnterHandler> m_mouse_enter_handlers;
    Observers<MouseLeaveHandler> m_mouse_leave_handlers;
    Observers<MouseClickHandler> m_mouse_click_handlers;
    std::map<sf::Keyboard::Key, Observers<KeyPressHandler>> m_key_pressed_handlers;
    std::map<sf::Keyboard::Key, Observers<KeyReleaseHandler>> m_key_released_handlers;
    Observers<TextEnterHandler> m_text_enter_handlers;
    std::pair<std::int16_t, std::int16_t> m_mouse_old_position;
};

}

#endif // SFUI_COMPONENT_HANDLERS_H_INCLUDED
