/*
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <vector>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
HWND hTextBar;
atomic<bool> loop(false);
map<HWND, bool> buttonStates; // 버튼 상태를 저장하는 맵
map<HWND, int> buttonKeys; // 버튼과 키 매핑 (int로 변경)
vector<int> activeKeys; // 활성화된 키 목록 (int로 변경)


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

    hWnd = CreateWindow(wc.lpszClassName, L"Boost Key", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 400, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 타이머를 설정하여 10ms마다 WM_TIMER 메시지를 발생시킴
    SetTimer(hWnd, 1, 10, NULL);

    // 안내 메시지 추가
    CreateWindow(L"STATIC",
        L"부스트 모드 ON : F9\n부스트 모드 OFF : F10\n*동작안할시 관리자모드로 재시작",
        WS_VISIBLE | WS_CHILD,
        50, 220, 700, 60,  // 높이를 50에서 60으로 변경
        hWnd, NULL, NULL, NULL);

    hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 700, 25, hWnd, (HMENU)3, NULL, NULL);

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

    return (int)msg.wParam;
}

void CreateKeyboardLayout(HWND hWnd) {
    // F1-F8 키
    for (int i = 0; i < 8; ++i) {
        HWND button = CreateWindow(L"BUTTON", (L"F" + to_wstring(i + 1)).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50 + i * 40, 10, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = VK_F1 + i;
    }

    // 숫자 및 특수 키
    HWND button = CreateWindow(L"BUTTON", L"`", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 50, 30, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_OEM_3;

    const wchar_t* numbers = L"1234567890-=";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, numbers[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 50, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = numbers[i];
    }

    // QWERTY 키
    button = CreateWindow(L"BUTTON", L"Tab", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 90, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_TAB;

    const wchar_t* qwerty = L"QWERTYUIOP[]";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, qwerty[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 90, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = qwerty[i];
    }

    // ASDF 키
    button = CreateWindow(L"BUTTON", L"Caps", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 130, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_CAPITAL;

    const wchar_t* asdf = L"ASDFGHJKL;'";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, asdf[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 130, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = asdf[i];
    }

    // ZXCV 키
    const wchar_t* zxcv = L"ZXCVBNM,./";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, zxcv[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 170, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = zxcv[i];
    }
}

void ToggleButtonState(HWND button) {
    buttonStates[button] = !buttonStates[button];
    if (buttonStates[button]) {
        activeKeys.push_back(buttonKeys[button]);
    }
    else {
        activeKeys.erase(remove(activeKeys.begin(), activeKeys.end(), buttonKeys[button]), activeKeys.end());
    }
    InvalidateRect(button, NULL, TRUE); // 버튼을 다시 그리도록 요청
}

void DrawButton(HWND button, DRAWITEMSTRUCT* dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    SetBkMode(hdc, TRANSPARENT);

    if (buttonStates[button]) {
        FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 0, 0))); // 활성화된 버튼의 배경색을 빨간색으로 설정
    }
    else {
        FillRect(hdc, &rc, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); // 비활성화된 버튼의 기본 배경색
    }

    // 텍스트 색상 설정
    SetTextColor(hdc, RGB(0, 0, 0)); // 검은색 텍스트
    wchar_t text[256];
    GetWindowText(button, text, sizeof(text) / sizeof(wchar_t));
    DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBar;
    static atomic<bool> loop(false);
    static thread t;
    static bool onMode = false; // ON/OFF 모드 상태
    static bool isBoostEnabled = false; // 기능 활성화 상태

    switch (message) {
    case WM_CREATE:
        hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 700, 25, hWnd, (HMENU)3, NULL, NULL);
        CreateKeyboardLayout(hWnd);
        break;
    case WM_TIMER:
        if (isBoostEnabled) { // 기능이 활성화된 경우에만 작동
            for (int key : activeKeys) {
                // 0x8000과 0x0001을 모두 체크하여 현재 키가 실제로 눌려있는지 확인
                if ((GetAsyncKeyState(key) & 0x8000) && (GetKeyState(key) & 0x8000)) {
                    keybd_event(key, 0, 0, 0);
                    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
                    keybd_event(key, MapVirtualKey(key, 0), 0, 0);
                }
            }
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            HWND button = (HWND)lParam;
            ToggleButtonState(button);
        }
        break;

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        DrawButton(dis->hwndItem, dis);
        return TRUE;
    }

    case WM_KEYDOWN:
        if (wParam == VK_F9) { // F9 키를 눌렀을 때 기능 활성화
            isBoostEnabled = true;
            SetWindowText(hTextBar, L"부스트 모드 ON");
        }
        else if (wParam == VK_F10) { // F10 키를 눌렀을 때 기능 비활성화
            isBoostEnabled = false;
            SetWindowText(hTextBar, L"부스트 모드 OFF");
        }
        break;

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
        if (pKey->vkCode == VK_F9) { // F9키가 눌렸을 때
            HWND hWnd = FindWindow(L"WindowClass", L"Boost Key"); // 윈도우 핸들 찾기
            if (hWnd) { // 윈도우 핸들이 있으면 메시지 전송
                SendMessage(hWnd, WM_KEYDOWN, VK_F9, 0);
            }
        }
        else if (pKey->vkCode == VK_F10) { // F10키가 눌렸을 때
            HWND hWnd = FindWindow(L"WindowClass", L"Boost Key"); // 윈도우 핸들 찾기
            if (hWnd) { // 윈도우 핸들이 있으면 메시지 전송
                SendMessage(hWnd, WM_KEYDOWN, VK_F10, 0);
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}*//*
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <vector>
#include <unordered_set>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
HWND hTextBar;
atomic<bool> loop(false);
map<HWND, bool> buttonStates; // 버튼 상태를 저장하는 맵
map<HWND, int> buttonKeys; // 버튼과 키 매핑 (int로 변경)
vector<int> activeKeys; // 활성화된 키 목록 (int로 변경)
unordered_set<int> pressedKeys; // 현재 눌린 키를 추적하는 집합
HWND hComboBox; // 콤보 박스 핸들
HWND hComboBoxAscend; // 상승키 콤보 박스 핸들
HWND hComboBoxDive;   // 강습키 콤보 박스 핸들
HWND hComboBoxMacro; // 강습매크로키 콤보 박스 핸들
HWND hComboBoxGlide; // (활강중) 콤보 박스 핸들
vector<wstring> allKeys; // 모든 키 목록을 전역 변수로 선언

void UpdateComboBoxItems(HWND comboBox, const vector<wstring>& items, const unordered_set<wstring>& exclude) {
    SendMessage(comboBox, CB_RESETCONTENT, 0, 0);
    for (const auto& item : items) {
        if (exclude.find(item) == exclude.end()) {
            SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }
    SendMessage(comboBox, CB_SETCURSEL, 0, 0); // 기본 선택값 설정
}

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

    hWnd = CreateWindow(wc.lpszClassName, L"Boost Key", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 400, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 반복 속도 레이블 추가
    CreateWindow(L"STATIC", L"반복 속도:", WS_VISIBLE | WS_CHILD,
        650, 10, 100, 20, hWnd, NULL, hInstance, NULL);

    // 콤보 박스 추가 (위치를 오른쪽 상단으로 조정)
    hComboBox = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        650, 30, 100, 100, hWnd, (HMENU)4, hInstance, NULL);
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"50");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"100");
    SendMessage(hComboBox, CB_SETCURSEL, 1, 0); // 기본 선택값을 50으로 설정

    // 상승키 레이블 추가
    CreateWindow(L"STATIC", L"상승키:", WS_VISIBLE | WS_CHILD,
        650, 60, 100, 20, hWnd, NULL, hInstance, NULL);

    // 상승키 콤보 박스 추가
    hComboBoxAscend = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 80, 100, 200, hWnd, (HMENU)5, hInstance, NULL);
    const wchar_t* keys = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ-=[];',./";
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxAscend, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxAscend, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습키 레이블 추가
    CreateWindow(L"STATIC", L"강습키:", WS_VISIBLE | WS_CHILD,
        650, 110, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습키 콤보 박스 추가
    hComboBoxDive = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 130, 100, 200, hWnd, (HMENU)6, hInstance, NULL);
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxDive, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxDive, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습매크로키 레이블 추가
    CreateWindow(L"STATIC", L"강습매크로키:", WS_VISIBLE | WS_CHILD,
        650, 160, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습매크로키 콤보 박스 추가
    hComboBoxMacro = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 180, 100, 200, hWnd, (HMENU)7, hInstance, NULL);

    // (활강중) 레이블 추가
    CreateWindow(L"STATIC", L"(활강중):", WS_VISIBLE | WS_CHILD,
        650, 210, 100, 20, hWnd, NULL, hInstance, NULL);

    // (활강중) 콤보 박스 추가
    hComboBoxGlide = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 230, 100, 200, hWnd, (HMENU)8, hInstance, NULL);

    // 모든 키 목록 생성
    allKeys = {
        L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z",
        L"-", L"=", L"[", L"]", L";", L"'", L",", L".", L"/",
        L"F1", L"F2", L"F3", L"F4", L"F5", L"F6", L"F7", L"F8",
        L"Up", L"Down", L"Left", L"Right",
        L"Num0", L"Num1", L"Num2", L"Num3", L"Num4", L"Num5", L"Num6", L"Num7", L"Num8", L"Num9"
    };

    // 초기 콤보 박스 설정
    UpdateComboBoxItems(hComboBoxMacro, allKeys, {});
    UpdateComboBoxItems(hComboBoxGlide, allKeys, {});

    // 타이머 설정
    SetTimer(hWnd, 1, 50, NULL); // 초기값은 50ms

    // 안내 메시지 추가
    CreateWindow(L"STATIC",
        L"부스트 모드 ON : F9\n부스트 모드 OFF : F10\n*동작안할시 관리자모드로 재시작",
        WS_VISIBLE | WS_CHILD,
        50, 220, 550, 60,  // 높이를 50에서 60으로 변경
        hWnd, NULL, NULL, NULL);

    hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);

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

    return (int)msg.wParam;
}

void CreateKeyboardLayout(HWND hWnd) {
    // F1-F8 키
    for (int i = 0; i < 8; ++i) {
        HWND button = CreateWindow(L"BUTTON", (L"F" + to_wstring(i + 1)).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50 + i * 40, 10, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = VK_F1 + i;
    }

    // 숫자 및 특수 키
    HWND button = CreateWindow(L"BUTTON", L"`", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 50, 30, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_OEM_3;

    const wchar_t* numbers = L"1234567890-=";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, numbers[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 50, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = numbers[i];
    }

    // QWERTY 키
    button = CreateWindow(L"BUTTON", L"Tab", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 90, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_TAB;

    const wchar_t* qwerty = L"QWERTYUIOP[]";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, qwerty[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 90, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = qwerty[i];
    }

    // ASDF 키
    button = CreateWindow(L"BUTTON", L"Caps", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 130, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_CAPITAL;

    const wchar_t* asdf = L"ASDFGHJKL;'";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, asdf[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 130, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = asdf[i];
    }

    // ZXCV 키
    const wchar_t* zxcv = L"ZXCVBNM,./";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, zxcv[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 170, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = zxcv[i];
    }
}

void ToggleButtonState(HWND button) {
    buttonStates[button] = !buttonStates[button];
    if (buttonStates[button]) {
        activeKeys.push_back(buttonKeys[button]);
    }
    else {
        activeKeys.erase(remove(activeKeys.begin(), activeKeys.end(), buttonKeys[button]), activeKeys.end());
    }
    InvalidateRect(button, NULL, TRUE); // 버튼을 다시 그리도록 요청
}

void DrawButton(HWND button, DRAWITEMSTRUCT* dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    SetBkMode(hdc, TRANSPARENT);

    if (buttonStates[button]) {
        FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 0, 0))); // 활성화된 버튼의 배경색을 빨간색으로 설정
    }
    else {
        FillRect(hdc, &rc, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); // 비활성화된 버튼의 기본 배경색
    }

    // 텍스트 색상 설정
    SetTextColor(hdc, RGB(0, 0, 0)); // 검은색 텍스트
    wchar_t text[256];
    GetWindowText(button, text, sizeof(text) / sizeof(wchar_t));
    DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBar;
    static atomic<bool> loop(false);
    static thread t;
    static bool onMode = false; // ON/OFF 모드 상태
    static bool isBoostEnabled = false; // 기능 활성화 상태

    switch (message) {
    case WM_CREATE:
        hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);
        CreateKeyboardLayout(hWnd);
        break;
    case WM_TIMER:
        if (isBoostEnabled) {
            for (int key : activeKeys) {
                if (pressedKeys.find(key) != pressedKeys.end() && (GetAsyncKeyState(key) & 0x8000)) {
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0); // 키 떼어짐
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                }
            }
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            HWND button = (HWND)lParam;
            ToggleButtonState(button);
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == 4) { // 콤보 박스 변경 감지
            int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            int interval = 50; // 기본값
            switch (index) {
            case 0: interval = 10; break;
            case 1: interval = 50; break;
            case 2: interval = 100; break;
            }
            KillTimer(hWnd, 1); // 기존 타이머 제거
            SetTimer(hWnd, 1, interval, NULL); // 새로운 간격으로 타이머 설정
        }
        break;

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        DrawButton(dis->hwndItem, dis);
        return TRUE;
    }

    case WM_KEYDOWN:
        if (wParam == VK_F9) { // F9 키를 눌렀을 때 기능 활성화
            isBoostEnabled = true;
            SetWindowText(hTextBar, L"부스트 모드 ON");
        }
        else if (wParam == VK_F10) { // F10 키를 눌렀을 때 기능 비활성화
            isBoostEnabled = false;
            SetWindowText(hTextBar, L"부스트 모드 OFF");
        }
        
        break;

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
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            pressedKeys.insert(pKey->vkCode); // 키가 눌렸을 때 집합에 추가
            if (pKey->vkCode == VK_F9) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F9, 0);
                }
            }
            else if (pKey->vkCode == VK_F10) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F10, 0);
                }
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            pressedKeys.erase(pKey->vkCode); // 키가 떼어졌을 때 집합에서 제거
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
*//*
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <vector>
#include <unordered_set>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
HWND hTextBar;
HWND hCheckBox; // 체크박스 핸들
atomic<bool> loop(false);
map<HWND, bool> buttonStates; // 버튼 상태를 저장하는 맵
map<HWND, int> buttonKeys; // 버튼과 키 매핑 (int로 변경)
vector<int> activeKeys; // 활성화된 키 목록 (int로 변경)
unordered_set<int> pressedKeys; // 현재 눌린 키를 추적하는 집합
HWND hComboBox; // 콤보 박스 핸들
HWND hComboBoxAscend; // 상승키 콤보 박스 핸들
HWND hComboBoxDive;   // 강습키 콤보 박스 핸들
HWND hComboBoxMacro; // 강습매크로키 콤보 박스 핸들
HWND hComboBoxGlide; // (활강중) 콤보 박스 핸들
vector<wstring> allKeys; // 모든 키 목록을 전역 변수로 선언
WORD ascendVk, diveVk, macroVk, glideVk; // 가상 키 코드 저장 변수
bool wasMacroKeyPressed = false; // 매크로 키의 이전 상태

WORD GetVirtualKeyCode(const wstring& keyName) {
    if (keyName == L"Up") return VK_UP;
    if (keyName == L"Down") return VK_DOWN;
    if (keyName == L"Left") return VK_LEFT;
    if (keyName == L"Right") return VK_RIGHT;
    if (keyName == L"Num0") return VK_NUMPAD0;
    if (keyName == L"Num1") return VK_NUMPAD1;
    if (keyName == L"Num2") return VK_NUMPAD2;
    if (keyName == L"Num3") return VK_NUMPAD3;
    if (keyName == L"Num4") return VK_NUMPAD4;
    if (keyName == L"Num5") return VK_NUMPAD5;
    if (keyName == L"Num6") return VK_NUMPAD6;
    if (keyName == L"Num7") return VK_NUMPAD7;
    if (keyName == L"Num8") return VK_NUMPAD8;
    if (keyName == L"Num9") return VK_NUMPAD9;
    if (keyName == L"Num .") return VK_DECIMAL;
    if (keyName == L"F1") return VK_F1;
    if (keyName == L"F2") return VK_F2;
    if (keyName == L"F3") return VK_F3;
    if (keyName == L"F4") return VK_F4;
    if (keyName == L"F5") return VK_F5;
    if (keyName == L"F6") return VK_F6;
    if (keyName == L"F7") return VK_F7;
    if (keyName == L"F8") return VK_F8;
    // 알파벳 및 기타 키
    if (keyName.length() == 1) return VkKeyScan(keyName[0]);
    return 0; // 알 수 없는 키
}

void UpdateComboBoxItems(HWND comboBox, const vector<wstring>& items, const unordered_set<wstring>& exclude) {
    SendMessage(comboBox, CB_RESETCONTENT, 0, 0);
    for (const auto& item : items) {
        if (exclude.find(item) == exclude.end()) {
            SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }
    SendMessage(comboBox, CB_SETCURSEL, 0, 0); // 기본 선택값 설정
}

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

    hWnd = CreateWindow(wc.lpszClassName, L"Boost Key", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 400, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 반복 속도 레이블 추가
    CreateWindow(L"STATIC", L"반응 속도:", WS_VISIBLE | WS_CHILD,
        650, 10, 100, 20, hWnd, NULL, hInstance, NULL);

    // 콤보 박스 추가 (위치를 오른쪽 상단으로 조정)
    hComboBox = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        650, 30, 100, 100, hWnd, (HMENU)4, hInstance, NULL);
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"50");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"100");
    SendMessage(hComboBox, CB_SETCURSEL, 1, 0); // 기본 선택값을 50으로 설정

    // 체크박스 추가
    hCheckBox = CreateWindow(L"BUTTON", L"강습매크로 적용", WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
        650, 60, 150, 20, hWnd, (HMENU)9, hInstance, NULL);

    // 상승키 레이블 추가
    CreateWindow(L"STATIC", L"상승키:", WS_VISIBLE | WS_CHILD,
        650, 90, 100, 20, hWnd, NULL, hInstance, NULL);

    // 상승키 콤보 박스 추가
    hComboBoxAscend = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 110, 100, 200, hWnd, (HMENU)5, hInstance, NULL);
    const wchar_t* keys = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ-=[];',./";
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxAscend, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxAscend, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습키 레이블 추가
    CreateWindow(L"STATIC", L"강습키:", WS_VISIBLE | WS_CHILD,
        650, 140, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습키 콤보 박스 추가
    hComboBoxDive = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 160, 100, 200, hWnd, (HMENU)6, hInstance, NULL);
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxDive, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxDive, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습매크로키 레이블 추가
    CreateWindow(L"STATIC", L"강습매크로키:", WS_VISIBLE | WS_CHILD,
        650, 190, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습매크로키 콤보 박스 추가
    hComboBoxMacro = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 210, 100, 200, hWnd, (HMENU)7, hInstance, NULL);

    // (활강중) 레이블 추가
    CreateWindow(L"STATIC", L"(활강중):", WS_VISIBLE | WS_CHILD,
        650, 240, 100, 20, hWnd, NULL, hInstance, NULL);

    // (활강중) 콤보 박스 추가
    hComboBoxGlide = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 260, 100, 200, hWnd, (HMENU)8, hInstance, NULL);

    // 모든 키 목록 생성
    allKeys = {
        L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z",
        L"-", L"=", L"[", L"]", L";", L"'", L",", L".", L"/",
        L"F1", L"F2", L"F3", L"F4", L"F5", L"F6", L"F7", L"F8",
        L"Up", L"Down", L"Left", L"Right",
        L"Num0", L"Num .",L"Num1", L"Num2", L"Num3", L"Num4", L"Num5", L"Num6", L"Num7", L"Num8", L"Num9"
    };

    // 초기 콤보 박스 설정
    UpdateComboBoxItems(hComboBoxMacro, allKeys, {});
    UpdateComboBoxItems(hComboBoxGlide, allKeys, {});

    // 타이머 설정
    SetTimer(hWnd, 1, 50, NULL); // 초기값은 50ms

    // 안내 메시지 추가
    CreateWindow(L"STATIC",
        L"부스트 모드 ON : F9\n부스트 모드 OFF : F10\n*동작안할시 관리자모드로 재시작",
        WS_VISIBLE | WS_CHILD,
        50, 220, 550, 60,  // 높이를 50에서 60으로 변경
        hWnd, NULL, NULL, NULL);

    hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);

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

    return (int)msg.wParam;
}

void CreateKeyboardLayout(HWND hWnd) {
    // F1-F8 키
    for (int i = 0; i < 8; ++i) {
        HWND button = CreateWindow(L"BUTTON", (L"F" + to_wstring(i + 1)).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50 + i * 40, 10, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = VK_F1 + i;
    }

    // 숫자 및 특수 키
    HWND button = CreateWindow(L"BUTTON", L"`", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 50, 30, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_OEM_3;

    const wchar_t* numbers = L"1234567890-=";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, numbers[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 50, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = numbers[i];
    }

    // QWERTY 키
    button = CreateWindow(L"BUTTON", L"Tab", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 90, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_TAB;

    const wchar_t* qwerty = L"QWERTYUIOP[]";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, qwerty[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 90, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = qwerty[i];
    }

    // ASDF 키
    button = CreateWindow(L"BUTTON", L"Caps", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 130, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_CAPITAL;

    const wchar_t* asdf = L"ASDFGHJKL;'";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, asdf[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 130, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = asdf[i];
    }

    // ZXCV 키
    const wchar_t* zxcv = L"ZXCVBNM,./";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, zxcv[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 170, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = zxcv[i];
    }
}

void ToggleButtonState(HWND button) {
    buttonStates[button] = !buttonStates[button];
    if (buttonStates[button]) {
        activeKeys.push_back(buttonKeys[button]);
    }
    else {
        activeKeys.erase(remove(activeKeys.begin(), activeKeys.end(), buttonKeys[button]), activeKeys.end());
    }
    InvalidateRect(button, NULL, TRUE); // 버튼을 다시 그리도록 요청
}

void DrawButton(HWND button, DRAWITEMSTRUCT* dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    SetBkMode(hdc, TRANSPARENT);

    if (buttonStates[button]) {
        FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 0, 0))); // 활성화된 버튼의 배경색을 빨간색으로 설정
    }
    else {
        FillRect(hdc, &rc, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); // 비활성화된 버튼의 기본 배경색
    }

    // 텍스트 색상 설정
    SetTextColor(hdc, RGB(0, 0, 0)); // 검은색 텍스트
    wchar_t text[256];
    GetWindowText(button, text, sizeof(text) / sizeof(wchar_t));
    DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBar;
    static atomic<bool> loop(false);
    static thread t;
    static bool onMode = false; // ON/OFF 모드 상태
    static bool isBoostEnabled = false; // 기능 활성화 상태

    switch (message) {
    case WM_CREATE:
        hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);
        CreateKeyboardLayout(hWnd);
        break;
    case WM_TIMER:
        if (isBoostEnabled) {
            // activeKeys에 있는 키를 반복적으로 입력
            for (int key : activeKeys) {
                if (pressedKeys.find(key) != pressedKeys.end() && (GetAsyncKeyState(key) & 0x8000)) {
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0); // 키 떼어짐
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                }
            }

            // 체크박스 상태 확인
            bool isChecked = (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED);

            // 매크로 키가 눌렸는지 확인
            bool isMacroKeyPressed = (GetAsyncKeyState(macroVk) & 0x8000) != 0;
            bool isGlideKeyPressed = (GetAsyncKeyState(glideVk) & 0x8000) != 0;

            // 체크박스가 체크되어 있을 때만 매크로 동작
            if (isChecked) {
                // 매크로 키가 눌렸고, 이전에 눌리지 않았을 때만 동작
                if (isMacroKeyPressed && !wasMacroKeyPressed) {
                    // 스페이스 두 번, 상승키 두 번, 강습키 두 번
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(100);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(300);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(160);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    

                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);

                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                }

                // (활강중) 키가 눌렸고, 이전에 눌리지 않았을 때만 동작
                if (isGlideKeyPressed && !wasMacroKeyPressed) {
                    // (활강중) 키에 대한 동작 추가
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);

                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                }
            }

            // 현재 상태를 이전 상태로 저장
            wasMacroKeyPressed = isMacroKeyPressed || isGlideKeyPressed;
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            HWND button = (HWND)lParam;
            if (button == hCheckBox) {
                // 체크박스 상태를 토글
                LRESULT state = SendMessage(hCheckBox, BM_GETCHECK, 0, 0);
                SendMessage(hCheckBox, BM_SETCHECK, (state == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED, 0);
            }
            else {
                ToggleButtonState(button);
            }
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == 4) { // 콤보 박스 변경 감지
            int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            int interval = 50; // 기본값
            switch (index) {
            case 0: interval = 10; break;
            case 1: interval = 50; break;
            case 2: interval = 100; break;
            }
            KillTimer(hWnd, 1); // 기존 타이머 제거
            SetTimer(hWnd, 1, interval, NULL); // 새로운 간격으로 타이머 설정
        }
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            int ascendIndex = SendMessage(hComboBoxAscend, CB_GETCURSEL, 0, 0);
            int diveIndex = SendMessage(hComboBoxDive, CB_GETCURSEL, 0, 0);
            int macroIndex = SendMessage(hComboBoxMacro, CB_GETCURSEL, 0, 0);
            int glideIndex = SendMessage(hComboBoxGlide, CB_GETCURSEL, 0, 0);

            wchar_t selectedKey[256];
            SendMessage(hComboBoxAscend, CB_GETLBTEXT, ascendIndex, (LPARAM)selectedKey);
            ascendVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxDive, CB_GETLBTEXT, diveIndex, (LPARAM)selectedKey);
            diveVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxMacro, CB_GETLBTEXT, macroIndex, (LPARAM)selectedKey);
            macroVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxGlide, CB_GETLBTEXT, glideIndex, (LPARAM)selectedKey);
            glideVk = GetVirtualKeyCode(selectedKey);
        }
        break;

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        DrawButton(dis->hwndItem, dis);
        return TRUE;
    }

    case WM_KEYDOWN:
        if (wParam == VK_F9) { // F9 키를 눌렀을 때 기능 활성화
            isBoostEnabled = true;
            SetWindowText(hTextBar, L"부스트 모드 ON");
        }
        else if (wParam == VK_F10) { // F10 키를 눌렀을 때 기능 비활성화
            isBoostEnabled = false;
            SetWindowText(hTextBar, L"부스트 모드 OFF");
        }
        break;

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
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            pressedKeys.insert(pKey->vkCode); // 키가 눌렸을 때 집합에 추가
            if (pKey->vkCode == VK_F9) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F9, 0);
                }
            }
            else if (pKey->vkCode == VK_F10) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F10, 0);
                }
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            pressedKeys.erase(pKey->vkCode); // 키가 떼어졌을 때 집합에서 제거
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
*/
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <vector>
#include <unordered_set>

