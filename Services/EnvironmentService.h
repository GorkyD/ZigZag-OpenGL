#pragma once

#include "Extension/Extension.h"

struct ZigZagContext;

class EnvironmentService
{
public:
    void Init(ZigZagContext& context);

private:
    VertexArrayObjectPtr platformVao;
    unsigned int platformIndexCount = 0;
};
