#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Math/math_fwd.h"

namespace ph
{

class PInfiniteSphere : public Primitive
{
public:
	explicit PInfiniteSphere(const PrimitiveMetadata* metadata);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	void calcIntersectionDetail(
		const Ray& ray, 
		HitProbe&  probe,
		HitDetail* out_detail) const override;

	bool isIntersectingVolumeConservative(const AABB3D& volume) const override;
	void calcAABB(AABB3D* out_aabb) const override;

	Vector3R uvwToPosition(const Vector3R& uvw) const;
};

}// end namespace ph