using namespace std;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK hHook;
HWND hTextBar;
HWND hCheckBox; // 체크박스 핸들
atomic<bool> loop(false);
map<HWND, bool> buttonStates; // 버튼 상태를 저장하는 맵
map<HWND, int> buttonKeys; // 버튼과 키 매핑 (int로 변경)
vector<int> activeKeys; // 활성화된 키 목록 (int로 변경)
unordered_set<int> pressedKeys; // 현재 눌린 키를 추적하는 집합
HWND hComboBox; // 콤보 박스 핸들
HWND hComboBoxAscend; // 상승키 콤보 박스 핸들
HWND hComboBoxDive;   // 강습키 콤보 박스 핸들
HWND hComboBoxMacro; // 강습매크로키 콤보 박스 핸들
HWND hComboBoxGlide; // (활강중) 콤보 박스 핸들
vector<wstring> allKeys; // 모든 키 목록을 전역 변수로 선언
WORD ascendVk, diveVk, macroVk, glideVk; // 가상 키 코드 저장 변수
bool wasMacroKeyPressed = false; // 매크로 키의 이전 상태

WORD GetVirtualKeyCode(const wstring& keyName) {
    if (keyName == L"Up") return VK_UP;
    if (keyName == L"Down") return VK_DOWN;
    if (keyName == L"Left") return VK_LEFT;
    if (keyName == L"Right") return VK_RIGHT;
    if (keyName == L"Num0") return VK_NUMPAD0;
    if (keyName == L"Num1") return VK_NUMPAD1;
    if (keyName == L"Num2") return VK_NUMPAD2;
    if (keyName == L"Num3") return VK_NUMPAD3;
    if (keyName == L"Num4") return VK_NUMPAD4;
    if (keyName == L"Num5") return VK_NUMPAD5;
    if (keyName == L"Num6") return VK_NUMPAD6;
    if (keyName == L"Num7") return VK_NUMPAD7;
    if (keyName == L"Num8") return VK_NUMPAD8;
    if (keyName == L"Num9") return VK_NUMPAD9;
    if (keyName == L"Num .") return VK_DECIMAL;
    if (keyName == L"F1") return VK_F1;
    if (keyName == L"F2") return VK_F2;
    if (keyName == L"F3") return VK_F3;
    if (keyName == L"F4") return VK_F4;
    if (keyName == L"F5") return VK_F5;
    if (keyName == L"F6") return VK_F6;
    if (keyName == L"F7") return VK_F7;
    if (keyName == L"F8") return VK_F8;
    // 알파벳 및 기타 키
    if (keyName.length() == 1) return VkKeyScan(keyName[0]);
    return 0; // 알 수 없는 키
}

