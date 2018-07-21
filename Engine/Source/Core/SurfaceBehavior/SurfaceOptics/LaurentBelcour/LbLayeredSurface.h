#pragma once

#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableFGD.h"
#include "Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/TableTIR.h"
#include "FileIO/FileSystem/CoreResource.h"

namespace ph
{

class LbLayeredSurface : public SurfaceOptics
{
public:
	LbLayeredSurface();
	~LbLayeredSurface() override;

private:
	void evalBsdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		SpectralStrength* out_bsdf) const override;

	void genBsdfSample(
		const SurfaceHit& X, const Vector3R& V,
		Vector3R* out_L,
		SpectralStrength* out_pdfAppliedBsdf) const override;

	void calcBsdfSamplePdf(
		const SurfaceHit& X, const Vector3R& L, const Vector3R& V,
		real* out_pdfW) const override;

	static const TableFGD& FGD();
	static const TableTIR& TIR();
};

// In-header Implementations:

inline const TableFGD& LbLayeredSurface::FGD()
{
	static const TableFGD table(CoreResource("LaurentBelcourBsdf/table_FGD.bin").getPath());
	return table;
}

inline const TableTIR& LbLayeredSurface::TIR()
{
	static const TableTIR table(CoreResource("LaurentBelcourBsdf/table_TIR.bin").getPath());
	return table;
}

}// end namespace ph