#include <CUI/Util.hpp>
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>

#else
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#endif

namespace
{
#ifdef _WIN32
void setRawMode(HANDLE hStdin, DWORD origMode, bool enableRaw)
{
    if (enableRaw)
    {
        DWORD rawMode = origMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hStdin, rawMode);
    }
    else
    {
        SetConsoleMode(hStdin, origMode);
    }
}
#else
void setRawMode(bool enableRaw)
{
    static struct termios origTermios;
    static int origFlags = -1;
    static bool firstRun = true;

    if (firstRun)
    {
        tcgetattr(STDIN_FILENO, &origTermios);
        origFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
        firstRun  = false;
    }

    if (enableRaw)
    {
        struct termios raw = origTermios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        fcntl(STDIN_FILENO, F_SETFL, origFlags | O_NONBLOCK);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
        fcntl(STDIN_FILENO, F_SETFL, origFlags);
    }
}
#endif
} // namespace

void mentor::initTerminal()
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

int32_t mentor::getTerminalWidth()
{
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

namespace
{
#ifdef _WIN32
void applyTerminalMode(mentor::UIInputMode mode)
{
    HANDLE hStdin         = GetStdHandle(STD_INPUT_HANDLE);
    static DWORD origMode = 0;
    static bool firstRun  = true;

    if (firstRun)
    {
        GetConsoleMode(hStdin, &origMode);
        firstRun = false;
    }

    if (mode == mentor::UIInputMode::QuickReact)
    {
        DWORD rawMode = origMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
        SetConsoleMode(hStdin, rawMode);
    }
    else
    {
        SetConsoleMode(hStdin, origMode);
    }
}
#else
void applyTerminalMode(UIInputMode mode)
{
    static struct termios origTermios;
    static int origFlags = -1;
    static bool firstRun = true;

    if (firstRun)
    {
        tcgetattr(STDIN_FILENO, &origTermios);
        origFlags = fcntl(STDIN_FILENO, F_GETFL, 0);
        firstRun  = false;
    }

    if (mode == UIInputMode::QuickReact)
    {
        struct termios raw = origTermios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
        fcntl(STDIN_FILENO, F_SETFL, origFlags | O_NONBLOCK);
    }
    else
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios);
        fcntl(STDIN_FILENO, F_SETFL, origFlags);
    }
}
#endif
} // namespace

std::string mentor::getTerminalInput(UIInputMode mode)
{
    static bool isFirstCall     = true;
    static UIInputMode lastMode = UIInputMode::WaitForEndline;

    if (isFirstCall || mode != lastMode)
    {
        applyTerminalMode(mode);
        lastMode    = mode;
        isFirstCall = false;

        std::cout << std::flush;
    }

    if (mode == UIInputMode::WaitForEndline)
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            return line;
        }
        return "";
    }
    else
    {
#ifdef _WIN32
        if (_kbhit())
        {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0)
            {
                int nextCh = _getch();
                if (nextCh == 72)
                    return "\033[A";
                if (nextCh == 80)
                    return "\033[B";
                if (nextCh == 75)
                    return "\033[D";
                if (nextCh == 77)
                    return "\033[C";
                return std::string(1, static_cast<char>(nextCh));
            }
            return std::string(1, static_cast<char>(ch));
        }
        return "";
#else
        char ch   = 0;
        ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n > 0)
        {
            if (ch == 0x1B)
            {
                std::string seq;
                seq += ch;
                char nextCh;
                while (read(STDIN_FILENO, &nextCh, 1) > 0)
                    seq += nextCh;
                return seq;
            }
            return std::string(1, ch);
        }
        return "";
#endif
    }
}