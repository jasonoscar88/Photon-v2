#pragma once

#include "Actor/Model/TextureMapper/TextureMapper.h"

namespace ph
{

class DefaultMapper final : public TextureMapper
{
public:
	virtual ~DefaultMapper() override;

	virtual void map(const Vector3f& position, const Vector3f& uvw, Vector3f* const out_uvw) const override;
};

}// end namespace ph