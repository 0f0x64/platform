#pragma once

#include <cmath>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace GameUI
{
    struct SubtitleCue
    {
        std::wstring speaker;
        std::wstring text;
        float durationSeconds = 4.0f;
    };

    class SubtitlePlayer
    {
    public:
        bool Start(std::vector<SubtitleCue> cues)
        {
            Stop();
            if (cues.empty())
            {
                return false;
            }
            for (const SubtitleCue& cue : cues)
            {
                if (cue.text.empty() || !std::isfinite(cue.durationSeconds) ||
                    cue.durationSeconds <= 0.0f)
                {
                    return false;
                }
            }
            cues_ = std::move(cues);
            return true;
        }

        void Update(float elapsedSeconds)
        {
            if (!IsPlaying() || !std::isfinite(elapsedSeconds) || elapsedSeconds <= 0.0f)
            {
                return;
            }

            double timeToAdvance = elapsedSeconds;
            while (IsPlaying())
            {
                const double cueTimeRemaining =
                    static_cast<double>(cues_[currentIndex_].durationSeconds) - elapsedInCue_;
                if (timeToAdvance < cueTimeRemaining)
                {
                    elapsedInCue_ += timeToAdvance;
                    return;
                }
                timeToAdvance -= cueTimeRemaining;
                elapsedInCue_ = 0.0;
                ++currentIndex_;
            }
            Stop();
        }

        const SubtitleCue* Current() const { return IsPlaying() ? &cues_[currentIndex_] : nullptr; }

        bool IsPlaying() const { return currentIndex_ < cues_.size(); }

        void Stop()
        {
            cues_.clear();
            currentIndex_ = 0;
            elapsedInCue_ = 0.0;
        }

    private:
        std::vector<SubtitleCue> cues_;
        std::size_t currentIndex_ = 0;
        double elapsedInCue_ = 0.0;
    };
} // namespace GameUI
