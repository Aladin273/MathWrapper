#include "MathWrapper.h"

MathWrapper::MathWrapper()
{
    std::cout << "\nTgBot loading..." << std::endl;

    m_bot = std::make_unique<TgBot::Bot>(m_token);
    m_menu = std::make_unique<TgBot::ReplyKeyboardMarkup::Ptr>(new TgBot::ReplyKeyboardMarkup);
    m_wrapper.setDirectory(m_temp);

    setupUI();
    setupCommands();
}

void MathWrapper::execute()
{
    signal(SIGINT, [](int s)
        {
            std::cout << "SIGINT got" << std::endl;
            system("pause");
            exit(0);
        });

    try
    {
        std::cout << "\nBot username: " << m_bot->getApi().getMe()->username.c_str() << std::endl;
        m_bot->getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(*m_bot);

        while (true)
        {
            std::cout << "Long poll started" << std::endl;
            longPoll.start();
        }
    }
    catch (std::exception& e)
    {
        std::cout << "error: " << e.what() << std::endl;
        system("pause");
    }
}

void MathWrapper::setupUI()
{
    createKeyBoard(m_menuLayout, *m_menu);
    setupMenu();
    setupInput();
    setupPlots();
    setupCalculator();
    setupTrigonometry();
}

void MathWrapper::setupCommands()
{
    for (auto& command : m_commandLayout)
    {
        TgBot::BotCommand::Ptr temp(new TgBot::BotCommand);
        temp->command = command.first;
        temp->description = command.second;
        m_commands.push_back(temp);
    }

    m_bot->getApi().setMyCommands(m_commands);

    m_bot->getEvents().onAnyMessage([&](TgBot::Message::Ptr message)
        {
            auto it = m_savedStates.find(message->chat->id);

            if (it != m_savedStates.end())
                it->second(message);
        });

    m_bot->getEvents().onCommand("start", [&](TgBot::Message::Ptr message)
        {
            m_bot->getApi().sendMessage(message->chat->id, m_info, false, 0, *m_menu);
        });

    m_bot->getEvents().onCommand("info", [&](TgBot::Message::Ptr message)
        {
            m_bot->getApi().sendMessage(message->chat->id, m_info, false, 0, *m_menu);
        });

    m_bot->getEvents().onCommand("clear", [&](TgBot::Message::Ptr message)
        {
            m_bot->getApi().sendMessage(message->chat->id, "Cleared!", false, 0, TgBot::ReplyKeyboardRemove::Ptr(new TgBot::ReplyKeyboardRemove));
        });
}

void MathWrapper::setupMenu()
{
    m_bot->getEvents().onAnyMessage([&](TgBot::Message::Ptr message)
        {
            for (size_t i = 0; i < m_menuLayout.size(); ++i)
            {
                for (size_t j = 0; j < m_menuLayout[i].size(); ++j)
                {
                    if (StringTools::startsWith(message->text, m_menuLayout[i][j]))
                    {
                        TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
                        createInlineKeyBoard(m_itemsLayout[i], keyboard);

                        m_savedStates.erase(message->chat->id);
                        m_bot->getApi().sendMessage(message->chat->id, "Choose one :", false, 0, keyboard);
                    }
                }
            }
        });
}

void MathWrapper::setupInput()
{
    m_bot->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
        {
            if (StringTools::startsWith(query->data, "Write ( html output )") || StringTools::startsWith(query->data, "Write"))
            {
                 m_bot->getApi().sendMessage(query->message->chat->id, "Write your expression ( Julia Syntax ) :");

                 m_savedStates.erase(query->message->chat->id);
                 m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                    {
                       if (StringTools::startsWith(query->data, "Write ( html output )"))
                          sendFileResult(message->chat->id, m_wrapper.executeWeave(message->text), m_mimeType);
                       else
                          sendResult(message->chat->id, m_wrapper.execute(message->text));
                    });
            }
            else if (StringTools::startsWith(query->data, "From file ( html output )") || (StringTools::startsWith(query->data, "From file")))
            {
                m_bot->getApi().sendMessage(query->message->chat->id, "Send your file ( .jl ) :");

                m_savedStates.erase(query->message->chat->id);
                m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                    {
                        m_result = m_bot->getApi().downloadFile(m_bot->getApi().getFile(message->document->fileId)->filePath);
                        
                        std::fstream filestream;
                        filestream.open(m_temp + message->document->fileName, std::ios::out);
                        filestream.write(m_result.c_str(), m_result.size());
                        filestream.close();

                        if (StringTools::startsWith(query->data, "From file ( html output )"))
                            sendFileResult(message->chat->id, m_wrapper.executeFileWeave(message->document->fileName), m_mimeType);
                        else
                            sendResult(message->chat->id, m_wrapper.executeFile(message->document->fileName));
                    });
            }
        });
}

