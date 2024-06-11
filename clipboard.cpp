#include <windows.h>
#include <iostream>
#include <codecvt>
#include <string>
#include <sstream>
#include <thread>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_APP_ICON 1001
#define ID_TRAY_EXIT 1002

// 当前是否停止了
bool isStop = false;
// 总共按下的次数
int pressCount = 0;
NOTIFYICONDATA nid;
HMENU hMenu;

// 设置剪切板文本内容
void SetStringToClipboard(const std::string& text) {
    if (!OpenClipboard(nullptr)) {
        std::cerr << "无法打开剪切板" << std::endl;
        return;
    }

    if (!EmptyClipboard()) {
        std::cerr << "无法清空剪切板" << std::endl;
        CloseClipboard();
        return;
    }

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (hGlobal == nullptr) {
        std::cerr << "无法分配内存" << std::endl;
        CloseClipboard();
        return;
    }

    memcpy(GlobalLock(hGlobal), text.data(), text.size() + 1);
    GlobalUnlock(hGlobal);

    if (SetClipboardData(CF_TEXT, hGlobal) == nullptr) {
        std::cerr << "无法设置剪切板数据" << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    CloseClipboard();
}

// 获取剪贴板文本内容
std::string GetClipboardText() {
    if (!OpenClipboard(nullptr)) {
        return "";
    }
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }
    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return "";
    }
    std::string text(pszText);
    GlobalUnlock(hData);
    CloseClipboard();
    return text;
}

// 键盘钩子回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN) {
            if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && pKeyboard->vkCode == 'C') {
                if (!isStop) {
                    std::thread the([]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        std::string clipboardText = GetClipboardText();
                        if (!clipboardText.empty()) {
                            std::cout << "已检测到剪切板的内容，一共复制了：" << ++pressCount << "次" << std::endl;
                            std::stringstream ss(clipboardText);
                            for (auto& i : clipboardText) {
                                if (i == 13 || i == 10) {
                                    i = ' ';
                                }
                            }
                            SetStringToClipboard(clipboardText);
                        }
                        else {
                            std::cout << "检测到非文本数据，不做处理" << std::endl;
                        }
                        });
                    the.detach();
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// 键盘钩子回调函数
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_SHIFT) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000) &&
            pKeyboard->vkCode == 'P' && wParam == WM_KEYDOWN) {
            isStop = !isStop;
            if (isStop) std::cout << "暂停程序" << std::endl;
            else std::cout << "启用程序" << std::endl;
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONDOWN) {
            POINT p;
            GetCursorPos(&p);
            SetForegroundWindow(hwnd);
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, p.x, p.y, 0, hwnd, NULL);
        }
        else if (lParam == WM_MOUSEHOVER) {
            std::wstring tooltip = isStop ? L"程序已暂停" : L"程序运行中";
            for (int i = 0; i < tooltip.size(); i++) {
                nid.szTip[i] = tooltip[i];
            }
            Shell_NotifyIcon(NIM_MODIFY, &nid);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_TRAY_EXIT) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"ClipboardModifierClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"ClipboardModifierClass", L"Clipboard Modifier", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, NULL, NULL, hInstance, NULL);

    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"退出");

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    std::wstring str = L"程序运行中";
    for (int i = 0; i < str.size(); i++) {
        nid.szTip[i] = str[i];
    }
    Shell_NotifyIcon(NIM_ADD, &nid);

    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    HHOOK control = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhkLowLevelKybd);
    UnhookWindowsHookEx(control);
    Shell_NotifyIcon(NIM_DELETE, &nid);
    DestroyMenu(hMenu);

    return 0;
}
