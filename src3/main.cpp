#include "psugui.hpp"

int main()
{
    PsuGui psuGui;

    // main loop
    while (!psuGui.shouldClose())
    {
        psuGui.startFrame();

        psuGui.render();

        psuGui.endFrameAndRender();
    }

    return 0;
}
