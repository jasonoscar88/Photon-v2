#include "Core/Quantity/SpectralStrength/LinearSrgbSpectralStrength.h"

namespace ph
{

LinearSrgbSpectralStrength::LinearSrgbSpectralStrength() :
	Parent()
{}

LinearSrgbSpectralStrength::LinearSrgbSpectralStrength(const LinearSrgbSpectralStrength& other) :
	Parent(other)
{}

LinearSrgbSpectralStrength::LinearSrgbSpectralStrength(const TArithmeticArray<real, 3>& other) :
	Parent(other)
{}

Vector3R LinearSrgbSpectralStrength::impl_genLinearSrgb() const
{
	return Vector3R(m[0], m[1], m[2]);
}

void LinearSrgbSpectralStrength::impl_setLinearSrgb(const Vector3R& linearSrgb)
{
	m[0] = linearSrgb.x;
	m[1] = linearSrgb.y;
	m[2] = linearSrgb.z;
}

void LinearSrgbSpectralStrength::impl_setSampled(const TArithmeticArray<real, 3>& samples)
{
	// TODO
}

}// end namespace ph