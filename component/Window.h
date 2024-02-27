#ifndef SFUI_WINDOW_H_INCLUDED
#define SFUI_WINDOW_H_INCLUDED

#include "details/Container.h"

namespace sfui {

class Window: public ComponentContainer {
    using CloseHandler = std::function<void()>;
public:
    Window(const nlohmann::json& json);
    ~Window() = default;

    std::string_view Title() const { return m_title; }
    void SetTitle(std::string_view title) { m_title = title; }
    void SetBackground(sf::Color color) { m_background = color; }
    void SetBackground(std::uint32_t color) { m_background = sf::Color(color); }

    // Handlers
    ObserverToken OnClose(CloseHandler&& handler) { return m_close_handlers.Set(std::forward<CloseHandler>(handler)); }

    void Render(sf::RenderWindow& window) override;

private:
    friend class UserInterface;
    void Update();
    void Render();

private:
    std::string m_title;
    std::unique_ptr<sf::RenderWindow> m_window_up;
    std::optional<sf::Color> m_background;
    Observers<CloseHandler> m_close_handlers;
};

}

#endif // SFUI_WINDOW_H_INCLUDED
