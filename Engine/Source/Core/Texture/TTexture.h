#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

template<typename OutputType>
class TTexture
{
public:
	inline TTexture();
	inline virtual ~TTexture() = 0;

	inline virtual void sample(const Vector3R& uvw, OutputType* out_value) const = 0;
};

}// end namespace ph

#include "Core/Texture/TTexture.ipp"