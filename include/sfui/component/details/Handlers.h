#ifndef SFUI_COMPONENT_HANDLERS_H_INCLUDED
#define SFUI_COMPONENT_HANDLERS_H_INCLUDED

#include "Geometry.h"
#include <CxxUtils/observer.h>
#include <CxxUtils/function_traits.h>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>

namespace sfui {

class ComponentHandlers : public ComponentGeometry {
protected:
    template<typename F> std::function<void()> CastToDefaultHandler(F&& handler);
    template<typename F> std::function<void(sf::RenderWindow& /*window*/)> CastToRenderHandler(F&& handler);
    template<typename F> std::function<void(std::uint32_t /*unicode*/)> CastToTextEnterHandler(F&& handler);
    template<typename F> std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)> CastToMouseMoveHandler(F&& handler);
    template<typename F> std::function<void(sf::Mouse::Button /*button*/, std::int16_t /*x*/, std::int16_t /*y*/)> CastToMouseClickHandler(F&& handler);

public:
    using Properties = ComponentGeometry::Properties;
    ComponentHandlers(ComponentBase& parent, const Properties& properties);
    ComponentHandlers(ComponentBase& parent, const nlohmann::json& json);
    virtual ~ComponentHandlers() = default;

    // Handlers
    template<typename Handler> CxxUtils::ObserverToken OnGainedFocus(Handler&& handler) { return m_gainedFocusHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnLostFocus(Handler&& handler) { return m_lostFocusHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnResize(Handler&& handler) { return m_resizeHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMove(Handler&& handler) { return m_moveHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnShow(Handler&& handler) { return m_showHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnHide(Handler&& handler) { return m_hideHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnEnable(Handler&& handler) { return m_enableHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnDisable(Handler&& handler) { return m_disableHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnUpdate(Handler&& handler) { return m_updateHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnRender(Handler&& handler) { return m_renderHandlers.Set(std::move(CastToRenderHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseMove(Handler&& handler) { return m_mouseMoveHandlers.Set(std::move(CastToMouseMoveHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseEnter(Handler&& handler) { return m_mouseEnterHandlers.Set(std::move(CastToMouseMoveHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseLeave(Handler&& handler) { return m_mouseLeaveHandlers.Set(std::move(CastToMouseMoveHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseButtonPress(Handler&& handler) { return m_mouseButtonPressHandlers.Set(std::move(CastToMouseClickHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseButtonRelease(Handler&& handler) { return m_mouseButtonReleaseHandlers.Set(std::move(CastToMouseClickHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseClick(Handler&& handler) { return m_mouseButtonPressHandlers.Set(std::move(CastToMouseClickHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseLeftClick(Handler&& handler) { return m_mouseButtonPressHandlers.Set([handler = CastToMouseClickHandler(std::forward<Handler>(handler)), this](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ if (button == sf::Mouse::Button::Left) handler(button, x, y); }); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseScrollUp(Handler&& handler) { return m_mouseScrollUpHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnMouseScrollDown(Handler&& handler) { return m_mouseScrollDownHandlers.Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnKeyPress(sf::Keyboard::Key key, Handler&& handler) { return m_keyPressHandlers[key].Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnKeyRelease(sf::Keyboard::Key key, Handler&& handler) { return m_keyReleaseHandlers[key].Set(std::move(CastToDefaultHandler(std::forward<Handler>(handler)))); }
    template<typename Handler> CxxUtils::ObserverToken OnTextEntered(Handler&& handler) { return m_textEnterHandlers.Set(std::move(CastToTextEnterHandler(std::forward<Handler>(handler)))); }

protected:
    void ParseHandlers(const nlohmann::json& json);
    void LinkEvent(CxxUtils::ObserverToken observerToken) { m_observers.push_back(observerToken); }

private:
    virtual bool HandleEvent(const sf::Event& event) final;
    virtual void OnRender(sf::RenderWindow& window) final;
    virtual void OnUpdate() final;
    virtual void OnShow() final;
    virtual void OnHide() final;
    virtual void OnEnable() final;
    virtual void OnDisable() final;
    virtual void OnGainFocus() final;
    virtual void OnLoseFocus() final;
    virtual void OnResize() final;
    virtual void OnMove() final;

private:
    std::vector<CxxUtils::ObserverToken> m_observers;
    CxxUtils::Observers<std::function<void()>> m_resizeHandlers;
    CxxUtils::Observers<std::function<void()>> m_moveHandlers;
    CxxUtils::Observers<std::function<void()>> m_showHandlers;
    CxxUtils::Observers<std::function<void()>> m_hideHandlers;
    CxxUtils::Observers<std::function<void()>> m_enableHandlers;
    CxxUtils::Observers<std::function<void()>> m_disableHandlers;
    CxxUtils::Observers<std::function<void()>> m_updateHandlers;
    CxxUtils::Observers<std::function<void(sf::RenderWindow&)>> m_renderHandlers;
    CxxUtils::Observers<std::function<void(std::int16_t, std::int16_t)>> m_mouseMoveHandlers;
    CxxUtils::Observers<std::function<void(std::int16_t, std::int16_t)>> m_mouseEnterHandlers;
    CxxUtils::Observers<std::function<void(std::int16_t, std::int16_t)>> m_mouseLeaveHandlers;
    CxxUtils::Observers<std::function<void(sf::Mouse::Button, std::int16_t, std::int16_t)>> m_mouseButtonPressHandlers;
    CxxUtils::Observers<std::function<void(sf::Mouse::Button, std::int16_t, std::int16_t)>> m_mouseButtonReleaseHandlers;
    std::unordered_map<sf::Keyboard::Key, CxxUtils::Observers<std::function<void()>>> m_keyPressHandlers;
    std::unordered_map<sf::Keyboard::Key, CxxUtils::Observers<std::function<void()>>> m_keyReleaseHandlers;
    CxxUtils::Observers<std::function<void(std::uint32_t)>> m_textEnterHandlers;
    CxxUtils::Observers<std::function<void()>> m_gainedFocusHandlers;
    CxxUtils::Observers<std::function<void()>> m_lostFocusHandlers;
    CxxUtils::Observers<std::function<void()>> m_mouseScrollUpHandlers;
    CxxUtils::Observers<std::function<void()>> m_mouseScrollDownHandlers;
    std::pair<std::int16_t, std::int16_t> m_mouseOldPosition;
};

template<typename F> std::function<void()> ComponentHandlers::CastToDefaultHandler(F&& handler) {
    if constexpr (CxxUtils::function_traits<F>::arity == 0) {
        return handler;
    } else if constexpr (CxxUtils::arguments_count<F> >= 1 && std::is_base_of_v<std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>, ComponentHandlers>) {
        using Component = std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>;
        return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](){ handler(component); };
    } else {
        static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
    }
}

template<typename F> std::function<void(sf::RenderWindow& /*window*/)> ComponentHandlers::CastToRenderHandler(F&& handler) {
    if constexpr (CxxUtils::function_traits<F>::arity == 0) {
        return [handler = std::forward<F>(handler)](sf::RenderWindow&){ handler(); };
    } else if constexpr (/*CxxUtils::arguments_count<F> >= 1 &&*/ std::is_base_of_v<std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>, ComponentHandlers>) {
        using Component = std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>;
        if constexpr (CxxUtils::arguments_count<F> == 1) {
            return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](sf::RenderWindow& window){ handler(component); };
        } else if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_same_v<CxxUtils::function_argument_t<F, 1>, sf::RenderWindow&>) {
            return [handler = std::forward<F>(handler), this](sf::RenderWindow& window){ handler(dynamic_cast<Component&>(*this), window); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    } else {
        if constexpr (CxxUtils::arguments_count<F> == 1 && std::is_same_v<F, std::function<void(sf::RenderWindow& window)>>) {
            return handler;
        } else if constexpr (CxxUtils::arguments_count<F> == 1 && std::is_same_v<CxxUtils::function_argument_t<F, 0>, sf::RenderWindow&>) {
            return [handler = std::forward<F>(handler)](sf::RenderWindow& window){ handler(window); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    }
}

template<typename F> std::function<void(std::uint32_t /*unicode*/)> ComponentHandlers::CastToTextEnterHandler(F&& handler)
{
    if constexpr (CxxUtils::function_traits<F>::arity == 0) {
        return [handler = std::forward<F>(handler)](std::uint32_t unicode){ handler(); };
    } else if constexpr (/*CxxUtils::arguments_count<F> >= 1 &&*/ std::is_base_of_v<std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>, ComponentHandlers>) {
        using Component = std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>;
        if constexpr (CxxUtils::arguments_count<F> == 1) {
            return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](std::uint32_t unicode){ handler(component); };
        } else if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_integral_v<CxxUtils::function_argument_t<F, 1>>) {
            return [handler = std::forward<F>(handler), this](std::uint32_t unicode){ handler(dynamic_cast<Component&>(*this), unicode); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    } else {
        if constexpr (CxxUtils::arguments_count<F> == 1 && std::is_same_v<F, std::function<void(std::uint32_t unicode)>>) {
            return handler;
        } else if constexpr (CxxUtils::arguments_count<F> == 1 && std::is_integral_v<CxxUtils::function_argument_t<F, 0>>) {
            return [handler = std::forward<F>(handler)](std::uint32_t unicode){ handler(unicode); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    }
}

template<typename F> std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)> ComponentHandlers::CastToMouseMoveHandler(F&& handler) {
    if constexpr (CxxUtils::function_traits<F>::arity == 0) {
        return [handler = std::forward<F>(handler)](std::int16_t x, std::int16_t y){ handler(); };
    } else if constexpr (/*CxxUtils::arguments_count<F> >= 1 &&*/ std::is_base_of_v<std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>, ComponentHandlers>) {
        using Component = std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>;
        if constexpr (CxxUtils::arguments_count<F> == 1) {
            return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](std::int16_t x, std::int16_t y){ handler(component); };
        } else if constexpr (CxxUtils::arguments_count<F> == 3 && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 1>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 2>>) {
            return [handler = std::forward<F>(handler), this](std::int16_t x, std::int16_t y){ handler(dynamic_cast<Component&>(*this), x, y); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    } else {
        if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_same_v<F, std::function<void(std::int16_t /*x*/, std::int16_t /*y*/)>>) {
            return handler;
        } else if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 0>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 1>>) {
            return [handler = std::forward<F>(handler)](std::int16_t x, std::int16_t y){ handler(x, y); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    }
}

template<typename F> std::function<void(sf::Mouse::Button /*button*/, std::int16_t /*x*/, std::int16_t /*y*/)> ComponentHandlers::CastToMouseClickHandler(F&& handler) {
    if constexpr (CxxUtils::function_traits<F>::arity == 0) {
        return [handler = std::forward<F>(handler)](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(); };
    } else if constexpr (/*CxxUtils::arguments_count<F> >= 1 &&*/ std::is_base_of_v<std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>, ComponentHandlers>) {
        using Component = std::remove_reference_t<CxxUtils::function_argument_t<F, 0>>;
#if 0
        return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](CxxUtils::function_arguments_t<F>&& args){ handler(component, std::forward<CxxUtils::function_arguments_t<F>>(args)...); };
#elif 0
        using Args = CxxUtils::tuple_without_first_t<typename CxxUtils::function_traits<F>::args_tuple>;
        static_assert(std::is_same_v<Args, std::tuple<int16_t, int16_t>>);
        return CastToMouseClickHandler([handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](Args args){ apply(handler, std::tuple_cat(std::tie(component), args)); });
#else
        if constexpr (CxxUtils::arguments_count<F> == 1) {
            return [handler = std::forward<F>(handler), &component = dynamic_cast<Component&>(*this)](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(component); };
        } else if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_same_v<CxxUtils::function_argument_t<F, 1>, sf::Mouse::Button>) {
            return [handler = std::forward<F>(handler), this](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(dynamic_cast<Component&>(*this), button); };
        } else if constexpr (CxxUtils::arguments_count<F> == 3 && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 1>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 2>>) {
            return [handler = std::forward<F>(handler), this](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(dynamic_cast<Component&>(*this), x, y); };
        } else if constexpr (CxxUtils::arguments_count<F> == 4 && std::is_same_v<CxxUtils::function_argument_t<F, 1>, sf::Mouse::Button> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 2>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 3>>) {
            return [handler = std::forward<F>(handler), this](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(dynamic_cast<Component&>(*this), button, x, y); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
#endif
    } else {
        if constexpr (CxxUtils::arguments_count<F> == 1 && std::is_same_v<CxxUtils::function_argument_t<F, 0>, sf::Mouse::Button>) {
            return [handler = std::forward<F>(handler)](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(button); };
        } else if constexpr (CxxUtils::arguments_count<F> == 2 && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 0>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 1>>) {
            return [handler = std::forward<F>(handler)](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(x, y); };
        } else if constexpr (CxxUtils::arguments_count<F> == 3 && std::is_same_v<F, std::function<void(sf::Mouse::Button /*button*/, std::int16_t /*x*/, std::int16_t /*y*/)>>) {
            return handler;
        } else if constexpr (CxxUtils::arguments_count<F> == 3 && std::is_same_v<CxxUtils::function_argument_t<F, 0>, sf::Mouse::Button> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 1>> && std::is_arithmetic_v<CxxUtils::function_argument_t<F, 2>>) {
            return [handler = std::forward<F>(handler)](sf::Mouse::Button button, std::int16_t x, std::int16_t y){ handler(button, x, y); };
        } else {
            static_assert(CxxUtils::always_false_v<F>, "Unsupported handler type conversion");
        }
    }
}

}

#endif // SFUI_COMPONENT_HANDLERS_H_INCLUDED
