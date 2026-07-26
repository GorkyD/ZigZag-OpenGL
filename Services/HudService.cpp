#include "Services/HudService.h"

#include <string>

#include "Context/ZigZagContext.h"
#include "Ecs/Components/TextComponent.h"
#include "Ecs/Core/EcsWorld.h"
#include "Engine/Engine.h"
#include "Math/Rect.h"
#include "Render/Font.h"
#include "Window/Window.h"

void HudService::Reset(ZigZagContext& context)
{
    gameOverTextEntity = 0;
    bestScoreTextEntity = 0;
    hintTextEntity = 0;
    scoreTextEntity = 0;
}

void HudService::Init(ZigZagContext& context)
{
    Reset(context);

    auto& world = context.engine->GetWorld();
    const Rect screen = context.engine->GetWindow()->GetInnerSize();

    if (!titleFont)
        titleFont = Font::LoadFromFile("Assets/Fonts/JetBrainsMono-Regular.ttf", 64.0f);
    if (titleFont)
    {
        gameOverTextEntity = world.CreateEntity();
        auto& text = world.AddComponent<TextComponent>(gameOverTextEntity);
        text.font = titleFont;
        text.text = "GAME OVER";
        text.color = {0.85f, 0.1f, 0.1f, 1.0f};
        text.scale = 1.0f;
        text.visible = false;

        const float textWidth = titleFont->MeasureWidth(text.text, text.scale);
        text.position = {(float)screen.width * 0.5f - textWidth * 0.5f, (float)screen.height * 0.5f - 40.0f};

        context.allEntities.push_back(gameOverTextEntity);
    }

    if (!bestScoreFont)
        bestScoreFont = Font::LoadFromFile("Assets/Fonts/JetBrainsMono-Regular.ttf", 30.0f);
    if (bestScoreFont)
    {
        bestScoreTextEntity = world.CreateEntity();
        auto& text = world.AddComponent<TextComponent>(bestScoreTextEntity);
        text.font = bestScoreFont;
        text.text = "Best: " + std::to_string(context.bestScore);
        text.color = {0.4f, 0.05f, 0.05f, 1.0f};
        text.scale = 1.0f;
        text.visible = false;

        const float textWidth = bestScoreFont->MeasureWidth(text.text, text.scale);
        text.position = {(float)screen.width * 0.5f - textWidth * 0.5f, (float)screen.height * 0.5f + 30.0f};

        context.allEntities.push_back(bestScoreTextEntity);
    }

    if (!hintFont)
        hintFont = Font::LoadFromFile("Assets/Fonts/JetBrainsMono-Regular.ttf", 28.0f);
    if (hintFont)
    {
        hintTextEntity = world.CreateEntity();
        auto& hint = world.AddComponent<TextComponent>(hintTextEntity);
        hint.font = hintFont;
        hint.text = "Press SPACE to start";
        hint.color = {0.05f, 0.1f, 0.3f, 1.0f};
        hint.scale = 0.8f;

        const float hintWidth = hintFont->MeasureWidth(hint.text, hint.scale);
        hint.position = {(float)screen.width * 0.5f - hintWidth * 0.5f, hintBaseY};

        context.allEntities.push_back(hintTextEntity);
    }

    if (!scoreFont)
        scoreFont = Font::LoadFromFile("Assets/Fonts/JetBrainsMono-Regular.ttf", 40.0f);
    if (scoreFont)
    {
        scoreTextEntity = world.CreateEntity();
        auto& scoreText = world.AddComponent<TextComponent>(scoreTextEntity);
        scoreText.font = scoreFont;
        scoreText.text = "Score: 0";
        scoreText.color = {0.05f, 0.1f, 0.3f, 1.0f};
        scoreText.scale = 1.0f;

        const float scoreWidth = scoreFont->MeasureWidth(scoreText.text, scoreText.scale);
        scoreText.position = {(float)screen.width * 0.5f - scoreWidth * 0.5f, 62.0f};

        context.allEntities.push_back(scoreTextEntity);
    }
}

void HudService::SetScoreText(ZigZagContext& context, int score)
{
    if (scoreTextEntity == 0)
        return;

    auto& world = context.engine->GetWorld();
    if (!world.HasComponent<TextComponent>(scoreTextEntity))
        return;

    auto& scoreText = world.GetComponent<TextComponent>(scoreTextEntity);
    const std::string newText = "Score: " + std::to_string(score);
    if (newText == scoreText.text)
        return;

    scoreText.text = newText;
    if (scoreText.font)
    {
        const Rect screen = context.engine->GetWindow()->GetInnerSize();
        const float width = scoreText.font->MeasureWidth(scoreText.text, scoreText.scale);
        scoreText.position.x = (float)screen.width * 0.5f - width * 0.5f;
    }
}

void HudService::ShowGameOver(ZigZagContext& context)
{
    auto& world = context.engine->GetWorld();
    const Rect screen = context.engine->GetWindow()->GetInnerSize();

    if (gameOverTextEntity != 0 && world.HasComponent<TextComponent>(gameOverTextEntity))
        world.GetComponent<TextComponent>(gameOverTextEntity).visible = true;

    if (bestScoreTextEntity != 0 && world.HasComponent<TextComponent>(bestScoreTextEntity))
    {
        auto& bestText = world.GetComponent<TextComponent>(bestScoreTextEntity);
        bestText.text = "Best: " + std::to_string(context.bestScore);
        bestText.visible = true;

        if (bestText.font)
        {
            const float bestWidth = bestText.font->MeasureWidth(bestText.text, bestText.scale);
            bestText.position = {(float)screen.width * 0.5f - bestWidth * 0.5f, (float)screen.height * 0.5f + 30.0f};
        }
    }

    SetHintText(context, "Press SPACE to restart");
}

void HudService::ShowStartHint(ZigZagContext& context)
{
    SetHintText(context, "Press SPACE to start");
}

void HudService::ShowPlayingHint(ZigZagContext& context)
{
    SetHintText(context, "SPACE - turn");
}

void HudService::SetHintBounceOffset(ZigZagContext& context, float offsetY)
{
    if (hintTextEntity == 0)
        return;

    auto& world = context.engine->GetWorld();
    if (!world.HasComponent<TextComponent>(hintTextEntity))
        return;

    world.GetComponent<TextComponent>(hintTextEntity).position.y = hintBaseY + offsetY;
}

void HudService::SetHintText(ZigZagContext& context, const std::string& text)
{
    if (hintTextEntity == 0)
        return;

    auto& world = context.engine->GetWorld();
    if (!world.HasComponent<TextComponent>(hintTextEntity))
        return;

    auto& hint = world.GetComponent<TextComponent>(hintTextEntity);
    hint.text = text;
    hint.visible = true;

    if (hint.font)
    {
        const Rect screen = context.engine->GetWindow()->GetInnerSize();
        const float hintWidth = hint.font->MeasureWidth(hint.text, hint.scale);
        hint.position = {(float)screen.width * 0.5f - hintWidth * 0.5f, hintBaseY};
    }
}
