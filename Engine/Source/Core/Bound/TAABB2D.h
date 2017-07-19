#pragma once

#include "Math/TVector2.h"

namespace ph
{

template<typename T>
class TAABB2D final
{
public:
	TVector2<T> minVertex;
	TVector2<T> maxVertex;

	inline TAABB2D();
	inline TAABB2D(const TAABB2D& other);
	inline TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex);

	template<typename U>
	explicit inline TAABB2D(const TAABB2D<U>& other);

	inline bool isIntersectingArea(const TAABB2D& other) const;
	inline bool isIntersectingArea(const TVector2<T>& point) const;
	inline T calcArea() const;
	inline TAABB2D& unionWith(const TAABB2D& other);
	inline TAABB2D& intersectWith(const TAABB2D& other);
	inline T getWidth() const;
	inline T getHeight() const;
	inline bool isValid() const;
	inline bool isPoint() const;
};

}// end namespace ph

#include "Core/Bound/TAABB2D.ipp"