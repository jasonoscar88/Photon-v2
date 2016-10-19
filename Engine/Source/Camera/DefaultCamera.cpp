#include "Camera/DefaultCamera.h"
#include "Core/Ray.h"

namespace ph
{

DefaultCamera::DefaultCamera() :
	Camera(), 
	m_fov(70.0f / 180.0f * 3.1415927f)
{

}

DefaultCamera::~DefaultCamera() = default;

void DefaultCamera::genSampleRay(Ray* const out_ray, const uint32 xResPx, const uint32 yResPx, float32 xPx, float32 yPx) const
{
	// Note: this will fail when the camera is facing directly on y-axis

	Vector3f rightDir = Vector3f(-getDirection().z, 0.0f, getDirection().x).normalizeLocal();
	Vector3f upDir = rightDir.cross(getDirection()).normalizeLocal();

	const float32 halfWidth = tan(m_fov / 2.0f);
	const float32 halfHeight = halfWidth * static_cast<float32>(yResPx) / static_cast<float32>(xResPx);

	const float32 pixelPosX = ((xPx + 0.5f) / (xResPx / 2.0f) - 1.0f) * halfWidth;
	const float32 pixelPosY = ((yPx + 0.5f) / (yResPx / 2.0f) - 1.0f) * halfHeight;

	rightDir.mulLocal(pixelPosX);
	upDir.mulLocal(pixelPosY);

	out_ray->setDirection(getDirection().add(rightDir.addLocal(upDir)).normalizeLocal());
	out_ray->setOrigin(getPosition());
}

}// end namespace ph