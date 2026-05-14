#ifndef SFUI_USER_INTERFACE_H_INCLUDED
#define SFUI_USER_INTERFACE_H_INCLUDED

#include "ConfigParser.h"
#include <memory>
#include <string>

namespace sfui {

class Component;

class UserInterface {
public:
    explicit UserInterface(ConfigView config);
    explicit UserInterface(const Config& config) : UserInterface(config.view()) {}
    explicit UserInterface(const std::filesystem::path& path) : UserInterface(Config(path)) {}
    ~UserInterface();

    void Update();
    void Render();
    Component& operator[](const std::string& name);

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};

}

#endif // SFUI_USER_INTERFACE_H_INCLUDED
