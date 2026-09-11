#ifndef APP_MAINCONTROLLER_HPP
#define APP_MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Texture.hpp>
#include <vector>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "app::MainController";
    }

protected:
    void initialize() override;
    bool loop() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;

private:
    void draw_skybox();
    void draw_asteroid();

    std::vector<engine::resources::Texture *> m_loaded_textures;
};

} // namespace app

#endif // APP_MAINCONTROLLER_HPP
