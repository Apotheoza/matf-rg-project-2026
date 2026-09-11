#ifndef APP_APP_HPP
#define APP_APP_HPP

#include <engine/core/Engine.hpp>

namespace app {

class App final : public engine::core::App {
protected:
    void app_setup() override;
};

} // namespace app

#endif // APP_APP_HPP
