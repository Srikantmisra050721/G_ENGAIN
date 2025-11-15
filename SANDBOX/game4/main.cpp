#include "../ENGAIN/core/Logger.h"
#include "../ENGAIN/core/TimeManager.h"
#include "../ENGAIN/core/Window.h"
#include "../ENGAIN/core/Texture.h"
#include "../ENGAIN/core/Input.h"
#include "../ENGAIN/core/Math.h"
#include "../ENGAIN/core/Font.h"
#include <SDL2/SDL.h>
#include <vector>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>

using namespace ENGAIN;

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

float randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// Base game object
class GameObject {
public:
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationSpeed;
    bool active;
    
    GameObject() : position(0, 0), velocity(0, 0), rotation(0), rotationSpeed(0), active(true) {}
    
    virtual void update(float dt, int screenWidth, int screenHeight) {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        rotation += rotationSpeed * dt;
        
        // Wrap around screen
        if (position.x < 0) position.x += screenWidth;
        if (position.x > screenWidth) position.x -= screenWidth;
        if (position.y < 0) position.y += screenHeight;
        if (position.y > screenHeight) position.y -= screenHeight;
    }
    
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual float getRadius() const = 0;
    virtual ~GameObject() {}
};

// Player ship inherates from GameObject
class Ship : public GameObject {
public:
    float size;
    bool thrusting;
    float thrustPower;
    float drag;
    int lives;
    bool invulnerable;
    float invulnerableTime;
    Texture* shipTexture;
    
    Ship(float x, float y, Texture* tex) : size(32.0f), thrusting(false), thrustPower(300.0f), 
                             drag(0.99f), lives(3), invulnerable(true), invulnerableTime(3.0f),
                             shipTexture(tex) {
        position = Vector2(x, y);
    }
    
    void update(float dt, int screenWidth, int screenHeight) override {
        Input& input = Input::getInstance();
        
        // Rotation
        if (input.isKeyDown(SDLK_LEFT) || input.isKeyDown(SDLK_a)) {
            rotationSpeed = -180.0f;
        } else if (input.isKeyDown(SDLK_RIGHT) || input.isKeyDown(SDLK_d)) {
            rotationSpeed = 180.0f;
        } else {
            rotationSpeed = 0;
        }
        
        // Thrust
        thrusting = input.isKeyDown(SDLK_UP) || input.isKeyDown(SDLK_w);
        if (thrusting) {
            float rad = rotation * M_PI / 180.0f;
            velocity.x += cos(rad) * thrustPower * dt;
            velocity.y += sin(rad) * thrustPower * dt;
        }
        
        // Apply drag
        velocity.x *= drag;
        velocity.y *= drag;
        
        // Update invulnerability
        if (invulnerable) {
            invulnerableTime -= dt;
            if (invulnerableTime <= 0) {
                invulnerable = false;
            }
        }
        
        GameObject::update(dt, screenWidth, screenHeight);
    }
    
    void render(SDL_Renderer* renderer) override {
        if (invulnerable) {
            int flash = (int)(invulnerableTime * 10) % 2;
            if (flash == 0) return; // Blink when invulnerable
        }
        
        // Render ship sprite with rotation
        // The sprite is drawn pointing right (0 degrees), so we add 90 to rotation
        SDL_Rect destRect = { (int)(position.x - 42), (int)(position.y - 64), 85, 128 };
        SDL_Point center = { 42, 64 };
        SDL_RenderCopyEx(renderer, shipTexture->getSDLTexture(), nullptr, &destRect, 
                        rotation + 90, &center, SDL_FLIP_NONE);
        
        // Thrust flame (still draw with lines for effect)
        if (thrusting) {
            float rad = rotation * M_PI / 180.0f;
            Vector2 back(position.x - cos(rad) * size * 0.8f, 
                        position.y - sin(rad) * size * 0.8f);
            Vector2 left(position.x + cos(rad + 2.5f) * size * 0.6f, 
                        position.y + sin(rad + 2.5f) * size * 0.6f);
            Vector2 right(position.x + cos(rad - 2.5f) * size * 0.6f, 
                         position.y + sin(rad - 2.5f) * size * 0.6f);
            
            SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);
            SDL_RenderDrawLine(renderer, left.x, left.y, back.x, back.y);
            SDL_RenderDrawLine(renderer, right.x, right.y, back.x, back.y);
        }
    }
    
    float getRadius() const override { return size; }
    
    void reset(float x, float y) {
        position = Vector2(x, y);
        velocity = Vector2(0, 0);
        rotation = 0;
        invulnerable = true;
        invulnerableTime = 3.0f;
    }
};

