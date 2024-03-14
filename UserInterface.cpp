#include "UserInterface.h"
#include "FontLibrary.h"
#include "component/Window.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <exception>
#include <fstream>
#include <map>

namespace sfui {

struct UserInterface::Impl {
    std::map<std::string, std::shared_ptr<Window>> windows;
    // TODO: std::list<Font> m_fonts;
};

static nlohmann::json import(const std::filesystem::path& configDir, const std::string& configFile)
{
    constexpr auto callback = nullptr;
    constexpr auto allow_exceptions = true;
    constexpr auto ignore_comments = true;
    if (!std::filesystem::exists(configDir/configFile)) return {}; // TODO: throw std::runtime_error(std::string(configDir/configFile) + ": doesn't exist");
    return nlohmann::json::parse(std::ifstream(configDir/configFile), callback, allow_exceptions, ignore_comments);
}

static void resolve_imports(const std::filesystem::path& configDir, nlohmann::json& json)
{
    if (json.is_array()) {
        for (auto& element: json) {
            resolve_imports(configDir, element);
        }
    } else if (json.is_object()) {
        if (json.contains("import")) {
            auto imported = import(configDir, json["import"].get<std::string>());
            if (!imported.is_null()) {
                json.update(imported);
            }
            json.erase("import");
        }
        for (auto& [key, value]: json.items()) {
            if (value.is_object() || value.is_array()) {
                resolve_imports(configDir, value);
            }
        }
    }
}

UserInterface::UserInterface(const std::string& configDir, const std::string& configFile)
    : m_pImpl(std::make_unique<UserInterface::Impl>())
{
    auto configJSON = import(configDir, configFile);
    resolve_imports(configDir, configJSON);

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
