// (Q) What’s the difference between a Player TR and a Global TR?
// (A) The difference is that Player means the form (ID) is not ignored and works for everything on a per-player basis. 
// Both the prompt (input) and the response (output) will respect the player’s ID. Meanwhile, for Global, the prompt (input) and response (output) ignore the ID.
// Since it’s global, the system cannot be changed individually per player.
#include <a_samp>
/*
 * API_KEY
 * your api key
 * see https://github.com/SimoSbara/samp-chatbot?tab=readme-ov-file#description
 */
#if ! defined API_KEY
	#define API_KEY "DO_HERE"
#endif
/*
 * MAX_PROMPT_RESPONSE
 * max len of input/output (I/O)
 * default 526
 */
#if ! defined MAX_PROMPT_RESPONSE
	#define MAX_PROMPT_RESPONSE 526
#endif
/*
 * MAX_LANGUAGE_SIZE
 * max len of language_target (1/2)
 * default 36
 */
#if ! defined MAX_LANGUAGE_SIZE
	#define MAX_LANGUAGE_SIZE 36
#endif
/*
 * TR_DEBUGGING
 * translator debugging (1 : 0)
 *
 */
#if ! defined TR_DEBUGGING
	#define TR_DEBUGGING 0
#endif
/*
 * TR_GLOBAL_ID
 * global id
 * default -1
 */
#if ! defined TR_GLOBAL_ID
	#define TR_GLOBAL_ID -1
#endif
/*
 * Translator Encoding
 * EN/US   0
 * Russian 1
 * Chinese 2
 */
#if ! defined TR_Encoding
	#define TR_Encoding 1
#endif
/*
 * Translator CHatbot
 * 0 ChatGPT
 * 1 GEMINI
 * 2 LLAMA
 * 3 DOUBAO
 * 4 Deepseek
 */
#if ! defined TR_ChatBot
	#define TR_ChatBot 2
#endif
/*
 * TR_FOR_PLAYER
 * 0 = true, global
 * 1 = false, player
 */
#if ! defined TR_FOR_PLAYER 
	#define TR_FOR_PLAYER 0
#endif

#if TR_FOR_PLAYER == 0 // global
	#if ! defined LANG_TARGET_G
		#define LANG_TARGET_G   "Russian"
	#endif
	#if ! defined LANG_TARGET_2
		#define LANG_TARGET_2_G "English/US"
	#endif
	new response_save_g[MAX_PROMPT_RESPONSE],
		prompt_save_g[MAX_PROMPT_RESPONSE],
		bool:mem_role_g = false
	;
#else 		     // player
	new LANG_TARGET[MAX_PLAYERS][MAX_LANGUAGE_SIZE],
			LANG_TARGET_2[MAX_PLAYERS][MAX_LANGUAGE_SIZE],
			response_save[MAX_PLAYERS][MAX_PROMPT_RESPONSE],
			prompt_save[MAX_PLAYERS][MAX_PROMPT_RESPONSE],
			requestBot[MAX_PLAYERS],
			bool:mem_role[MAX_PLAYERS] = false
	;
	stock SetTargetLanguage(const lang[], const lang2[], const id) {
		format(LANG_TARGET[id], sizeof(LANG_TARGET[]), lang);
		format(LANG_TARGET_2[id], sizeof(LANG_TARGET[]), lang2);
		return TR_SetSystemPrompt(id);
	}
#endif

/*
 * _@RequestToChatBot
 * replace/modify for RequestChatBot
 */
stock _@RequestToChatBot(const _@prompt[], id)
{
	if ( _@prompt[0] != '\0' )
	{
    	new _prompt_[MAX_PROMPT_RESPONSE + 1024]
			;
        /* 
		 * change role to translator
		 */
#if TR_FOR_PLAYER == 0
		if ( mem_role_g == false ) {
	        	format(_prompt_, sizeof(_prompt_), 
				"%s" \
				"%s",
				prompt_save_g, _@prompt // prompt
			);
		}
#elseif TR_FOR_PLAYER == 1
		requestBot[id] = 1;
		if ( mem_role[id] == false ) {
	        	format(_prompt_, sizeof(_prompt_), 
				"%s" \
				"%s",
				prompt_save[id], _@prompt // prompt
			);
		}
#endif

    	RequestToChatBot(_prompt_, id);
    }
    return 1;
}
#if defined _ALS_RequestToChatBot
    #undef RequestToChatBot
#else
    #define _ALS_RequestToChatBot
#endif
#define RequestToChatBot _@RequestToChatBot

