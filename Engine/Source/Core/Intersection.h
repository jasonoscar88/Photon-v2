#pragma once

#include "Math/Vector3f.h"

namespace ph
{

class Triangle;

class Intersection final
{
public:
	Intersection();

	inline void clear()
	{
		m_hitPosition.set(0, 0, 0);
		m_hitNormal.set(0, 0, -1);
		m_hitUVW.set(0, 0, 0);
		m_hitTriangle = nullptr;
	}

	inline const Vector3f& getHitPosition() const
	{
		return m_hitPosition;
	}

	inline const Vector3f& getHitNormal() const
	{
		return m_hitNormal;
	}

	inline const Vector3f& getHitUVW() const
	{
		return m_hitUVW;
	}

	inline const Triangle* getHitTriangle() const
	{
		return m_hitTriangle;
	}

	inline void setHitPosition(const Vector3f& hitPosition)
	{
		m_hitPosition.set(hitPosition);
	}

	inline void setHitNormal(const Vector3f& hitNormal)
	{
		m_hitNormal.set(hitNormal);
	}

	inline void setHitUVW(const Vector3f& hitUVW)
	{
		m_hitUVW.set(hitUVW);
	}

	inline void setHitTriangle(const Triangle* const hitTriangle)
	{
		m_hitTriangle = hitTriangle;
	}

private:
	Vector3f m_hitPosition;
	Vector3f m_hitNormal;
	Vector3f m_hitUVW;
	const Triangle* m_hitTriangle;
};

}// end namespace ph