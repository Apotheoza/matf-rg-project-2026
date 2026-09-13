#include <app/App.hpp>

int main(int argc, char **argv) {
    return std::make_unique<app::App>()->run(argc, argv);
}
