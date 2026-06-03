#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/UI.hpp>
#include <CUI/ContentStringIO.hpp>
#include <iostream>

namespace mentor
{

class Renderer
{
public:
    void render(const ContentFrame &frame)
    {
        for (size_t i = 0; i < frame.components.size(); ++i)
        {
            if (i)
                std::cout << '\n';

            switch (frame.components[i])
            {
            case UIComponent::Text:
                renderText(frame.datas[i]);
                break;
            case UIComponent::ConfirmDialog:
                renderConfirmDialog(frame.datas[i]);
                break;
            case UIComponent::List:
                renderList(frame.datas[i]);
                break;
            case UIComponent::InputBox:
            case UIComponent::FileSelector:
            case UIComponent::AnsweringBox:
                renderInputBox(frame.datas[i]);
                break;
            case UIComponent::PreviewPanel:
                renderPreviewPanel(frame.datas[i]);
                break;
            case UIComponent::ChoicePanel:
                renderChoicePanel(frame.datas[i]);
                break;
            default:
                break;
            }
        }
    }

private:
    void renderText(const std::vector<ContentString> &content)
    {
        for (auto &str : content)
            std::cout << str << '\n';
    }

    void renderConfirmDialog(const std::vector<ContentString> &content)
    {
        std::cout << content.front() << " (Y/N)\n";
    }

    void renderList(const std::vector<ContentString> &content)
    {
        std::cout << content.front() << '\n';

        for (size_t i = 1; i < content.size(); ++i)
            std::cout << i << ". " << content[i] << '\n';
    }

    void renderInputBox(const std::vector<ContentString> &content)
    {
        std::cout << content.front() << ":";
    }

    void renderPreviewPanel(const std::vector<ContentString> &content)
    {
        for (auto &str : content)
            std::cout << str << '\n';
    }

    void renderChoicePanel(const std::vector<ContentString> &content)
    {
        for (size_t i = 0; i < content.size(); ++i)
            std::cout << static_cast<char>('A' + i) << ". " << content[i] << '\n';
    }
};

} // namespace mentor