#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

int main()
{
    Bot bot("token-here");


    ReplyKeyboardMarkup::Ptr keyboardOneCol(new ReplyKeyboardMarkup);
    ReplyKeyboardMarkup::Ptr keyboardWithLayout(new ReplyKeyboardMarkup);

    vector<string> buttonStrings{ "Option 1", "Option 2", "Option 3" };
    vector<vector<string>>buttonLayout
    { { "Dog", "Cat", "Mouse" },
    { "Green", "White", "Red" },
    { "On", "Off" },
    { "Back" },
    { "Info", "About", "Map", "Etc" } };

    for (size_t i = 0; i < buttonStrings.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        KeyboardButton::Ptr button(new KeyboardButton);
        button->text = buttonStrings[i];
        row.push_back(button);
        keyboardOneCol->keyboard.push_back(row);
    }

    for (size_t i = 0; i < buttonLayout.size(); ++i) {
        vector<KeyboardButton::Ptr> row;
        for (size_t j = 0; j < buttonLayout[i].size(); ++j) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = buttonLayout[i][j];
            row.push_back(button);
        }
        keyboardWithLayout->keyboard.push_back(row);
    }

    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);

    vector<InlineKeyboardButton::Ptr> row0;
    vector<InlineKeyboardButton::Ptr> row1;

    InlineKeyboardButton::Ptr checkButton(new InlineKeyboardButton);
    InlineKeyboardButton::Ptr oneButton(new InlineKeyboardButton);
    InlineKeyboardButton::Ptr twoButton(new InlineKeyboardButton);
    InlineKeyboardButton::Ptr threeButton(new InlineKeyboardButton);

    checkButton->text = "check";
    checkButton->callbackData = "check";

    oneButton->text = "one";
    oneButton->callbackData = "one";

    twoButton->text = "two";
    twoButton->callbackData = "two";

    threeButton->text = "three";
    threeButton->callbackData = "three";

    row0.push_back(checkButton);
    row0.push_back(oneButton);
    row1.push_back(twoButton);
    row1.push_back(threeButton);

    keyboard->inlineKeyboard.push_back(row0);
    keyboard->inlineKeyboard.push_back(row1);

    bot.getEvents().onCommand("start", [&](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "Hi!", false, 0, keyboard);
            bot.getApi().sendMessage(message->chat->id, "", false, 0, keyboardWithLayout);
        });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query)
        {
            if (StringTools::startsWith(query->data, "check")) {
                string response = "ok";
                //bot.getApi().sendMessage(query->message->chat->id, response, false, 0, keyboard, "Markdown");
            }
        });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query)
        {
            if (StringTools::startsWith(query->data, "one")) {
                string response = "one";
                bot.getApi().sendMessage(query->message->chat->id, response);
            }
        });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query)
        {
            if (StringTools::startsWith(query->data, "two")) {
                string response = "two";
                bot.getApi().sendMessage(query->message->chat->id, response);
            }
        });

    bot.getEvents().onCallbackQuery([&](CallbackQuery::Ptr query)
        {
            if (StringTools::startsWith(query->data, "three")) {
                string response = "three";
                bot.getApi().sendMessage(query->message->chat->id, response);
                keyboard->inlineKeyboard;
            }
        });

    bot.getEvents().onCommand("replyone", [&bot, &keyboardOneCol](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "/start for one column keyboard\n/layout for a more complex keyboard", false, 0, keyboardOneCol);
        });
    bot.getEvents().onCommand("reply", [&bot, &keyboardWithLayout](Message::Ptr message)
        {
            bot.getApi().sendMessage(message->chat->id, "/start for one column keyboard\n/layout for a more complex keyboard", false, 0, keyboardWithLayout);
        });
    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) // For Reply
        {
            printf("User wrote %s\n", message->text.c_str());
            if (StringTools::startsWith(message->text, "/start") || StringTools::startsWith(message->text, "/layout")) {
                return;
            }
            bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        });




    signal(SIGINT, [](int s) {
        printf("SIGINT got\n");
        exit(0);
        });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (exception& e) {
        printf("error: %s\n", e.what());
        system("pause");
    }

    return 0;
}
