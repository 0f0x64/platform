#pragma once

#include "../../ui/uiRuntime.h"
#include "../../ui/dialogue.h"
#include "npcCube.h"
#include "../../ui/subtitles.h"

#pragma comment(lib, "comdlg32.lib")

namespace GameUI
{
    class NpcInteraction
    {
    public:
        dx11::XMVECTOR position = dx11::XMVectorSet(0, 0, 0, 1);
        float interactionRadius = 12.0f;
        float cubeHalfSize = .85f;

        void Initialize(dx11::XMVECTOR worldPosition)
        {
            if (initialized_)
            {
                return;
            }
            position = worldPosition;
            initialized_ = cube_.Initialize(dx11::device, dx11::context);
            if (!initialized_)
            {
                OutputDebugStringA("NPC UI initialization failed.\n");
                return;
            }
            promptRoot_ = CreateRoot("NPC interaction prompt");
            dialogueRoot_ = CreateRoot("NPC dialogue", true);
            subtitleRoot_ = CreateRoot("NPC subtitles");
            BuildPrompt();
            BuildSubtitles();
        }

        bool IsOpen() const { return session_.IsOpen(); }

        // Game-specific interaction only; the UI library owns UI input and rendering.
        void Update(HWND window, bool gameplay, dx11::XMVECTOR playerPosition, bool playerAlive)
        {
            if (!initialized_)
            {
                return;
            }
            const ULONGLONG now = GetTickCount64();
            const float elapsed = lastSubtitleTime_ == 0
                                      ? 0.0f
                                      : static_cast<float>(now - lastSubtitleTime_) / 1000.0f;
            lastSubtitleTime_ = now;
            const bool advanceSubtitles =
                ui.Active() && gameplay && !ui.canvas.WantsInput() && !ui.Resuming();
            subtitles_.Update(advanceSubtitles ? elapsed : 0.0f);
            const float distance =
                dx11::XMVectorGetX(dx11::XMVector3Length(position - playerPosition));
            const bool nearby = gameplay && playerAlive && distance <= interactionRadius;
            const ElementId modal = ui.canvas.ModalRoot();
            const bool ownsModal = modal == dialogueRoot_;
            const bool otherModal = modal != NoElement && !ownsModal;
            if (!gameplay || !playerAlive)
            {
                Close();
                subtitles_.Stop();
            }
            if (ui.Active() && ownsModal && ui.KeyPressed(VK_ESCAPE) && IsOpen())
            {
                ui.ConsumeEscape();
                Close();
            }
            if (ui.Active() && !otherModal && nearby && !IsOpen() && ui.KeyPressed('E'))
            {
                subtitles_.Stop();
                session_.Start(CreateDialogue());
                BuildDialogue();
            }
            if (ui.Active() && !otherModal && nearby && ui.KeyPressed(VK_F2))
            {
                PickTexture(window);
            }
            if (ui.Active() && !otherModal && session_.IsOpen())
            {
                const DialogueLine* line = session_.Current();
                for (int i = 0; line && i < 9 && i < static_cast<int>(line->choices.size()); ++i)
                {
                    if (ui.KeyPressed('1' + i))
                    {
                        Choose(static_cast<std::size_t>(i));
                        break;
                    }
                }
            }
            UpdateVisibility(nearby);
        }

        void Render()
        {
            if (!initialized_)
            {
                return;
            }
            cube_.Render(position, cubeHalfSize,
                         dx11::XMMatrixTranspose(dx11::ConstBuf::camera.view[0]),
                         dx11::XMMatrixTranspose(dx11::ConstBuf::camera.proj[0]),
                         dx11::Textures::Texture[static_cast<int>(texture::pBuf)].DepthResView,
                         dx11::width, dx11::height);
        }

        void Close()
        {
            session_.Close();
            SetVisible(dialogueRoot_, false);
        }

