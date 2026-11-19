#pragma once

#include "uiCore.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace GameUI
{
    using ElementId = std::uint64_t;
    constexpr ElementId NoElement = 0;

    struct UIPointerEvent
    {
        Vec2 position;
        bool down = false;
    };
    using PointerEvent = UIPointerEvent;

    struct UIInputFrame
    {
        float width = 1.0f;
        float height = 1.0f;
        Vec2 pointer;
        bool pointerDown = false;
        bool active = true;
        std::vector<UIPointerEvent> pointerEvents;
        bool focusNext = false;
        bool focusPrevious = false;
        bool activate = false;
    };

    class Canvas;

    class Element
    {
    public:
        Transform2D transform;
        bool visible = true;
        bool enabled = true;
        bool modal = false;
        float opacity = 1.0f;
        bool drawRect = false;
        Color color;
        std::wstring text;
        Color textColor;
        float fontSize = 24.0f; // Reference pixels at viewport height 1080.
        bool useTransformScale = false;
        bool interactable = false;
        std::function<void()> onClick;
        bool isClicked = false;
        bool hovered = false;
        bool pressed = false;
        bool focused = false;

        Element(const Element&) = delete;
        Element& operator=(const Element&) = delete;
        Element(Element&&) = delete;
        Element& operator=(Element&&) = delete;

        Element& AddChild(std::string name = {})
        {
            auto child =
                std::unique_ptr<Element>(new Element((*nextId_)++, std::move(name), this, nextId_));
            children_.push_back(std::move(child));
            return *children_.back();
        }

        void ClearChildren() { children_.clear(); }
        ElementId Id() const { return id_; }
        Element* Parent() const { return parent_; }
        const std::string& Name() const { return name_; }
        const std::vector<std::unique_ptr<Element>>& Children() const { return children_; }

        bool ReparentTo(Element& parent)
        {
            if (!parent_ || nextId_ != parent.nextId_)
            {
                return false;
            }
            for (Element* ancestor = &parent; ancestor; ancestor = ancestor->parent_)
            {
                if (ancestor == this)
                {
                    return false;
                }
            }
            if (parent_ == &parent)
            {
                return true;
            }
            parent.children_.reserve(parent.children_.size() + 1);
            auto& siblings = parent_->children_;
            for (auto it = siblings.begin(); it != siblings.end(); ++it)
            {
                if (it->get() == this)
                {
                    auto owned = std::move(*it);
                    siblings.erase(it);
                    parent_ = &parent;
                    parent.children_.push_back(std::move(owned));
                    return true;
                }
            }
            return false;
        }

    private:
        friend class Canvas;
        Element(ElementId id, std::string name, Element* parent, ElementId* nextId)
            : id_(id), name_(std::move(name)), parent_(parent), nextId_(nextId)
        {
        }

        ElementId id_;
        std::string name_;
        Element* parent_;
        ElementId* nextId_;
        std::vector<std::unique_ptr<Element>> children_;
        WorldTransform2D worldTransform_;
        bool inheritedVisible_ = true;
        bool inheritedEnabled_ = true;
        float inheritedOpacity_ = 1.0f;
    };

    class Canvas
    {
        ElementId nextId_ = 1;

    public:
        Element root{nextId_++, "Root", nullptr, &nextId_};

        Canvas() = default;
        Canvas(const Canvas&) = delete;
        Canvas& operator=(const Canvas&) = delete;
        Canvas(Canvas&&) = delete;
        Canvas& operator=(Canvas&&) = delete;

        Element* Find(ElementId id) const
        {
            if (id == NoElement)
            {
                return nullptr;
            }
            std::vector<Element*> pending{const_cast<Element*>(&root)};
            while (!pending.empty())
            {
                Element* element = pending.back();
                pending.pop_back();
                if (element->id_ == id)
                {
                    return element;
                }
                for (auto it = element->children_.rbegin(); it != element->children_.rend(); ++it)
                {
                    pending.push_back(it->get());
                }
            }
            return nullptr;
        }

        bool Remove(ElementId id)
        {
            Element* element = Find(id);
            if (!element || !element->parent_)
            {
                return false;
            }
            auto& siblings = element->parent_->children_;
            for (auto it = siblings.begin(); it != siblings.end(); ++it)
            {
                if ((*it)->id_ == id)
                {
                    siblings.erase(it);
                    Refresh();
                    return true;
                }
            }
            return false;
        }

        void Clear()
        {
            root.ClearChildren();
            root.transform = {};
            root.visible = true;
            root.enabled = true;
            root.modal = false;
            root.drawRect = false;
            root.interactable = false;
            root.opacity = 1.0f;
            root.color = {};
            root.textColor = {};
            root.text.clear();
            root.fontSize = 24.0f;
            root.useTransformScale = false;
            root.onClick = {};
            root.isClicked = false;
            hoveredId_ = NoElement;
            pressedId_ = NoElement;
            focusedId_ = NoElement;
            awaitingPointerRelease_ = frame_.pointerDown;
            previousPointerDown_ = frame_.pointerDown;
            Refresh();
        }

        void Update(const UIInputFrame& frame)
        {
            frame_ = frame;
            Refresh();
            for (Element* element : drawOrder_)
            {
                element->isClicked = false;
            }
            if (!frame.active)
            {
                hoveredId_ = NoElement;
                pressedId_ = NoElement;
                previousPointerDown_ = frame.pointerDown;
                // A held pointer on focus return must not start a new press.
                awaitingPointerRelease_ = true;
                UpdateInteractionFlags(drawOrder_);
                return;
            }
            // Keep the event sequence stable when callbacks rebuild the screen.
            const auto events = frame.pointerEvents;
            for (const UIPointerEvent& event : events)
            {
                UpdatePointer(event.position, event.down);
            }
            UpdatePointer(frame.pointer, frame.pointerDown);
            if (frame.focusNext)
            {
                FocusNext();
            }
            if (frame.focusPrevious)
            {
                FocusNext(true);
            }
            if (frame.activate)
            {
                ActivateFocused();
            }
            Refresh();
        }

        void Refresh()
        {
            drawOrder_ = CollectElements();
            modalId_ = NoElement;
            WorldTransform2D screen;
            screen.viewportAspect = Positive(frame_.width) / Positive(frame_.height);
            for (Element* element : drawOrder_)
            {
                const Element* parent = element->parent_;
                element->worldTransform_ =
                    ResolveTransform(element->transform, parent ? parent->worldTransform_ : screen,
                                     screen.viewportAspect);
                element->inheritedVisible_ =
                    element->visible && (!parent || parent->inheritedVisible_);
                element->inheritedEnabled_ =
                    element->enabled && (!parent || parent->inheritedEnabled_);
                element->inheritedOpacity_ =
                    Unit(element->opacity) * (parent ? parent->inheritedOpacity_ : 1.0f);
                if (element->modal && element->inheritedVisible_ && element->inheritedEnabled_ &&
                    element->inheritedOpacity_ > 0.0f)
                {
                    modalId_ = element->id_;
                }
            }
            ClearInvalidSelection();
            hoveredId_ =
                frame_.active && !awaitingPointerRelease_ ? HitTest(frame_.pointer) : NoElement;
            UpdateInteractionFlags(drawOrder_);
            drawList_.clear();
            for (const Element* element : drawOrder_)
            {
                if (!element->inheritedVisible_ || element->inheritedOpacity_ <= 0.0f)
                {
                    continue;
                }
                if (element->drawRect)
                {
                    Color color = ApplyInheritedStyle(element->color, *element);
                    if (element->inheritedEnabled_ && element->interactable &&
                        (element->hovered || element->focused))
                    {
                        color.r = (std::min)(1.0f, color.r + .12f);
                        color.g = (std::min)(1.0f, color.g + .12f);
                        color.b = (std::min)(1.0f, color.b + .12f);
                    }
                    drawList_.push_back({element->worldTransform_, color, {}, 24.0f, false});
                }
                if (!element->text.empty())
                {
                    drawList_.push_back(
                        {element->worldTransform_,
                         ApplyInheritedStyle(element->textColor, *element), element->text,
                         element->fontSize * (element->useTransformScale
                                                  ? std::fabs(element->worldTransform_.scale.y)
                                                  : 1.0f),
                         true});
                }
            }
        }

        const UIDrawList& DrawList() const { return drawList_; }
        ElementId Hovered() const { return hoveredId_; }
        ElementId Pressed() const { return pressedId_; }
        ElementId Focused() const { return focusedId_; }
        ElementId ModalRoot() const { return Find(modalId_) ? modalId_ : NoElement; }
        bool WantsInput() const { return ModalRoot() != NoElement; }

        void FocusNext(bool reverse = false)
        {
            Refresh();
            std::vector<ElementId> choices;
            for (const Element* element : drawOrder_)
            {
                if (IsInteractive(element))
                {
                    choices.push_back(element->id_);
                }
            }
            if (choices.empty())
            {
                focusedId_ = NoElement;
            }
            else
            {
                const auto it = std::find(choices.begin(), choices.end(), focusedId_);
                if (it == choices.end())
                {
                    focusedId_ = reverse ? choices.back() : choices.front();
                }
                else
                {
                    const std::size_t index = static_cast<std::size_t>(it - choices.begin());
                    focusedId_ = choices[reverse ? (index + choices.size() - 1) % choices.size()
                                                 : (index + 1) % choices.size()];
                }
            }
            Refresh();
        }

        void ActivateFocused()
        {
            Refresh();
            InvokeClick(focusedId_);
        }

        void CancelPointerCapture()
        {
            hoveredId_ = NoElement;
            pressedId_ = NoElement;
            awaitingPointerRelease_ = true;
            previousPointerDown_ = true;
            UpdateInteractionFlags(CollectElements());
        }

    private:
        UIInputFrame frame_;
        std::vector<Element*> drawOrder_;
        UIDrawList drawList_;
        ElementId hoveredId_ = NoElement;
        ElementId pressedId_ = NoElement;
        ElementId focusedId_ = NoElement;
        ElementId modalId_ = NoElement;
        bool previousPointerDown_ = false;
        bool awaitingPointerRelease_ = false;

        std::vector<Element*> CollectElements() const
        {
            std::vector<Element*> result;
            std::vector<Element*> pending{const_cast<Element*>(&root)};
            while (!pending.empty())
            {
                Element* element = pending.back();
                pending.pop_back();
                result.push_back(element);
                for (auto it = element->children_.rbegin(); it != element->children_.rend(); ++it)
                {
                    pending.push_back(it->get());
                }
            }
            return result;
        }

        static float Positive(float value)
        {
            return std::isfinite(value) && value > 0.0f ? value : 1.0f;
        }
        static float Unit(float value)
        {
            return std::isfinite(value) ? (std::clamp)(value, 0.0f, 1.0f) : 0.0f;
        }

        bool IsInteractive(const Element* element) const
        {
            if (!element || !element->interactable || !element->inheritedVisible_ ||
                !element->inheritedEnabled_ || element->inheritedOpacity_ <= 0.0f)
            {
                return false;
            }
            if (modalId_ == NoElement)
            {
                return true;
            }
            for (const Element* ancestor = element; ancestor; ancestor = ancestor->parent_)
            {
                if (ancestor->id_ == modalId_)
                {
                    return true;
                }
            }
            return false;
        }

        ElementId HitTest(Vec2 pointer) const
        {
            for (auto it = drawOrder_.rbegin(); it != drawOrder_.rend(); ++it)
            {
                if (IsInteractive(*it) && (*it)->worldTransform_.Contains(pointer))
                {
                    return (*it)->id_;
                }
            }
            return NoElement;
        }

        void ClearInvalidSelection()
        {
            if (!IsInteractive(Find(hoveredId_)))
            {
                hoveredId_ = NoElement;
            }
            if (!IsInteractive(Find(pressedId_)))
            {
                pressedId_ = NoElement;
            }
            if (!IsInteractive(Find(focusedId_)))
            {
                focusedId_ = NoElement;
            }
        }

        void UpdateInteractionFlags(const std::vector<Element*>& elements)
        {
            for (Element* element : elements)
            {
                element->hovered = frame_.active && element->id_ == hoveredId_;
                element->pressed = frame_.active && element->id_ == pressedId_;
                element->focused = element->id_ == focusedId_;
            }
        }

        void InvokeClick(ElementId id)
        {
            Element* element = Find(id);
            if (!IsInteractive(element))
            {
                return;
            }
            element->isClicked = true;
            auto action = element->onClick;
            if (action)
            {
                action();
            }
            // A callback may delete itself or replace the complete UI subtree.
            Refresh();
        }

        void UpdatePointer(Vec2 position, bool down)
        {
            if (awaitingPointerRelease_)
            {
                hoveredId_ = NoElement;
                pressedId_ = NoElement;
                previousPointerDown_ = down;
                if (!down)
                {
                    awaitingPointerRelease_ = false;
                }
                return;
            }
            ClearInvalidSelection();
            hoveredId_ = HitTest(position);
            if (down && !previousPointerDown_)
            {
                pressedId_ = hoveredId_;
                if (pressedId_ != NoElement)
                {
                    focusedId_ = pressedId_;
                }
            }
            const ElementId clicked =
                !down && previousPointerDown_ && pressedId_ == hoveredId_ ? pressedId_ : NoElement;
            if (!down && previousPointerDown_)
            {
                pressedId_ = NoElement;
            }
            previousPointerDown_ = down;
            if (clicked != NoElement)
            {
                InvokeClick(clicked);
            }
        }

        static Color ApplyInheritedStyle(Color color, const Element& element)
        {
            color.a *= element.inheritedOpacity_;
            if (!element.inheritedEnabled_)
            {
                color.r *= .55f;
                color.g *= .55f;
                color.b *= .55f;
            }
            return color;
        }
    };
} // namespace GameUI
