#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "FileIO/FileSystem/Path.h"

#include <string>
#include <vector>

namespace ph
{

class ValueParser final
{
public:
	ValueParser(const Path& workingDirectory);

	integer               parseInteger(const std::string& integerString) const;
	real                  parseReal(const std::string& realString) const;
	std::string           parseString(const std::string& stringString) const;
	Vector3R              parseVector3(const std::string& vector3String) const;
	QuaternionR           parseQuaternion(const std::string& quaternionstring) const;
	std::vector<real>     parseRealArray(const std::string& realArrayString) const;
	std::vector<Vector3R> parseVector3Array(const std::string& vector3ArrayString) const;

private:
	Path m_workingDirectory;

	std::string loadResource(const std::string& identifier) const;

	static bool startsWithNumber(const std::string& string);
};

}// end namespace ph