        void Shutdown()
        {
            for (ElementId id : {promptRoot_, dialogueRoot_, subtitleRoot_})
            {
                ui.canvas.Remove(id);
            }
            promptRoot_ = NoElement;
            dialogueRoot_ = NoElement;
            subtitleRoot_ = NoElement;
            subtitleSpeaker_ = NoElement;
            subtitleText_ = NoElement;
            session_.Close();
            subtitles_.Stop();
            lastSubtitleTime_ = 0;
            nearby_ = false;
            textureStatus_.clear();
            cube_.Shutdown();
            initialized_ = false;
        }

    private:
        NpcCube cube_;
        DialogueSession session_;
        SubtitlePlayer subtitles_;
        ElementId promptRoot_ = NoElement;
        ElementId dialogueRoot_ = NoElement;
        ElementId subtitleRoot_ = NoElement;
        ElementId subtitleSpeaker_ = NoElement;
        ElementId subtitleText_ = NoElement;
        ULONGLONG lastSubtitleTime_ = 0;
        bool initialized_ = false;
        bool nearby_ = false;
        std::wstring textureStatus_;

        static ElementId CreateRoot(const char* name, bool modal = false)
        {
            Element& element = ui.canvas.root.AddChild(name);
            element.visible = false;
            element.modal = modal;
            return element.Id();
        }

        static void SetVisible(ElementId id, bool visible)
        {
            if (Element* element = ui.canvas.Find(id))
            {
                element->visible = visible;
            }
        }

        static Element& AddPanel(Element& parent, const char* name, Vec2 position, Vec2 scale,
                                 Color color)
        {
            Element& element = parent.AddChild(name);
            element.transform.position = position;
            element.transform.scale = scale;
            element.drawRect = true;
            element.color = color;
            return element;
        }

        static Element& AddLabel(Element& parent, const char* name, const std::wstring& text,
                                 Vec2 position, Vec2 scale, float fontSize)
        {
            Element& element = parent.AddChild(name);
            element.transform.position = position;
            element.transform.scale = scale;
            element.text = text;
            element.fontSize = fontSize;
            element.textColor = {.92f, .95f, 1, 1};
            return element;
        }

        DialogueTree CreateDialogue()
        {
            return {{L"Привет! Я хранитель этой линии. О чём хочешь спросить?",
                     {{L"Расскажи об этой линии", -1, [this] { StartSubtitles(); }},
                      {L"Кто ты?", 1},
                      {L"До встречи!", -1}}},
                    {L"Пока я куб, но у меня уже есть свой диалог. Внешность можно менять: нажми "
                     L"F2 и выбери картинку.",
                     {{L"А как устроен диалог?", 2},
                      {L"Расскажи об этой линии", -1, [this] { StartSubtitles(); }},
                      {L"До встречи!", -1}}},
                    {L"Выбирай ответ мышью, цифрами или стрелками и Enter. Каждый ответ может "
                     L"вести к новой реплике. Esc завершит разговор.",
                     {{L"Расскажи об этой линии", -1, [this] { StartSubtitles(); }},
                      {L"Ещё один вопрос", 0},
                      {L"До встречи!", -1}}}};
        }

        void Choose(std::size_t index)
        {
            if (!session_.Choose(index))
            {
                return;
            }
            BuildDialogue();
            UpdateVisibility(nearby_);
        }

        void StartSubtitles()
        {
            subtitles_.Start(
                {{L"ХРАНИТЕЛЬ ЛИНИИ", L"Эти линии связывают далёкие созвездия.", 6},
                 {L"ХРАНИТЕЛЬ ЛИНИИ", L"Я останусь здесь. Если появятся вопросы — возвращайся.", 6},
                 {L"ХРАНИТЕЛЬ ЛИНИИ", L"До встречи, путник.", 4}});
            lastSubtitleTime_ = GetTickCount64();
        }

        void UpdateVisibility(bool nearby)
        {
            nearby_ = nearby;
            SetVisible(promptRoot_, nearby && !IsOpen() && !subtitles_.IsPlaying());
            SetVisible(dialogueRoot_, session_.IsOpen());
            const SubtitleCue* cue = subtitles_.Current();
            SetVisible(subtitleRoot_, cue && !IsOpen());
            if (!cue)
            {
                return;
            }
            if (Element* element = ui.canvas.Find(subtitleSpeaker_))
            {
                element->text = cue->speaker;
            }
            if (Element* element = ui.canvas.Find(subtitleText_))
            {
                element->text = cue->text;
            }
        }

