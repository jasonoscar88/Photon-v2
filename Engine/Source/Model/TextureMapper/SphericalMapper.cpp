#include "Model/TextureMapper/SphericalMapper.h"
#include "Math/Vector3f.h"
#include "Math/Math.h"
#include "Math/constant.h"

#include <iostream>

namespace ph
{

SphericalMapper::~SphericalMapper() = default;

void SphericalMapper::map(const Vector3f& position, const float32 u, const float32 v, float32* const out_u, float32* const out_v) const
{
	if(position.length() < 1e-8)
	{
		std::cerr << "warning: at SphericalMapper::map(), positions too close to geometry origin may induce errors during mapping" << std::endl;
	}

	const Vector3f positionDir = position.normalize();

	const float32 cosTheta = Math::clamp(positionDir.y, -1.0f, 1.0f);
	const float32 cosPhi = Math::clamp(positionDir.x, -1.0f, 1.0f);
	const float32 sinPhi = Math::clamp(positionDir.z, -1.0f, 1.0f);

	const float32 theta = acos(cosTheta);     // [  0, pi]
	const float32 phi = atan2(sinPhi, cosPhi);// [-pi, pi]

	*out_u = (-phi + PI_FLOAT32) / (2.0f * PI_FLOAT32);// [0, 1]
	*out_v = (PI_FLOAT32 - theta) / PI_FLOAT32;        // [0, 1]
}

}// end namespace ph