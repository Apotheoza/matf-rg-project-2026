#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();                      // Enable depth testing for 3D rendering

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->camera()->Position = glm::vec3(0.0f, 1.5f, 16.0f);           // Starting camera position
    graphics->camera()->Yaw = -90.0f;                                      // Face towards -Z axis
    graphics->camera()->Pitch = 0.0f;                                      // Level view angle
    graphics->camera()->rotate_camera(0.0f, 0.0f);                         // Recalculate camera vectors

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    m_asteroid_texture = resources->texture("asteroid");                   // Asteroid diffuse texture
    m_prince_texture = resources->texture("mali_princ");                   // Little Prince diffuse texture
    m_fox_texture = resources->texture("fox");                             // Fox diffuse texture
    m_sun1_texture = resources->texture("sun1");                           // Sun layer 1 texture (spinning rays)
    m_sun2_texture = resources->texture("sun2");                           // Sun layer 2 texture (front disc)
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;                                                      // Exit program when ESC is pressed
    }
    return true;
}

void MainController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();                                             // Time between frames in seconds

    // Toggle orbit view when 'S' is pressed
    if (platform->key(engine::platform::KeyId::KEY_S).state() == engine::platform::Key::State::JustPressed) {
        m_orbit_active = !m_orbit_active;
    }

    // Smoothly interpolate between asteroid close-up and sun-centered orbit view
    if (m_orbit_active) {
        m_orbit_progress = glm::clamp(m_orbit_progress + dt * 0.8f, 0.0f, 1.0f); // Smooth zoom-out
        m_orbit_angle += dt * 0.4f;                                        // Orbit revolution speed
    } else {
        m_orbit_progress = glm::clamp(m_orbit_progress - dt * 1.2f, 0.0f, 1.0f); // Smooth return
    }

    // Camera target: centers on asteroid when normal, zooms out and centers on sun when orbiting
    glm::vec3 normal_pos(0.0f, 1.5f, 16.0f);                               // Close-up on asteroid
    glm::vec3 orbit_pos(28.0f, 8.0f, 56.0f);                               // Centered on sun at (28, 0, 0)
    camera->Position = glm::mix(normal_pos, orbit_pos, m_orbit_progress);  // Interpolated position
    camera->Yaw = -90.0f;                                                  // Keep facing -Z axis
    camera->Pitch = glm::mix(0.0f, -8.13f, m_orbit_progress);              // Tilt down slightly to center on sun
    camera->rotate_camera(0.0f, 0.0f);                                     // Update camera orientation
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();                             // Clear color and depth buffers
}

void MainController::draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float time = platform->frame_time().current;                           // Total runtime in seconds
    float rotation_angle = time * 0.4f;                                    // Asteroid self-rotation angle

    draw_skybox();                                                         // Render space cubemap

    // Calculate planet position in world space
    glm::vec3 planet_pos(0.0f);                                            // Resting position at origin
    if (m_orbit_progress > 0.0f) {
        float radius = 28.0f;                                              // Orbit radius around the sun
        glm::vec3 orbit_offset(-radius * std::cos(m_orbit_angle), 0.0f, -radius * std::sin(m_orbit_angle));
        glm::vec3 full_orbit_pos = m_sun_pos + orbit_offset;               // Circular orbit coordinate
        planet_pos = glm::mix(glm::vec3(0.0f), full_orbit_pos, m_orbit_progress); // Interpolate into orbit
    }

    // Set up lighting uniforms
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("basic");

    if (shader) {
        shader->use();                                                     // Activate shader program
        shader->set_mat4("projection", graphics->projection_matrix());     // Set camera projection
        shader->set_mat4("view", graphics->camera()->view_matrix());       // Set camera view matrix
        shader->set_vec3("viewPos", graphics->camera()->Position);         // Pass camera position for specular

        // Baseline ambient light
        shader->set_vec3("ambientColor", glm::vec3(0.25f, 0.25f, 0.3f));

        // Directional sunlight coming from the sun
        glm::vec3 dir_light_dir = glm::normalize(planet_pos - m_sun_pos);
        shader->set_vec3("dirLightDir", dir_light_dir);
        shader->set_vec3("dirLightColor", glm::vec3(0.9f, 0.85f, 0.72f));
    }

    draw_asteroid(planet_pos, rotation_angle);                             // Draw asteroid (smooth shaded)
    draw_prince(planet_pos, rotation_angle);                               // Draw Little Prince (flat shaded)
    draw_fox(planet_pos, rotation_angle);                                  // Draw Fox (flat shaded)
    draw_sun(time);                                                        // Draw 2.5D sun at (28, 0, 0)
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto skybox_shader = resources->shader("skybox");                      // Skybox shader
    auto skybox = resources->skybox("space");                              // Space cubemap

    if (skybox_shader && skybox) {
        graphics->draw_skybox(skybox_shader, skybox);                      // Draw cubemap background
    }
}

