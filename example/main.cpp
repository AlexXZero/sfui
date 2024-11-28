#include <thread>
#include <sfui/UserInterface.h>
#include <sfui/component/Window.h>

int main()
{
    sfui::UserInterface ui("gui/", "gui.json");
    //ui["OpenSR"].Show();

    bool run = true;
    auto close_observer = dynamic_cast<sfui::Window&>(ui["OpenSR"]).OnClose([&run]{ run = false;});

    while (run) {
        ui.Update();
        ui.Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
