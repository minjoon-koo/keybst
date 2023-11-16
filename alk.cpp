#include <windows.h>
#include <string>
#include <thread>
#include <atomic>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
HWND hTextBar;
atomic<bool> loop(false);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc;
    HWND hWnd;
    MSG msg;

    ZeroMemory(&wc, sizeof(WNDCLASS));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = L"WindowClass";

    RegisterClass(&wc);

    hWnd = CreateWindow(wc.lpszClassName, L"Boost Key", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 300, 200, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 타이머를 설정하여 10ms마다 WM_TIMER 메시지를 발생시킴
    SetTimer(hWnd, 1, 10, NULL);

    HWND hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 80, 150, 25, hWnd, (HMENU)3, NULL, NULL);
    
    // 키보드 후킹 설정
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
    if (!hHook) {
        MessageBox(NULL, L"Failed to set keyboard hook!", L"Error", MB_ICONERROR | MB_OK);
        return 0;
    }


    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 후킹 해제
    if (hHook) {
        UnhookWindowsHookEx(hHook);
    }


    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBar;
    static atomic<bool> loop(false);
    static thread t;
    static bool onMode = false; // ON/OFF 모드 상태
    static bool isBoostEnabled = false;

    switch (message) {
    case WM_CREATE:
        hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 80, 150, 25, hWnd, (HMENU)3, NULL, NULL);
        break;
    case WM_TIMER:
        if (true /*isBoostEnabled  && onMode*/) { // ON/OFF 모드일 때만 작동
            if (GetAsyncKeyState('1') & 0x8000) {
                keybd_event(49, 0, 0, 0);
                keybd_event(49, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('1', MapVirtualKey('1', 0), 0, 0);
            }
            if (GetAsyncKeyState('2') & 0x8000) {
                keybd_event(50, 0, 0, 0);
                keybd_event(50, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('2', MapVirtualKey('2', 0), 0, 0);
            }
            if (GetAsyncKeyState('3') & 0x8000) {
                keybd_event(51, 0, 0, 0);
                keybd_event(51, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('3', MapVirtualKey('3', 0), 0, 0);
            }
            if (GetAsyncKeyState('4') & 0x8000) {
                keybd_event(52, 0, 0, 0);
                keybd_event(52, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('4', MapVirtualKey('4', 0), 0, 0);
            }
            if (GetAsyncKeyState('5') & 0x8000) {
                keybd_event(53, 0, 0, 0);
                keybd_event(53, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('5', MapVirtualKey('5', 0), 0, 0);
            }
            if (GetAsyncKeyState('6') & 0x8000) {
                keybd_event(54, 0, 0, 0);
                keybd_event(54, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('6', MapVirtualKey('6', 0), 0, 0);
            }
            if (GetAsyncKeyState('7') & 0x8000) {
                keybd_event(55, 0, 0, 0);
                keybd_event(55, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('7', MapVirtualKey('7', 0), 0, 0);
            }
            if (GetAsyncKeyState('8') & 0x8000) {
                keybd_event(56, 0, 0, 0);
                keybd_event(56, 0, KEYEVENTF_KEYUP, 0);
                keybd_event('8', MapVirtualKey('8', 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F1) & 0x8000) {
                keybd_event(VK_F1, 0, 0, 0);
                keybd_event(VK_F1, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F2) & 0x8000) {
                keybd_event(VK_F2, 0, 0, 0);
                keybd_event(VK_F2, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F2, MapVirtualKey(VK_F2, 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F3) & 0x8000) {
                keybd_event(VK_F3, 0, 0, 0);
                keybd_event(VK_F3, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F3, MapVirtualKey(VK_F3, 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F4) & 0x8000) {
                keybd_event(VK_F4, 0, 0, 0);
                keybd_event(VK_F4, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F4, MapVirtualKey(VK_F4, 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F5) & 0x8000) {
                keybd_event(VK_F5, 0, 0, 0);
                keybd_event(VK_F5, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F5, MapVirtualKey(VK_F5, 0), 0, 0);
            }
            if (GetAsyncKeyState(VK_F6) & 0x8000) {
                keybd_event(VK_F6, 0, 0, 0);
                keybd_event(VK_F6, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_F6, MapVirtualKey(VK_F6, 0), 0, 0);
            }

            if (GetAsyncKeyState(VK_OEM_3) & 0x8000) {
                keybd_event(192, 0, 0, 0);
                keybd_event(192, 0, KEYEVENTF_KEYUP, 0);
                keybd_event(VK_OEM_3, MapVirtualKey(VK_OEM_3, 0), 0, 0);
            }
            //Sleep(1);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 2:
            if (loop.load()) {
                loop = false;
                t.join();
                SetWindowText(hTextBar, L"부스트 모드 종료");
            }
            break;

        }
        break;

        /*
    case WM_KEYDOWN:
        if (wParam == VK_F12) { // F12키를 눌렀을 때 ON/OFF 모드 전환
            onMode = !onMode;
            if (onMode) {
                SetWindowText(hTextBar, L"부스트 모드 ON");
            }
            else {
                SetWindowText(hTextBar, L"부스트 모드 OFF");
            }
        }
        break;
        */

    case WM_DESTROY:
        if (loop.load()) {
            loop = false;
            t.join();
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (pKey->vkCode == VK_F12) { // F12키가 눌렸을 때
            HWND hWnd = FindWindow(L"WindowClass", L"Boost Key"); // 윈도우 핸들 찾기
            if (hWnd) { // 윈도우 핸들이 있으면 메시지 전송
                SendMessage(hWnd, WM_KEYDOWN, VK_F12, 0);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
