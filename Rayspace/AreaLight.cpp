#include "AreaLight.h"

AreaLight::AreaLight(glm::vec3 _Position, glm::vec3 _ColorInt, glm::vec3 _Size) : Light(Position, ColorIntensity)
{
	Position = _Position;
	ColorIntensity = _ColorInt;
	MinPoint = Position - _Size;
	MaxPoint = Position + _Size;
}

AreaLight::~AreaLight()
{
}
