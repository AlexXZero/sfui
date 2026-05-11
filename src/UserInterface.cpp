#include "UserInterface.h"
#include "FontLibrary.h"
#include "component/Window.h"
#include <CxxUtils/json5e.h>
#include <filesystem>
#include <exception>
#include <fstream>
#include <map>

namespace sfui {

struct UserInterface::Impl {
    std::map<std::string, std::shared_ptr<Window>> windows;
    // TODO: std::list<Font> m_fonts;
};

UserInterface::UserInterface(const std::string& configDir, const std::string& configFile)
    : m_pImpl(std::make_unique<UserInterface::Impl>())
{
    auto configJSON = CxxUtils::json5e::parse(std::filesystem::path(configDir)/configFile);

    if (configJSON.contains("fonts")) {
        for (const auto& fontConfig: configJSON["fonts"]) {
            FontLibrary::Load(fontConfig["name"], fontConfig["file"]);
            if (fontConfig.contains("default") && fontConfig["default"].get<bool>()) {
                FontLibrary::SetDefaultFont(FontLibrary::Get(fontConfig["name"]));
            }
        }
    }

    if (configJSON.contains("windows")) {
        for (const auto& windowConfig: configJSON["windows"]) {
            m_pImpl->windows.emplace(windowConfig["name"], std::make_shared<Window>(windowConfig));
        }
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
