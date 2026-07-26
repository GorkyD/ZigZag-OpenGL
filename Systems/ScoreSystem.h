#pragma once

class HudService;
struct ZigZagContext;

class ScoreSystem
{
public:
    explicit ScoreSystem(HudService& hudService);

    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

    void AddDistanceScore(ZigZagContext& context, float travelDistance);
    void AddCrystalScore(ZigZagContext& context);
    void CommitGameOver(ZigZagContext& context);

private:
    HudService& hudService;
};
