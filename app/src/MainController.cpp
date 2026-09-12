#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->camera()->Position = glm::vec3(0.0f, 1.5f, 16.0f);
    graphics->camera()->MovementSpeed = 5.0f;

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    m_asteroid_texture = resources->texture("asteroid");
    m_prince_texture = resources->texture("mali_princ");
    m_fox_texture = resources->texture("fox");
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    }
}

void MainController::update() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();

    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_SPACE).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::UP, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_LEFT_SHIFT).is_down() ||
        platform->key(engine::platform::KeyId::KEY_LEFT_CONTROL).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::DOWN, dt);
    }

    const auto &mouse = platform->mouse();
    bool is_rotating = platform->key(engine::platform::KeyId::MOUSE_BUTTON_RIGHT).is_down() ||
                       platform->key(engine::platform::KeyId::MOUSE_BUTTON_LEFT).is_down() ||
                       !m_cursor_enabled;
    if (is_rotating && (mouse.dx != 0.0f || mouse.dy != 0.0f)) {
        camera->rotate_camera(mouse.dx, mouse.dy);
    }

    if (mouse.scroll != 0.0f) {
        camera->zoom(mouse.scroll);
        graphics->perspective_params().FOV = glm::radians(camera->Zoom);
    }
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float time = platform->frame_time().current;
    float rotation_angle = time * 0.4f;

    draw_skybox();

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = resources->shader("basic");

    if (shader) {
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()->view_matrix());
        shader->set_vec3("viewPos", graphics->camera()->Position);

        glm::vec3 light_pos(4.0f, 4.0f, -4.0f);
        glm::vec3 light_color(1.0f, 0.98f, 0.9f);
        glm::vec3 ambient_color(0.25f, 0.25f, 0.3f);
        shader->set_vec3("lightPos", light_pos);
        shader->set_vec3("lightColor", light_color);
        shader->set_vec3("ambientColor", ambient_color);
    }

    draw_asteroid(rotation_angle);
    draw_prince(rotation_angle);
    draw_fox(rotation_angle);
}

void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Shader *skybox_shader = resources->shader("skybox");
    engine::resources::Skybox *skybox = resources->skybox("space");

    if (skybox_shader && skybox) {
        graphics->draw_skybox(skybox_shader, skybox);
    }
}

void MainController::draw_asteroid(float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("basic");
    engine::resources::Model *asteroid = resources->model("asteroid");

    if (!shader || !asteroid) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", false);

    shader->set_int("texture_diffuse1", 0);
    if (m_asteroid_texture) {
        m_asteroid_texture->bind(0x84C0);
    }

    asteroid->draw(shader);
}

void MainController::draw_prince(float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("basic");
    engine::resources::Model *prince = resources->model("mali_princ");

    if (!shader || !prince) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 3.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.35f));
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 5.17f, 0.0f));
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", true);

    shader->set_int("texture_diffuse1", 0);
    if (m_prince_texture) {
        m_prince_texture->bind(0x84C0);
    }

    prince->draw(shader);
}

void MainController::draw_fox(float rotation_angle) {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    engine::resources::Shader *shader = resources->shader("basic");
    engine::resources::Model *fox = resources->model("fox");

    if (!shader || !fox) {
        return;
    }

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::rotate(model_matrix, rotation_angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::translate(model_matrix, glm::vec3(1.1f, 2.75f, 0.3f));
    model_matrix = glm::rotate(model_matrix, glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(0.25f));
    model_matrix = glm::translate(model_matrix, glm::vec3(-0.8f, 2.54f, 0.0f));
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);
    shader->set_bool("flatShading", true);

    shader->set_int("texture_diffuse1", 0);
    if (m_fox_texture) {
        m_fox_texture->bind(0x84C0);
    }

    fox->draw(shader);
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

} // namespace app