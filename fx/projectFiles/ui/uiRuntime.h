#pragma once

#include "ui.h"
#include "uiRenderer.h"
#include "modalInput.h"

namespace GameUI
{
    // Platform window/input adapter for the independent UI canvas.
    class UIRuntime
    {
        static constexpr int KeyCount = 256;
        static constexpr int ResumeFramesAfterNativeDialog = 2;

    public:
        Canvas canvas;

        bool Initialize(dx11::ID3D11Device* device, dx11::ID3D11DeviceContext* context)
        {
            if (initialized_)
            {
                return true;
            }
            initialized_ = renderer_.Initialize(device, context);
            return initialized_;
        }

        bool HandleMessage(HWND, UINT message, WPARAM key, LPARAM data)
        {
            if (message == WM_KILLFOCUS || (message == WM_ACTIVATEAPP && !key))
            {
                canvas.CancelPointerCapture();
                pointerEvents_.clear();
                for (int i = 0; i < KeyCount; ++i)
                {
                    queuedKeys_[i] = false;
                    pressedKeys_[i] = false;
                }
                active_ = false;
                gate_.Update(canvas.WantsInput(), downKeys_, false);
                gameUiCapturesInput = true;
            }
            if ((message == WM_KEYDOWN || message == WM_SYSKEYDOWN) && key < KeyCount &&
                !(data & (1LL << 30)))
            {
                queuedKeys_[key] = true;
            }
            if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
            {
                pointerEvents_.push_back(
                    {ClientToNdc(static_cast<float>(static_cast<short>(LOWORD(data))),
                                 static_cast<float>(static_cast<short>(HIWORD(data)))),
                     message == WM_LBUTTONDOWN});
            }
            const bool input = (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) ||
                               (message >= WM_KEYFIRST && message <= WM_KEYLAST);
            return input && (gameUiCapturesInput || gameUiEscapeConsumed);
        }

        void BeginFrame(HWND window)
        {
            if (resumeFrames_ > 0)
            {
                --resumeFrames_;
            }
            canvas.Refresh();
            active_ = GetActiveWindow() == window;
            for (int i = 0; i < KeyCount; ++i)
            {
                const bool down = (GetAsyncKeyState(i) & 0x8000) != 0;
                pressedKeys_[i] = active_ && ((down && !downKeys_[i]) || queuedKeys_[i]);
                downKeys_[i] = down;
                queuedKeys_[i] = false;
            }
            POINT point{};
            GetCursorPos(&point);
            ScreenToClient(window, &point);
            pointer_ = ClientToNdc(static_cast<float>(point.x), static_cast<float>(point.y));
        }

        void Update()
        {
            UIInputFrame frame;
            frame.width = static_cast<float>(dx11::width);
            frame.height = static_cast<float>(dx11::height);
            frame.pointer = pointer_;
            frame.pointerDown = downKeys_[VK_LBUTTON];
            frame.active = active_;
            frame.pointerEvents = std::move(pointerEvents_);
            pointerEvents_.clear();
            frame.focusNext = KeyPressed(VK_DOWN) || (KeyPressed(VK_TAB) && !downKeys_[VK_SHIFT]);
            frame.focusPrevious = KeyPressed(VK_UP) || (KeyPressed(VK_TAB) && downKeys_[VK_SHIFT]);
            frame.activate = KeyPressed(VK_RETURN) || KeyPressed(VK_SPACE);
            canvas.Update(frame);
            gameUiCapturesInput =
                gate_.Update(canvas.WantsInput(), downKeys_, active_) || resumeFrames_ > 0;
            gameUiEscapeConsumed = gate_.EscapeConsumed();
            if (active_ && canvas.WantsInput())
            {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }
        }

        void Render(int width, int height)
        {
            if (initialized_)
            {
                renderer_.Render(canvas.DrawList(), width, height);
            }
        }

        void Shutdown()
        {
            renderer_.Shutdown();
            canvas.Clear();
            pointerEvents_.clear();
            for (int i = 0; i < KeyCount; ++i)
            {
                downKeys_[i] = false;
                queuedKeys_[i] = false;
                pressedKeys_[i] = false;
            }
            gate_ = {};
            resumeFrames_ = 0;
            active_ = false;
            gameUiCapturesInput = false;
            gameUiEscapeConsumed = false;
            initialized_ = false;
        }

        bool KeyPressed(int key) const { return key >= 0 && key < KeyCount && pressedKeys_[key]; }
        bool Active() const { return active_; }
        bool Resuming() const { return resumeFrames_ > 0; }
        void ConsumeEscape()
        {
            gate_.ConsumeEscape();
            gameUiEscapeConsumed = true;
        }

        void CaptureNative()
        {
            canvas.CancelPointerCapture();
            gate_.Capture();
            gameUiCapturesInput = true;
        }

        void ReleaseNative()
        {
            gate_.Release();
            resumeFrames_ = ResumeFramesAfterNativeDialog;
            for (int i = 0; i < KeyCount; ++i)
            {
                downKeys_[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
                queuedKeys_[i] = false;
                pressedKeys_[i] = false;
            }
            pointerEvents_.clear();
            if (downKeys_[VK_ESCAPE])
            {
                ConsumeEscape();
            }
        }

    private:
        Renderer renderer_;
        ModalInputGate gate_;
        bool initialized_ = false;
        bool active_ = false;
        bool downKeys_[KeyCount]{};
        bool queuedKeys_[KeyCount]{};
        bool pressedKeys_[KeyCount]{};
        int resumeFrames_ = 0;
        Vec2 pointer_;
        std::vector<PointerEvent> pointerEvents_;

        static Vec2 ClientToNdc(float x, float y)
        {
            return {2.0f * x / (std::max)(dx11::width, 1) - 1.0f,
                    1.0f - 2.0f * y / (std::max)(dx11::height, 1)};
        }
    };

    inline UIRuntime ui;
} // namespace GameUI
