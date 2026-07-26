#pragma once

class TileSpawnSystem;
class ScoreSystem;
struct ZigZagContext;

class BallMovementSystem
{
public:
    BallMovementSystem(TileSpawnSystem& tileSpawnSystem, ScoreSystem& scoreSystem);

    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

    void SyncInputEdge(ZigZagContext& context);

    bool HasFallenOff() const
    {
        return fellOff;
    }

private:
    TileSpawnSystem& tileSpawnSystem;
    ScoreSystem& scoreSystem;

    bool spaceWasPressed = false;
    bool fellOff = false;
};