void UpdateComboBoxItems(HWND comboBox, const vector<wstring>& items, const unordered_set<wstring>& exclude) {
    SendMessage(comboBox, CB_RESETCONTENT, 0, 0);
    for (const auto& item : items) {
        if (exclude.find(item) == exclude.end()) {
            SendMessage(comboBox, CB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }
    SendMessage(comboBox, CB_SETCURSEL, 0, 0); // 기본 선택값 설정
}

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

    hWnd = CreateWindow(wc.lpszClassName, L"Boost Key", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 400, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 반복 속도 레이블 추가
    CreateWindow(L"STATIC", L"반응 속도:", WS_VISIBLE | WS_CHILD,
        650, 10, 100, 20, hWnd, NULL, hInstance, NULL);

    // 콤보 박스 추가 (위치를 오른쪽 상단으로 조정)
    hComboBox = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        650, 30, 100, 100, hWnd, (HMENU)4, hInstance, NULL);
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"10");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"50");
    SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"100");
    SendMessage(hComboBox, CB_SETCURSEL, 1, 0); // 기본 선택값을 50으로 설정

    // 체크박스 추가
    hCheckBox = CreateWindow(L"BUTTON", L"강습매크로 적용", WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
        650, 60, 150, 20, hWnd, (HMENU)9, hInstance, NULL);

    // 상승키 레이블 추가
    CreateWindow(L"STATIC", L"상승키:", WS_VISIBLE | WS_CHILD,
        650, 90, 100, 20, hWnd, NULL, hInstance, NULL);

    // 상승키 콤보 박스 추가
    hComboBoxAscend = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 110, 100, 200, hWnd, (HMENU)5, hInstance, NULL);
    const wchar_t* keys = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ-=[];',./";
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxAscend, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxAscend, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습키 레이블 추가
    CreateWindow(L"STATIC", L"강습키:", WS_VISIBLE | WS_CHILD,
        650, 140, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습키 콤보 박스 추가
    hComboBoxDive = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 160, 100, 200, hWnd, (HMENU)6, hInstance, NULL);
    for (int i = 0; keys[i] != '\0'; ++i) {
        wchar_t key[2] = { keys[i], '\0' };
        SendMessage(hComboBoxDive, CB_ADDSTRING, 0, (LPARAM)key);
    }
    SendMessage(hComboBoxDive, CB_SETCURSEL, 0, 0); // 기본 선택값 설정

    // 강습매크로키 레이블 추가
    CreateWindow(L"STATIC", L"강습매크로키:", WS_VISIBLE | WS_CHILD,
        650, 190, 100, 20, hWnd, NULL, hInstance, NULL);

    // 강습매크로키 콤보 박스 추가
    hComboBoxMacro = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 210, 100, 200, hWnd, (HMENU)7, hInstance, NULL);

    // (활강중) 레이블 추가
    CreateWindow(L"STATIC", L"(활강중):", WS_VISIBLE | WS_CHILD,
        650, 240, 100, 20, hWnd, NULL, hInstance, NULL);

    // (활강중) 콤보 박스 추가
    hComboBoxGlide = CreateWindow(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
        650, 260, 100, 200, hWnd, (HMENU)8, hInstance, NULL);

    // 모든 키 목록 생성
    allKeys = {
        L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z",
        L"-", L"=", L"[", L"]", L";", L"'", L",", L".", L"/",
        L"F1", L"F2", L"F3", L"F4", L"F5", L"F6", L"F7", L"F8",
        L"Up", L"Down", L"Left", L"Right",
        L"Num0", L"Num .",L"Num1", L"Num2", L"Num3", L"Num4", L"Num5", L"Num6", L"Num7", L"Num8", L"Num9"
    };

    // 초기 콤보 박스 설정
    UpdateComboBoxItems(hComboBoxMacro, allKeys, {});
    UpdateComboBoxItems(hComboBoxGlide, allKeys, {});

    // 타이머 설정
    SetTimer(hWnd, 1, 50, NULL); // 초기값은 50ms

    // 안내 메시지 추가
    CreateWindow(L"STATIC",
        L"부스트 모드 ON : F9\n부스트 모드 OFF : F10\n*동작안할시 관리자모드로 재시작",
        WS_VISIBLE | WS_CHILD,
        50, 220, 550, 60,  // 높이를 50에서 60으로 변경
        hWnd, NULL, NULL, NULL);

    hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);

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

    return (int)msg.wParam;
}

