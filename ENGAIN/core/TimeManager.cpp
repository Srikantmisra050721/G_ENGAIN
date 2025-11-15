#include "TimeManager.h"
#include "Logger.h"
#include <thread>
#include <numeric>
#include <sstream>

namespace ENGAIN {

TimeManager::TimeManager(int targetFPS)
    : targetFPS(targetFPS),
      targetFrameTime(targetFPS > 0 ? 1.0f / targetFPS : 0.0f),
      deltaTime(0.0f),
      totalTime(0.0f),
      frameCount(0),
      fps(0.0f),
      fpsUpdateInterval(0.5f),
      fpsTimer(0.0f),
      fpsFrameCount(0) {
    
    lastTime = Clock::now();
    currentTime = Clock::now();
    
    std::ostringstream oss;
    oss << "TimeManager initialized with target FPS: " << targetFPS;
    Logger::getInstance().info(oss.str());
}

void TimeManager::update() {
    currentTime = Clock::now();
    
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    deltaTime = elapsed.count();
    lastTime = currentTime;
    
    // Update total time and frame count
    totalTime += deltaTime;
    frameCount++;
    
    // Store frame time for averaging
    frameTimes.push_back(deltaTime);
    if (frameTimes.size() > MAX_FRAME_HISTORY) {
        frameTimes.pop_front();
    }
    
    // Update FPS
    fpsTimer += deltaTime;
    fpsFrameCount++;
    
    if (fpsTimer >= fpsUpdateInterval) {
        fps = fpsFrameCount / fpsTimer;
        fpsTimer = 0.0f;
        fpsFrameCount = 0;
    }
}

void TimeManager::limitFPS() {
    if (targetFPS <= 0) return;
    
    auto now = Clock::now();
    std::chrono::duration<float> elapsed = now - lastTime;
    float sleepTime = targetFrameTime - elapsed.count();
    
    if (sleepTime > 0) {
        std::this_thread::sleep_for(
            std::chrono::duration<float>(sleepTime)
        );
    }
}

float TimeManager::getAverageFrameTime() const {
    if (frameTimes.empty()) return 0.0f;
    
    float sum = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0f);
    return sum / frameTimes.size();
}

void TimeManager::reset() {
    lastTime = Clock::now();
    currentTime = Clock::now();
    deltaTime = 0.0f;
    frameCount = 0;
    totalTime = 0.0f;
    fps = 0.0f;
    fpsTimer = 0.0f;
    fpsFrameCount = 0;
    frameTimes.clear();
    
    Logger::getInstance().info("TimeManager reset");
}

} // namespace ENGAIN
