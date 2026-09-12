#include <app/App.hpp>
#include <app/MainController.hpp>

namespace app { 

void App::app_setup() {
    auto main_controller = register_controller<MainController>();
    main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
}

} // namespace app
