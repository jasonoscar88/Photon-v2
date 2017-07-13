#include "Actor/ALight.h"
#include "Math/Math.h"
#include "Actor/TextureMapper/DefaultMapper.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/LightSource/LightSource.h"
#include "Core/CookedActor.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Actor/LightSource/EmitterBuildingMaterial.h"
#include "Core/Intersectable/TransformedIntersectable.h"

#include <algorithm>
#include <iostream>

namespace ph
{

ALight::ALight() : 
	PhysicalActor(), 
	m_geometry(nullptr), m_material(nullptr), m_lightSource(nullptr)
{

}

ALight::ALight(const std::shared_ptr<LightSource>& lightSource) : 
	PhysicalActor(),
	m_geometry(nullptr), m_material(nullptr), m_lightSource(lightSource)
{

}

ALight::ALight(const ALight& other) : 
	PhysicalActor(other),
	m_geometry(other.m_geometry), m_material(other.m_material), m_lightSource(other.m_lightSource)
{

}

ALight::~ALight() = default;


ALight& ALight::operator = (ALight rhs)
{
	swap(*this, rhs);

	return *this;
}

void ALight::cook(CookedActor* const out_cookedActor) const
{
	CookedActor               cookedActor;
	PrimitiveBuildingMaterial primitiveBuildingMaterial;
	EmitterBuildingMaterial   emitterBuildingMaterial;

	if(m_lightSource)
	{
		auto localToWorld = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
		auto worldToLocal = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));

		if(m_geometry && m_material)
		{
			if(m_localToWorld.hasScaleEffect())
			{
				std::cerr << "warning: at ALight::cook(), "
				          << "scale detected; scaling on light with attached geometry may have " 
				          << "unexpected behaviors such as miscalculated primitive surface area, "
				          << "which can cause severe rendering artifacts" << std::endl;
			}

			// TODO: transform must be static, and must bake into primitive to solve the
			// scaling problem


			std::unique_ptr<PrimitiveMetadata> metadata = std::make_unique<PrimitiveMetadata>();
			

			primitiveBuildingMaterial.metadata = metadata.get();
			std::vector<std::unique_ptr<Primitive>> primitives;
			m_geometry->genPrimitive(primitiveBuildingMaterial, primitives);
			m_material->populateSurfaceBehavior(&(metadata->surfaceBehavior));

			for(auto& primitive : primitives)
			{
				emitterBuildingMaterial.primitives.push_back(primitive.get());

				auto localToWorld  = std::make_unique<StaticTransform>(StaticTransform::makeForward(m_localToWorld));
				auto worldToLocal  = std::make_unique<StaticTransform>(StaticTransform::makeInverse(m_localToWorld));
				auto intersectable = std::make_unique<TransformedIntersectable>(std::move(primitive),
				                                                                std::move(localToWorld),
				                                                                std::move(worldToLocal));
				cookedActor.intersectables.push_back(std::move(intersectable));
			}
			cookedActor.primitiveMetadata = std::move(metadata);
		}
		
		cookedActor.emitter = m_lightSource->buildEmitter(emitterBuildingMaterial);
		cookedActor.emitter->setTransform(localToWorld.get(), worldToLocal.get());
		cookedActor.primitiveMetadata->surfaceBehavior.setEmitter(cookedActor.emitter.get());
		cookedActor.transforms.push_back(std::move(localToWorld));
		cookedActor.transforms.push_back(std::move(worldToLocal));
	}
	else
	{
		std::cerr << "warning: at ALight::cook(), " 
		          << "incomplete data detected" << std::endl;
	}

	*out_cookedActor = std::move(cookedActor);
}

const Geometry* ALight::getGeometry() const
{
	return m_geometry.get();
}

const Material* ALight::getMaterial() const
{
	return m_material.get();
}

const LightSource* ALight::getLightSource() const
{
	return m_lightSource.get();
}

void ALight::setGeometry(const std::shared_ptr<Geometry>& geometry)
{
	m_geometry = geometry;
}

void ALight::setMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
}

void ALight::setLightSource(const std::shared_ptr<LightSource>& lightSource)
{
	m_lightSource = lightSource;
}

void swap(ALight& first, ALight& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<PhysicalActor&>(first), static_cast<PhysicalActor&>(second));
	swap(first.m_geometry,                   second.m_geometry);
	swap(first.m_material,                   second.m_material);
	swap(first.m_lightSource,                second.m_lightSource);
}

ALight::ALight(const InputPacket& packet) :
	PhysicalActor(packet),
	m_geometry(nullptr), m_material(nullptr), m_lightSource(nullptr)
{
	const DataTreatment requiredData(EDataImportance::REQUIRED, "ALight requires at least a LightSource");
	m_geometry = packet.get<Geometry>("geometry");
	m_material = packet.get<Material>("material");
	m_lightSource = packet.get<LightSource>("light-source", requiredData);
}

SdlTypeInfo ALight::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "light");
}

ExitStatus ALight::ciExecute(const std::shared_ptr<ALight>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return PhysicalActor::ciExecute(targetResource, functionName, packet);
}

}// end namespace ph