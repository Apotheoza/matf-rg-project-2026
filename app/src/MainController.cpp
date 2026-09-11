#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

namespace app {

void MainController::initialize() {
    engine::graphics::OpenGL::enable_depth_testing();

    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->camera()->Position = glm::vec3(0.0f, 0.0f, 15.0f);

    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto asteroid_texture = resources->texture("asteroid");
    if (asteroid_texture) {
        m_loaded_textures.push_back(asteroid_texture);
    }
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    draw_skybox();
    draw_asteroid();
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

void MainController::draw_asteroid() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Shader *shader = resources->shader("basic");
    engine::resources::Model *asteroid = resources->model("asteroid");

    if (!shader || !asteroid) {
        return;
    }

    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    shader->set_mat4("model", model_matrix);

    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model_matrix)));
    shader->set_mat3("normalMatrix", normal_matrix);

    glm::vec3 light_pos(5.0f, 5.0f, 8.0f);
    glm::vec3 light_color(1.0f, 1.0f, 1.0f);
    float ambient_strength = 0.15f;
    glm::vec3 ambient_color = ambient_strength * light_color;

    shader->set_vec3("lightPos", light_pos);
    shader->set_vec3("lightColor", light_color);
    shader->set_vec3("ambientColor", ambient_color);
    shader->set_vec3("viewPos", graphics->camera()->Position);

    shader->set_int("texture_diffuse1", 0);
    if (!m_loaded_textures.empty()) {
        m_loaded_textures[0]->bind(0x84C0);
    }

    asteroid->draw(shader);
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

} // namespace app