forward TR_SetSystemPrompt(const _target);
public TR_SetSystemPrompt(const _target) {
	/*
	 * 10 rule translator
	 * LANG_TARGET -> LANG_TARGET_2
	 * LANG_TARGET_2 -> LANG_TARGET
	 */

#if TR_FOR_PLAYER == 0
    format(prompt_save_g, sizeof(prompt_save_g),
"You are a strict translator bot for GTA SA-MP. always give output translate only, always translate the translate:\n" \
" see https://github.com/laughdance/translator/blob/main/rules_tr.txt\n" \
"1. If the input is in %s, translate it into %s.\n" \
"2. If the input is in %s, translate it into %s.\n" \
"TRANSLATE:",
		LANG_TARGET_G, LANG_TARGET_2_G,
		LANG_TARGET_2_G, LANG_TARGET_G
    );
#elseif TR_FOR_PLAYER == 1
    format(prompt_save[_target], sizeof(prompt_save[]),
"You are a strict translator bot for GTA SA-MP. always give output translate only, always translate the translate:\n" \
" see https://github.com/laughdance/translator/blob/main/rules_tr.txt\n" \
"1. If the input is in %s, translate it into %s.\n" \
"2. If the input is in %s, translate it into %s.\n" \
"TRANSLATE:",
		LANG_TARGET[_target], LANG_TARGET_2[_target],
		LANG_TARGET_2[_target], LANG_TARGET[_target]
    );
#endif
    return 1;
}

stock TR_ClearMemory(const id)
{
#if TR_FOR_PLAYER == 0
	mem_role_g = false;
#elseif TR_FOR_PLAYER == 1
	mem_role[id] = false;
#endif
#if defined ClearMemory
	return ClearMemory(id);
#else
	return 1;
#endif
}

#if defined TR_OnGameModeInit
	forward TR_OnGameModeInit();
#endif

public OnGameModeInit()
{
#if TR_FOR_PLAYER == 0
	TR_SetSystemPrompt(TR_GLOBAL_ID);
#endif
#if defined SetChatBotTimeout
	SetChatBotTimeout(10000);
#endif
	SetSystemPrompt("Assistant");
	SetAPIKey(API_KEY);
#if TR_Encoding == 0
    SetChatBotEncoding(W1252);
#elseif TR_Encoding == 1
	SetChatBotEncoding(W1251);
#elseif TR_Encoding == 2
	SetChatBotEncoding(UTF8);
#else
	SetChatBotEncoding(W1252);
#endif
#if TR_ChatBot == 0
	SelectChatBot(CHAT_GPT);
	SetModel("gpt-3.5-turbo"); // model
#elseif TR_ChatBot == 1
	SelectChatBot(GEMINI_AI);
	SetModel("gemini-2.5-flash"); // model
#elseif TR_ChatBot == 2
	SelectChatBot(LLAMA);
	SetModel("llama-3.1-8b-instant"); // model
#elseif TR_ChatBot == 3
	SelectChatBot(DOUBAO);
	SetModel("doubao-1.5-vision-lite-250315"); // model
#elseif TR_ChatBot == 4
	SelectChatBot(DEEPSEEK);
	SetModel("deepseek-chat"); // model
#else
	SelectChatBot(LLAMA);
#endif

	#if defined TR_OnGameModeInit
		return TR_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
  #undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit TR_OnGameModeInit

#if defined TR_OnPlayerConnect
	forward TR_OnPlayerConnect(playerid);
#endif

public OnGameModeExit()
{
#if TR_FOR_PLAYER == 0
	TR_ClearMemory(TR_GLOBAL_ID);
#elseif TR_FOR_PLAYER == 0
	for (new i = 0; i < GetMaxPlayers(); i++) {
		if ( requestBot [ i ] == 1 ) { TR_ClearMemory(i); }
	}
#endif
	#if defined TR_OnGameModeExit
		return TR_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
  #undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif
#define OnGameModeExit TR_OnGameModeExit

#if defined TR_OnPlayerConnect
	forward TR_OnPlayerConnect(playerid);
#endif
	
public OnPlayerConnect(playerid)
{
	#if defined TR_OnPlayerConnect
		return TR_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
  #undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect TR_OnPlayerConnect

#if defined TR_OnPlayerDisconnect
	forward TR_OnPlayerDisconnect(playerid, reason);
#endif
	
public OnPlayerDisconnect(playerid, reason)
{
#if TR_FOR_PLAYER == 1
	for (new i = 0; i < GetMaxPlayers(); i++) {
		if ( requestBot [ i ] == 1 ) { TR_ClearMemory(i); }
	}
#endif
	#if defined TR_OnPlayerDisconnect
		return TR_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
  #undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect TR_OnPlayerDisconnect

public OnChatBotResponse(prompt[], response[], id) {
#if TR_DEBUGGING == 1
	printf("[translator]: @OnChatBotResponse: prompt:%s\tresponse:%s\tid:%d", prompt, response, id);
#endif
#if TR_FOR_PLAYER == 0
	/*
	 * global
	 */
	if ( strlen(response) > 0 ) {
		format(response_save_g, MAX_PROMPT_RESPONSE, "%s", response); // save response to 'response_save_g'
		SendClientMessageToAll(-1, response_save_g);
    }
#elseif TR_FOR_PLAYER == 1
	/*
	 * per-player
	 */
    if ( id >= 0 && id < MAX_PLAYERS && strlen(response) > 0 ) {
		for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++) 
		{
			if ( i == id ) {
				/* if `i` is the `id` */
				format(response_save[id], MAX_PROMPT_RESPONSE, "%s", response); // save response to 'response_save[]'
				SendClientMessage(id, -1, response_save[id]);
			}
		}
	}
#endif
}


