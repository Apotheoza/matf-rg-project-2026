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
    void poll_events() override;
    void update() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;

private:
    void draw_skybox();
    void draw_asteroid(float rotation_angle);
    void draw_prince(float rotation_angle);
    void draw_fox(float rotation_angle);

    engine::resources::Texture *m_asteroid_texture = nullptr;
    engine::resources::Texture *m_prince_texture = nullptr;
    engine::resources::Texture *m_fox_texture = nullptr;
    bool m_cursor_enabled = true;
};

} // namespace app

#endif // APP_MAINCONTROLLER_HPP
