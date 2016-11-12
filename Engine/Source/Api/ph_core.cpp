#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Core/BruteForceRenderer.h"
#include "Core/ImportanceRenderer.h"
#include "World/World.h"
#include "Camera/DefaultCamera.h"
#include "Core/RenderTask.h"

#include <memory>
#include <iostream>

PHint32 phStart()
{
	return PH_TRUE;
}

void phExit()
{
	ph::ApiDatabase::releaseAllData();
}

void phCreateRenderer(PHuint64* out_rendererId, const PHint32 rendererType)
{
	using namespace ph;

	switch(rendererType)
	{
	case PH_BRUTE_FORCE_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<BruteForceRenderer>()));
		break;

	case PH_IMPORTANCE_RENDERER_TYPE:
		*out_rendererId = static_cast<std::size_t>(ApiDatabase::addRenderer(std::make_unique<ImportanceRenderer>()));
		break;

	default:
		std::cerr << "unidentified renderer type at phCreateRenderer()" << std::endl;
	}
}

void phDeleteRenderer(const PHuint64 rendererId)
{
	if(ph::ApiDatabase::removeRenderer(rendererId))
	{
		std::cout << "Renderer<" << rendererId << "> deleted" << std::endl;
	}
	else
	{
		std::cout << "error while deleting Renderer<" << rendererId << ">" << std::endl;
	}
}

void phCreateWorld(PHuint64* out_worldId)
{
	*out_worldId = static_cast<std::size_t>(ph::ApiDatabase::addWorld(std::make_unique<ph::World>()));
}

void phDeleteWorld(const PHuint64 worldId)
{
	if(!ph::ApiDatabase::removeWorld(worldId))
	{
		std::cerr << "error while deleting World<" << worldId << ">" << std::endl;
	}
}

void phCreateCamera(PHuint64* out_cameraId, const PHint32 cameraType, const PHuint32 filmWidthPx, const PHuint32 filmHeightPx)
{
	using namespace ph;

	switch(cameraType)
	{
	case PH_DEFAULT_CAMERA_TYPE:
		*out_cameraId = static_cast<std::size_t>(ApiDatabase::addCamera(std::make_unique<DefaultCamera>(filmWidthPx, filmHeightPx)));
		break;

	default:
		std::cerr << "unidentified Camera type at phCreateCamera()" << std::endl;
	}
}

void phDeleteCamera(const PHuint64 cameraId)
{
	if(!ph::ApiDatabase::removeCamera(cameraId))
	{
		std::cerr << "error while deleting Camera<" << cameraId << ">" << std::endl;
	}
}

void phCreateRenderTask(PHuint64* out_renderTaskId, const PHuint64 worldId, const PHuint64 cameraId, const PHuint64 rendererId, const PHuint64 frameId)
{
	using namespace ph;

	World*    world    = ApiDatabase::getWorld(worldId);
	Camera*   camera   = ApiDatabase::getCamera(cameraId);
	Renderer* renderer = ApiDatabase::getRenderer(rendererId);
	Frame*    frame    = ApiDatabase::getFrame(frameId);

	*out_renderTaskId = static_cast<std::size_t>(ApiDatabase::addRenderTask(std::make_unique<RenderTask>(world, camera, renderer, frame)));
}

void phDeleteRenderTask(const PHuint64 renderTaskId)
{
	if(!ph::ApiDatabase::removeRenderTask(renderTaskId))
	{
		std::cerr << "error while deleting RenderTask<" << renderTaskId << ">" << std::endl;
	}
}

void phRunRenderTask(const PHuint64 renderTaskId)
{
	ph::RenderTask* renderTask = ph::ApiDatabase::getRenderTask(renderTaskId);
	renderTask->run();
}