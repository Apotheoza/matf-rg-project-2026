#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    m_cam_pos = glm::vec3(0.0f, 18.0f, 58.0f);
    m_cam_target = glm::vec3(0.0f, 2.0f, 0.0f);
    graphics->camera()->Position = m_cam_pos;
    glm::vec3 init_dir = glm::normalize(m_cam_target - m_cam_pos);
    graphics->camera()->Pitch = glm::degrees(std::asin(glm::clamp(init_dir.y, -1.0f, 1.0f)));
    graphics->camera()->Yaw = glm::degrees(std::atan2(init_dir.z, init_dir.x));
    graphics->camera()->rotate_camera(0.0f, 0.0f);

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    m_asteroid_texture = resources->texture("asteroid");
    m_prince_texture = resources->texture("mali_princ");
    m_fox_texture = resources->texture("fox");
    m_sun1_texture = resources->texture("sun1");
    m_sun2_texture = resources->texture("sun2");
    m_fenjer_texture = resources->texture("fenjer");
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_A).state() == engine::platform::Key::State::JustPressed) {
        if (m_camera_mode == CameraMode::PRINCE_ASTEROID) {
            m_camera_mode = CameraMode::STAR_SYSTEM;
            m_is_spinning = true;
            m_lamplight_timer = 0.0f;
        } else {
            m_camera_mode = CameraMode::PRINCE_ASTEROID;
            m_is_spinning = false;
            m_lamplight_timer = 0.0f;
        }
    }

    if (platform->key(engine::platform::KeyId::KEY_D).state() == engine::platform::Key::State::JustPressed) {
        if (m_camera_mode == CameraMode::LAMPLIGHT_ASTEROID) {
            m_camera_mode = CameraMode::STAR_SYSTEM;
            m_is_spinning = true;
            m_lamplight_timer = 0.0f;
        } else {
            m_camera_mode = CameraMode::LAMPLIGHT_ASTEROID;
            m_is_spinning = false;
            m_lamplight_timer = 0.0f;
        }
    }

    if (platform->key(engine::platform::KeyId::KEY_S).state() == engine::platform::Key::State::JustPressed ||
        platform->key(engine::platform::KeyId::KEY_SPACE).state() == engine::platform::Key::State::JustPressed) {
        m_camera_mode = CameraMode::STAR_SYSTEM;
        m_is_spinning = true;
        m_lamplight_timer = 0.0f;
    }

    const auto &mouse = platform->mouse();
    if (mouse.scroll != 0.0f) {
        m_lamp_intensity = glm::clamp(m_lamp_intensity + mouse.scroll * 0.5f, 0.0f, 10.0f);
    }

    if (platform->key(engine::platform::KeyId::KEY_C).state() == engine::platform::Key::State::JustPressed) {
        m_auto_cycle_colors = !m_auto_cycle_colors;
        m_color_cycle_timer = 0.0f;
    }

    if (platform->key(engine::platform::KeyId::KEY_RIGHT).state() == engine::platform::Key::State::JustPressed ||
        platform->key(engine::platform::KeyId::KEY_UP).state() == engine::platform::Key::State::JustPressed) {
        m_lamp_color_index = (m_lamp_color_index + 1) % m_lamp_colors.size();
        m_color_cycle_timer = 0.0f;
    }
    if (platform->key(engine::platform::KeyId::KEY_LEFT).state() == engine::platform::Key::State::JustPressed ||
        platform->key(engine::platform::KeyId::KEY_DOWN).state() == engine::platform::Key::State::JustPressed) {
        m_lamp_color_index = (m_lamp_color_index + m_lamp_colors.size() - 1) % m_lamp_colors.size();
        m_color_cycle_timer = 0.0f;
    }

    if (m_auto_cycle_colors) {
        m_color_cycle_timer += dt;
        if (m_color_cycle_timer >= 0.6f) {
            m_color_cycle_timer = 0.0f;
            m_lamp_color_index = (m_lamp_color_index + 1) % m_lamp_colors.size();
        }
    }

    if (m_camera_mode == CameraMode::LAMPLIGHT_ASTEROID) {
        m_lamplight_timer += dt;
    }

    if (m_is_spinning) {
        m_orbit_angle += dt * 0.35f;
        m_self_rotation += dt * 0.4f;
    }

    glm::vec3 prince_pos = m_star_pos + glm::vec3(18.0f * std::cos(m_orbit_angle), 0.0f, 18.0f * std::sin(m_orbit_angle));
    glm::vec3 fenjer_pos = m_star_pos + glm::vec3(26.0f * std::cos(m_orbit_angle + 2.4f), 0.0f, 26.0f * std::sin(m_orbit_angle + 2.4f));

    glm::vec3 target_pos;
    glm::vec3 target_look;

    if (m_camera_mode == CameraMode::PRINCE_ASTEROID) {
        target_pos = prince_pos + glm::vec3(0.0f, 0.5f, 16.5f);
        target_look = prince_pos + glm::vec3(0.0f, 1.95f, 0.0f);
    } else if (m_camera_mode == CameraMode::LAMPLIGHT_ASTEROID) {
        target_pos = fenjer_pos + glm::vec3(0.0f, 8.0f, 19.5f);
        target_look = fenjer_pos + glm::vec3(0.0f, 7.5f, 0.0f);
    } else {
        target_pos = glm::vec3(0.0f, 18.0f, 58.0f);
        target_look = glm::vec3(0.0f, 2.0f, 0.0f);
    }

    float blend = glm::clamp(dt * 3.5f, 0.0f, 1.0f);
    m_cam_pos = glm::mix(m_cam_pos, target_pos, blend);
    m_cam_target = glm::mix(m_cam_target, target_look, blend);

    camera->Position = m_cam_pos;
    glm::vec3 view_dir = glm::normalize(m_cam_target - m_cam_pos);
    camera->Pitch = glm::degrees(std::asin(glm::clamp(view_dir.y, -1.0f, 1.0f)));
    camera->Yaw = glm::degrees(std::atan2(view_dir.z, view_dir.x));
    camera->rotate_camera(0.0f, 0.0f);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float time = platform->frame_time().current;

    draw_skybox();

    glm::vec3 prince_pos = m_star_pos + glm::vec3(18.0f * std::cos(m_orbit_angle), 0.0f, 18.0f * std::sin(m_orbit_angle));
    glm::vec3 fenjer_pos = m_star_pos + glm::vec3(26.0f * std::cos(m_orbit_angle + 2.4f), 0.0f, 26.0f * std::sin(m_orbit_angle + 2.4f));

    glm::mat4 model_fenjer = glm::mat4(1.0f);
    model_fenjer = glm::translate(model_fenjer, fenjer_pos);
    model_fenjer = glm::rotate(model_fenjer, m_self_rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 spot_pos = glm::vec3(model_fenjer * glm::vec4(0.0f, 9.93f, 0.0f, 1.0f));
    glm::vec3 spot_dir = glm::normalize(glm::mat3(model_fenjer) * glm::vec3(0.0f, -0.92f, -0.38f));

    float flicker_factor = 1.0f;
    if (m_camera_mode == CameraMode::LAMPLIGHT_ASTEROID && m_lamplight_timer >= 2.0f && m_lamplight_timer <= 3.6f) {
        float phase = m_lamplight_timer - 2.0f;
        float pattern = std::sin(phase * 45.0f) * std::cos(phase * 23.0f);
        flicker_factor = (pattern > 0.15f) ? 1.0f : ((pattern > -0.3f) ? 0.2f : 0.0f);
    }

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("basic");

    if (shader) {
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_vec3("viewPos", graphics->camera()->Position);

        shader->set_vec3("ambientColor", glm::vec3(0.42f, 0.42f, 0.46f));

        shader->set_vec3("pointLightPos", m_star_pos);
        shader->set_vec3("pointLightColor", glm::vec3(1.0f, 0.96f, 0.88f) * 1.8f);

        shader->set_vec3("spotLightPos", spot_pos);
        shader->set_vec3("spotLightDir", spot_dir);
        shader->set_vec3("spotLightColor", m_lamp_colors[m_lamp_color_index] * m_lamp_intensity * flicker_factor);
        shader->set_float("spotCutOff", glm::cos(glm::radians(25.0f)));
        shader->set_float("spotOuterCutOff", glm::cos(glm::radians(45.0f)));
    }

    draw_prince_asteroid(prince_pos, m_self_rotation);
    draw_fenjer_asteroid(model_fenjer);
    draw_star(time);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    auto skybox_shader = resources->shader("skybox");
    auto skybox = resources->skybox("space");

    if (skybox_shader && skybox) {
        graphics->draw_skybox(skybox_shader, skybox);
    }
}

