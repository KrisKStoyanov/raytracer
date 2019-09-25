#include "Light.h"

Light::Light(glm::vec3 _Position, glm::vec3 _ColorInt)
{
	Position = _Position;
	ColorIntensity = _ColorInt;
}

Light::~Light()
{
}
