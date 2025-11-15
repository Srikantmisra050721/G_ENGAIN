#include "Window.h"
#include "Logger.h"
#include <sstream>

namespace ENGAIN {

Window::Window(const std::string& title, int width, int height, bool resizable)
    : title(title),
      width(width),
      height(height),
      resizable(resizable),
      running(false),
      focused(true),
      initialized(false),
      window(nullptr),
      renderer(nullptr) {
}

Window::~Window() {
    close();
}

bool Window::initialize() {
    if (initialized) return true;
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::ostringstream oss;
        oss << "SDL initialization failed: " << SDL_GetError();
        Logger::getInstance().error(oss.str());
        return false;
    }
    
    Logger::getInstance().info("SDL initialized");
    
    // Create window
    Uint32 flags = SDL_WINDOW_SHOWN;
    if (resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags
    );
    
    if (!window) {
        std::ostringstream oss;
        oss << "Window creation failed: " << SDL_GetError();
        Logger::getInstance().error(oss.str());
        SDL_Quit();
        return false;
    }
    
    // Create renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        std::ostringstream oss;
        oss << "Renderer creation failed: " << SDL_GetError();
        Logger::getInstance().error(oss.str());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    
    std::ostringstream oss;
    oss << "Window created: " << width << "x" << height << " - '" << title << "'";
    Logger::getInstance().info(oss.str());
    
    initialized = true;
    running = true;
    
    return true;
}

void Window::handleEvents() {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                Logger::getInstance().info("Window close requested");
                break;
                
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        width = event.window.data1;
                        height = event.window.data2;
                        {
                            std::ostringstream oss;
                            oss << "Window resized to " << width << "x" << height;
                            Logger::getInstance().debug(oss.str());
                        }
                        break;
                        
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        focused = true;
                        Logger::getInstance().debug("Window gained focus");
                        break;
                        
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        focused = false;
                        Logger::getInstance().debug("Window lost focus");
                        break;
                }
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    Logger::getInstance().info("ESC pressed - closing window");
                }
                break;
        }
    }
}

void Window::clear(const Color& color) {
    if (renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }
}

void Window::present() {
    if (renderer) {
        SDL_RenderPresent(renderer);
    }
}

void Window::close() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    if (initialized) {
        SDL_Quit();
        Logger::getInstance().info("Window closed");
        initialized = false;
    }
}

void Window::setTitle(const std::string& newTitle) {
    title = newTitle;
    if (window) {
        SDL_SetWindowTitle(window, title.c_str());
    }
}

} // namespace ENGAIN
