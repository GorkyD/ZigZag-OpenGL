#pragma once

struct ZigZagContext;

class CameraFollowSystem
{
public:
    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);
};
