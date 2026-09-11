#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <vector>

namespace app {

class MainController final : public engine::core::Controller {
public:
    std::string_view name() const override {
        return "app::MainController";
    }

protected:
    void initialize() override {
        engine::graphics::OpenGL::enable_depth_testing();

        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = graphics->camera();
        camera->Position = glm::vec3(0.0f, 0.0f, 15.0f);

        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto asteroid_texture = resources->texture("asteroid");
        if (asteroid_texture) {
            m_loaded_textures.push_back(asteroid_texture);
        }
    }

    bool loop() override {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_ESCAPE).state() == engine::platform::Key::State::JustPressed) {
            return false;
        }
        return true;
    }

    void begin_draw() override {
        engine::graphics::OpenGL::clear_buffers();
    }

    void draw() override {
        auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

        engine::resources::Shader *skybox_shader = resources->shader("skybox");
        engine::resources::Skybox *skybox = resources->skybox("space");
        if (skybox_shader && skybox) {
            graphics->draw_skybox(skybox_shader, skybox);
        }

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

        shader->set_int("texture_diffuse1", 0);
        if (!m_loaded_textures.empty()) {
            m_loaded_textures[0]->bind(0x84C0);
        }

        asteroid->draw(shader);
    }

    void end_draw() override {
        engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
    }

private:
    std::vector<engine::resources::Texture *> m_loaded_textures;
};

class MyApp final : public engine::core::App {
protected:
    void app_setup() override {
        auto main_controller = register_controller<MainController>();
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    }
};

} // namespace app

int main(int argc, char **argv) {
    return std::make_unique<app::MyApp>()->run(argc, argv);
}


