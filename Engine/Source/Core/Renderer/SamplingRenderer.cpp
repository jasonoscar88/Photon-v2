#include "Core/Renderer/SamplingRenderer.h"
#include "Common/primitive_type.h"
#include "Core/Filmic/Film.h"
#include "World/VisualWorld.h"
#include "Core/Camera/Camera.h"
#include "Core/Ray.h"
#include "Math/constant.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "FileIO/Description.h"
#include "Core/Filmic/HdrRgbFilm.h"
#include "Core/Renderer/RenderWork.h"
#include "Core/Renderer/RenderWorker.h"
#include "Core/Renderer/RendererProxy.h"
#include "Common/assertion.h"
#include "Core/Filmic/SampleFilterFactory.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
#include <utility>

namespace ph
{

SamplingRenderer::SamplingRenderer() :
	Renderer(),
	m_scene(nullptr),
	m_sg(nullptr),
	m_integrator(nullptr),
	m_film(nullptr),
	m_camera(nullptr),
	m_numRemainingWorks(0),
	m_numFinishedWorks(0),
	m_workSgs(),
	m_workFilms(),
	m_updatedRegions(),
	m_rendererMutex()
{}

SamplingRenderer::~SamplingRenderer() = default;

AttributeTags SamplingRenderer::supportedAttributes() const
{
	AttributeTags supports;
	supports.tag(EAttribute::LIGHT_ENERGY);
	return supports;
}

void SamplingRenderer::init(const Description& description)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	const uint32 numWorks = m_numThreads;

	clearWorkData();
	m_scene      = &description.visualWorld.getScene();
	m_sg         = description.getSampleGenerator().get();
	m_integrator = description.getIntegrator().get();
	m_film       = description.getFilm().get();
	m_camera     = description.getCamera().get();
	m_numRemainingWorks = numWorks;
	m_numFinishedWorks  = 0;

	m_sg->genSplitted(numWorks, m_workSgs);

	for(uint32 i = 0; i < numWorks; i++)
	{
		m_workFilms.push_back(m_film->genChild(m_film->getEffectiveWindowPx()));
	}
}

bool SamplingRenderer::getNewWork(const uint32 workerId, RenderWork* out_work)
{
	PH_ASSERT(out_work != nullptr);

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_numRemainingWorks == 0)
	{
		return false;
	}

	const uint32 workIndex = m_numRemainingWorks - 1;
	*out_work = RenderWork(m_scene,
	                       m_camera,
	                       m_integrator,
	                       m_workSgs[workIndex].get(),
	                       m_workFilms[workIndex].get());
	m_numRemainingWorks--;

	return true;
}

void SamplingRenderer::submitWork(const uint32 workerId, const RenderWork& work, const bool isUpdating)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	work.film->mergeToParent();
	work.film->clear();

	addUpdatedRegion(work.film->getEffectiveWindowPx(), isUpdating);
}

void SamplingRenderer::clearWorkData()
{
	m_workSgs.clear();
	m_workFilms.clear();
	m_updatedRegions.clear();
}

ERegionStatus SamplingRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region != nullptr);

	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_updatedRegions.empty())
	{
		return ERegionStatus::INVALID;
	}

	*out_region = m_updatedRegions.front().first;
	m_updatedRegions.pop_front();

	if(m_numFinishedWorks != m_workFilms.size())
	{
		return ERegionStatus::UPDATING;
	}
	else
	{
		return ERegionStatus::FINISHED;
	}
}

void SamplingRenderer::asyncDevelopFilmRegion(HdrRgbFrame& out_frame, const Region& region)
{
	std::lock_guard<std::mutex> lock(m_rendererMutex);

	if(m_film != nullptr)
	{
		m_film->develop(out_frame, region);
	}
}

void SamplingRenderer::addUpdatedRegion(const Region& region, const bool isUpdating)
{
	if(!isUpdating)
	{
		m_numFinishedWorks++;
	}

	for(auto& pendingRegion : m_updatedRegions)
	{
		if(pendingRegion.first.equals(region))
		{
			pendingRegion.second = isUpdating;
			return;
		}
	}

	m_updatedRegions.push_back(std::make_pair(region, isUpdating));
}

// command interface

SamplingRenderer::SamplingRenderer(const InputPacket& packet) :

	Renderer(packet),


	m_scene(nullptr),
	m_sg(nullptr),
	m_integrator(nullptr),
	m_film(nullptr),
	m_camera(nullptr),
	m_numRemainingWorks(0),
	m_numFinishedWorks(0),
	m_workSgs(),
	m_workFilms(),
	m_updatedRegions(),
	m_rendererMutex()
	/*m_filter(SampleFilterFactory::createGaussianFilter()),
	m_filmMutex(),

	m_lightEnergy(nullptr)*/
{
	/*const std::string filterName = packet.getString("filter-name", "box");

	if(filterName == "box")
	{
		m_filter = SampleFilterFactory::createBoxFilter();
	}
	else if(filterName == "gaussian")
	{
		m_filter = SampleFilterFactory::createGaussianFilter();
	}
	else if(filterName == "mn")
	{
		m_filter = SampleFilterFactory::createMNFilter();
	}*/
}

SdlTypeInfo SamplingRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "sampling");
}

void SamplingRenderer::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<SamplingRenderer>([](const InputPacket& packet)
	{
		return std::make_unique<SamplingRenderer>(packet);
	});
	cmdRegister.setLoader(loader);
}

}// end namespace ph