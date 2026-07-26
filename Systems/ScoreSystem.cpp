#include "Systems/ScoreSystem.h"

#include "Context/ZigZagContext.h"
#include "Engine/Engine.h"
#include "Save/SaveService.h"
#include "Services/HudService.h"

ScoreSystem::ScoreSystem(HudService& hudService) : hudService(hudService) {}

void ScoreSystem::Reset(ZigZagContext& context)
{
    context.score = 0.0f;
}

void ScoreSystem::Init(ZigZagContext& context)
{
    Reset(context);

    if (auto* save = context.engine->GetSaveService())
        context.bestScore = save->GetInt("zigzag_best_score", 0);
}

void ScoreSystem::Update(ZigZagContext& context, float deltaTime)
{
    hudService.SetScoreText(context, static_cast<int>(context.score));
}

void ScoreSystem::AddDistanceScore(ZigZagContext& context, float travelDistance)
{
    context.score += travelDistance * context.distanceScoreRate;
}

void ScoreSystem::AddCrystalScore(ZigZagContext& context)
{
    context.score += context.crystalScoreValue;
}

void ScoreSystem::CommitGameOver(ZigZagContext& context)
{
    const int finalScore = static_cast<int>(context.score);
    if (finalScore > context.bestScore)
    {
        context.bestScore = finalScore;
        if (auto* save = context.engine->GetSaveService())
            save->SetInt("zigzag_best_score", context.bestScore);
    }
}