// Bullet
class Bullet : public GameObject {
public:
    float size;
    float lifetime;
    float maxLifetime;
    Texture* missileTexture;
    
    Bullet(Texture* tex) : size(8.0f), lifetime(0), maxLifetime(2.0f), missileTexture(tex) {
        active = false;
    }
    
    void fire(Vector2 pos, float rot, Vector2 shipVel) {
        position = pos;
        rotation = rot;
        float rad = rot * M_PI / 180.0f;
        velocity.x = cos(rad) * 500.0f + shipVel.x;
        velocity.y = sin(rad) * 500.0f + shipVel.y;
        lifetime = 0;
        active = true;
    }
    
    void update(float dt, int screenWidth, int screenHeight) override {
        lifetime += dt;
        if (lifetime > maxLifetime) {
            active = false;
        }
        GameObject::update(dt, screenWidth, screenHeight);
    }
    
    void render(SDL_Renderer* renderer) override {
        // Render missile sprite with rotation
        SDL_Rect destRect = { (int)(position.x - 8), (int)(position.y - 8), 16, 16 };
        SDL_Point center = { 8, 8 };
        SDL_RenderCopyEx(renderer, missileTexture->getSDLTexture(), nullptr, &destRect, 
                        rotation + 90, &center, SDL_FLIP_NONE);
    }
    
    float getRadius() const override { return size; }
};

// Asteroid
class Asteroid : public GameObject {
public:
    float size;
    int points;
    
    enum Size { LARGE, MEDIUM, SMALL };
    Size asteroidSize;
    Texture* texture;
    
    Asteroid(Texture* largeTex, Texture* mediumTex, Texture* smallTex) 
        : size(0), points(0), asteroidSize(LARGE), texture(nullptr),
          largeTexture(largeTex), mediumTexture(mediumTex), smallTexture(smallTex) {
        active = false;
    }
    
    void spawn(Vector2 pos, Size sz, Vector2 vel = Vector2(0, 0)) {
        position = pos;
        asteroidSize = sz;
        
        switch (sz) {
            case LARGE:
                size = 64.0f;
                points = 20;
                texture = largeTexture;
                break;
            case MEDIUM:
                size = 32.0f;
                points = 50;
                texture = mediumTexture;
                break;
            case SMALL:
                size = 16.0f;
                points = 100;
                texture = smallTexture;
                break;
        }
        
        // Random velocity if not provided
        if (vel.x == 0 && vel.y == 0) {
            float angle = randomFloat(0, 2 * M_PI);
            float speed = randomFloat(30, 80);
            velocity = Vector2(cos(angle) * speed, sin(angle) * speed);
        } else {
            velocity = vel;
        }
        
        rotationSpeed = randomFloat(-90, 90);
        active = true;
    }
    
    void render(SDL_Renderer* renderer) override {
        int spriteSize = 0;
        switch (asteroidSize) {
            case LARGE: spriteSize = 128; break;
            case MEDIUM: spriteSize = 64; break;
            case SMALL: spriteSize = 32; break;
        }
        
        SDL_Rect destRect = { (int)(position.x - spriteSize/2), (int)(position.y - spriteSize/2), 
                             spriteSize, spriteSize };
        SDL_Point center = { spriteSize/2, spriteSize/2 };
        SDL_RenderCopyEx(renderer, texture->getSDLTexture(), nullptr, &destRect, 
                        rotation, &center, SDL_FLIP_NONE);
    }
    
    float getRadius() const override { return size; }
    
private:
    Texture* largeTexture;
    Texture* mediumTexture;
    Texture* smallTexture;
};

// Check collision between two game objects
bool checkCollision(GameObject* a, GameObject* b) {
    float dx = a->position.x - b->position.x;
    float dy = a->position.y - b->position.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (a->getRadius() + b->getRadius());
}

