#include "Systems/ShadowSystem.h"

#include "Components/ZigZagBallComponent.h"
#include "Context/ZigZagContext.h"
#include "Ecs/Components/MaterialComponent.h"
#include "Ecs/Components/MeshComponent.h"
#include "Ecs/Components/ShaderComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Math/Vector3.h"
#include "Resource/MeshFactory.h"

void ShadowSystem::Reset(ZigZagContext& context) {}

void ShadowSystem::Init(ZigZagContext& context)
{
    Reset(context);

    auto& world = context.engine->GetWorld();

    if (!shadowVao)
        shadowVao = MeshFactory::CreateDisc(context.engine->GetRenderEngine(), shadowIndexCount, 20, context.ballRadius * 1.2f);

    context.ballShadowEntity = world.CreateEntity();

    auto& transform = world.AddComponent<TransformComponent>(context.ballShadowEntity);
    transform.position = {0.0f, context.platformTopY + 0.02f, 0.0f};

    auto& mesh = world.AddComponent<MeshComponent>(context.ballShadowEntity);
    mesh.vao = shadowVao;
    mesh.indexCount = shadowIndexCount;

    auto& material = world.AddComponent<MaterialComponent>(context.ballShadowEntity);
    material.diffuseColor = {0.0f, 0.0f, 0.0f, 0.35f};

    auto& shaderComp = world.AddComponent<ShaderComponent>(context.ballShadowEntity);
    shaderComp.shader = context.unlitShader;
    shaderComp.shaderType = ShaderRenderType::Shadow;

    context.allEntities.push_back(context.ballShadowEntity);
}

void ShadowSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (context.ballShadowEntity == 0 || context.ballEntity == 0)
        return;

    auto& world = context.engine->GetWorld();
    const auto& ball = world.GetComponent<ZigZagBallComponent>(context.ballEntity);
    const auto& ballTransform = world.GetComponent<TransformComponent>(context.ballEntity);
    auto& shadowTransform = world.GetComponent<TransformComponent>(context.ballShadowEntity);

    shadowTransform.scale = ball.alive ? Vector3(1.0f, 1.0f, 1.0f) : Vector3(0.0f, 0.0f, 0.0f);
    shadowTransform.position.x = ballTransform.position.x;
    shadowTransform.position.z = ballTransform.position.z;
}
