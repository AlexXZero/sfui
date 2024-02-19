#ifndef SFUI_USER_INTERFACE_H_INCLUDED
#define SFUI_USER_INTERFACE_H_INCLUDED

#include <memory>
#include <string>

namespace sfui {

class Component;

class UserInterface {
public:
    UserInterface(const std::string& configDir, const std::string& configFile); // configPath is a path to json file inside the configDir directory, e.g. "ui.json"
    UserInterface(const std::string& configPath) : UserInterface(".", configPath) {}
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
