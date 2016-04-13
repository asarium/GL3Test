//
//

#include <SDL.h>

#include <memory>
#include <util/Timing.hpp>
#include <renderer/Renderer.hpp>
#include <renderer/opengl/GL3Renderer.hpp>
#include <util/DefaultFileLoader.hpp>
#include <renderer/Exceptions.hpp>
#include "test/Application.hpp"

namespace {
    std::unique_ptr<Timing> timing;
    std::unique_ptr<Renderer> renderer;

    std::unique_ptr<Application> app;

    SDL_Window *window = nullptr;

    void render() {
        app->render(renderer.get());
    }

    bool process_events() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    return false;
                default:
                    app->handleEvent(&event);
                    break;
            }
        }

        return true;
    }

    void run_mainloop() {
        int counter = 0;
        float lastTime = 0.f;

        while (true) {
            timing->tick();

            render();

            if (!process_events()) {
                return;
            }
            ++counter;
            if (timing->getTotalTime() - lastTime >= 1.f) {
                printf("%2.0f\n", counter / (timing->getTotalTime() - lastTime));
                counter = 0;
                lastTime = timing->getTotalTime();
            }
        }
    }

    bool init() {
        renderer.reset(new GL3Renderer(std::unique_ptr<FileLoader>(new DefaultFileLoader())));
        try {
            RendererSettings settings;
            settings.resolution = glm::uvec2(1680, 1050);
            settings.vertical_sync = true;
            settings.msaa_samples = 0;
            settings.shadow_quality = SettingsLevel::High;
            renderer->getSettingsManager()->changeSettings(settings);

            window = renderer->initialize();
        } catch (const RendererException& e) {
            printf("Failed to initialize renderer: %s\n", e.what());
            return false;
        }

        // Check that the window was successfully created
        if (window == NULL) {
            // In the case that the window could not be made...
            printf("Could not create window: %s\n", SDL_GetError());
            return false;
        }
        SDL_ShowWindow(window);

        timing.reset(new Timing());

        app.reset(new Application(renderer.get(), timing.get()));

        return true;
    }

    void deinit() {
        app.reset();

        renderer->deinitialize();

        timing.reset();
        renderer.reset();
    }
}

#undef main

int main(int argc, char **argv) {
    SDL_Init(0);

    if (!init()) {
        return 1;
    }

    run_mainloop();

    deinit();

    SDL_Quit();
}
