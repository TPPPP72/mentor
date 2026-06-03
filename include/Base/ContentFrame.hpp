#pragma once

#include <Base/Diag.hpp>
#include <Base/UI.hpp>
#include <cstdint>
#include <string_view>
#include <string>
#include <vector>

namespace mentor
{

struct alignas(2) ContentChar
{
    char text;              ///< 字符
    Diag diag = Diag::Null; ///< 诊断
};

enum class ContentStringType : uint32_t
{
    Normal,  ///< 正常
    Success, ///< 成功
    Error,   ///< 错误
    Warning, ///< 警告
    Note     ///< 标记
};

struct ContentString
{
    std::vector<ContentChar> text;
    ContentStringType type = ContentStringType::Normal;

    ContentString()
    {
        text.reserve(64);
    }
    ContentString(ContentStringType type, std::string_view text) : type(type)
    {
        this->text.reserve(text.size());
        for (char ch : text)
            this->text.emplace_back(ch);
    }
    ContentString(std::string_view text)
    {
        this->text.reserve(text.size());
        for (char ch : text)
            this->text.emplace_back(ch);
    }
};

using UIComponentData = std::vector<ContentString>;

struct ContentFrame
{
    std::vector<UIComponent> components;
    std::vector<UIComponentData> datas;
    UIAction action = UIAction::Null;
    UIInputMode input_mode = UIInputMode::WaitForEndline;

    ContentFrame()
    {
        datas.reserve(4);
        for (auto &component_data : datas)
            component_data.reserve(32);
        components.reserve(4);
    }

    std::size_t getComponentId(UIComponent component){
        components.emplace_back(component);
        datas.emplace_back();
        return datas.size() - 1;
    }

    UIComponentData &getComponentData(size_t componentId)
    {
        return datas[componentId];
    }
};

/// 构建列表
inline std::vector<ContentString> buildList(std::string_view title, const std::vector<std::string> &items)
{
    std::vector<ContentString> list;

    list.emplace_back(title);
    for (auto item : items)
        list.emplace_back(item);

    return list;
}

/// 构建列表
inline std::vector<ContentString> buildNonTitleList(const std::vector<std::string> &items)
{
    std::vector<ContentString> list;

    for (auto item : items)
        list.emplace_back(item);

    return list;
}

} // namespace mentor