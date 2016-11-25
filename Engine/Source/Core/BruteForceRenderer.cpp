#include "Core/BruteForceRenderer.h"
#include "Common/primitive_type.h"
#include "Image/Film.h"
#include "World/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Material/Material.h"
#include "Model/Material/SurfaceIntegrand.h"
#include "Math/constant.h"
#include "Core/SampleGenerator.h"
#include "Model/Geometry/Triangle.h"

#include <cmath>
#include <iostream>

namespace ph
{

BruteForceRenderer::~BruteForceRenderer() = default;

void BruteForceRenderer::render(const World& world, const Camera& camera) const
{
	Film* film = camera.getFilm();

	const uint32 widthPx = film->getWidthPx();
	const uint32 heightPx = film->getHeightPx();

	for(uint32 y = 0; y < heightPx; y++)
	{
		for(uint32 x = 0; x < widthPx; x++)
		{
			Vector3f accuRadiance(0, 0, 0);
			const uint32 spp = 64;
			uint32 numSamples = 0;

			while(numSamples < spp)
			{
				Ray ray;
				Intersection intersection;
				const uint32 maxBounces = 5;
				uint32 numBounces = 0;

				//camera.genSampleRay(&ray, widthPx, heightPx, x, y);

				while(numBounces <= maxBounces && world.isIntersecting(ray, &intersection))
				{
					const Model* hitModel = intersection.getHitTriangle()->getParentModel();
					const Material* hitMaterial = hitModel->getMaterial();

					Vector3f L;
					Vector3f N(intersection.getHitNormal());
					Vector3f V(ray.getDirection().mul(-1.0f));

					hitMaterial->getSurfaceIntegrand()->genUniformRandomV(intersection, V, &L);

					if(hitMaterial->getSurfaceIntegrand()->isEmissive())
					{
						Vector3f radiance;
						hitMaterial->getSurfaceIntegrand()->evaluateEmittedRadiance(intersection, L, V, &radiance);

						//ray.addLiRadiance(radiance);
						//ray.calcWeightedLiRadiance(&radiance);
						accuRadiance.addLocal(radiance);

						break;
					}

					Vector3f liWeight;
					Vector3f pdf;
					hitMaterial->getSurfaceIntegrand()->evaluateLiWeight(intersection, L, V, &liWeight);
					hitMaterial->getSurfaceIntegrand()->evaluateUniformRandomVPDF(intersection, L, V, &pdf);

					//ray.accumulateLiWeight(liWeight.div(pdf));

					Vector3f nextRayOrigin(intersection.getHitPosition().add(N.mul(0.0001f)));
					Vector3f nextRayDirection(L);
					ray.setOrigin(nextRayOrigin);
					ray.setDirection(nextRayDirection);

					numBounces++;
				}// end while

				numSamples++;
			}// end while

			//accuRadiance.divLocal(static_cast<float32>(spp));
			//out_frame->setPixel(x, y, accuRadiance.x, accuRadiance.y, accuRadiance.z);
		}
	}
}

void BruteForceRenderer::queryIntermediateFilm(Film* const out_film) const
{
	std::cerr << "warning: BruteForceRenderer::queryIntermediateFilm() not implemented" << std::endl;
}

float32 BruteForceRenderer::queryPercentageProgress() const
{
	return 0.0f;
}

}// end namespace ph