        void BuildPrompt()
        {
            Element& panel = AddPanel(*ui.canvas.Find(promptRoot_), "Interaction prompt",
                                      {0, -.80f}, {.52f, .10f}, {.025f, .04f, .08f, .94f});
            AddLabel(panel, "Prompt", L"E — поговорить с кубом     F2 — выбрать текстуру", {},
                     {.96f, .88f}, 21);
        }

        void BuildSubtitles()
        {
            Element& panel = AddPanel(*ui.canvas.Find(subtitleRoot_), "Subtitle panel", {0, -.70f},
                                      {.78f, .13f}, {.02f, .03f, .055f, .94f});
            Element& speaker =
                AddLabel(panel, "Subtitle speaker", L"", {0, .64f}, {.94f, .20f}, 21);
            subtitleSpeaker_ = speaker.Id();
            speaker.textColor = {.35f, .80f, 1, 1};
            subtitleText_ =
                AddLabel(panel, "Subtitle text", L"", {0, -.13f}, {.94f, .48f}, 26).Id();
        }

        void BuildDialogue()
        {
            Element* root = ui.canvas.Find(dialogueRoot_);
            if (!root)
            {
                return;
            }
            root->ClearChildren();
            SetVisible(dialogueRoot_, session_.IsOpen());
            const DialogueLine* line = session_.Current();
            if (!line)
            {
                return;
            }
            AddPanel(*root, "Backdrop", {}, {1, 1}, {0, .01f, .025f, .58f});
            Element& panel = AddPanel(*root, "Dialogue panel", {0, -.23f}, {.78f, .65f},
                                      {.035f, .055f, .10f, .99f});
            AddLabel(panel, "Speaker", L"ХРАНИТЕЛЬ ЛИНИИ", {0, .80f}, {.90f, .09f}, 25);
            AddPanel(panel, "Accent", {0, .64f}, {.90f, .006f}, {.25f, .75f, .95f, 1});
            AddLabel(panel, "Dialogue text", line->prompt, {0, .38f}, {.90f, .21f}, 25);
            for (std::size_t i = 0; i < line->choices.size(); ++i)
            {
                Element& button = AddPanel(panel, ("Choice " + std::to_string(i + 1)).c_str(),
                                           {0, -.05f - .28f * static_cast<float>(i)}, {.91f, .115f},
                                           {.075f, .13f, .21f, 1});
                button.interactable = true;
                button.onClick = [this, i] { Choose(i); };
                AddLabel(button, "Choice text",
                         std::to_wstring(i + 1) + L". " + line->choices[i].text, {}, {.96f, .88f},
                         21);
            }
            AddLabel(panel, "Controls",
                     textureStatus_.empty()
                         ? L"↑ ↓ / Tab • Enter • 1–3 • Esc — закрыть • F2 — текстура"
                         : textureStatus_,
                     {0, -.88f}, {.91f, .065f}, 13);
            ui.canvas.FocusNext();
        }

        void PickTexture(HWND window)
        {
            wchar_t file[32768]{};
            OPENFILENAMEW dialog{};
            dialog.lStructSize = sizeof(dialog);
            dialog.hwndOwner = window;
            dialog.lpstrFilter =
                L"Изображения\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tif;*.tiff\0Все файлы\0*.*\0";
            dialog.lpstrFile = file;
            dialog.nMaxFile = static_cast<DWORD>(std::size(file));
            dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
            ui.CaptureNative();
            const bool selected = GetOpenFileNameW(&dialog) != FALSE;
            ui.ReleaseNative();
            if (!selected)
            {
                return;
            }
            textureStatus_ = cube_.LoadTexture(file)
                                 ? L"Текстура куба изменена"
                                 : L"Не удалось прочитать изображение. Текстура сохранена.";
            if (session_.IsOpen())
            {
                BuildDialogue();
            }
        }
    };

    inline NpcInteraction npcInteraction;
} // namespace GameUI
