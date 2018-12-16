#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../../imgui/imgui.h"
#include <cctype>
#include <cstdlib>
#include <functional>
#include "../utility/cmdline.h"
#include <unordered_map>



class TinyConsole
{
public:
	using EchoCallback = std::function<void(const char* fmt, ...)>;
	using CmdCallback = std::function<void(const char*)>;
	TinyConsole();
	~TinyConsole();
	void ConfigCommand(const char * prog, CmdCallback callback);
	void Draw(const char* title, bool* p_open);
private:
	char                  InputBuf[256];
	ImVector<char*>       Items;
	bool                  ScrollToBottom;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImVector<const char*> Commands;

	std::unordered_map<std::string,CmdCallback> Callbacks;


	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2) { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
	static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
	static char* Strdup(const char *str) { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
	static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }
	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void ExecCommand(const char* command_line);
	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data);
	// In C++11 you are better off using lambdas for this sort of forwarding callbacks
	int TextEditCallback(ImGuiTextEditCallbackData* data);

};


#endif