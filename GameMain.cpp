#include <iostream>
#include <memory>
#include "Engine/Engine.h"
#include "ZigZagScene.h"

int main()
{
    try
    {
        Engine engine(EngineMode::Play);
        engine.RegisterScene("ZigZag", [] { return std::make_unique<ZigZagScene>(); }, OPENGLENGINE_GAME_NAME, OPENGLENGINE_GAME_ASSETS);
        engine.LoadScene(std::make_unique<ZigZagScene>(), "ZigZag");
        engine.Run();
    }
    catch (const std::exception& ex)
    {
        std::clog << ex.what() << '\n';
        return 1;
    }

    return 0;
}
