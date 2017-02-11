#pragma once

#include "Common/primitive_type.h"
#include "FileIO/ValueClause.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"

#include <vector>
#include <string>
#include <memory>

namespace ph
{

class NamedResourceCache;
class Geometry;
class Texture;
class Material;
class LightSource;
class PhysicalActor;
class InputPrototype;

class InputPacket final
{
public:
	InputPacket(const std::vector<ValueClause>& vClauses, const NamedResourceCache& cache);
	InputPacket(InputPacket&& other);

	std::string getString(const std::string& name, const std::string& defaultString = "", const std::string& notFoundMessage = "") const;
	integer getInteger(const std::string& name, const integer defaultInteger = 0, const std::string& notFoundMessage = "") const;
	real getReal(const std::string& name, const real defaultReal = 0.0f, const std::string& notFoundMessage = "") const;
	Vector3R getVector3r(const std::string& name, 
	                     const Vector3R& defaultVector3r = Vector3R(), 
	                     const std::string& notFoundMessage = "") const;
	QuaternionR getQuaternionR(const std::string& name,
	                           const QuaternionR& defaultQuaternionR = QuaternionR(),
	                           const std::string& notFoundMessage = "") const;
	std::vector<Vector3R> getVector3rArray(const std::string& name, 
	                                       const std::vector<Vector3R>& defaultVector3rArray = std::vector<Vector3R>(), 
	                                       const std::string& notFoundMessage = "") const;
	std::shared_ptr<Geometry> getGeometry(const std::string& name, const std::string& notFoundMessage = "") const;
	std::shared_ptr<Texture> getTexture(const std::string& name, const std::string& notFoundMessage = "") const;
	std::shared_ptr<Material> getMaterial(const std::string& name, const std::string& notFoundMessage = "") const;
	std::shared_ptr<LightSource> getLightSource(const std::string& name, const std::string& notFoundMessage = "") const;
	PhysicalActor* getPhysicalActor(const std::string& name, const std::string& notFoundMessage = "") const;

	bool isPrototypeMatched(const InputPrototype& prototype) const;

	// forbid copying
	InputPacket(const InputPacket& other) = delete;
	InputPacket& operator = (const InputPacket& rhs) = delete;

private:
	const std::vector<ValueClause> m_vClauses;
	const NamedResourceCache& m_cache;

	bool findStringValue(const std::string& typeName, const std::string& name, const std::string& notFoundMessage,
	                     std::string* const out_value) const;

	static void printNotFoundMessage(const std::string& typeName, const std::string& name, const std::string& message);
};

}// end namespace ph