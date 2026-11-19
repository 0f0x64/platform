#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace GameUI
{

    struct DialogueChoice
    {
        std::wstring text;
        int nextNode = -1; // -1 closes the conversation.
        std::function<void()> action = {};
    };

    struct DialogueLine
    {
        std::wstring prompt;
        std::vector<DialogueChoice> choices;
    };

    using DialogueTree = std::vector<DialogueLine>;

    class DialogueSession
    {
    public:
        // Keep a snapshot so temporary trees and later edits cannot invalidate a session.
        bool Start(const DialogueTree& tree, int startIndex = 0)
        {
            Close();
            if (startIndex < 0 || static_cast<std::size_t>(startIndex) >= tree.size())
            {
                return false;
            }

            for (const DialogueLine& line : tree)
            {
                for (const DialogueChoice& choice : line.choices)
                {
                    const bool closesDialogue = choice.nextNode == -1;
                    const bool validDestination =
                        choice.nextNode >= 0 &&
                        static_cast<std::size_t>(choice.nextNode) < tree.size();
                    if (!closesDialogue && !validDestination)
                    {
                        return false;
                    }
                }
            }

            tree_ = tree;
            currentIndex_ = startIndex;
            return true;
        }

        bool Choose(std::size_t choiceIndex)
        {
            const DialogueLine* line = Current();
            if (!line || choiceIndex >= line->choices.size())
            {
                return false;
            }

            const int nextNode = line->choices[choiceIndex].nextNode;
            auto action = line->choices[choiceIndex].action;
            if (nextNode == -1)
            {
                Close();
            }
            else
            {
                currentIndex_ = nextNode;
            }
            // Commit the transition first. An action may replace or close this session.
            if (action)
            {
                action();
            }
            return true;
        }

        void Close()
        {
            currentIndex_ = -1;
            tree_.clear();
        }

        bool IsOpen() const { return currentIndex_ >= 0; }
        int CurrentIndex() const { return currentIndex_; }

        const DialogueLine* Current() const
        {
            return IsOpen() ? &tree_[static_cast<std::size_t>(currentIndex_)] : nullptr;
        }

    private:
        DialogueTree tree_;
        int currentIndex_ = -1;
    };

} // namespace GameUI
