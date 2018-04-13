#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Emitter/Emitter.h"

#include <memory>

namespace ph
{

class SurfaceBehavior final
{
public:
	SurfaceBehavior();

	void setOptics(std::unique_ptr<SurfaceOptics> optics);
	void setEmitter(const Emitter* emitter);

	inline const SurfaceOptics* getOptics() const
	{
		return m_optics.get();
	}

	inline const Emitter* getEmitter() const
	{
		return m_emitter;
	}

private:
	std::unique_ptr<SurfaceOptics> m_optics;
	const Emitter*                 m_emitter;
};

}// end namespace ph