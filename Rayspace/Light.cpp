#include "Light.h"

Light::Light(glm::vec3 _Position, glm::vec3 _ColorInt)
{
	Position = _Position;
	ColorIntensity = _ColorInt;
}

Light::Light()
{
}

Light::~Light()
{
}

glm::vec3 Light::GetPosition()
{
	return Position;
}

void Light::SetPosition(glm::vec3 _UpdatedPos)
{
	Position = _UpdatedPos;
}
