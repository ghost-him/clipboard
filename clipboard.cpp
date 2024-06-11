#include <windows.h>
#include <iostream>
#include <codecvt>
#include <string>
#include <sstream>
#include <thread>

// 当前是否停止了
bool isStop = false;
// 总共按下的次数
int pressCount = 0;


void SetStringToClipboard(const std::string& text) {

    // 打开剪切板
    if (!OpenClipboard(nullptr)) {
        std::cerr << "无法打开剪切板" << std::endl;
        return;
    }

    // 清空剪切板
    if (!EmptyClipboard()) {
        std::cerr << "无法清空剪切板" << std::endl;
        CloseClipboard();
        return;
    }

    // 为文本分配全局内存
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, std::size(text) + 1);
    if (hGlobal == nullptr) {
        std::cerr << "无法分配内存" << std::endl;
        CloseClipboard();
        return;
    }

    // 将文本复制到全局内存
    memcpy(GlobalLock(hGlobal), text.data(), std::size(text) + 1);
    GlobalUnlock(hGlobal);

    // 将全局内存放到剪切板上
    if (SetClipboardData(CF_TEXT, hGlobal) == nullptr) {
        std::cerr << "无法设置剪切板数据" << std::endl;
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    // 关闭剪切板
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
                    //按下后开一个线程，过100ms进行更新操作
                    std::thread the([]() {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        std::string clipboardText = GetClipboardText();
                        if (!clipboardText.empty()) {
                            std::cout << "已检测到剪切板的内容，一共复制了：" << ++pressCount << "次" << std::endl;
                            std::stringstream ss(clipboardText);
                            for (auto& i : clipboardText) {
                                // 13:回车键,10:换行键
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

        // 检查是否按下了 Ctrl + Shift + Alt + P
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

int main()
{
    std::cout << "Ciallo～(∠·ω< )⌒★" << std::endl;
    std::cout << "作者：https://github.com/ghost-him" << std::endl;
    std::cout << "开始监听 ctrl + c " << std::endl;
    // 安装全局键盘钩子
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
    HHOOK control = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // 卸载钩子
    UnhookWindowsHookEx(hhkLowLevelKybd);
    UnhookWindowsHookEx(control);
    return 0;
}
