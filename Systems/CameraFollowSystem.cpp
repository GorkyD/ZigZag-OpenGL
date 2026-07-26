#include "Systems/CameraFollowSystem.h"

#include <cmath>

#include "Context/ZigZagContext.h"
#include "Ecs/Components/CameraComponent.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Math/Vector3.h"

void CameraFollowSystem::Reset(ZigZagContext& context) {}

void CameraFollowSystem::Init(ZigZagContext& context)
{
    Reset(context);

    auto& world = context.engine->GetWorld();

    context.cameraEntity = world.CreateEntity();

    auto& transform = world.AddComponent<TransformComponent>(context.cameraEntity);
    transform.position = {context.cameraOffset.x, context.platformTopY + context.cameraOffset.y, context.cameraOffset.z};

    auto& cam = world.AddComponent<CameraComponent>(context.cameraEntity);
    cam.forward = Vector3::Normalize(-context.cameraOffset);
    cam.nearPlane = 0.05f;
    cam.farPlane = 200.0f;
    cam.orthographic = true;
    cam.orthoSize = 8.0f;

    context.allEntities.push_back(context.cameraEntity);
}

void CameraFollowSystem::Update(ZigZagContext& context, float deltaTime)
{
    if (context.cameraEntity == 0 || context.ballEntity == 0)
        return;

    auto& world = context.engine->GetWorld();
    const auto& ballTransform = world.GetComponent<TransformComponent>(context.ballEntity);
    auto& camTransform = world.GetComponent<TransformComponent>(context.cameraEntity);
    auto& cam = world.GetComponent<CameraComponent>(context.cameraEntity);

    const Vector3 desiredPosition = ballTransform.position + context.cameraOffset;

    constexpr float followSpeed = 6.0f;
    const float followT = 1.0f - std::exp(-followSpeed * deltaTime);
    camTransform.position += (desiredPosition - camTransform.position) * followT;

    cam.forward = Vector3::Normalize(ballTransform.position - camTransform.position);
}
