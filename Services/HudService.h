#pragma once

#include "Ecs/Core/Entity.h"
#include "Extension/Extension.h"
#include <string>

struct ZigZagContext;

class HudService
{
public:
    void Init(ZigZagContext& context);
    void Reset(ZigZagContext& context);

    void SetScoreText(ZigZagContext& context, int score);
    void ShowGameOver(ZigZagContext& context);

    void ShowStartHint(ZigZagContext& context);
    void ShowPlayingHint(ZigZagContext& context);
    void SetHintBounceOffset(ZigZagContext& context, float offsetY);

private:
    void SetHintText(ZigZagContext& context, const std::string& text);

    Entity gameOverTextEntity = 0;
    Entity bestScoreTextEntity = 0;
    Entity hintTextEntity = 0;
    Entity scoreTextEntity = 0;

    FontPtr titleFont;
    FontPtr bestScoreFont;
    FontPtr hintFont;
    FontPtr scoreFont;

    static constexpr float hintBaseY = 24.0f;
};
