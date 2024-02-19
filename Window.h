#ifndef SFUI_WINDOW_H_INCLUDED
#define SFUI_WINDOW_H_INCLUDED

#include "ComponentContainer.h"
#include "Parsers.h"

namespace sfui {

class Window: public ComponentContainer {
    using CloseHandler = std::function<void()>;
public:
    Window(const nlohmann::json& json) : ComponentContainer(*this, json), m_title(json["name"]) {
        // parse optional properties
        if (json.contains("title")) SetTitle(json["title"].get<std::string>());
        //if (!json.contains("width")) SetWidth(0u);
        //if (!json.contains("height")) SetHeight(0u);
        if (json.contains("background")) SetBackground(ParseColor(json["background"]));
        if (IsVisible()) m_window_up = std::make_unique<sf::RenderWindow>(sf::VideoMode(Width(), Height()), m_title);
        LinkEvent(OnShow([this]{ std::make_unique<sf::RenderWindow>(sf::VideoMode(Width(), Height()), m_title); }));
        LinkEvent(OnHide([this]{ m_window_up.reset(); }));
    }
    ~Window() = default;

    std::string_view Title() const { return m_title; }
    void SetTitle(std::string_view title) { m_title = title; }
    void SetBackground(sf::Color color) { m_background = color; }
    void SetBackground(std::uint32_t color) { m_background = sf::Color(color); }

    // Handlers
    ObserverToken OnClose(CloseHandler&& handler) { return m_close_handlers.Set(std::forward<CloseHandler>(handler)); }

protected:
    void Render(sf::RenderWindow& window) override {
        if (m_background.has_value()) {
            window.clear(m_background.value());
        }
    }

private:
    friend class UserInterface;
    void Update() {
        if (m_window_up && m_window_up->isOpen()) {
            sf::Event event;
            while (m_window_up->pollEvent(event)) {
                switch (event.type) {
                case sf::Event::Closed:
                    m_close_handlers.Invoke();
                    break;
                case sf::Event::Resized:
                    SetWidth(event.size.width);
                    SetHeight(event.size.height);

                    // Update window view (TODO: `WIndow::onResize([]{/* TODO */})`)
                    {
                        auto view = m_window_up->getView();
                        view.setSize(sf::Vector2f(event.size.width, event.size.height));
                        view.setCenter(sf::Vector2f(event.size.width / 2, event.size.height / 2));
                        m_window_up->setView(view);
                    }
                    break;
                case sf::Event::LostFocus:
                case sf::Event::GainedFocus:
                case sf::Event::MouseEntered:
                case sf::Event::MouseLeft:
                    // todo/ignore
                    break;
                default: // TextEntered, KeyPressed/KeyReleased, Mouse, Joystick, etc.
                    HandleEvent_(event);
                    break;
                }
            }
        }

        // Update_();
    }

    void Render() {
        Render_(*m_window_up);
        m_window_up->display();
    }

private:
    std::string m_title;
    std::unique_ptr<sf::RenderWindow> m_window_up;
    std::optional<sf::Color> m_background;
    Observers<CloseHandler> m_close_handlers;
};

}

#endif // SFUI_WINDOW_H_INCLUDED
