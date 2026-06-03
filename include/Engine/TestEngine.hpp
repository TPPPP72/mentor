#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/Type.hpp>
#include <Base/UI.hpp>
#include <Engine/Tester.hpp>
#include <Tool/ConfigIO.hpp>
#include <Tool/Platform.hpp>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <system_error>

namespace mentor
{

/// 测试引擎
class TestEngine
{
public:
    TestEngine()
    {
        m_jsons.reserve(32);
    }

    ContentFrame getInit()
    {
        ContentFrame frame;
        if (tryLoadFromCurrentDirectory())
        {
            auto id = frame.getComponentId(UIComponent::ConfirmDialog);
            frame.getComponentData(id).emplace_back("检测到当前目录下有json文件，是否采用？");
            frame.input_mode = UIInputMode::QuickReact;
            m_state          = State::WaitForEnsureLoadFromCurrentDir;
        }
        else
        {
            auto id = frame.getComponentId(UIComponent::FileSelector);
            frame.getComponentData(id).emplace_back("请选择测试文件或其所在目录");
            m_state = State::WaitForDirOrFile;
        }
        return frame;
    }

    ContentFrame feedInput(std::string_view input)
    {
        switch (m_state)
        {
        case State::WaitForEnsureLoadFromCurrentDir:
            return handleJudgeLoadFromCurrentDir(input);
        case State::WaitForDirOrFile:
            return handleDirOrFilePath(input);
        case State::WaitForChooseFileIndex:
            return handleChooseFileIndex(input);
        case State::Testing:
            return handleTesting(input);
        case State::WaitForEnterReport:
            return handleWaitReport(input);
        default:
            unreachable();
        }
    }

private:
    ContentFrame handleJudgeLoadFromCurrentDir(std::string_view input)
    {
        ContentFrame frame;
        if (input == "Y" || input == "y")
        {
            auto id1                    = frame.getComponentId(UIComponent::List);
            frame.getComponentData(id1) = buildTestList();
            auto id2                    = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id2).emplace_back("输入序号选择");
            m_state = State::WaitForChooseFileIndex;
        }
        else
        {
            m_jsons.clear();
            auto id = frame.getComponentId(UIComponent::FileSelector);
            frame.getComponentData(id).emplace_back("请选择测试文件或其所在目录");
            m_state = State::WaitForDirOrFile;
        }
        return frame;
    }

    ContentFrame handleDirOrFilePath(std::string_view input)
    {
        ContentFrame frame;

        if (std::filesystem::is_regular_file(input) && tryLoadFromFile(input))
        {
            m_state = State::Testing;
            return m_tester.getInit(m_test);
        }
        else if (std::filesystem::is_directory(input) && tryLoadFromDirectory(input))
        {
            frame.action                = UIAction::Clear;
            auto id1                    = frame.getComponentId(UIComponent::List);
            frame.getComponentData(id1) = buildTestList();
            auto id2                    = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id2).emplace_back("输入序号选择");
            m_state = State::WaitForChooseFileIndex;
            return frame;
        }

        auto id = frame.getComponentId(UIComponent::FileSelector);
        frame.getComponentData(id).emplace_back(ContentStringType::Error, "所选文件或目录异常，请重试");
        return frame;
    }

    ContentFrame handleChooseFileIndex(std::string_view input)
    {
        ContentFrame frame;

        std::int32_t index;
        auto result = std::from_chars(input.data(), input.data() + input.size(), index);

        if (result.ec == std::errc())
        {
            if (index > m_jsons.size())
            {
                auto id = frame.getComponentId(UIComponent::InputBox);
                frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入编号大于最大编号，请重新输入");
                return frame;
            }
            else if (index <= 0)
            {
                auto id = frame.getComponentId(UIComponent::InputBox);
                frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入编号小于最小编号，请重新输入");
                return frame;
            }
        }
        else if (result.ec == std::errc::invalid_argument)
        {
            auto id = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入不是一个数字，请重新输入");
            return frame;
        }
        else
        {
            auto id = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入异常，请重新输入");
            return frame;
        }

        if (!tryLoadFromFile(m_jsons[index - 1]))
        {
            auto id = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "所选文件异常，请重试");
            return frame;
        }

        m_state = State::Testing;
        return m_tester.getInit(m_test);
    }

    ContentFrame handleTesting(std::string_view input)
    {
        ContentFrame frame = m_tester.feedInput(input);

        if (m_tester.hasFinish())
            m_state = State::WaitForEnterReport;

        return frame;
    }

    ContentFrame handleWaitReport(std::string_view input)
    {
        return {};
    }

private:
    std::vector<ContentString> buildTestList()
    {
        std::vector<std::string> items;

        for (auto &item : m_jsons)
            items.emplace_back(item.filename().string());

        return buildList("测试列表", items);
    }

private:
    bool tryLoadFromCurrentDirectory()
    {
        return tryLoadFromDirectory(std::filesystem::current_path());
    }

    bool tryLoadFromDirectory(const std::filesystem::path &p)
    {
        for (const auto &entry : std::filesystem::directory_iterator(p))
        {
            if (std::filesystem::is_regular_file(entry.status()))
            {
                auto path = entry.path();
                if (path.extension() == ".json")
                    m_jsons.emplace_back(path);
            }
        }

        return !m_jsons.empty();
    }

    bool tryLoadFromFile(const std::filesystem::path &p)
    {
        try
        {
            std::ifstream rd(p);
            readTestFromJson(rd, m_test);
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

private:
    enum class State
    {
        Init,
        WaitForEnsureLoadFromCurrentDir,
        WaitForDirOrFile,
        WaitForChooseFileIndex,
        Testing,
        WaitForEnterReport,
        Report,
    } m_state = State::Init;
    std::vector<std::filesystem::path> m_jsons;
    Test m_test;
    Tester m_tester;
};

} // namespace mentor