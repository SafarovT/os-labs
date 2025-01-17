// ellipseApp.cpp : Определяет точку входа для приложения.
#include "framework.h"
#include "ellipseApp.h"
#include <ctime>
#include <cstdlib>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

struct WindowData
{
    COLORREF ellipseColor = RGB(255, 0, 0);
};
COLORREF GenerateRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

int WINAPI ReportError()
{
    MessageBox(nullptr, L"Failed to register window class!", L"Error", MB_ICONERROR);
    return FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ELLIPSEAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ELLIPSEAPP));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ELLIPSEAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ELLIPSEAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;
   HWND hwnd = CreateWindowEx
   (
       0,
       szTitle,
       L"Ellipse Application",
       WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
       nullptr,
       nullptr,
       hInstance,
       nullptr
   );

   if (!hwnd)
   {
       return ReportError();
   }

   ShowWindow(hwnd, nCmdShow);
   UpdateWindow(hwnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static RECT clientRect;
    switch (msg)
    {
    case WM_CREATE:
    {
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreateMenu();
        HMENU hHelpMenu = CreateMenu();

        AppendMenu(hFileMenu, MF_STRING, 1, L"Exit");
        AppendMenu(hHelpMenu, MF_STRING, 2, L"About");

        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

        WindowData* dataPtr = new WindowData;

        SetMenu(hWnd, hMenu);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)dataPtr);
        std::srand(static_cast<unsigned>(std::time(0)));
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1:
            DestroyWindow(hWnd);
            break;
        case 2:
            MessageBox(hWnd, L"Ellipse Application\nAuthor: Tahir", L"About", MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;
    }
    case WM_SIZE:
    {
        GetClientRect(hWnd, &clientRect);
        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    }
    case WM_LBUTTONDOWN:
    {
        WindowData* dataPtr = reinterpret_cast<WindowData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        HRGN hRegion = CreateEllipticRgnIndirect(&clientRect);
        if (PtInRegion(hRegion, x, y))
        {
            dataPtr->ellipseColor = GenerateRandomColor();
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        WindowData* dataPtr = reinterpret_cast<WindowData*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

        if (dataPtr)
        {
            HBRUSH hBrush = CreateSolidBrush(dataPtr->ellipseColor);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            Ellipse(hdc, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);

            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);
        }

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// клики должны отслеживаться внутри еллипса
// не использовать глобальную перменную
// разобраться как работает поддержание окна
// добавить пункт меню About