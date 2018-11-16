#include "Core/Renderer/PM/PMRenderer.h"
#include "Core/Filmic/SampleFilterFactory.h"
#include "FileIO/SDL/SdlResourcePack.h"
#include "Core/Renderer/PM/TViewpointTracingWork.h"
#include "Core/Renderer/PM/TPhotonMappingWork.h"
#include "Core/Renderer/PM/TPhotonMap.h"
#include "Core/Renderer/PM/VPMRadianceEvaluator.h"
#include "Core/Renderer/PM/FullPhoton.h"
#include "FileIO/SDL/InputPacket.h"
#include "Utility/FixedSizeThreadPool.h"
#include "Utility/concurrent.h"
#include "Common/Logger.h"
#include "Core/Renderer/PM/PPMRadianceEvaluationWork.h"
#include "Core/Renderer/PM/PPMPhoton.h"
#include "Core/Renderer/PM/PPMViewpoint.h"
#include "Utility/Timer.h"

#include <numeric>

namespace ph
{

namespace
{
	const Logger logger(LogSender("PM Renderer"));
}

void PMRenderer::doUpdate(const SdlResourcePack& data)
{
	m_film = std::make_unique<HdrRgbFilm>(getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

	m_scene = &(data.visualWorld.getScene());
	m_camera = data.getCamera().get();
	m_sg = data.getSampleGenerator().get();

	m_statistics.zero();
	m_photonsPerSecond = 0;
	m_isFilmUpdated = false;
}

void PMRenderer::doRender()
{
	if(m_mode == EPMMode::VANILLA)
	{
		logger.log("rendering mode: vanilla photon mapping");

		renderWithVanillaPM();
	}
	else if(m_mode == EPMMode::PROGRESSIVE)
	{
		logger.log("rendering mode: progressive photon mapping");

		renderWithProgressivePM();
	}
	else if(m_mode == EPMMode::STOCHASTIC_PROGRESSIVE)
	{
		logger.log("rendering mode: stochastic progressive photon mapping");

		renderWithStochasticProgressivePM();
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, "unsupported PM mode, renders nothing");
	}
}

void PMRenderer::renderWithVanillaPM()
{
	logger.log("target number of photons: " + std::to_string(m_numPhotons));
	logger.log("size of photon buffer: " + std::to_string(sizeof(FullPhoton) * m_numPhotons / 1024 / 1024) + " MB");
	logger.log("start shooting photons...");

	std::vector<FullPhoton>  photonBuffer(m_numPhotons);
	std::vector<std::size_t> numPhotonPaths(getNumWorkers(), 0);
	parallel_work(m_numPhotons, getNumWorkers(),
		[this, &photonBuffer, &numPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(1);

			TPhotonMappingWork<FullPhoton> photonMappingWork(
				m_scene,
				m_camera,
				sampleGenerator.get(),
				&(photonBuffer[workStart]),
				workEnd - workStart,
				&(numPhotonPaths[workerIdx]));
			photonMappingWork.setPMStatistics(&m_statistics);

			photonMappingWork.work();
		});
	const std::size_t totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), std::size_t(0));

	logger.log("building photon map...");

	TPhotonMap<FullPhoton> photonMap(2, TPhotonCenterCalculator<FullPhoton>());
	photonMap.build(std::move(photonBuffer));

	logger.log("estimating radiance...");

	parallel_work(m_numSamplesPerPixel, getNumWorkers(),
		[this, &photonMap, totalPhotonPaths](
			const std::size_t workerIdx, 
			const std::size_t workStart, 
			const std::size_t workEnd)
		{
			auto sampleGenerator = m_sg->genCopied(workEnd - workStart);
			auto film            = std::make_unique<HdrRgbFilm>(
				getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

			VPMRadianceEvaluator evaluator(
				&photonMap, 
				totalPhotonPaths, 
				film.get(), 
				m_scene);
			evaluator.setPMRenderer(this);
			evaluator.setPMStatistics(&m_statistics);
			evaluator.setKernelRadius(m_kernelRadius);

			TViewpointTracingWork<VPMRadianceEvaluator> radianceEstimator(
				&evaluator,
				m_scene,
				m_camera,
				sampleGenerator.get(),
				getRenderWindowPx());

			radianceEstimator.work();
		});
}

