#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <tgbot/tgbot.h>

using namespace std;
using namespace TgBot;

int main() 
{
    const string photoFilePath = "../Files/crash.png";
    const string photoMimeType = "image/png";

    Bot bot("5440868921:AAGeOx3vKyrJlra7eQt8tWXUTmWlsiTjatI");

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) 
        {
            bot.getApi().sendMessage(message->chat->id, "Hi!");
        });

    bot.getEvents().onCommand("photo", [&bot, &photoFilePath, &photoMimeType](Message::Ptr message) 
        {
            bot.getApi().sendPhoto(message->chat->id, InputFile::fromFile(photoFilePath, photoMimeType));
        });

    signal(SIGINT, [](int s) 
        {
            printf("SIGINT got\n");
            exit(0);
        });


    try 
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgLongPoll longPoll(bot);
        while (true) 
        {
            printf("Long poll started\n");
            longPoll.start();
        }
    } 
    catch (exception& e) 
    {
        printf("error: %s\n", e.what());
    }

    return 0;
}
