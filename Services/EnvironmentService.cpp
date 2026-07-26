#include "Services/EnvironmentService.h"

#include "Context/ZigZagContext.h"
#include "Ecs/Components/AmbientLightComponent.h"
#include "Ecs/Components/FogComponent.h"
#include "Ecs/Components/LightComponent.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Resource/EntityFactory.h"

void EnvironmentService::Init(ZigZagContext& context)
{
    auto& world = context.engine->GetWorld();
    auto* renderEngine = context.engine->GetRenderEngine();

    const auto ambientEntity = world.CreateEntity();
    auto& ambient = world.AddComponent<AmbientLightComponent>(ambientEntity);
    ambient.color = {0.75f, 0.85f, 1.0f};
    ambient.intensity = 0.75f;
    context.allEntities.push_back(ambientEntity);

    const auto sunEntity = world.CreateEntity();
    auto& sun = world.AddComponent<LightComponent>(sunEntity);
    sun.type = LightType::Directional;
    sun.color = {1.0f, 0.98f, 0.9f};
    sun.direction = {0.35f, 1.0f, -0.55f};
    sun.intensity = 1.8f;
    context.allEntities.push_back(sunEntity);

    const auto fogEntity = world.CreateEntity();
    auto& fog = world.AddComponent<FogComponent>(fogEntity);
    fog.color = {0.65f, 0.85f, 1.0f};
    fog.start = 30.0f;
    fog.end = 80.0f;
    context.allEntities.push_back(fogEntity);

    constexpr float platformHeight = 0.6f;

    const auto platformEntity = EntityFactory::CreateModelEntity(world, renderEngine, "Assets/Models/cube.obj", context.litShader);

    auto& transform = world.GetComponent<TransformComponent>(platformEntity);
    transform.position = {0.0f, context.platformTopY - platformHeight * 0.5f, 0.0f};
    transform.scale = {context.platformHalfSize * 2.0f, platformHeight, context.platformHalfSize * 2.0f};

    auto& material = world.GetComponent<MaterialComponent>(platformEntity);
    material.diffuseTexture = nullptr;
    material.diffuseColor = context.zigZagColor;

    auto& shaderComp = world.GetComponent<ShaderComponent>(platformEntity);
    shaderComp.shaderType = ShaderRenderType::Lit;

    context.allEntities.push_back(platformEntity);
}
