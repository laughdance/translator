#include <a_samp>
#include <sscanf2>
#include <samp-chatbot>
#include <zcmd>

#define TR_FOR_PLAYER 0 			      // global
#define LANG_TARGET_G   "Russian"   // lang 1
#define LANG_TARGET_2_G "English/US"// lang 2
#include <translator>

main(){}

COMMAND:translate(playerid, params[])
{
  	extract params -> new string:translate[144+1]; else return 1;
  	RequestToChatBot(translate, TR_GLOBAL_ID);
  	return 1;
}

COMMAND:clearmemory(playerid)
{
#if TR_FOR_PLAYER == 1
  	return SendClientMessage(playerid, -1, "Error: command not available!");
#endif
  	TR_ClearMemory(TR_GLOBAL_ID);
  	return 1;
}

