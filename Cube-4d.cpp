#include "graph.h"

unsigned int keyCode = 0;

// Window processing function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    if (uMessage == WM_KEYDOWN)
    {
        if (wParam == 27) PostQuitMessage(0);
        else keyCode = wParam;
    }
    else if (uMessage == WM_KEYUP) keyCode = 0;
    else if (uMessage == WM_DESTROY) PostQuitMessage(0);

    return DefWindowProc(hwnd, uMessage, wParam, lParam);
}

int main()
{
    // Register the window class
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    WNDCLASS wc = {};
    const wchar_t CLASS_NAME[] = L"myWindow";

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create the window
    const int windowWidth = 400;
    const int windowHeight = 400;
    HWND hwnd = CreateWindow(CLASS_NAME, L"Tesseract (Cube 4d)", WS_OVERLAPPEDWINDOW, 0, 0, windowWidth + 16, windowHeight + 39, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, SW_SHOWNORMAL);
    //ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL); // SW_HIDE or SW_SHOWNORMAL - Hide or Show console
    MSG msg = {};

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Frame
    FRAME frame(windowWidth, windowHeight, hwnd);
    FIGURE_4D figure("figures-4d/cube-4d.dat");

    while (true)
    {
        // Processing window messages
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) break;
        }

        frame.clear({ 0, 0, 0 });
       
        /* Most popular type of rotation: */
        figure.rotate_3d(0, 0.15f, 0, 200, 200, 100);
        figure.rotate_4d(0, 0.15f, 0, 200, 200, 100, 100);

        frame.set_figure(figure);
        frame.print();
    }

    // Clear
    DestroyWindow(hwnd);
    UnregisterClass(CLASS_NAME, hInstance);

    return 0;
}