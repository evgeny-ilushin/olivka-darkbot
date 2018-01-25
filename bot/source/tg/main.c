#include <stdio.h>

#include "tg.h"

int main(int argc, char *argv[])
{
    char nick[512] = "tg!tg@redip.ru";
    char channel[512] = "#telegram";
    //char text[10240] = ":[\2""10Evgeny Ilushin\2] ахуеть";
    char text[10240] = ":[\3""07Maks\3] 3209485sk lfjhd oisdfgiosdf";

    printf("nick: %s\n", nick);
    printf("channel: %s\n", channel);
    printf("text: %s\n", text);

    substituteTgMessage(nick, channel, text);

    printf("\nnick: %s\n", nick);
    printf("channel: %s\n", channel);
    printf("text: %s\n", text);
    return 0;
}
