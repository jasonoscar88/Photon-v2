#pragma once

#include "Actor/Geometry/Geometry.h"
#include "Common/primitive_type.h"

namespace ph
{

class GRectangle : public Geometry
{
public:
	GRectangle(const float32 width, const float32 height);
	GRectangle(const InputPacket& packet);
	virtual ~GRectangle() override;

	virtual void discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const override;

private:
	float32 m_width;
	float32 m_height;
};

}// end namespace ph