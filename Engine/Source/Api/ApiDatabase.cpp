#include "Api/ApiDatabase.h"
#include "Frame/TFrame.h"
#include "Core/Engine.h"

#include <utility>
#include <iostream>

namespace ph
{

TStableIndexDenseArray<std::unique_ptr<Engine>>& ApiDatabase::ENGINES()
{
	static TStableIndexDenseArray<std::unique_ptr<Engine>> engines;
	return engines;
}

TStableIndexDenseArray<std::unique_ptr<HdrRgbFrame>>& ApiDatabase::FRAMES()
{
	static TStableIndexDenseArray<std::unique_ptr<HdrRgbFrame>> frames;
	return frames;
}

std::mutex& ApiDatabase::MUTEX()
{
	static std::mutex lock;
	return lock;
}

std::size_t ApiDatabase::addEngine(std::unique_ptr<Engine> engine)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return ENGINES().add(std::move(engine));
}

bool ApiDatabase::removeEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return ENGINES().remove(engineId);
}

Engine* ApiDatabase::getEngine(const std::size_t engineId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* engine = ENGINES().get(engineId);
	if(engine == nullptr)
	{
		std::cerr << "Engine<" << engineId << "> does not exist" << std::endl;
		return nullptr;
	}

	return engine->get();
}

std::size_t ApiDatabase::addFrame(std::unique_ptr<HdrRgbFrame> frame)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return FRAMES().add(std::move(frame));
}

bool ApiDatabase::removeFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	return FRAMES().remove(frameId);
}

HdrRgbFrame* ApiDatabase::getFrame(const std::size_t frameId)
{
	std::lock_guard<std::mutex> lock(MUTEX());

	auto* frame = FRAMES().get(frameId);
	if(frame == nullptr)
	{
		std::cerr << "Frame<" << frameId << "> does not exist" << std::endl;
		return nullptr;
	}

	return frame->get();
}

void ApiDatabase::clear()
{
	std::lock_guard<std::mutex> lock(MUTEX());

	ENGINES().removeAll();
	FRAMES().removeAll();
}

}// end namespace ph