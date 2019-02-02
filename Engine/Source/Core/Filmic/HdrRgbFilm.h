#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/Camera/RadianceSensor.h"
#include "Core/Filmic/TSamplingFilm.h"

#include <vector>

namespace ph
{

/*
	A film that records RGB values internally. Although film samples are
	added as spectral values, they are automatically converted to linear sRGB
	values for storage. 
*/

	// TODO: make film copyable

class HdrRgbFilm : public SpectralSamplingFilm
{
public:
	HdrRgbFilm(
		int64 actualWidthPx, int64 actualHeightPx,
		const SampleFilter& filter);

	HdrRgbFilm(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter& filter);

	void addSample(float64 xPx, float64 yPx, const SpectralStrength& spectrum) override;
	void clear() override;
	std::unique_ptr<SpectralSamplingFilm> genSamplingChild(const TAABB2D<int64>& effectiveWindowPx) override;
	void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow) override;

	void addSample(float64 xPx, float64 yPx, const Vector3R& rgb);
	void mergeWith(const HdrRgbFilm& other);

	// HACK
	void setPixel(float64 xPx, float64 yPx, const SpectralStrength& spectrum);

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override;

	std::vector<RadianceSensor> m_pixelRadianceSensors;

	void resizeRadianceSensorBuffer();
};

}// end namespace ph