void MainController::draw_sun(float time) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto sun1 = resources->model("sun1");                                  // Sun rays model
    auto sun2 = resources->model("sun2");                                  // Sun center disc model

    if (!shader || !sun1 || !sun2) {
        return;
    }

    shader->set_bool("isEmissive", true);                                  // Unlit self-illumination for the sun

    // Sun 1: Spinning layer placed behind sun 2
    glm::mat4 model_sun1 = glm::mat4(1.0f);
    model_sun1 = glm::translate(model_sun1, m_sun_pos);                    // Position at sun center (28, 0, 0)
    model_sun1 = glm::rotate(model_sun1, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face +Z
    model_sun1 = glm::rotate(model_sun1, time * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));          // Spin rays
    model_sun1 = glm::translate(model_sun1, glm::vec3(-0.35f, 0.0f, 0.0f));                   // Place behind disc
    model_sun1 = glm::scale(model_sun1, glm::vec3(1.1f));                  // Scale slightly larger
    shader->set_mat4("model", model_sun1);
    shader->set_mat3("normalMatrix", glm::mat3(1.0f));
    shader->set_int("texture_diffuse1", 0);
    if (m_sun1_texture) {
        m_sun1_texture->bind(0x84C0);                                      // Bind sun1 texture to GL_TEXTURE0
    }
    sun1->draw(shader);                                                    // Render sun rays

    // Sun 2: Front sun disc
    glm::mat4 model_sun2 = glm::mat4(1.0f);
    model_sun2 = glm::translate(model_sun2, m_sun_pos);                    // Position at sun center (28, 0, 0)
    model_sun2 = glm::rotate(model_sun2, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Face +Z
    model_sun2 = glm::translate(model_sun2, glm::vec3(0.15f, 0.0f, 0.0f));                    // Place in front
    shader->set_mat4("model", model_sun2);
    shader->set_mat3("normalMatrix", glm::mat3(1.0f));
    if (m_sun2_texture) {
        m_sun2_texture->bind(0x84C0);                                      // Bind sun2 texture to GL_TEXTURE0
    }
    sun2->draw(shader);                                                    // Render sun disc

    shader->set_bool("isEmissive", false);                                 // Restore lit shading
}

void MainController::draw_asteroid(const glm::vec3 &planet_pos, float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto asteroid = resources->model("asteroid");

    if (!shader || !asteroid) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, planet_pos);               // World orbit translation
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Self-rotation
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix))); // Normal transformation
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", false);                                // Asteroid uses smooth shading

    shader->set_int("texture_diffuse1", 0);
    if (m_asteroid_texture) {
        m_asteroid_texture->bind(0x84C0);                                  // Bind asteroid texture
    }

    asteroid->draw(shader);                                                // Render asteroid mesh
}

void MainController::draw_prince(const glm::vec3 &planet_pos, float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto prince = resources->model("mali_princ");

    if (!shader || !prince) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, planet_pos);               // World orbit translation
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate with asteroid
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 3.0f, 0.0f));  // Position on asteroid surface
    model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));             // Scale prince
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 5.17f, 0.0f)); // Origin offset
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", true);                                 // Prince uses flat shading

    shader->set_int("texture_diffuse1", 0);
    if (m_prince_texture) {
        m_prince_texture->bind(0x84C0);                                    // Bind prince texture
    }

    prince->draw(shader);                                                  // Render prince mesh
}

void MainController::draw_fox(const glm::vec3 &planet_pos, float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto fox = resources->model("fox");

    if (!shader || !fox) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, planet_pos);               // World orbit translation
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate with asteroid
    model_matrix = glm::translate(model_matrix, glm::vec3(1.1f, 2.75f, 0.3f)); // Position next to prince
    model_matrix = glm::rotate(model_matrix, glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Turn to prince
    model_matrix = glm::scale(model_matrix, glm::vec3(0.25f));             // Scale fox
    model_matrix = glm::translate(model_matrix, glm::vec3(-0.8f, 2.54f, 0.0f)); // Origin offset
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", true);                                 // Fox uses flat shading

    shader->set_int("texture_diffuse1", 0);
    if (m_fox_texture) {
        m_fox_texture->bind(0x84C0);                                       // Bind fox texture
    }

    fox->draw(shader);                                                     // Render fox mesh
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers(); // Swap front/back buffers
}

} // namespace app