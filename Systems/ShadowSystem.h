#pragma once

#include "Extension/Extension.h"

struct ZigZagContext;

class ShadowSystem
{
public:
    void Init(ZigZagContext& context);
    void Update(ZigZagContext& context, float deltaTime);
    void Reset(ZigZagContext& context);

private:
    VertexArrayObjectPtr shadowVao;
    unsigned int shadowIndexCount = 0;
};
