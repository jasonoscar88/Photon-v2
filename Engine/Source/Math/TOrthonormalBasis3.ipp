#pragma once

#include "Math/TOrthonormalBasis3.h"
#include "Math/Math.h"

#include <algorithm>
#include <cmath>

namespace ph
{

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3() :
	xAxis(1, 0, 0),
	yAxis(0, 1, 0),
	zAxis(0, 0, 1)
{

}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(const TOrthonormalBasis3& other) :
	TOrthonormalBasis3(other.xAxis, other.yAxis, other.zAxis)
{

}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis, 
	const TVector3<T>& zAxis) : 
	xAxis(xAxis),
	yAxis(yAxis),
	zAxis(zAxis)
{

}

template<typename T>
inline TVector3<T> TOrthonormalBasis3<T>::worldToLocal(const TVector3<T>& worldVec) const
{
	return TVector3<T>(xAxis.dot(worldVec), 
	                   yAxis.dot(worldVec), 
	                   zAxis.dot(worldVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosPhi(const TVector3<T>& unitVec) const
{
	const T cosT            = cosTheta(unitVec);
	TVector3<T>& xzPlaneVec = unitVec.sub(yAxis.mul(cosT));
	const T length2         = xzPlaneVec.lengthSquared();

	if(length2 != 0)
	{
		xzPlaneVec.mulLocal(1 / std::sqrt(length2));
		return Math::clamp(xzPlaneVec.normalizeLocal().dot(xAxis), -1, 1);
	}
	else
	{
		return 1;
	}
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinPhi(const TVector3<T>& unitVec) const
{
	return std::sqrt(sin2Phi(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cos2Phi(const TVector3<T>& unitVec) const
{
	const T cosP = cosPhi(unitVec);
	return cosP * cosP;
}

template<typename T>
inline T TOrthonormalBasis3<T>::sin2Phi(const TVector3<T>& unitVec) const
{
	return 1 - cos2Phi(unitVec);
}

template<typename T>
inline T TOrthonormalBasis3<T>::cosTheta(const TVector3<T>& unitVec) const
{
	return Math::clamp(yAxis.dot(unitVec), -1, 1);
}

template<typename T>
inline T TOrthonormalBasis3<T>::sinTheta(const TVector3<T>& unitVec) const
{
	return std::sqrt(sin2Theta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::absCosTheta(const TVector3<T>& unitVec) const
{
	return std::abs(cosTheta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::absSinTheta(const TVector3<T>& unitVec) const
{
	return std::abs(sinTheta(unitVec));
}

template<typename T>
inline T TOrthonormalBasis3<T>::cos2Theta(const TVector3<T>& unitVec) const
{
	const T cosT = cosTheta(unitVec);
	return cosT * cosT;
}

template<typename T>
inline T TOrthonormalBasis3<T>::sin2Theta(const TVector3<T>& unitVec) const
{
	return 1 - cos2Theta(unitVec);
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalize()
{
	xAxis.normalizeLocal();
	yAxis.normalizeLocal();
	zAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline void TOrthonormalBasis3<T>::set(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis,
	const TVector3<T>& zAxis)
{
	this->xAxis = xAxis;
	this->yAxis = yAxis;
	this->zAxis = zAxis;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::operator = (const TOrthonormalBasis3& rhs)
{
	set(rhs.xAxis, rhs.yAxis, rhs.zAxis);

	return *this;
}

}// end namespace ph