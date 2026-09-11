#include <app/App.hpp>
#include <app/MainController.hpp>

namespace app { //not sure what this file does at all

void App::app_setup() {
    auto main_controller = register_controller<MainController>(); //wjat?
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}

} // namespace app
