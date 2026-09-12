#ifndef APP_MAINCONTROLLER_HPP
#define APP_MAINCONTROLLER_HPP

#include <engine/core/Controller.hpp>
#include <engine/resources/Texture.hpp>
#include <glm/glm.hpp>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "app::MainController";
    }

protected:
    void initialize() override;                                            // Set up OpenGL state, camera, and textures
    bool loop() override;                                                  // Process ESC key to exit
    void update() override;                                                // Handle 'S' toggle, orbit progress, and camera
    void begin_draw() override;                                            // Clear framebuffers
    void draw() override;                                                  // Render skybox, models, and lighting
    void end_draw() override;                                              // Swap buffers

private:
    void draw_skybox();                                                    // Render cubemap skybox
    void draw_sun(float time);                                             // Render 2.5D sun models
    void draw_asteroid(const glm::vec3 &planet_pos, float rotation_angle);    // Render asteroid
    void draw_prince(const glm::vec3 &planet_pos, float rotation_angle);      // Render Little Prince
    void draw_fox(const glm::vec3 &planet_pos, float rotation_angle);         // Render Fox

    engine::resources::Texture *m_asteroid_texture = nullptr;              // Asteroid texture
    engine::resources::Texture *m_prince_texture = nullptr;                // Little Prince texture
    engine::resources::Texture *m_fox_texture = nullptr;                   // Fox texture
    engine::resources::Texture *m_sun1_texture = nullptr;                  // Sun layer 1 texture
    engine::resources::Texture *m_sun2_texture = nullptr;                  // Sun layer 2 texture

    bool m_orbit_active = false;                                           // Toggle state when 'S' is pressed
    float m_orbit_progress = 0.0f;                                         // 0.0 = asteroid view, 1.0 = sun view
    float m_orbit_angle = 0.0f;                                            // Orbital revolution angle around the sun
    glm::vec3 m_sun_pos = glm::vec3(28.0f, 0.0f, 0.0f);                    // Fixed sun position in world space
};

} // namespace app

#endif // APP_MAINCONTROLLER_HPP
