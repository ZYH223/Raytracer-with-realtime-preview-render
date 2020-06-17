#include "camera.h"

void VerifyScreenCoordinate(Vec2f &point)
{
	float x = point.x(), y = point.y();
	if (x < 0.0f) x = 0.0f;
	else if (x > 1.0f) x = 1.0f;
	if (y < 0.0f) y = 0.0f;
	else if (y > 1.0f) y = 1.0f;
	point.Set(x, y);
}

OrthographicCamera::OrthographicCamera(Vec3f c, Vec3f d, Vec3f u, float s)
{
	/*d = Vec3f(1.0f, 0.0f, 1.0f);
	u = Vec3f(1.0f, 1.0f, 1.0f);*/
	tmin = -FLT_MAX;
	center = c;
	size = s;
	direction = d;
	direction.Normalize();
	up = (u - (u.Dot3(direction))*direction);
	up.Normalize();
	assert(-0.0001f <= direction.Dot3(up) && direction.Dot3(up) <= 0.0001f);
	Vec3f::Cross3(horizontal, direction, up);
	//Vec3f::Cross3(horizontal, direction, up);
}

Ray OrthographicCamera::generateRay(Vec2f point)
{
	VerifyScreenCoordinate(point);
	return Ray(
		(point.x() * size - size / 2.0f)*horizontal + (point.y() * size - size / 2.0f)*up + center,
		direction);
}
