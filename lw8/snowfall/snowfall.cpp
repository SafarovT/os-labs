#include "snowfall.h"

struct Snowflake
{
    int x;
    int y;
    int radius;
    int speed;
};

class SnowAnimation
{
public:
    SnowAnimation(int width, int height, int count)
    {
        windowWidth = width;
        windowHeight = height;
        snowflakeCount = count;

        display = XOpenDisplay(nullptr);
        if (!display)
        {
            throw std::runtime_error("Ошибка при открытии дисплея");
        }

        screen = DefaultScreen(display);
        window = XCreateSimpleWindow
        (
            display,
            RootWindow(display, screen),
            0,
            0,
            windowWidth,
            windowHeight,
            1,
            BlackPixel(display, screen),
            WhitePixel(display, screen)
        );
        XSelectInput(display, window, ExposureMask | KeyPressMask);
        XMapWindow(display, window);
        graphicsContext = XCreateGC(display, window, 0, nullptr);

        InitializeSnowflakes();
    }

    ~SnowAnimation()
    {
        XFreeGC(display, graphicsContext);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }

    void InitializeSnowflakes()
    {
        srand(static_cast<unsigned>(time(0)));
        for (int i = 0; i < snowflakeCount; ++i)
        {
            snowflakes.push_back(CreateRandomSnowflake());
        }
    }

    Snowflake CreateRandomSnowflake()
    {
        Snowflake snowflake;
        snowflake.x = rand() % windowWidth;
        snowflake.y = rand() % windowHeight;
        snowflake.radius = rand() % 4 + 1;
        snowflake.speed = rand() % 3 + 1;
        return snowflake;
    }

    void UpdateSnowflakes()
    {
        for (auto& snowflake : snowflakes)
        {
            snowflake.y += snowflake.speed;
            if (snowflake.y > windowHeight)
            {
                snowflake = CreateRandomSnowflake();
                snowflake.y = 0;
            }
        }
    }

    void DrawSnowflakes()
    {
        XClearWindow(display, window);
        for (const auto& snowflake : snowflakes)
        {
            XFillArc
            (
                display,
                window,
                graphicsContext,
                snowflake.x,
                snowflake.y,
                snowflake.radius * 2,
                snowflake.radius * 2,
                0, 360 * 64
            );
        }
    }

    void Run()
    {
        XEvent event;
        while (true)
        {
            if (XPending(display))
            {
                XNextEvent(display, &event);
                if (event.type == KeyPress)
                {
                    break;
                }
            }

            UpdateSnowflakes();
            DrawSnowflakes();
            XFlush(display);
            usleep(3000);
        }
    }

 private:
    Display* display;
    Window window;
    GC graphicsContext;
    int screen;
    std::vector<Snowflake> snowflakes;
    int windowWidth;
    int windowHeight;
    int snowflakeCount;
};

int main(int argc, char* argv[])
{
    try
    {
        SnowAnimation animation(800, 600, 100);
        animation.Run();
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
