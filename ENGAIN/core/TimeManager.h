#pragma once

#include <chrono>
#include <deque>

namespace ENGAIN {

class TimeManager {
public:
    TimeManager(int targetFPS = 60);
    
    void update();
    void limitFPS();
    void reset();
    
    // Getters
    float getDeltaTime() const { return deltaTime; }
    float getFPS() const { return fps; }
    float getAverageFrameTime() const;
    unsigned int getFrameCount() const { return frameCount; }
    float getTotalTime() const { return totalTime; }
    int getTargetFPS() const { return targetFPS; }
    
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    
    int targetFPS;
    float targetFrameTime;
    
    TimePoint lastTime;
    TimePoint currentTime;
    
    float deltaTime;
    float totalTime;
    unsigned int frameCount;
    
    // FPS tracking
    float fps;
    float fpsUpdateInterval;
    float fpsTimer;
    unsigned int fpsFrameCount;
    
    // Frame time history
    std::deque<float> frameTimes;
    static const size_t MAX_FRAME_HISTORY = 60;
};

} // namespace ENGAIN
