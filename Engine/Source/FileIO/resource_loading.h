#pragma once

#include "FileIO/InputPacket.h"

#include <vector>
#include <string>
#include <memory>

namespace ph
{

class Geometry;
class Texture;
class Material;
class AModel;
class ALight;
class LightSource;

extern std::shared_ptr<Geometry> load_geometry(const InputPacket& packet);
extern std::shared_ptr<Texture> load_texture(const InputPacket& packet);
extern std::shared_ptr<Material> load_material(const InputPacket& packet);
extern std::shared_ptr<LightSource> load_light_source(const InputPacket& packet);

extern std::unique_ptr<AModel> load_actor_model(const InputPacket& packet);
extern std::unique_ptr<ALight> load_actor_light(const InputPacket& packet);

}// end namespace ph