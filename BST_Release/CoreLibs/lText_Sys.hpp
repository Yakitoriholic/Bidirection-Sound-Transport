/* Description:String processing related to operating system.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_TEXT_SYS
#define LIB_TEXT_SYS

#include "lText.hpp"
#include "lContainer.hpp"

namespace nsText{

	nsContainer::Vector<String_Sys> GetCmdArgList();
	//Returns a list of command argument strings.
	//Remember that constructing a string list requires a lot of memory allocation. Don't use it frequently.
	
	UBINT GetNamePos(const char *lpString, UBINT StrLen);
	UBINT GetNamePos(const wchar_t *lpString, UBINT StrLen);
	inline UBINT GetNamePos(const String *lpString){ return GetNamePos((const char *)lpString->cbegin(), lpString->size()); }
	inline UBINT GetNamePos(const String_W *lpString){ return GetNamePos(lpString->cbegin(), lpString->size()); }
	//Returns the position of the file name(without the path).
	//When the file name is not found. the value returned is [StrLen] + 1.

	UBINT GetNameExtPos(const char *lpString, UBINT StrLen);
	UBINT GetNameExtPos(const wchar_t *lpString, UBINT StrLen);
	inline UBINT GetNameExtPos(const String *lpString){ return GetNameExtPos((const char *)lpString->cbegin(), lpString->size()); }
	inline UBINT GetNameExtPos(const String_W *lpString){ return GetNameExtPos(lpString->cbegin(), lpString->size()); }
	//Returns the position of the extension name(without the first dot).
	//When no extension name is found. the value returned is [StrLen] + 1.

	String_Sys GetWorkingDir();
	//Returns the path of the working directory.

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	nsContainer::Vector<String_Sys> GetCmdArgList(){
		nsContainer::Vector<String_Sys> RetValue;
#if defined LIBENV_OS_WIN
		//we have to parse the command string ourselves. And the parsing rule of Microsoft is horribly complicated.
		wchar_t *argv = (wchar_t *)nsEnv::CmdLineArr, *argv_headptr = argv, *argv_tailptr = argv;
		UBINT argc = 0;

		bool inside_param = false, inside_quote = false;

		auto process_outside_param = [&](){
			while (L' ' == *argv_tailptr || L'\t' == *argv_tailptr)argv_tailptr++;
			if (L'\0' != *argv_tailptr)RetValue.push_back(String_Sys());
			argv_headptr = argv_tailptr;
			inside_param = true;
		};

		auto process_outside_quote = [&](){
			while (true){
				if (L'\0' == *argv_tailptr || L' ' == *argv_tailptr || L'\t' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					inside_param = false;
					argv_headptr = argv_tailptr;
					break;
				}
				else if (L'\\' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					argv_headptr = argv_tailptr;
					argv_tailptr++;

					UBINT BackSlashCount = 1;
					while (L'\\' == *argv_tailptr){ argv_tailptr++; BackSlashCount++; }
					if (L'\"' == *argv_tailptr){
						if (BackSlashCount & 1){
							RetValue.back().append(argv_tailptr - (BackSlashCount >> 1), (BackSlashCount + 1) >> 1);
							argv_tailptr++;
							argv_headptr = argv_tailptr;
						}
						else{
							RetValue.back().append(argv_tailptr - (BackSlashCount >> 1), BackSlashCount >> 1);
							argv_tailptr++;
							argv_headptr = argv_tailptr;
							inside_quote = true;
							break;
						}
					}
					else{
						RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
						argv_headptr = argv_tailptr;
					}
				}
				else if (L'\"' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					inside_quote = true;
					argv_tailptr++;
					argv_headptr = argv_tailptr;
					break;
				}
				else argv_tailptr++;
			}
		};

		auto process_inside_quote = [&](){
			while (true){
				if (L'\"' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					argv_tailptr++;
					if (L'\"' == *argv_tailptr){
						argv_tailptr++;
						argv_headptr = argv_tailptr;
						RetValue.back().append(L'\"');
					}
					else{
						argv_headptr = argv_tailptr;
						inside_quote = false;
						break;
					}
				}
				else if (L'\\' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					argv_headptr = argv_tailptr;
					argv_tailptr++;

					UBINT BackSlashCount = 1;
					while (L'\\' == *argv_tailptr){ argv_tailptr++; BackSlashCount++; }
					if (L'\"' == *argv_tailptr){
						if (BackSlashCount & 1){
							RetValue.back().append(argv_tailptr - (BackSlashCount >> 1), (BackSlashCount + 1) >> 1);
							argv_tailptr++;
							argv_headptr = argv_tailptr;
						}
						else{
							RetValue.back().append(argv_tailptr - (BackSlashCount >> 1), BackSlashCount >> 1);
							argv_tailptr++;
							argv_headptr = argv_tailptr;
							inside_quote = false;
							break;
						}
					}
					else{
						RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
						argv_headptr = argv_tailptr;
					}
				}
				else if (L'\0' == *argv_tailptr){
					RetValue.back().append(argv_headptr, argv_tailptr - argv_headptr);
					argv_headptr = argv_tailptr;
					inside_quote = false;
					break;
				}
				else argv_tailptr++;
			}
		};

		while (L'\0' != *argv_tailptr){
			if (false == inside_param)process_outside_param();
			else{
				if (false == inside_quote)process_outside_quote();
				else process_inside_quote();
			}
		}

#elif defined LIBENV_OS_LINUX
		char **argv = (char **)nsEnv::CmdLineArr;
		for (UBINT i = 0; i < nsEnv::CmdLineCnt; i++){
			RetValue.push_back(String_Sys(argv[i]));
		}
#endif
		return RetValue;
	}
	UBINT GetNamePos(const char *lpString, UBINT StrLen){
		if (0 == StrLen)return 1;

		const char *ptr = lpString + StrLen - 1;
		do{
			if ('/' == *ptr || '\\' == *ptr)break;
			ptr--;
		} while (ptr != lpString);
		return ptr - lpString + 1;
	}
	UBINT GetNamePos(const wchar_t *lpString, UBINT StrLen){
		if (0 == StrLen)return 1;

		const wchar_t *ptr = lpString + StrLen - 1;
		do{
			if (L'/' == *ptr || L'\\' == *ptr)break;
			ptr--;
		} while (ptr != lpString);
		return ptr - lpString + 1;
	}
	UBINT GetNameExtPos(const char *lpString, UBINT StrLen){
		if (0 == StrLen)return 1;

		UBINT RetValue = StrLen + 1;
		const char *ptr = lpString + StrLen - 1;
		do{
			if ('.' == *ptr)RetValue = ptr - lpString + 1;
			else if ('/' == *ptr || '\\' == *ptr)break;
			ptr--;
		} while (ptr != lpString);
		return RetValue;
	}
	UBINT GetNameExtPos(const wchar_t *lpString, UBINT StrLen){
		if (0 == StrLen)return 1;

		UBINT RetValue = StrLen + 1;
		const wchar_t *ptr = lpString + StrLen - 1;
		do{
			if (L'.' == *ptr)RetValue = ptr - lpString + 1;
			else if (L'/' == *ptr || L'\\' == *ptr)break;
			ptr--;
		} while (ptr != lpString);
		return RetValue;
	}

	String_Sys GetWorkingDir(){
#if defined LIBENV_OS_WIN
		UINT4b PathLen = GetCurrentDirectory(0, nullptr);
		String_Sys RetValue(PathLen - 1);
		GetCurrentDirectory(PathLen, RetValue.begin());
		return std::move(RetValue);
#elif defined LIBENV_OS_LINUX
		char *lpPath = getcwd(nullptr, (size_t)-1);
		String_Sys RetValue = lpPath;
		free(lpPath);
		return std::move(RetValue);
#endif
	}
};
#endif