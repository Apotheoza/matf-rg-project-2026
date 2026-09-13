#ifndef APP_MAINCONTROLLER_HPP
#define APP_MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Texture.hpp>
#include <glm/glm.hpp>
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
    void update() override;
    void begin_draw() override;
    void draw() override;
    void end_draw() override;

private:
    enum class CameraMode {
        STAR_SYSTEM,
        PRINCE_ASTEROID,
        LAMPLIGHT_ASTEROID
    };

    void draw_skybox();
    void draw_star(float time);
    void draw_prince_asteroid(const glm::vec3 &planet_pos, float rotation_angle);
    void draw_fenjer_asteroid(const glm::mat4 &model_matrix);

    engine::resources::Texture *m_asteroid_texture = nullptr;
    engine::resources::Texture *m_prince_texture = nullptr;
    engine::resources::Texture *m_fox_texture = nullptr;
    engine::resources::Texture *m_sun1_texture = nullptr;
    engine::resources::Texture *m_sun2_texture = nullptr;
    engine::resources::Texture *m_fenjer_texture = nullptr;

    CameraMode m_camera_mode = CameraMode::STAR_SYSTEM;
    bool m_is_spinning = true;
    float m_orbit_angle = 0.0f;
    float m_self_rotation = 0.0f;
    float m_lamp_intensity = 3.5f;
    float m_lamplight_timer = 0.0f;
    std::vector<glm::vec3> m_lamp_colors = {
        glm::vec3(1.0f, 0.85f, 0.3f),
        glm::vec3(1.0f, 0.45f, 0.1f),
        glm::vec3(0.2f, 0.85f, 1.0f),
        glm::vec3(0.3f, 1.0f, 0.4f),
        glm::vec3(0.9f, 0.3f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };
    std::size_t m_lamp_color_index = 0;
    glm::vec3 m_cam_pos = glm::vec3(0.0f, 24.0f, 52.0f);
    glm::vec3 m_cam_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_star_pos = glm::vec3(0.0f, 0.0f, 0.0f);
};

} // namespace app

#endif // APP_MAINCONTROLLER_HPP