void MathWrapper::setupPlots()
{
    m_bot->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
        {
            static size_t item = 1, argA = 0, argB = 1;

            for (size_t i = 0; i < m_itemsLayout[item].size(); ++i)
            {
                for (size_t j = 0; j < m_itemsLayout[item][i].size(); ++j)
                {
                    if (StringTools::startsWith(query->data, m_itemsLayout[item][i][j]))
                    {
                        m_bot->getApi().sendMessage(query->message->chat->id,
                            "Enter 'data' such that : plot( 'data' )\n"
                            "----------------------------------------------------\n"
                            "f(x) = 'data'  \nplot(f)\n"
                            "----------------------------------------------------\n"
                            "f( x , y ) = 'data' \nsurface( 'range', 'range', f)\n"
                            "----------------------------------------------------\n"
                            "f(x, y) = 'data' \nx = 'range' \ny = 'range' \nsurface(x, y, f)");

                        m_savedStates.erase(query->message->chat->id);
                        m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                            {
                                m_args[argA].clear(); m_args[argB].clear();
                                m_args[argA] = m_wrapper.execute(message->text);

                                if (m_args[argA].find("plot") || m_args[argA].find("surface"))
                                    m_args[argB] = m_wrapper.execute("savefig(\"" + m_temp + "plot." + query->data + "\")");

                                if (m_args[argA].empty() || m_args[argB].empty())
                                    sendResult(message->chat->id, "");
                                else
                                    sendFileResult(message->chat->id, m_temp + "plot." + query->data, m_mimeType);
                            });
                    }
                }
            }
        });
}

void MathWrapper::setupCalculator()
{
    m_bot->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
        {
            static size_t item = 2, subItemZero = 0, subItemOne = 1, argA = 0, argB = 1;

            for (size_t i = 0; i < m_itemsLayout[item][subItemZero].size(); ++i)
            {
                if (StringTools::startsWith(query->data, m_itemsLayout[item][subItemZero][i]))
                {
                    m_bot->getApi().sendMessage(query->message->chat->id,
                        "Enter 'a', 'b' such that  :  'a' 'space' 'b'");

                    m_savedStates.erase(query->message->chat->id);
                    m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                        {
                            m_sstream.clear();
                            m_sstream.str(message->text);
                            m_sstream >> m_args[argA];
                            m_sstream >> m_args[argB];
                            m_args[argB] = m_wrapper.execute(m_args[argA] + " " + query->data + " " + m_args[argB]);

                            sendResult(message->chat->id, m_args[argB]);
                        });
                }
            }

            for (size_t i = 0; i < m_itemsLayout[item][subItemOne].size(); ++i)
            {
                if (StringTools::startsWith(query->data, m_itemsLayout[item][subItemOne][i]))
                {
                    m_bot->getApi().sendMessage(query->message->chat->id,
                        "Enter 'data' such that  :  " + query->data + "( 'data' )");

                    m_savedStates.erase(query->message->chat->id);
                    m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                        {
                            sendResult(message->chat->id, m_wrapper.execute(query->data + "(" + message->text + ")"));
                        });
                }
            }
        });
}

void MathWrapper::setupTrigonometry()
{
    m_bot->getEvents().onCallbackQuery([&](TgBot::CallbackQuery::Ptr query)
        {
            static size_t item = 3;

            for (size_t i = 0; i < m_itemsLayout[item].size(); ++i)
            {
                for (size_t j = 0; j < m_itemsLayout[item][i].size(); ++j)
                {
                    if (StringTools::startsWith(query->data, m_itemsLayout[item][i][j]))
                    {
                        m_bot->getApi().sendMessage(query->message->chat->id, 
                            "Enter 'data' such that  :  " + query->data + "( 'data' )");

                        m_savedStates.erase(query->message->chat->id);
                        m_savedStates.emplace(query->message->chat->id, [=](TgBot::Message::Ptr message)
                            {
                                sendResult(message->chat->id, m_wrapper.execute(query->data + "(" + message->text + ")"));
                            });
                    }
                }
            }
        });
}

void MathWrapper::sendResult(int64_t chatId, const std::string& result)
{
    if (!result.empty())
        m_bot->getApi().sendMessage(chatId, result);
    else
        m_bot->getApi().sendMessage(chatId, "Sorry, error!");
}

void MathWrapper::sendFileResult(int64_t chatId, const std::string& file, const std::string& mime)
{
    if (!file.empty())
        m_bot->getApi().sendDocument(chatId, TgBot::InputFile::fromFile(file, mime));
    else
        m_bot->getApi().sendMessage(chatId, "Sorry, error!");
}

void MathWrapper::createKeyBoard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::ReplyKeyboardMarkup::Ptr& kb)
{
    for (size_t i = 0; i < buttonLayout.size(); ++i)
    {
        std::vector<TgBot::KeyboardButton::Ptr> row;

        for (size_t j = 0; j < buttonLayout[i].size(); ++j)
        {
            TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);

            button->text = buttonLayout[i][j];
            row.push_back(button);
        }

        kb->keyboard.push_back(row);
    }
}

void MathWrapper::createInlineKeyBoard(const std::vector<std::vector<std::string>>& buttonLayout, TgBot::InlineKeyboardMarkup::Ptr& kb)
{
    for (size_t i = 0; i < buttonLayout.size(); ++i)
    {
        std::vector<TgBot::InlineKeyboardButton::Ptr> row;

        for (size_t j = 0; j < buttonLayout[i].size(); ++j)
        {
            TgBot::InlineKeyboardButton::Ptr button(new TgBot::InlineKeyboardButton);

            button->text = buttonLayout[i][j];
            button->callbackData = buttonLayout[i][j];
            row.push_back(button);
        }

        kb->inlineKeyboard.push_back(row);
    }
}