void MainController::draw_star(float time) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto sun1 = resources->model("sun1");
    auto sun2 = resources->model("sun2");

    if (!shader || !sun1 || !sun2) {
        return;
    }

    shader->set_bool("isEmissive", true);

    glm::mat4 model_sun1 = glm::mat4(1.0f);
    model_sun1 = glm::translate(model_sun1, m_star_pos);
    model_sun1 = glm::rotate(model_sun1, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_sun1 = glm::rotate(model_sun1, time * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    model_sun1 = glm::translate(model_sun1, glm::vec3(-0.35f, 0.0f, 0.0f));
    model_sun1 = glm::scale(model_sun1, glm::vec3(1.3f));
    shader->set_mat4("model", model_sun1);
    shader->set_mat3("normalMatrix", glm::mat3(1.0f));
    shader->set_int("texture_diffuse1", 0);
    if (m_sun1_texture) {
        m_sun1_texture->bind(0x84C0);
    }
    sun1->draw(shader);

    glm::mat4 model_sun2 = glm::mat4(1.0f);
    model_sun2 = glm::translate(model_sun2, m_star_pos);
    model_sun2 = glm::rotate(model_sun2, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_sun2 = glm::translate(model_sun2, glm::vec3(0.15f, 0.0f, 0.0f));
    model_sun2 = glm::scale(model_sun2, glm::vec3(1.2f));
    shader->set_mat4("model", model_sun2);
    shader->set_mat3("normalMatrix", glm::mat3(1.0f));
    if (m_sun2_texture) {
        m_sun2_texture->bind(0x84C0);
    }
    sun2->draw(shader);

    shader->set_bool("isEmissive", false);
}

void MainController::draw_prince_asteroid(const glm::vec3 &planet_pos, float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto asteroid = resources->model("asteroid");
    auto prince = resources->model("mali_princ");
    auto fox = resources->model("fox");

    if (!shader || !asteroid) {
        return;
    }

    glm::mat4 model_asteroid = glm::mat4(1.0f);
    model_asteroid = glm::translate(model_asteroid, planet_pos);
    model_asteroid = glm::rotate(model_asteroid, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    shader->set_mat4("model", model_asteroid);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_asteroid)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", false);
    shader->set_int("texture_diffuse1", 0);
    if (m_asteroid_texture) {
        m_asteroid_texture->bind(0x84C0);
    }
    asteroid->draw(shader);

    if (prince) {
        glm::mat4 model_prince = model_asteroid;
        model_prince = glm::translate(model_prince, glm::vec3(0.0f, 3.0f, 0.0f));
        model_prince = glm::scale(model_prince, glm::vec3(0.35f));
        model_prince = glm::translate(model_prince, glm::vec3(0.0f, 5.17f, 0.0f));
        shader->set_mat4("model", model_prince);
        glm::mat3 norm_prince = glm::transpose(glm::inverse(glm::mat3(model_prince)));
        shader->set_mat3("normalMatrix", norm_prince);
        shader->set_bool("flatShading", true);
        if (m_prince_texture) {
            m_prince_texture->bind(0x84C0);
        }
        prince->draw(shader);
    }

    if (fox) {
        glm::mat4 model_fox = model_asteroid;
        model_fox = glm::scale(model_fox, glm::vec3(0.32f));
        model_fox = glm::rotate(model_fox, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model_fox = glm::rotate(model_fox, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
       
        model_fox = glm::translate(model_fox, glm::vec3(1.1f, 2.75f, 0.3f));
        //model_fox = glm::translate(model_fox, glm::vec3(-0.8f, 2.54f, 0.0f));
        model_fox = glm::translate(model_fox, glm::vec3(-2.0f, 9.7f, 0.0f));

        shader->set_mat4("model", model_fox);
        glm::mat3 norm_fox = glm::transpose(glm::inverse(glm::mat3(model_fox)));
        shader->set_mat3("normalMatrix", norm_fox);
        shader->set_bool("flatShading", true);
        if (m_fox_texture) {
            m_fox_texture->bind(0x84C0);
        }
        fox->draw(shader);
    }
}

void MainController::draw_fenjer_asteroid(const glm::mat4 &model_matrix) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = resources->shader("basic");
    auto fenjer_model = resources->model("fenjer");

    if (!shader || !fenjer_model) {
        return;
    }

    shader->set_mat4("model", model_matrix);
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", false);
    shader->set_int("texture_diffuse1", 0);
    if (m_fenjer_texture) {
        m_fenjer_texture->bind(0x84C0);
    }

    fenjer_model->draw(shader);
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

} // namespace app