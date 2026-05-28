#include "UserInterface.h"
#include "FontLibrary.h"
#include "component/Window.h"
#include <sol/sol.hpp>
#include <map>

namespace sfui {

struct UserInterface::Impl {
    std::map<std::string, std::shared_ptr<Window>> windows;
    // TODO: std::list<Font> m_fonts;
    sol::state luaContextState;
};

UserInterface::UserInterface(ConfigView config)
    : m_pImpl(std::make_unique<UserInterface::Impl>())
{
    //=== Initialize lua context ===//

    static const auto getComponent = [](UserInterface& ui, const std::string& key) {
        return std::static_pointer_cast<ComponentBase>(ui[key].shared_from_this());
    };

    auto& luaState = m_pImpl->luaContextState;
    luaState.open_libraries(sol::lib::base);

    luaState.new_usertype<ComponentBase>(
        "Component",
        sol::no_constructor,

        // General
        "name", &ComponentBase::Name,

        // Visibility
        "show", &ComponentBase::Show,
        "hide", &ComponentBase::Hide,
        "isVisible", &ComponentBase::IsVisible,

        // Enabled
        "enable", &ComponentBase::Enable,
        "disable", &ComponentBase::Disable,
        "isEnabled", &ComponentBase::IsEnabled,

        // Focus
        "gainFocus", &ComponentBase::GainFocus,
        "loseFocus", &ComponentBase::LoseFocus,
        "isFocused", &ComponentBase::IsFocused,

        // Layout
        "left", &ComponentBase::Left,
        "top", &ComponentBase::Top,
        "width", &ComponentBase::Width,
        "height", &ComponentBase::Height
    );

    // TODO:
    /*luaState.new_usertype<Label>(
        "Label",
        sol::base_classes,
        sol::bases<ComponentBase>(),

        "setText", &Label::SetText
    );*/

    luaState.new_usertype<UserInterface>(
        "Ui",
        "get", getComponent,
        sol::meta_method::index, getComponent
    );

    luaState.set("ui", this);

    //=== Load UI tree ===//

    for (const auto& fontConfig : config.array("fonts")) {
        auto fontName = fontConfig.required<std::string>("name");
        FontLibrary::Load(fontName, fontConfig.required<std::string>("file"));
        if (fontConfig.valueOr<bool>("default", false)) {
            FontLibrary::SetDefaultFont(FontLibrary::Get(fontName));
        }
    }

    for (const auto& windowConfig : config.array("windows")) {
        m_pImpl->windows.emplace(windowConfig.required<std::string>("name"), std::make_shared<Window>(*this, windowConfig));
    }
}

UserInterface::~UserInterface() = default;

void UserInterface::Update()
{
    for (auto& [_, window]: m_pImpl->windows) {
        if (window->IsVisible()) {
            window->Update();
        }
    }
}

void UserInterface::Render()
{
    for (auto& [_, window]: m_pImpl->windows) {
        if (window->IsVisible()) {
            window->Render();
        }
    }
}

ComponentBase& UserInterface::operator[](const std::string& name)
{
    auto nameEnd = name.find(".");
    if (nameEnd == std::string::npos) {
        return *m_pImpl->windows.at(name);
    }

    return operator[](name.substr(0u, nameEnd)).operator[](name.substr(nameEnd + 1));
}

sol::state& UserInterface::script()
{
    return m_pImpl->luaContextState;
}

} // end of namespace sfui
