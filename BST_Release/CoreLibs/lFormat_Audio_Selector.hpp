/* Description:Functions for Bitmaps.
 * Language:C++
 * Author:***
 *
 */

#ifndef LIB_FORMAT_AUDIO_SELECTOR
#define LIB_FORMAT_AUDIO_SELECTOR

#include "lAlg.hpp"
#include "lFormat_Audio.hpp"

#ifdef LIB_FORMAT_WAV
#define LIB_FORMAT_AUDIO_AVAILABLE
#endif

#ifdef LIB_FORMAT_OGG
#define LIB_FORMAT_AUDIO_AVAILABLE
#endif

#ifndef LIB_FORMAT_AUDIO_AVAILABLE
#error This header requires the user to include at least one audio format coder/decoder in previous.
#endif

namespace nsFormat{
	const char *_Audio_ExtNameList[] = {
#ifdef LIB_FORMAT_OGG
		"OGG",
#endif
#ifdef LIB_FORMAT_WAV
		"WAV"
#endif
	};
	const UBINT _Audio_ExtNameList_MaxLen = 3;

#ifdef LIB_FORMAT_OGG
	AudioReader *_GetAudioReader_OGG();
#endif
#ifdef LIB_FORMAT_WAV
	AudioReader *_GetAudioReader_WAV();
#endif

	AudioReader *(* _GetAudioReader_Routine_List[])() = {
#ifdef LIB_FORMAT_OGG
		_GetAudioReader_OGG,
#endif
#ifdef LIB_FORMAT_WAV
		_GetAudioReader_WAV
#endif
	};

	AudioReader *GetAudioReader(const char *lpExtName, UBINT Length);
	AudioReader *GetAudioReader(const wchar_t *lpExtName, UBINT Length);
	inline AudioReader *GetAudioReader(const nsText::String *lpExtName){ return GetAudioReader((const char *)lpExtName->cbegin(), lpExtName->size()); }
	inline AudioReader *GetAudioReader(const nsText::String_W *lpExtName){ return GetAudioReader(lpExtName->cbegin(), lpExtName->size()); };

	AudioReader *GetAudioWriter(const char *ExtName, UBINT Length);

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

#ifdef LIB_FORMAT_OGG
	AudioReader *_GetAudioReader_OGG(){ return new FormatReader_OGG; }
#endif

#ifdef LIB_FORMAT_WAV
	AudioReader *_GetAudioReader_WAV(){ return new FormatReader_WAV; }
#endif

	AudioReader *GetAudioReader(const char *lpExtName, UBINT Length){
		if (Length > _Audio_ExtNameList_MaxLen)return nullptr;
		//do some text processing
		char ExtName_Processed[_Audio_ExtNameList_MaxLen + 1];
		UBINT i;
		for (i = 0; i < Length; i++){
			if (lpExtName[i] > 0x7F)return nullptr;
			ExtName_Processed[i] = lpExtName[i];
			if (ExtName_Processed[i] >= 'a' && ExtName_Processed[i] <= 'z')ExtName_Processed[i] -= 'a' - 'A';
		}
		ExtName_Processed[i] = '\0';

		struct StrCmpFunctor{
			inline int operator()(const char *lhs, const char *rhs) const{
				return strcmp(lhs, rhs);
			}
		};

		UBINT _Audio_ExtNameList_Length = sizeof(_Audio_ExtNameList) / sizeof(const char *);
		UBINT RoutineIndex = nsAlg::Binary_Search(_Audio_ExtNameList, _Audio_ExtNameList + _Audio_ExtNameList_Length, ExtName_Processed, StrCmpFunctor());
		if ((UBINT)-1 == RoutineIndex)return nullptr;
		else if (nullptr == _GetAudioReader_Routine_List[RoutineIndex])return nullptr;
		else return _GetAudioReader_Routine_List[RoutineIndex]();
	}
	AudioReader *GetAudioReader(const wchar_t *lpExtName, UBINT Length){
		if (Length > _Audio_ExtNameList_MaxLen)return nullptr;
		//do some text processing
		char ExtName_Processed[_Audio_ExtNameList_MaxLen + 1];
		UBINT i;
		for (i = 0; i < Length; i++){
			if (lpExtName[i] > 0x7F)return nullptr; //suppose that all the valid extension name use characters in ASCII charset.
			ExtName_Processed[i] = (char)lpExtName[i];
			if (ExtName_Processed[i] >= 'a' && ExtName_Processed[i] <= 'z')ExtName_Processed[i] -= 'a' - 'A';
		}
		ExtName_Processed[i] = '\0';

		struct StrCmpFunctor{
			inline int operator()(const char *lhs, const char *rhs) const{
				return strcmp(lhs, rhs);
			}
		};

		UBINT _Audio_ExtNameList_Length = sizeof(_Audio_ExtNameList) / sizeof(const char *);
		UBINT RoutineIndex = nsAlg::Binary_Search(_Audio_ExtNameList, _Audio_ExtNameList + _Audio_ExtNameList_Length, ExtName_Processed, StrCmpFunctor());
		if ((UBINT)-1 == RoutineIndex)return nullptr;
		else if (nullptr == _GetAudioReader_Routine_List[RoutineIndex])return nullptr;
		else return _GetAudioReader_Routine_List[RoutineIndex]();
	}

	AudioReader *GetAudioWriter(const char *ExtName, UBINT Length){
		return nullptr;
	}
}
#endif