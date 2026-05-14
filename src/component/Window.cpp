#include "component/Window.h"
#include "component/details/Parsers.h"

using namespace sfui;

Window::Window(ConfigView config)
    : ComponentBase(*this, config)
    , m_title{config.required<std::string>("name")}
{
    // parse optional properties
    if (auto title = config.optional<std::string>("title")) SetTitle(*title);
    //if (!config.contains("width")) SetWidth(0u);
    //if (!config.contains("height")) SetHeight(0u);
    if (auto backgroundColor = config.optional<sf::Color>("background")) SetBackground(*backgroundColor);
    if (IsVisible()) m_window_up = std::make_unique<sf::RenderWindow>(sf::VideoMode(Width(), Height()), m_title); // TODO: should not be processed in constructor, but after UI has been built
    LinkEvent(OnShow([this]{ std::make_unique<sf::RenderWindow>(sf::VideoMode(Width(), Height()), m_title); }));
    LinkEvent(OnHide([this]{ m_window_up.reset(); }));
    for (const auto& actionConfig : config.array("onClose")) {
        auto action = ParseComponentAction(*this, actionConfig);
        LinkEvent(OnClose(std::move(action)));
    }
}

void Window::Render(sf::RenderWindow& window)
{
    if (m_background.has_value()) {
        window.clear(m_background.value());
    }
}

void Window::Update()
{
    if (m_window_up && m_window_up->isOpen()) {
        sf::Event event;
        while (m_window_up->pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                m_closeHandlers.notify();
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

    Update_();
}

void Window::Render()
{
    Render_(*m_window_up);
    m_window_up->display();
}