void PMRenderer::renderWithProgressivePM()
{
	logger.log("start gathering viewpoints...");

	std::vector<PPMViewpoint> viewpoints;
	{
		// TODO
		/*auto viewpointSampleGenerator = m_sg->genCopied(m_numSamplesPerPixel);

		TViewpointTracingWork<PPMViewpoint> viewpointWork(
			m_scene, 
			m_camera, 
			viewpointSampleGenerator.get(),
			getRenderWindowPx(),
			m_kernelRadius);

		viewpointWork.work();

		viewpoints = viewpointWork.claimViewpoints();*/
	}
	
	logger.log("size of viewpoint buffer: " + 
		std::to_string(math::byte_to_MB<real>(sizeof(PPMViewpoint) * viewpoints.size())) + " MB");

	const std::size_t numPhotonsPerPass = m_numPhotons;

	logger.log("number of photons per pass: " + std::to_string(numPhotonsPerPass));
	logger.log("size of photon buffer: " 
		+ std::to_string(math::byte_to_MB<real>(sizeof(PPMPhoton) * numPhotonsPerPass)) + " MB");
	logger.log("start accumulating passes...");

	std::mutex resultFilmMutex;
	auto resultFilm = std::make_unique<HdrRgbFilm>(
		getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

	Timer passTimer;
	std::size_t numFinishedPasses = 0;
	std::size_t totalPhotonPaths  = 0;
	while(numFinishedPasses < m_numPasses)
	{
		passTimer.start();
		std::vector<PPMPhoton> photonBuffer(numPhotonsPerPass);

		std::vector<std::size_t> numPhotonPaths(getNumWorkers(), 0);
		parallel_work(numPhotonsPerPass, getNumWorkers(),
			[this, &photonBuffer, &numPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto sampleGenerator = m_sg->genCopied(1);

				TPhotonMappingWork<PPMPhoton> photonMappingWork(
					m_scene,
					m_camera,
					sampleGenerator.get(),
					&(photonBuffer[workStart]),
					workEnd - workStart,
					&(numPhotonPaths[workerIdx]));
				photonMappingWork.setPMStatistics(&m_statistics);

				photonMappingWork.work();
			});
		totalPhotonPaths = std::accumulate(numPhotonPaths.begin(), numPhotonPaths.end(), totalPhotonPaths);

		TPhotonMap<PPMPhoton> photonMap(2, TPhotonCenterCalculator<PPMPhoton>());
		photonMap.build(std::move(photonBuffer));

		parallel_work(viewpoints.size(), getNumWorkers(),
			[this, &photonMap, &viewpoints, &resultFilm, &resultFilmMutex, totalPhotonPaths](
				const std::size_t workerIdx, 
				const std::size_t workStart, 
				const std::size_t workEnd)
			{
				auto film = std::make_unique<HdrRgbFilm>(
					getRenderWidthPx(), getRenderHeightPx(), getRenderWindowPx(), m_filter);

				PPMRadianceEvaluationWork radianceEstimator(
					&photonMap, 
					totalPhotonPaths,
					film.get(),
					&(viewpoints[workStart]),
					workEnd - workStart,
					m_scene);
				radianceEstimator.setPMStatistics(&m_statistics);

				radianceEstimator.work();

				{
					std::lock_guard<std::mutex> lock(resultFilmMutex);

					resultFilm->mergeWith(*film);
				}
			});

		asyncReplaceFilm(*resultFilm);
		resultFilm->clear();

		passTimer.finish();

		const real passTimeMs   = static_cast<real>(passTimer.getDeltaMs());
		const real photonsPerMs = passTimeMs != 0 ? static_cast<real>(numPhotonsPerPass) / passTimeMs : 0;
		m_photonsPerSecond.store(static_cast<std::uint32_t>(photonsPerMs * 1000 + 0.5_r), std::memory_order_relaxed);

		m_statistics.asyncIncrementNumIterations();
		++numFinishedPasses;
	}// end while more pass needed
}

void PMRenderer::renderWithStochasticProgressivePM()
{
	// TODO
}

ERegionStatus PMRenderer::asyncPollUpdatedRegion(Region* const out_region)
{
	PH_ASSERT(out_region);

	if(m_isFilmUpdated.load(std::memory_order_relaxed))
	{
		*out_region = getRenderWindowPx();
		m_isFilmUpdated.store(false, std::memory_order_relaxed);

		return ERegionStatus::UPDATING;
	}
	else
	{
		return ERegionStatus::INVALID;
	}
}

RenderProgress PMRenderer::asyncQueryRenderProgress()
{
	return RenderProgress(
		m_mode != EPMMode::VANILLA ? m_numPasses : m_numSamplesPerPixel, 
		m_statistics.asyncGetNumIterations(), 
		0);
}

void PMRenderer::asyncDevelopRegion(HdrRgbFrame& out_frame, const Region& region, const EAttribute attribute)
{
	std::lock_guard<std::mutex> lock(m_filmMutex);

	m_film->develop(out_frame, region);
}

void PMRenderer::develop(HdrRgbFrame& out_frame, const EAttribute attribute)
{
	m_film->develop(out_frame);
}

AttributeTags PMRenderer::supportedAttributes() const
{
	AttributeTags supported;
	supported.tag(EAttribute::LIGHT_ENERGY);
	return supported;
}

std::string PMRenderer::renderStateName(const RenderState::EType type, const std::size_t index) const
{
	PH_ASSERT_LT(index, RenderState::numStates(type));

	if(type == RenderState::EType::INTEGER)
	{
		switch(index)
		{
		case 0: return m_mode != EPMMode::VANILLA ? "finished passes" : "finished samples";
		case 1: return "traced photons";
		case 2: return "photons/second";
		default: return "";
		}
	}
	else if(type == RenderState::EType::REAL)
	{
		return "";
	}
	else
	{
		return "";
	}
}

RenderState PMRenderer::asyncQueryRenderState()
{
	RenderState state;
	state.setIntegerState(0, m_statistics.asyncGetNumIterations());
	state.setIntegerState(1, m_statistics.asyncGetNumTracedPhotons());
	state.setIntegerState(2, static_cast<RenderState::IntegerState>(m_photonsPerSecond.load(std::memory_order_relaxed)));
	return state;
}

void PMRenderer::asyncMergeFilm(const HdrRgbFilm& srcFilm)
{
	{
		std::lock_guard<std::mutex> lock(m_filmMutex);

		m_film->mergeWith(srcFilm);
	}
	
	m_isFilmUpdated.store(true, std::memory_order_relaxed);
}

void PMRenderer::asyncReplaceFilm(const HdrRgbFilm& srcFilm)
{
	{
		std::lock_guard<std::mutex> lock(m_filmMutex);

		m_film->clear();
		m_film->mergeWith(srcFilm);
	}

	m_isFilmUpdated.store(true, std::memory_order_relaxed);
}

// command interface

PMRenderer::PMRenderer(const InputPacket& packet) : 
	Renderer(packet),
	m_film(),
	m_scene(nullptr),
	m_camera(nullptr),
	m_sg(nullptr),
	m_filter(SampleFilterFactory::createBlackmanHarrisFilter()),

	m_mode(),
	m_numPhotons(),
	m_kernelRadius(),
	m_numPasses(),
	m_numSamplesPerPixel(),

	m_filmMutex(),
	m_statistics()
{
	const std::string& mode = packet.getString("mode", "vanilla");
	if(mode == "vanilla")
	{
		m_mode = EPMMode::VANILLA;
	}
	else if(mode == "progressive")
	{
		m_mode = EPMMode::PROGRESSIVE;
	}
	else if(mode == "stochastic-progressive")
	{
		m_mode = EPMMode::STOCHASTIC_PROGRESSIVE;
	}

	m_numPhotons = packet.getInteger("num-photons", 100000);
	m_kernelRadius = packet.getReal("radius", 0.1_r);
	m_numPasses = packet.getInteger("num-passes", 1);
	m_numSamplesPerPixel = packet.getInteger("num-samples-per-pixel", 4);
}

SdlTypeInfo PMRenderer::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RENDERER, "pm");
}

void PMRenderer::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<PMRenderer>(packet);
	}));
}

}// end namespace ph