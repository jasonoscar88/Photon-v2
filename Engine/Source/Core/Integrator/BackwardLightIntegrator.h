#pragma once

#include "Core/Integrator/Integrator.h"
#include "Math/math_fwd.h"

namespace ph
{

class BackwardLightIntegrator final : public Integrator, public TCommandInterface<BackwardLightIntegrator>
{
public:
	virtual ~BackwardLightIntegrator() override;

	virtual void update(const Scene& scene) override;
	virtual void radianceAlongRay(const Ray& ray, const RenderData& data, std::vector<SenseEvent>& out_senseEvents) const override;

private:
	static void rationalClamp(Vector3R& value);

// command interface
public:
	BackwardLightIntegrator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<BackwardLightIntegrator> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<BackwardLightIntegrator>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph