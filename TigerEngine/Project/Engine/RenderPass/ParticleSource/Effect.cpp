#include "Effect.h"
#include "../Engine/EngineSystem/EffectSystem.h"
#include "../Base/Datas/ReflectionMedtaDatas.hpp"
#include "../Engine/Util/JsonHelper.h"
#include "../Engine/Object/GameObject.h"

#include <algorithm>
#include <string>

namespace
{
    nlohmann::json Vec2ToJson(const Vector2& v)
    {
        return nlohmann::json::array({ v.x, v.y });
    }

    nlohmann::json Vec3ToJson(const Vector3& v)
    {
        return nlohmann::json::array({ v.x, v.y, v.z });
    }

    nlohmann::json Vec4ToJson(const Vector4& v)
    {
        return nlohmann::json::array({ v.x, v.y, v.z, v.w });
    }

    void JsonToVec2(const nlohmann::json& j, Vector2& outV)
    {
        if (!j.is_array() || j.size() < 2) return;
        outV.x = j[0].get<float>();
        outV.y = j[1].get<float>();
    }

    void JsonToVec3(const nlohmann::json& j, Vector3& outV)
    {
        if (!j.is_array() || j.size() < 3) return;
        outV.x = j[0].get<float>();
        outV.y = j[1].get<float>();
        outV.z = j[2].get<float>();
    }

    void JsonToVec4(const nlohmann::json& j, Vector4& outV)
    {
        if (!j.is_array() || j.size() < 4) return;
        outV.x = j[0].get<float>();
        outV.y = j[1].get<float>();
        outV.z = j[2].get<float>();
        outV.w = j[3].get<float>();
    }

    const char* BillboardToString(BillboardType v)
    {
        switch (v)
        {
        case BillboardType::ScreenFacing: return "ScreenFacing";
        case BillboardType::YAxis: return "YAxis";
        default: return "ScreenFacing";
        }
    }

    BillboardType BillboardFromJson(const nlohmann::json& j, BillboardType fallback)
    {
        if (j.is_string())
        {
            const std::string s = j.get<std::string>();
            if (s == "ScreenFacing") return BillboardType::ScreenFacing;
            if (s == "YAxis") return BillboardType::YAxis;
        }
        else if (j.is_number_integer())
        {
            const int v = j.get<int>();
            if (v == 0) return BillboardType::ScreenFacing;
            if (v == 1) return BillboardType::YAxis;
        }
        return fallback;
    }

    const char* ParticleModeToString(ParticleMode v)
    {
        switch (v)
        {
        case ParticleMode::Dynamic: return "Dynamic";
        case ParticleMode::Fixed: return "Fixed";
        default: return "Dynamic";
        }
    }

    ParticleMode ParticleModeFromJson(const nlohmann::json& j, ParticleMode fallback)
    {
        if (j.is_string())
        {
            const std::string s = j.get<std::string>();
            if (s == "Dynamic") return ParticleMode::Dynamic;
            if (s == "Fixed") return ParticleMode::Fixed;
        }
        else if (j.is_number_integer())
        {
            const int v = j.get<int>();
            if (v == 0) return ParticleMode::Dynamic;
            if (v == 1) return ParticleMode::Fixed;
        }
        return fallback;
    }

    const char* FlipbookPlayModeToString(FlipbookPlayMode v)
    {
        switch (v)
        {
        case FlipbookPlayMode::Once_Then_Die: return "Once_Then_Die";
        case FlipbookPlayMode::Once_Then_Hold: return "Once_Then_Hold";
        case FlipbookPlayMode::Loop: return "Loop";
        default: return "Loop";
        }
    }

    FlipbookPlayMode FlipbookPlayModeFromJson(const nlohmann::json& j, FlipbookPlayMode fallback)
    {
        if (j.is_string())
        {
            const std::string s = j.get<std::string>();
            if (s == "Once_Then_Die") return FlipbookPlayMode::Once_Then_Die;
            if (s == "Once_Then_Hold") return FlipbookPlayMode::Once_Then_Hold;
            if (s == "Loop") return FlipbookPlayMode::Loop;
        }
        else if (j.is_number_integer())
        {
            const int v = j.get<int>();
            if (v == 0) return FlipbookPlayMode::Once_Then_Die;
            if (v == 1) return FlipbookPlayMode::Once_Then_Hold;
            if (v == 2) return FlipbookPlayMode::Loop;
        }
        return fallback;
    }

    const char* VelocityShapeToString(VelocityShape v)
    {
        switch (v)
        {
        case VelocityShape::Sphere: return "Sphere";
        case VelocityShape::Directional: return "Directional";
        case VelocityShape::Cone: return "Cone";
        case VelocityShape::Disk: return "Disk";
        default: return "Sphere";
        }
    }

    VelocityShape VelocityShapeFromJson(const nlohmann::json& j, VelocityShape fallback)
    {
        if (j.is_string())
        {
            const std::string s = j.get<std::string>();
            if (s == "Sphere") return VelocityShape::Sphere;
            if (s == "Directional") return VelocityShape::Directional;
            if (s == "Cone") return VelocityShape::Cone;
            if (s == "Disk") return VelocityShape::Disk;
        }
        else if (j.is_number_integer())
        {
            const int v = j.get<int>();
            if (v == 0) return VelocityShape::Sphere;
            if (v == 1) return VelocityShape::Directional;
            if (v == 2) return VelocityShape::Cone;
            if (v == 3) return VelocityShape::Disk;
        }
        return fallback;
    }

    nlohmann::json SpriteSheetToJson(const SpriteSheet& sheet)
    {
        nlohmann::json j = nlohmann::json::object();
        j["texturePath"] = sheet.texturePath;
        j["cols"] = sheet.cols;
        j["rows"] = sheet.rows;
        j["baseSizeScale"] = sheet.baseSizeScale;
        j["frameCount"] = sheet.frameCount;
        j["fps"] = sheet.fps;
        j["loop"] = sheet.loop;
        return j;
    }

    void JsonToSpriteSheet(const nlohmann::json& j, SpriteSheet& outSheet)
    {
        if (!j.is_object()) return;

        if (j.contains("cols") && j["cols"].is_number_integer())
            outSheet.cols = std::max(1, j["cols"].get<int>());
        if (j.contains("rows") && j["rows"].is_number_integer())
            outSheet.rows = std::max(1, j["rows"].get<int>());
        if (j.contains("baseSizeScale") && j["baseSizeScale"].is_number())
            outSheet.baseSizeScale = std::max(0.001f, j["baseSizeScale"].get<float>());
        if (j.contains("frameCount") && j["frameCount"].is_number_integer())
            outSheet.frameCount = std::max(1, j["frameCount"].get<int>());
        if (j.contains("fps") && j["fps"].is_number())
            outSheet.fps = std::max(0.0f, j["fps"].get<float>());
        if (j.contains("loop") && j["loop"].is_boolean())
            outSheet.loop = j["loop"].get<bool>();

        if (j.contains("texturePath") && j["texturePath"].is_string())
        {
            const std::string path = j["texturePath"].get<std::string>();
            if (!path.empty())
            {
                outSheet.SetPath(path);
            }
            else
            {
                outSheet.texturePath.clear();
                outSheet.resource.reset();
            }
        }

        // clamp to grid size
        const int maxFrames = std::max(1, outSheet.cols * outSheet.rows);
        outSheet.frameCount = std::clamp(outSheet.frameCount, 1, maxFrames);
    }

    nlohmann::json SpawnDynamicToJson(const SpawnDynamic& d)
    {
        nlohmann::json j = nlohmann::json::object();
        j["lifeMin"] = d.lifeMin;
        j["lifeMax"] = d.lifeMax;
        j["rotationMin"] = d.rotationMin;
        j["rotationMax"] = d.rotationMax;
        j["sizeMin"] = Vec2ToJson(d.sizeMin);
        j["sizeMax"] = Vec2ToJson(d.sizeMax);
        j["speedMin"] = d.speedMin;
        j["speedMax"] = d.speedMax;
        j["angularMin"] = d.angularMin;
        j["angularMax"] = d.angularMax;
        j["colorMin"] = Vec4ToJson(d.colorMin);
        j["colorMax"] = Vec4ToJson(d.colorMax);
        return j;
    }

    void JsonToSpawnDynamic(const nlohmann::json& j, SpawnDynamic& outD)
    {
        if (!j.is_object()) return;

        if (j.contains("lifeMin") && j["lifeMin"].is_number()) outD.lifeMin = j["lifeMin"].get<float>();
        if (j.contains("lifeMax") && j["lifeMax"].is_number()) outD.lifeMax = j["lifeMax"].get<float>();
        if (j.contains("rotationMin") && j["rotationMin"].is_number()) outD.rotationMin = j["rotationMin"].get<float>();
        if (j.contains("rotationMax") && j["rotationMax"].is_number()) outD.rotationMax = j["rotationMax"].get<float>();
        if (j.contains("sizeMin")) JsonToVec2(j["sizeMin"], outD.sizeMin);
        if (j.contains("sizeMax")) JsonToVec2(j["sizeMax"], outD.sizeMax);
        if (j.contains("speedMin") && j["speedMin"].is_number()) outD.speedMin = j["speedMin"].get<float>();
        if (j.contains("speedMax") && j["speedMax"].is_number()) outD.speedMax = j["speedMax"].get<float>();
        if (j.contains("angularMin") && j["angularMin"].is_number()) outD.angularMin = j["angularMin"].get<float>();
        if (j.contains("angularMax") && j["angularMax"].is_number()) outD.angularMax = j["angularMax"].get<float>();
        if (j.contains("colorMin")) JsonToVec4(j["colorMin"], outD.colorMin);
        if (j.contains("colorMax")) JsonToVec4(j["colorMax"], outD.colorMax);

        // basic sanity
        outD.lifeMax = std::max(outD.lifeMin, outD.lifeMax);
        outD.rotationMax = std::max(outD.rotationMin, outD.rotationMax);
        outD.sizeMax.x = std::max(outD.sizeMin.x, outD.sizeMax.x);
        outD.sizeMax.y = std::max(outD.sizeMin.y, outD.sizeMax.y);
        outD.speedMax = std::max(outD.speedMin, outD.speedMax);
        outD.angularMax = std::max(outD.angularMin, outD.angularMax);
    }

    nlohmann::json SpawnFixedToJson(const SpawnFixed& f)
    {
        nlohmann::json j = nlohmann::json::object();
        j["rotation"] = f.rotation;
        j["size"] = Vec2ToJson(f.size);
        j["startColor"] = Vec4ToJson(f.startColor);
        return j;
    }

    void JsonToSpawnFixed(const nlohmann::json& j, SpawnFixed& outF)
    {
        if (!j.is_object()) return;

        if (j.contains("rotation") && j["rotation"].is_number()) outF.rotation = j["rotation"].get<float>();
        if (j.contains("size")) JsonToVec2(j["size"], outF.size);
        if (j.contains("startColor")) JsonToVec4(j["startColor"], outF.startColor);
    }

    nlohmann::json EmitterToJson(const Emitter& em)
    {
        nlohmann::json j = nlohmann::json::object();

        j["sheet"] = SpriteSheetToJson(em.sheet);
        j["billboard"] = BillboardToString(em.billboard);

        j["position"] = Vec3ToJson(em.position);
        j["localOffset"] = Vec3ToJson(em.localOffset);

        j["enabled"] = em.enabled;
        j["playing"] = em.playing;

        j["duration"] = em.duration;
        j["looping"] = em.looping;
        j["emitRate"] = em.emitRate;
        j["burstCount"] = em.burstCount;
        j["maxParticles"] = em.maxParticles;

        j["particleMode"] = ParticleModeToString(em.particleMode);
        j["dynamicData"] = SpawnDynamicToJson(em.dynamicData);
        j["fixedData"] = SpawnFixedToJson(em.fixedData);

        j["velocityShape"] = VelocityShapeToString(em.velocityShape);
        j["emitDir"] = Vec3ToJson(em.emitDir);
        j["coneAngleDeg"] = em.coneAngleDeg;

        j["filpbookPlayMode"] = FlipbookPlayModeToString(em.filpbookPlayMode);
        j["holdTime"] = em.holdTime;
        j["infinite"] = em.infinite;

        return j;
    }

    void JsonToEmitter(const nlohmann::json& j, Emitter& outEm)
    {
        if (!j.is_object()) return;

        if (j.contains("sheet")) JsonToSpriteSheet(j["sheet"], outEm.sheet);
        if (j.contains("billboard")) outEm.billboard = BillboardFromJson(j["billboard"], outEm.billboard);

        if (j.contains("position")) JsonToVec3(j["position"], outEm.position);
        if (j.contains("localOffset")) JsonToVec3(j["localOffset"], outEm.localOffset);

        if (j.contains("enabled") && j["enabled"].is_boolean()) outEm.enabled = j["enabled"].get<bool>();
        if (j.contains("playing") && j["playing"].is_boolean()) outEm.playing = j["playing"].get<bool>();

        if (j.contains("duration") && j["duration"].is_number()) outEm.duration = j["duration"].get<float>();
        if (j.contains("looping") && j["looping"].is_boolean()) outEm.looping = j["looping"].get<bool>();
        if (j.contains("emitRate") && j["emitRate"].is_number()) outEm.emitRate = j["emitRate"].get<float>();
        if (j.contains("burstCount") && j["burstCount"].is_number_integer()) outEm.burstCount = j["burstCount"].get<int>();
        if (j.contains("maxParticles") && j["maxParticles"].is_number_integer()) outEm.maxParticles = j["maxParticles"].get<int>();

        if (j.contains("particleMode")) outEm.particleMode = ParticleModeFromJson(j["particleMode"], outEm.particleMode);
        if (j.contains("dynamicData")) JsonToSpawnDynamic(j["dynamicData"], outEm.dynamicData);
        if (j.contains("fixedData")) JsonToSpawnFixed(j["fixedData"], outEm.fixedData);

        if (j.contains("velocityShape")) outEm.velocityShape = VelocityShapeFromJson(j["velocityShape"], outEm.velocityShape);
        if (j.contains("emitDir")) JsonToVec3(j["emitDir"], outEm.emitDir);
        if (j.contains("coneAngleDeg") && j["coneAngleDeg"].is_number()) outEm.coneAngleDeg = j["coneAngleDeg"].get<float>();

        if (j.contains("filpbookPlayMode")) outEm.filpbookPlayMode = FlipbookPlayModeFromJson(j["filpbookPlayMode"], outEm.filpbookPlayMode);
        if (j.contains("holdTime") && j["holdTime"].is_number()) outEm.holdTime = j["holdTime"].get<float>();
        if (j.contains("infinite") && j["infinite"].is_number()) outEm.infinite = j["infinite"].get<float>();

        // sanity
        outEm.maxParticles = std::max(1, outEm.maxParticles);
        outEm.burstCount = std::max(0, outEm.burstCount);
        outEm.emitRate = std::max(0.0f, outEm.emitRate);
        outEm.duration = std::max(0.0f, outEm.duration);
        outEm.coneAngleDeg = std::clamp(outEm.coneAngleDeg, 0.0f, 180.0f);
        outEm.holdTime = std::max(0.0f, outEm.holdTime);
    }
}

RTTR_REGISTRATION
{
    using namespace rttr;

    registration::class_<Effect>("Effect")
        .constructor<>()(policy::ctor::as_raw_ptr) // 엔진 스타일에 맞게 as_object/as_raw_ptr 선택
        // --- properties ---
        .property("emitters", &Effect::emitters)
        .property("allFinished", &Effect::allFinished)

        .property("enabled", &Effect::enabled)
            (metadata(META_BOOL, true))
        .property("playing", &Effect::playing)
            (metadata(META_BOOL, true))
        .property("looping", &Effect::looping)
            (metadata(META_BOOL, true));
}

void Effect::OnInitialize()
{
    transform = GetOwner()->GetTransform(); // find transform
}

void Effect::Play()
{
    playing = true;

    for (auto& e : emitters)
    {
        e.playing = true;
        if (transform)
        {
            auto worldPosition = transform->GetWorldPosition(); // instead vec pos
            e.position = worldPosition + e.localOffset;
        }
        e.elapsed = 0.0f;
        e.emitAcc = 0.0f;
        e.particles.clear();

        // burst
        if (e.burstCount > 0)
            e.Spawn(e.burstCount);
    }
}

void Effect::Stop()
{
    playing = false;
    for (auto& e : emitters) e.playing = false;
}

void Effect::Update()
{
    if (!enabled || !playing) return;

    allFinished = true;

    auto worldPosition = transform->GetWorldPosition(); // instead vec pos

    // emitter udpate
    for (auto& e : emitters)
    {
        e.position = e.localOffset + worldPosition;
        e.Update();

        if (e.playing || !e.particles.empty())
            allFinished = false;
    }

    // loop
    if (allFinished)
    {
        if (looping)
            Play();
        else
            playing = false;
    }
}

void Effect::Enable_Inner()
{
    EffectSystem::Instance().Register(this);
}

void Effect::Disable_Inner()
{
    EffectSystem::Instance().UnRegister(this);
}


nlohmann::json Effect::Serialize()
{
    nlohmann::json data = JsonHelper::MakeSaveData(this);

    if (!data.contains("properties") || !data["properties"].is_object())
        data["properties"] = nlohmann::json::object();

    auto& props = data["properties"];
    props["emitters"] = nlohmann::json::array();
    for (const auto& em : emitters)
    {
        props["emitters"].push_back(EmitterToJson(em));
    }

    return data;
}

void Effect::Deserialize(nlohmann::json data)
{
    JsonHelper::SetDataFromJson(this, data);

    transform = GetOwner() ? GetOwner()->GetTransform() : nullptr;

    if (!data.contains("properties") || !data["properties"].is_object())
        return;

    const auto& props = data["properties"];
    if (props.contains("emitters") && props["emitters"].is_array())
    {
        emitters.clear();
        emitters.reserve(props["emitters"].size());

        for (const auto& jEm : props["emitters"])
        {
            Emitter em{};
            JsonToEmitter(jEm, em);

            // runtime reset (serialize is for config)
            em.elapsed = 0.0f;
            em.emitAcc = 0.0f;
            em.particles.clear();

            emitters.push_back(std::move(em));
        }

        if (transform)
        {
            const Vector3 worldPosition = transform->GetWorldPosition();
            for (auto& em : emitters)
            {
                em.position = worldPosition + em.localOffset;
            }
        }
    }
}
