#pragma once

#include "Core/Emitter/SurfaceEmitter.h"
#include "Common/primitive_type.h"

#include <memory>
#include <vector>

namespace ph
{

class Primitive;

class PrimitiveAreaEmitter final : public SurfaceEmitter
{
public:
	PrimitiveAreaEmitter(const Primitive* primitive);
	virtual ~PrimitiveAreaEmitter() override;

	virtual void evalEmittedRadiance(const SurfaceHit& X, SpectralStrength* out_radiance) const override;
	virtual void genDirectSample(DirectLightSample& sample) const override;
	virtual void genSensingRay(Ray* out_ray, SpectralStrength* out_Le, Vector3R* out_eN, real* out_pdfA, real* out_pdfW) const override;
	virtual real calcDirectSamplePdfW(const Vector3R& targetPos, const Vector3R& emitPos, const Vector3R& emitN, const Primitive* hitPrim) const override;

	const Primitive* getPrimitive() const;

private:
	const Primitive* m_primitive;
	real m_reciExtendedArea;
};

}// end namespace ph