void CreateKeyboardLayout(HWND hWnd) {
    // F1-F8 키
    for (int i = 0; i < 8; ++i) {
        HWND button = CreateWindow(L"BUTTON", (L"F" + to_wstring(i + 1)).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50 + i * 40, 10, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = VK_F1 + i;
    }

    // 숫자 및 특수 키
    HWND button = CreateWindow(L"BUTTON", L"`", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 50, 30, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_OEM_3;

    const wchar_t* numbers = L"1234567890-=";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, numbers[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 50, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = numbers[i];
    }

    // QWERTY 키
    button = CreateWindow(L"BUTTON", L"Tab", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 90, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_TAB;

    const wchar_t* qwerty = L"QWERTYUIOP[]";
    for (int i = 0; i < 12; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, qwerty[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 90, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = qwerty[i];
    }

    // ASDF 키
    button = CreateWindow(L"BUTTON", L"Caps", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 50, 130, 50, 30, hWnd, NULL, NULL, NULL);
    buttonStates[button] = false;
    buttonKeys[button] = VK_CAPITAL;

    const wchar_t* asdf = L"ASDFGHJKL;'";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, asdf[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 110 + i * 40, 130, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = asdf[i];
    }

    // ZXCV 키
    const wchar_t* zxcv = L"ZXCVBNM,./";
    for (int i = 0; i < 10; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, zxcv[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 90 + i * 40, 170, 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = zxcv[i];
    }

    // 방향키 추가 (위치를 두 번째 사진처럼 조정)
    const wchar_t* arrows = L"↑↓←→";
    int arrowKeys[] = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT };
    int arrowPositions[4][2] = { {550, 130}, {550, 170}, {510, 170}, {590, 170} };
    for (int i = 0; i < 4; ++i) {
        HWND button = CreateWindow(L"BUTTON", wstring(1, arrows[i]).c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, arrowPositions[i][0], arrowPositions[i][1], 30, 30, hWnd, NULL, NULL, NULL);
        buttonStates[button] = false;
        buttonKeys[button] = arrowKeys[i];
    }
}

void ToggleButtonState(HWND button) {
    buttonStates[button] = !buttonStates[button];
    if (buttonStates[button]) {
        activeKeys.push_back(buttonKeys[button]);
    }
    else {
        activeKeys.erase(remove(activeKeys.begin(), activeKeys.end(), buttonKeys[button]), activeKeys.end());
    }
    InvalidateRect(button, NULL, TRUE); // 버튼을 다시 그리도록 요청
}

void DrawButton(HWND button, DRAWITEMSTRUCT* dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    SetBkMode(hdc, TRANSPARENT);

    if (buttonStates[button]) {
        FillRect(hdc, &rc, CreateSolidBrush(RGB(255, 0, 0))); // 활성화된 버튼의 배경색을 빨간색으로 설정
    }
    else {
        FillRect(hdc, &rc, CreateSolidBrush(GetSysColor(COLOR_BTNFACE))); // 비활성화된 버튼의 기본 배경색
    }

    // 텍스트 색상 설정
    SetTextColor(hdc, RGB(0, 0, 0)); // 검은색 텍스트
    wchar_t text[256];
    GetWindowText(button, text, sizeof(text) / sizeof(wchar_t));
    DrawText(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hTextBar;
    static atomic<bool> loop(false);
    static thread t;
    static bool onMode = false; // ON/OFF 모드 상태
    static bool isBoostEnabled = false; // 기능 활성화 상태

    switch (message) {
    case WM_CREATE:
        hTextBar = CreateWindow(L"EDIT", L"부스트 모드 준비", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 50, 300, 550, 25, hWnd, (HMENU)3, NULL, NULL);
        CreateKeyboardLayout(hWnd);
        break;
    case WM_TIMER:
        if (isBoostEnabled) {
            // activeKeys에 있는 키를 반복적으로 입력
            for (int key : activeKeys) {
                if (pressedKeys.find(key) != pressedKeys.end() && (GetAsyncKeyState(key) & 0x8000)) {
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0); // 키 떼어짐
                    keybd_event(key, MapVirtualKey(key, MAPVK_VK_TO_VSC), 0, 0); // 키 눌림
                }
            }

            // 체크박스 상태 확인
            bool isChecked = (SendMessage(hCheckBox, BM_GETCHECK, 0, 0) == BST_CHECKED);

            // 매크로 키가 눌렸는지 확인
            bool isMacroKeyPressed = (GetAsyncKeyState(macroVk) & 0x8000) != 0;
            bool isGlideKeyPressed = (GetAsyncKeyState(glideVk) & 0x8000) != 0;

            // 체크박스가 체크되어 있을 때만 매크로 동작
            if (isChecked) {
                // 매크로 키가 눌렸고, 이전에 눌리지 않았을 때만 동작
                if (isMacroKeyPressed && !wasMacroKeyPressed) {
                    // 스페이스 두 번, 상승키 두 번, 강습키 두 번
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(100);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(300);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(160);
                    keybd_event(VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);


                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);

                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                }

                // (활강중) 키가 눌렸고, 이전에 눌리지 않았을 때만 동작
                if (isGlideKeyPressed && !wasMacroKeyPressed) {
                    // (활강중) 키에 대한 동작 추가
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(30);
                    keybd_event(ascendVk, MapVirtualKey(ascendVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);

                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), 0, 0);
                    Sleep(10);
                    keybd_event(diveVk, MapVirtualKey(diveVk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
                }
            }

            // 현재 상태를 이전 상태로 저장
            wasMacroKeyPressed = isMacroKeyPressed || isGlideKeyPressed;
        }
        break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED) {
            HWND button = (HWND)lParam;
            if (button == hCheckBox) {
                // 체크박스 상태를 토글
                LRESULT state = SendMessage(hCheckBox, BM_GETCHECK, 0, 0);
                SendMessage(hCheckBox, BM_SETCHECK, (state == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED, 0);
            }
            else {
                ToggleButtonState(button);
            }
        }
        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == 4) { // 콤보 박스 변경 감지
            int index = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
            int interval = 50; // 기본값
            switch (index) {
            case 0: interval = 10; break;
            case 1: interval = 50; break;
            case 2: interval = 100; break;
            }
            KillTimer(hWnd, 1); // 기존 타이머 제거
            SetTimer(hWnd, 1, interval, NULL); // 새로운 간격으로 타이머 설정
        }
        if (HIWORD(wParam) == CBN_SELCHANGE) {
            int ascendIndex = SendMessage(hComboBoxAscend, CB_GETCURSEL, 0, 0);
            int diveIndex = SendMessage(hComboBoxDive, CB_GETCURSEL, 0, 0);
            int macroIndex = SendMessage(hComboBoxMacro, CB_GETCURSEL, 0, 0);
            int glideIndex = SendMessage(hComboBoxGlide, CB_GETCURSEL, 0, 0);

            wchar_t selectedKey[256];
            SendMessage(hComboBoxAscend, CB_GETLBTEXT, ascendIndex, (LPARAM)selectedKey);
            ascendVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxDive, CB_GETLBTEXT, diveIndex, (LPARAM)selectedKey);
            diveVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxMacro, CB_GETLBTEXT, macroIndex, (LPARAM)selectedKey);
            macroVk = GetVirtualKeyCode(selectedKey);

            SendMessage(hComboBoxGlide, CB_GETLBTEXT, glideIndex, (LPARAM)selectedKey);
            glideVk = GetVirtualKeyCode(selectedKey);
        }
        break;

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        DrawButton(dis->hwndItem, dis);
        return TRUE;
    }

    case WM_KEYDOWN:
        if (wParam == VK_F9) { // F9 키를 눌렀을 때 기능 활성화
            isBoostEnabled = true;
            SetWindowText(hTextBar, L"부스트 모드 ON");
        }
        else if (wParam == VK_F10) { // F10 키를 눌렀을 때 기능 비활성화
            isBoostEnabled = false;
            SetWindowText(hTextBar, L"부스트 모드 OFF");
        }
        break;

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
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            pressedKeys.insert(pKey->vkCode); // 키가 눌렸을 때 집합에 추가
            if (pKey->vkCode == VK_F9) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F9, 0);
                }
            }
            else if (pKey->vkCode == VK_F10) {
                HWND hWnd = FindWindow(L"WindowClass", L"Boost Key");
                if (hWnd) {
                    SendMessage(hWnd, WM_KEYDOWN, VK_F10, 0);
                }
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            pressedKeys.erase(pKey->vkCode); // 키가 떼어졌을 때 집합에서 제거
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}