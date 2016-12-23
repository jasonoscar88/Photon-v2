#include "Camera/DefaultCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"

#include <limits>

namespace ph
{

DefaultCamera::DefaultCamera() :
	Camera(),
	m_fov(70.0f / 180.0f * 3.1415927f)
{

}

DefaultCamera::~DefaultCamera() = default;

void DefaultCamera::genSensingRay(const Sample& sample, Ray* const out_ray, const float32 aspectRatio) const
{
	// Note: this will fail when the camera is facing directly on y-axis

	Vector3f rightDir = Vector3f(-getDirection().z, 0.0f, getDirection().x).normalizeLocal();
	Vector3f upDir = rightDir.cross(getDirection()).normalizeLocal();

	const float32 halfWidth = tan(m_fov / 2.0f);
	const float32 halfHeight = halfWidth / aspectRatio;

	const float32 pixelPosX = sample.m_cameraX * halfWidth;
	const float32 pixelPosY = sample.m_cameraY * halfHeight;

	rightDir.mulLocal(pixelPosX);
	upDir.mulLocal(pixelPosY);

	out_ray->setDirection(getDirection().add(rightDir.addLocal(upDir)).mulLocal(-1.0f).normalizeLocal());
	out_ray->setOrigin(getPosition());
	out_ray->setMinT(RAY_T_EPSILON);
	out_ray->setMaxT(RAY_T_MAX);
}

}// end namespace ph