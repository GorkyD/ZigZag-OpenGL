#pragma once

class TileSpawnSystem;
struct ZigZagContext;

class AutoMoveSystem
{
public:
    explicit AutoMoveSystem(TileSpawnSystem& tileSpawnSystem);

    void Init(ZigZagContext& context);
    void Reset(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);

    void Enable(ZigZagContext& context);

    bool IsEnabled() const
    {
        return enabled;
    }

private:
    TileSpawnSystem& tileSpawnSystem;

    bool enabled = false;
    bool rWasPressed = false;
};
