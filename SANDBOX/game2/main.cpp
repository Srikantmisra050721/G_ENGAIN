#include "../ENGAIN/core/Logger.h"
#include "../ENGAIN/core/TimeManager.h"
#include "../ENGAIN/core/Window.h"
#include "../ENGAIN/core/Texture.h"
#include "../ENGAIN/core/Input.h"
#include "../ENGAIN/core/Math.h"
#include "../ENGAIN/core/Font.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <cmath>
#include <sstream>

using namespace ENGAIN;

// Player class with improved physics
class Player {
public:
    Vector2 position;
    Vector2 velocity;
    float width, height;
    bool onGround;
    bool facingRight;
    
    Texture* texture;
    
    // Physics constants
    const float MAX_SPEED = 250.0f;
    const float ACCELERATION = 1200.0f;
    const float FRICTION = 800.0f;
    const float AIR_FRICTION = 200.0f;
    const float JUMP_FORCE = -450.0f;
    const float GRAVITY = 1200.0f;
    const float MAX_FALL_SPEED = 500.0f;
    
    Player(float x, float y, Texture* tex) 
        : position(x, y), velocity(0, 0), width(48), height(48),
          onGround(false), facingRight(true), texture(tex) {}
    
    void handleInput(float deltaTime) {
        Input& input = Input::getInstance();
        
        // Horizontal movement with acceleration
        float targetVelX = 0;
        if (input.isKeyDown(SDLK_LEFT) || input.isKeyDown(SDLK_a)) {
            targetVelX = -MAX_SPEED;
            facingRight = false;
        }
        if (input.isKeyDown(SDLK_RIGHT) || input.isKeyDown(SDLK_d)) {
            targetVelX = MAX_SPEED;
            facingRight = true;
        }
        
        // Apply acceleration or friction
        if (targetVelX != 0) {
            if (velocity.x < targetVelX) {
                velocity.x += ACCELERATION * deltaTime;
                if (velocity.x > targetVelX) velocity.x = targetVelX;
            } else if (velocity.x > targetVelX) {
                velocity.x -= ACCELERATION * deltaTime;
                if (velocity.x < targetVelX) velocity.x = targetVelX;
            }
        } else {
            // Apply friction
            float friction = onGround ? FRICTION : AIR_FRICTION;
            if (velocity.x > 0) {
                velocity.x -= friction * deltaTime;
                if (velocity.x < 0) velocity.x = 0;
            } else if (velocity.x < 0) {
                velocity.x += friction * deltaTime;
                if (velocity.x > 0) velocity.x = 0;
            }
        }
        
        // Jump
        if ((input.isKeyPressed(SDLK_SPACE) || input.isKeyPressed(SDLK_UP) || input.isKeyPressed(SDLK_w)) && onGround) {
            velocity.y = JUMP_FORCE;
            onGround = false;
        }
    }
    
    void update(float deltaTime) {
        // Apply gravity
        if (!onGround) {
            velocity.y += GRAVITY * deltaTime;
            if (velocity.y > MAX_FALL_SPEED) {
                velocity.y = MAX_FALL_SPEED;
            }
        }
        
        // Update position
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
    }
    
    Rectangle getBounds() const {
        return Rectangle(position.x, position.y, width, height);
    }
    
