#include "DayNightCycle.h"

void DayNightCycle::setup()
{
	using namespace glm;

	sun.direction = vec3(-1, 0, 0);
	sun.color = vec3(1);
	sun.intensity = 0;

}

void DayNightCycle::updateTime(float dt)
{
	timeOfDay += dt;

	if (timeOfDay >= cycleLength) {//new day
		timeOfDay = 0.0;
	}

	adjustSunPosition(dt);
	sunIntensity();
	sunColor();

}


void DayNightCycle::adjustSunPosition(float dt)
{
	using namespace glm;

	if (timeOfDay <= quad) {
		//x: -1 -> 0
		//y: 0 -> -1
		//(-1, 0, 0)   +   (+, -, 0) -> (0, 0) (1, -1).
		sun.direction += mix(vec3(0, 0, 0), vec3(1, -1, 0), (dt*0.5)*(1.8/quad)); //dt*0.5
		sun.direction.x = clamp(sun.direction.x, -1.0f, 0.0f);
		sun.direction.y = clamp(sun.direction.y, -1.0f, 0.0f);
	}
	else if (timeOfDay <= quad*2) {
		//x: 0 -> 1
		//y: -1 -> 0
		//(0, -1, 0)   +   (+, +, 0) -> (0, 0) (1, 1)
		sun.direction += mix(vec3(0, 0, 0), vec3(1, 1, 0),(dt * 0.5) * (1.8 / quad));
		sun.direction.x = clamp(sun.direction.x, 0.0f, 1.0f);
		sun.direction.y = clamp(sun.direction.y, -1.0f, 0.0f);
	}

	else if (timeOfDay <= quad*3) {
		//x: 1 -> 0
		//y: 0 -> 1
		//(1, 0, 0)   +   (-, +, 0) -> (0, 0) (-1, 1)
		sun.direction += mix(vec3(0, 0, 0), vec3(-1, 1, 0), (dt * 0.5) * (1.8 / quad));
		sun.direction.x = clamp(sun.direction.x, 0.0f, 1.0f);
		sun.direction.y = clamp(sun.direction.y, 0.0f, 1.0f);
	}
	else {
		//x: 0 -> -1
		//y: 1 -> 0
		//(0, 1, 0)   +   (-, -, 0) -> (0, 0) (-1, -1)
		sun.direction += mix(vec3(0, 0, 0), vec3(-1, -1, 0), (dt * 0.5) * (1.8 / quad));
		sun.direction.x = clamp(sun.direction.x, -1.0f, 0.0f);
		sun.direction.y = clamp(sun.direction.y, 0.0f, 1.0f);
	}

}

void DayNightCycle::sunIntensity()
{

	if (timeOfDay <= quad) {//morning
		float sunriseRate = glm::mix(0.0f, 0.02f, timeOfDay / quad);
		sun.intensity += sunriseRate;
	}
	else {
		float sunsetRate = glm::mix(0.0f, 0.02f, (timeOfDay-quad) / quad);
		sun.intensity -= sunsetRate;

		sun.intensity = glm::max(0.0f, sun.intensity);
	}

}

void DayNightCycle::sunColor()
{
	using namespace glm;

	//quad1: blue -> yellowWhite
	//quad2: yellowWhite -> orange
	//quad3: orange -> darkblue
	//quad4: darkblue -> blue
	vec3 blue = vec3(102, 145, 166) / 255;
	vec3 yellowWhite = vec3(255, 255, 220) / 255;
	vec3 orange = vec3(237, 99, 46) / 255;
	vec3 darkblue = vec3(0, 25, 70) / 255;

	if (timeOfDay <= quad) {
		sun.color = mix(blue, yellowWhite, timeOfDay / quad);
	}
	else if (timeOfDay <= quad * 2) {
		sun.color = mix(yellowWhite, orange, (timeOfDay-quad) / quad);
	}
	else if (timeOfDay <= quad * 3) {
		sun.color = mix(orange, darkblue, (timeOfDay - quad*2) / quad);
	}
	else {
		sun.color = mix(darkblue, blue, (timeOfDay - quad*3) / quad);
	}
}


