#pragma once

#include "DirectionalLight.h"

class DayNightCycle
{
public:
    void setup();
    void updateTime(float dt);
    DirectionalLight sun;
    glm::vec3 color;


private:
    void adjustSunPosition(float dt);
    void sunIntensity();
    void sunColor();

    float timeOfDay{ 0.0f };
    float cycleLength{ 32.0f };
    float quad = (cycleLength / 4);



};