    void render(SDL_Renderer* renderer) {
        if (texture && texture->getSDLTexture()) {
            SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            texture->renderScaled(renderer, 
                                static_cast<int>(position.x), 
                                static_cast<int>(position.y),
                                static_cast<int>(width),
                                static_cast<int>(height),
                                flip);
        } else {
            // Draw colored rectangle if no texture
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
            SDL_Rect rect = {static_cast<int>(position.x), static_cast<int>(position.y), 
                           static_cast<int>(width), static_cast<int>(height)};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

// Platform class
class Platform {
public:
    Rectangle bounds;
    Color color;
    
    Platform(float x, float y, float w, float h, Color c = Color(100, 100, 100))
        : bounds(x, y, w, h), color(c) {}
    
    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect rect = bounds.toSDLRect();
        SDL_RenderFillRect(renderer, &rect);
        
        // Draw border
        SDL_SetRenderDrawColor(renderer, color.r - 30, color.g - 30, color.b - 30, 255);
        SDL_RenderDrawRect(renderer, &rect);
    }
};

int main() {
    // Initialize logger
    Logger::getInstance().initialize();
    Logger::getInstance().info("=== Game2 Platformer Starting ===");
    
    // Create window
    Window window("Game2 - Platformer", 1024, 600, false);
    if (!window.initialize()) {
        Logger::getInstance().error("Failed to initialize window");
        return -1;
    }
    
    // Initialize SDL_image and SDL_ttf
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        Logger::getInstance().error("SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
        return -1;
    }
    
    if (TTF_Init() == -1) {
        Logger::getInstance().error("SDL_ttf could not initialize! SDL_ttf Error: " + std::string(TTF_GetError()));
        return -1;
    }
    
    // Load player texture
    Texture kittyTexture;
    kittyTexture.loadFromFile("assets/kitty.png", window.getRenderer());
    
    // Load font
    Font gameFont;
    if (!gameFont.loadFromFile("assets/font.ttf", 24)) {
        Logger::getInstance().warning("Failed to load font");
    }
    
    // Create time manager
    TimeManager timeManager(60);
    
    // Create player
    Player player(50, 100, &kittyTexture);
    
    // Create level platforms
    std::vector<Platform> platforms;
    
    // Ground
    platforms.push_back(Platform(0, 560, 1024, 40, Color(60, 180, 60)));
    
    // Starting area
    platforms.push_back(Platform(150, 480, 120, 15, Color(139, 69, 19)));
    platforms.push_back(Platform(320, 400, 120, 15, Color(139, 69, 19)));
    
    // Mid section with gaps
    platforms.push_back(Platform(500, 320, 100, 15, Color(139, 69, 19)));
    platforms.push_back(Platform(650, 380, 80, 15, Color(139, 69, 19)));
    platforms.push_back(Platform(780, 300, 100, 15, Color(139, 69, 19)));
    
    // Upper platforms
    platforms.push_back(Platform(200, 240, 100, 15, Color(139, 69, 19)));
    platforms.push_back(Platform(400, 180, 120, 15, Color(139, 69, 19)));
    platforms.push_back(Platform(600, 120, 100, 15, Color(139, 69, 19)));
    
    // Floating platforms
    platforms.push_back(Platform(850, 480, 60, 15, Color(100, 100, 200)));
    platforms.push_back(Platform(950, 420, 60, 15, Color(100, 100, 200)));
    
    // Background color
    Color skyColor(100, 150, 230);
    
    Logger::getInstance().info("Entering main loop");
    
    // Main game loop
    while (window.isRunning()) {
        // Handle events
        window.handleEvents();
        
        // Update time
        timeManager.update();
        float deltaTime = timeManager.getDeltaTime();
        
        // Update input
        Input::getInstance().update();
        
        // Handle player input
        player.handleInput(deltaTime);
        
        // Update player physics
        player.update(deltaTime);
        
        // Collision detection - more robust
        player.onGround = false;
        Rectangle playerBounds = player.getBounds();
        
        for (auto& platform : platforms) {
            if (playerBounds.intersects(platform.bounds)) {
                // Calculate overlap on each axis
                float overlapLeft = (playerBounds.x + playerBounds.width) - platform.bounds.x;
                float overlapRight = (platform.bounds.x + platform.bounds.width) - playerBounds.x;
                float overlapTop = (playerBounds.y + playerBounds.height) - platform.bounds.y;
                float overlapBottom = (platform.bounds.y + platform.bounds.height) - playerBounds.y;
                
                // Find minimum overlap
                float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : -overlapRight;
                float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : -overlapBottom;
                
                // Resolve collision on axis with smallest overlap
                if (std::abs(minOverlapX) < std::abs(minOverlapY)) {
                    // Horizontal collision
                    player.position.x -= minOverlapX;
                    player.velocity.x = 0;
                } else {
                    // Vertical collision
                    player.position.y -= minOverlapY;
                    if (minOverlapY > 0 && player.velocity.y >= 0) {
                        // Landing on top - only set onGround if falling or stationary
                        player.velocity.y = 0;
                        player.onGround = true;
                    } else {
                        // Hit from below
                        player.velocity.y = 0;
                    }
                }
            }
        }
        
        // Keep player in horizontal bounds
        if (player.position.x < 0) {
            player.position.x = 0;
            player.velocity.x = 0;
        }
        if (player.position.x + player.width > window.getWidth()) {
            player.position.x = window.getWidth() - player.width;
            player.velocity.x = 0;
        }
        
        // Reset if player falls off screen
        if (player.position.y > window.getHeight()) {
            player.position = Vector2(50, 100);
            player.velocity = Vector2(0, 0);
            Logger::getInstance().info("Player respawned");
        }
        
        // Render
        window.clear(skyColor);
        
        SDL_Renderer* renderer = window.getRenderer();
        
        // Draw platforms
        for (auto& platform : platforms) {
            platform.render(renderer);
        }
        
        // Draw player
        player.render(renderer);
        
        // Draw UI
        std::ostringstream fpsText;
        fpsText << "FPS: " << static_cast<int>(timeManager.getFPS());
        SDL_Color whiteColor = {255, 255, 255, 255};
        gameFont.drawText(renderer, fpsText.str(), 10, 10, whiteColor);
        
        gameFont.drawText(renderer, "Controls: A/D or Arrow Keys to move, SPACE/W/UP to jump", 
                         10, window.getHeight() - 30, whiteColor);
        
        window.present();
    }
    
    // Cleanup
    Logger::getInstance().info("=== Shutting down ===");
    TTF_Quit();
    IMG_Quit();
    
    return 0;
}
