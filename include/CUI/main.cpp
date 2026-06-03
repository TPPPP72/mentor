#include <CUI/Renderer.hpp>
#include <CUI/Util.hpp>
#include <Engine/TestEngine.hpp>
#include <thread>

int main()
{
    using namespace mentor;
    initTerminal();
    TestEngine e;
    ContentFrame frame = e.getInit();
    Renderer renderer;
    renderer.render(frame);

    std::string temp;
    while (true)
    {
        temp = getTerminalInput(frame.input_mode);

        if (!temp.empty())
        {
            frame = e.feedInput(temp);

            if (frame.action == UIAction::Exit)
                break;

            if (frame.action == UIAction::Clear)
                std::system("cls");

            renderer.render(frame);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
