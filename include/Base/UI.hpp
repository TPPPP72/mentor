#pragma once

#include <cstdint>

namespace mentor
{

enum class UIAction : uint32_t
{
    Null,     ///< 不进行操作
    Clear,    ///< 清空
    Refresh,  ///< 刷新
    Previous, ///< 上一题
    Next,     ///< 下一题
    Submit,   ///< 提交
    Retry,    ///< 重答
    Exit,     ///< 退出程序
};

enum class UIComponent : uint32_t
{
    Text,          ///< 纯文本
    ConfirmDialog, ///< 二选一对话框 Yes or No
    FileSelector,  ///< 文件选择器 约定[0]为title
    Dialog,        ///< 对话框 约定[0]为title
    InputBox,      ///< 输入框 约定[0]为title
    List,          ///< 列表 约定[0]为title
    ChoicePanel,   ///< 选择面板
    PreviewPanel,  ///< 测试预览面板
    AnsweringBox,  ///< 答题输入框
};

enum class UIInputMode : uint32_t
{
    QuickReact,    ///< 快速反应
    WaitForEndline ///< 直到换行反应
};

} // namespace mentor