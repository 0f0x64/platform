#pragma once

#include <array>

namespace GameUI
{
    // Prevent modal UI, native file windows and focus changes from leaking held
    // buttons back into gameplay. Key numbers use the Windows virtual-key values,
    // but the gate itself has no platform dependency.
    class ModalInputGate
    {
    public:
        bool Update(bool modalOpen, const bool* keyDown, bool focused = true)
        {
            const bool capturing = modalOpen || captured_;
            justOpened_ = capturing && !previousModal_;
            justClosed_ = !capturing && (previousModal_ || pendingRelease_);
            const bool regainedFocus = focused && !previousFocused_;
            pendingRelease_ = false;

            if (capturing || !focused || justClosed_ || regainedFocus)
            {
                awaitingRelease_ = true;
            }
            // Keep the closing/focus-return frame blocked even when the last
            // physical button was released by the event which closed the modal.
            if (awaitingRelease_ && focused && !capturing && !justClosed_ && !regainedFocus &&
                !AnyActionDown(keyDown))
            {
                awaitingRelease_ = false;
            }

            if (!Down(keyDown, Escape))
            {
                escapeConsumed_ = false;
            }
            else if (capturing || !focused || awaitingRelease_)
            {
                escapeConsumed_ = true;
            }

            blocked_ = capturing || !focused || awaitingRelease_;
            previousModal_ = capturing;
            previousFocused_ = focused;
            return blocked_;
        }

        bool Update(bool modalOpen, const std::array<bool, 256>& keyDown, bool focused = true)
        {
            return Update(modalOpen, keyDown.data(), focused);
        }

        bool Blocked() const { return blocked_; }
        bool EscapeConsumed() const { return escapeConsumed_; }
        bool JustOpened() const { return justOpened_; }
        bool JustClosed() const { return justClosed_; }

        // Native modal windows can run their own message loop between updates.
        // Explicit capture blocks that loop immediately; Release arms the same
        // transition/release guard used when modalOpen becomes false.
        void Capture()
        {
            captured_ = true;
            awaitingRelease_ = true;
            blocked_ = true;
        }

        void Release()
        {
            captured_ = false;
            pendingRelease_ = true;
            awaitingRelease_ = true;
            blocked_ = true;
        }

        // Call from an event handler when Esc closes a UI before the next poll.
        void ConsumeEscape() { escapeConsumed_ = true; }

    private:
        static constexpr int Escape = 27;
        bool blocked_ = false;
        bool captured_ = false;
        bool awaitingRelease_ = false;
        bool pendingRelease_ = false;
        bool escapeConsumed_ = false;
        bool previousModal_ = false;
        bool previousFocused_ = true;
        bool justOpened_ = false;
        bool justClosed_ = false;

        static bool Down(const bool* keys, int index) { return keys && keys[index]; }

        static bool AnyActionDown(const bool* keys)
        {
            // Mouse attack/defend, jump/accept, Esc, interact, respawn and Ctrl.
            constexpr int actions[]{1, 2, 32, Escape, 69, 82, 17};
            for (int key : actions)
            {
                if (Down(keys, key))
                {
                    return true;
                }
            }
            return false;
        }
    };
} // namespace GameUI
