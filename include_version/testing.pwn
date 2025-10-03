#include <a_samp>
#include <sscanf2>
#include <samp-chatbot>
#include <zcmd>

#define TR_FOR_PLAYER 1 // player
#include <translator>

main(){}

public OnPlayerConnect(playerid)
{
	SetTargetLanguage("Russian", "English/US", playerid);
	return 1;
}

COMMAND:translate(playerid, params[])
{
    extract params -> new string:translate[144+1]; else return 1;
    RequestToChatBot(translate, playerid);
    return 1;
}

COMMAND:clearmemory(playerid)
{
#if TR_FOR_PLAYER == 0
    return SendClientMessage(playerid, -1, "Error: command not available!");
#endif
    TR_ClearMemory(playerid);
    return 1;
}

COMMAND:changelang(playerid, params[])
{
    new
		lang_one[36],
        lang_two[36]
    ;
    if (sscanf(params, "s[36]s[36]", lang_one, lang_two)) return SendClientMessage(playerid, -1, "changelang [lang_one] [lang_two]");
    SetTargetLanguage(lang_one, lang_two, playerid);
    return 1;
}