int main() {
    Logger::getInstance().initialize();
    Logger::getInstance().info("=== Game6 - Asteroids with Sprites Starting ===");
    
    // Create fullscreen window
    Window window("Game6 - Asteroids | ESC to Exit", 1920, 1080, false);
    if (!window.initialize()) {
        Logger::getInstance().error("Failed to initialize window");
        return -1;
    }
    
    // Set fullscreen
    SDL_SetWindowFullscreen(window.getSDLWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_Delay(16);
    
    // Load textures
    Texture shipTexture;
    Texture missileTexture;
    Texture asteroidLargeTexture;
    Texture asteroidMediumTexture;
    Texture asteroidSmallTexture;
    Texture spaceBackground;
    
    if (!shipTexture.loadFromFile("assets/ship.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load ship texture");
        return -1;
    }
    if (!missileTexture.loadFromFile("assets/missile.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load missile texture");
        return -1;
    }
    if (!asteroidLargeTexture.loadFromFile("assets/asteroid_large.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load large asteroid texture");
        return -1;
    }
    if (!asteroidMediumTexture.loadFromFile("assets/asteroid_medium.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load medium asteroid texture");
        return -1;
    }
    if (!asteroidSmallTexture.loadFromFile("assets/asteroid_small.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load small asteroid texture");
        return -1;
    }
    if (!spaceBackground.loadFromFile("assets/space_bg.png", window.getRenderer())) {
        Logger::getInstance().error("Failed to load space background texture");
        return -1;
    }
    
    // Initialize text renderer
    TextRenderer& textRenderer = TextRenderer::getInstance();
    textRenderer.initialize();
    textRenderer.loadFont("default", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    textRenderer.loadFont("large", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 48);
    
    TimeManager timeManager(60);
    
    int screenWidth = window.getWidth();
    int screenHeight = window.getHeight();
    
    // Game objects
    Ship ship(screenWidth / 2, screenHeight / 2, &shipTexture);
    ship.rotation = -90;  // Point upward
    std::vector<Bullet> bullets;
    for (int i = 0; i < 20; i++) {
        bullets.push_back(Bullet(&missileTexture));
    }
    
    std::vector<Asteroid> asteroids;
    for (int i = 0; i < 50; i++) {
        asteroids.push_back(Asteroid(&asteroidLargeTexture, &asteroidMediumTexture, &asteroidSmallTexture));
    }
    
    // Game state
    int score = 0;
    int level = 1;
    bool gameOver = false;
    float shootCooldown = 0;
    const float SHOOT_DELAY = 0.25f;
    
    // Spawn initial asteroids
    auto spawnLevel = [&](int numAsteroids) {
        for (int i = 0; i < numAsteroids; i++) {
            for (auto& asteroid : asteroids) {
                if (!asteroid.active) {
                    // Spawn at edges
                    Vector2 pos;
                    if (rand() % 2 == 0) {
                        pos.x = (rand() % 2 == 0) ? 0 : screenWidth;
                        pos.y = randomFloat(0, screenHeight);
                    } else {
                        pos.x = randomFloat(0, screenWidth);
                        pos.y = (rand() % 2 == 0) ? 0 : screenHeight;
                    }
                    asteroid.spawn(pos, Asteroid::LARGE);
                    break;
                }
            }
        }
    };
    
    spawnLevel(3 + level);
    
    Logger::getInstance().info("Entering main loop");
    
    // Main game loop
    while (window.isRunning()) {
        window.handleEvents();
        Input::getInstance().update();
        
        // Exit with ESC
        if (Input::getInstance().isKeyPressed(SDLK_ESCAPE)) {
            break;
        }
        
        timeManager.update();
        float dt = timeManager.getDeltaTime();
        if (dt > 0.1f) dt = 0.1f;
        
        shootCooldown -= dt;
        
        if (!gameOver) {
            // Update ship
            ship.update(dt, screenWidth, screenHeight);
            
            // Shoot
            if ((Input::getInstance().isKeyDown(SDLK_SPACE) || 
                 Input::getInstance().isKeyDown(SDLK_RETURN)) && shootCooldown <= 0) {
                for (auto& bullet : bullets) {
                    if (!bullet.active) {
                        float rad = ship.rotation * M_PI / 180.0f;
                        // Fire from the front of the ship sprite (32 pixels from center)
                        Vector2 gunPos(ship.position.x + cos(rad) * 32.0f,
                                      ship.position.y + sin(rad) * 32.0f);
                        bullet.fire(gunPos, ship.rotation, ship.velocity);
                        shootCooldown = SHOOT_DELAY;
                        break;
                    }
                }
            }
            
            // Update bullets
            for (auto& bullet : bullets) {
                if (bullet.active) {
                    bullet.update(dt, screenWidth, screenHeight);
                }
            }
            
            // Update asteroids
            for (auto& asteroid : asteroids) {
                if (asteroid.active) {
                    asteroid.update(dt, screenWidth, screenHeight);
                }
            }
            
            // Check bullet-asteroid collisions
            for (auto& bullet : bullets) {
                if (!bullet.active) continue;
                
                for (auto& asteroid : asteroids) {
                    if (!asteroid.active) continue;
                    
                    if (checkCollision(&bullet, &asteroid)) {
                        bullet.active = false;
                        asteroid.active = false;
                        score += asteroid.points;
                        
                        // Split asteroid if not small
                        if (asteroid.asteroidSize == Asteroid::LARGE) {
                            for (int i = 0; i < 2; i++) {
                                for (auto& newAst : asteroids) {
                                    if (!newAst.active) {
                                        float angle = randomFloat(0, 2 * M_PI);
                                        float speed = randomFloat(60, 120);
                                        Vector2 vel(cos(angle) * speed, sin(angle) * speed);
                                        newAst.spawn(asteroid.position, Asteroid::MEDIUM, vel);
                                        break;
                                    }
                                }
                            }
                        } else if (asteroid.asteroidSize == Asteroid::MEDIUM) {
                            for (int i = 0; i < 2; i++) {
                                for (auto& newAst : asteroids) {
                                    if (!newAst.active) {
                                        float angle = randomFloat(0, 2 * M_PI);
                                        float speed = randomFloat(80, 150);
                                        Vector2 vel(cos(angle) * speed, sin(angle) * speed);
                                        newAst.spawn(asteroid.position, Asteroid::SMALL, vel);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
            
            // Check ship-asteroid collisions
            if (!ship.invulnerable) {
                for (auto& asteroid : asteroids) {
                    if (asteroid.active && checkCollision(&ship, &asteroid)) {
                        ship.lives--;
                        if (ship.lives > 0) {
                            ship.reset(screenWidth / 2, screenHeight / 2);
                        } else {
                            gameOver = true;
                        }
                        break;
                    }
                }
            }
            
            // Check if level complete
            bool allClear = true;
            for (const auto& asteroid : asteroids) {
                if (asteroid.active) {
                    allClear = false;
                    break;
                }
            }
            
            if (allClear) {
                level++;
                spawnLevel(3 + level);
            }
        } else {
            // Game over - restart with R
            if (Input::getInstance().isKeyPressed(SDLK_r)) {
                ship.lives = 3;
                ship.reset(screenWidth / 2, screenHeight / 2);
                score = 0;
                level = 1;
                gameOver = false;
                
                // Clear all
                for (auto& bullet : bullets) bullet.active = false;
                for (auto& asteroid : asteroids) asteroid.active = false;
                
                spawnLevel(3 + level);
            }
        }
        
        // Render
        SDL_Renderer* renderer = window.getRenderer();
        
        // Draw space background
        SDL_Rect bgRect = { 0, 0, screenWidth, screenHeight };
        SDL_RenderCopy(renderer, spaceBackground.getSDLTexture(), nullptr, &bgRect);
        
        // Draw asteroids
        for (auto& asteroid : asteroids) {
            if (asteroid.active) {
                asteroid.render(renderer);
            }
        }
        
        // Draw bullets
        for (auto& bullet : bullets) {
            if (bullet.active) {
                bullet.render(renderer);
            }
        }
        
        // Draw ship
        if (!gameOver) {
            ship.render(renderer);
        }
        
        // Draw UI
        std::ostringstream scoreText;
        scoreText << "SCORE: " << score;
        textRenderer.drawText(renderer, scoreText.str(), 20, 20, "default", 
                             SDL_Color{255, 255, 255, 255});
        
        std::ostringstream levelText;
        levelText << "LEVEL: " << level;
        textRenderer.drawText(renderer, levelText.str(), 20, 50, "default",
                             SDL_Color{255, 255, 255, 255});
        
        // Draw lives (using ship sprite icons)
        for (int i = 0; i < ship.lives; i++) {
            SDL_Rect lifeRect = { 20 + i * 30, 85, 24, 24 };
            SDL_RenderCopy(renderer, shipTexture.getSDLTexture(), nullptr, &lifeRect);
        }
        
        if (gameOver) {
            std::string gameOverText = "GAME OVER";
            textRenderer.drawText(renderer, gameOverText, screenWidth / 2 - 150, screenHeight / 2 - 50, 
                                 "large", SDL_Color{255, 0, 0, 255});
            
            std::string restartText = "Press R to Restart";
            textRenderer.drawText(renderer, restartText, screenWidth / 2 - 120, screenHeight / 2 + 20,
                                 "default", SDL_Color{255, 255, 255, 255});
        }
        
        window.present();
    }
    
    Logger::getInstance().info("Game ended");
    Logger::getInstance().info("Final score: " + std::to_string(score));
    
    return 0;
}
