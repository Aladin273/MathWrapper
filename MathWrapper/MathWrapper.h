#pragma once

#include <tgbot/tgbot.h>
#include "JuliaWrapper.h"

#include <fstream>
#include <sstream>
#include <functional>
#include <map>

class MathWrapper
{
public:
    MathWrapper();

    void execute();

    void setupUI();
    void setupCommands();

    void setupMenu();
    void setupInput();
    void setupPlots();
    void setupCalculator();
    void setupTrigonometry();

    void sendResult(int64_t chatId, const std::string& result);
    void sendFileResult(int64_t chatId, const std::string& file, const std::string& mime);

    void createKeyBoard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb);
    void createInlineKeyBoard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::InlineKeyboardMarkup::Ptr& kb);

private:
    JuliaWrapper m_wrapper;

    std::unique_ptr<TgBot::Bot> m_bot;
    std::unique_ptr<TgBot::ReplyKeyboardMarkup::Ptr> m_menu;
    
    std::vector<TgBot::BotCommand::Ptr> m_commands;
    std::map<int64_t, std::function<void(TgBot::Message::Ptr)>> m_savedStates;

    std::string m_result;
    std::stringstream m_sstream;
    std::vector<std::string> m_args{{""},{""}};

    std::vector<std::vector<std::string>> m_menuLayout
    {
        {"Input"},              // 0
        {"Plots"},              // 1
        {"Calculator"},         // 2
        {"Trigonometry"},       // 3
    };

    std::vector<std::pair<std::string, std::string>> m_commandLayout
    {
        {"start", "Start work with Bot"},    // 1
        {"info", "Send your info"},          // 2
        {"clear", "Clear keyboards"},        // 3
    };

    std::vector<std::vector<std::vector<std::string>>> m_itemsLayout
    {
        {
            {"Write", "Write ( html output )"},         // 0
            {"From file", "From file ( html output )"}
        },
        {
            {"png", "svg", "pdf", "html"}             // 1
        },
        {
            {"+", "-", "*", "/"},                     // 2
            {"sqrt", "cbrt", "log", "exp"}  
        },
        {
            {"sin", "cos", "tan"},                    // 3
            {"asin", "acos", "atan"}
        },
    };

    std::string m_info
    {
        "Hi!\nMy name is MathWrapper, and i am wrapper :)\n"
        "I mean, Julia wrapper.\n\n"
        "In \"Input\" you can write code on Julia Syntax and get output\n\n"
        "\"Plots\" send you plot from Julia Syntax : plot(), surface()\n\n"
        "\"Calculator\" and \"Trigonometry\", you know what to do)\n\n"
        "More functionality coming soon!",
    };

    const std::string m_mimeType = "";
    const std::string m_temp = "../Temp/";
    const std::string m_token = "token";
};
