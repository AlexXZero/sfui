#include "UserInterface.h"
#include "FontLibrary.h"
#include "component/Window.h"
#include <map>

namespace sfui {

struct UserInterface::Impl {
    std::map<std::string, std::shared_ptr<Window>> windows;
    // TODO: std::list<Font> m_fonts;
};

UserInterface::UserInterface(ConfigView config)
    : m_pImpl(std::make_unique<UserInterface::Impl>())
{
    for (const auto& fontConfig : config.array("fonts")) {
        auto fontName = fontConfig.required<std::string>("name");
        FontLibrary::Load(fontName, fontConfig.required<std::string>("file"));
        if (fontConfig.valueOr<bool>("default", false)) {
            FontLibrary::SetDefaultFont(FontLibrary::Get(fontName));
        }
    }

    for (const auto& windowConfig : config.array("windows")) {
        m_pImpl->windows.emplace(windowConfig.required<std::string>("name"), std::make_shared<Window>(windowConfig));
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

Component& UserInterface::operator[](const std::string& name)
{
    auto nameEnd = name.find(".");
    if (nameEnd == std::string::npos) {
        return *m_pImpl->windows.at(name);
    }

    return operator[](name.substr(0u, nameEnd)).operator[](name.substr(nameEnd + 1));
}

} // end of namespace sfui
