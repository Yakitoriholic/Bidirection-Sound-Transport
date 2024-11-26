/* Description: Platform-independent functions for sound playback and recording.
 * Language:C++
 * Author:***
 *
 * The following macros should be defined before you use this library:
 * Sound library:LIBENV_SLIB_DSOUND / LIBENV_SLIB_WASAPI / LIBENV_SLIB_ALSA
 *
 * You must enable multithreading to use the following functions.
 */

#ifndef LIB_AUDIO
#define LIB_AUDIO

#include "lGeneral.hpp"

namespace nsAudio{
	
	// inline void Audio_Convert(T &lhs, T &rhs)
	// template region --BEGIN--
	template < typename T, bool SigChange > struct __Audio_SigInvert;
	template < typename T1, typename T2, bool Sig1, bool Sig2, int SizeCmpResult > struct __Audio_Convert_Int;
	template < typename T1, typename T2, UBINT Cate1, UBINT Cate2 > struct __Audio_Convert_Float;
	template < typename T1, typename T2, bool IsInt > struct __Audio_Convert;
	template < typename T1, typename T2 > struct __Audio_Convert<T1, T2, true>{
		static inline void Func(T1 &lhs, T2 &rhs){
			__Audio_Convert_Int<T1, T2,
				nsMath::NumericTrait<T1>::Category == nsMath::TypeID_INT,
				nsMath::NumericTrait<T2>::Category == nsMath::TypeID_INT,
				(sizeof(T1) > sizeof(T2)) ? 1 : ((sizeof(T1) < sizeof(T2)) ? -1 : 0)>::Func(lhs, rhs);
		}
	};
	template < typename T1, typename T2 > struct __Audio_Convert<T1, T2, false>{
		static inline void Func(T1 &lhs, T2 &rhs){
			__Audio_Convert_Float<T1, T2, nsMath::NumericTrait<T1>::Category, nsMath::NumericTrait<T2>::Category>::Func(lhs, rhs);
		}
	};
	template < typename T1, typename T2 > inline void Audio_Convert(T1 &lhs, T2 &rhs){
		__Audio_Convert<T1, T2,
			(nsMath::NumericTrait<T1>::Category == nsMath::TypeID_INT || nsMath::NumericTrait<T1>::Category == nsMath::TypeID_UINT) &&
			(nsMath::NumericTrait<T2>::Category == nsMath::TypeID_INT || nsMath::NumericTrait<T2>::Category == nsMath::TypeID_UINT)>::Func(lhs, rhs);
	}
	// template region --END--

	template < typename T > inline void Audio_Convert(T &lhs, T &rhs){ lhs = rhs; }

	// inline void Audio_BulkConvert(T &lhs, T &rhs)
	// template region --BEGIN--
	template <typename T1, typename T2, bool Sig1, bool Sig2, int SizeCmpResult> struct __Audio_Convert_Bulk_Int;
	template <typename T1, typename T2, UBINT Cate1, UBINT Cate2> struct __Audio_Convert_Bulk_Float;
	template < typename T1, typename T2, bool IsInt > struct __Audio_Convert_Bulk;
	template < typename T1, typename T2 > struct __Audio_Convert_Bulk<T1, T2, true>{
		static void(*Func)(void *lpDest, void *lpSrc, UBINT Count);
	};
	template < typename T1, typename T2 > struct __Audio_Convert_Bulk<T1, T2, false>{
		static void(*Func)(void *lpDest, void *lpSrc, UBINT Count);
	};
	template < typename T1, typename T2 > struct _Audio_Convert_Bulk{
		static void(*Func)(void *lpDest, void *lpSrc, UBINT Count);
	};
	template < typename T > struct _Audio_Convert_Bulk<T, T>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){};
	};

	template < typename T1, typename T2 > void(*_Audio_Convert_Bulk<T1, T2>::Func)(void *lpDest, void *lpSrc, UBINT Count) =
		__Audio_Convert_Bulk<T1, T2,
		(nsMath::NumericTrait<T1>::Category == nsMath::TypeID_INT || nsMath::NumericTrait<T1>::Category == nsMath::TypeID_UINT) &&
		(nsMath::NumericTrait<T2>::Category == nsMath::TypeID_INT || nsMath::NumericTrait<T2>::Category == nsMath::TypeID_UINT)>::Func;
	template < typename T1, typename T2 > void(*__Audio_Convert_Bulk<T1, T2, true>::Func)(void *lpDest, void *lpSrc, UBINT Count) =
		__Audio_Convert_Bulk_Int<T1, T2,
		nsMath::NumericTrait<T1>::Category == nsMath::TypeID_INT,
		nsMath::NumericTrait<T2>::Category == nsMath::TypeID_INT, 
		(sizeof(T1) > sizeof(T2)) ? 1 : ((sizeof(T1) < sizeof(T2)) ? -1 : 0)>::Func;
	template < typename T1, typename T2 > void(*__Audio_Convert_Bulk<T1, T2, false>::Func)(void *lpDest, void *lpSrc, UBINT Count) =
		__Audio_Convert_Bulk_Float<T1, T2, nsMath::NumericTrait<T1>::Category, nsMath::NumericTrait<T2>::Category>::Func;
	// template region --BEGIN--

	template < typename T1, typename T2 > inline void Audio_BulkConvert(T1 *lpDest, T2 *lpSrc, UBINT Count){ _Audio_Convert_Bulk<T1, T2>::Func(lpDest, lpSrc, Count); };
	//convert the audio data in [lpSrc] to [lpDest] using the audio data conversion routine.

	template < typename T1, typename T2 > void _Audio_BulkChannelMerge(void *lpDest, void *lpSrc, UBINT Channels, UBINT Count);
	template < typename T1, typename T2 > inline void Audio_BulkChannelMerge(T1 *lpDest, T2 *lpSrc, UBINT Channels, UBINT Count){ _Audio_BulkChannelMerge<T1, T2>::Func(lpDest, lpSrc, Channels, Count); };
	//convert the audio data in [lpSrc] to [lpDest] using the audio data conversion routine.

	void Audio_Expand24BitTo32Bit(void *lpDest, void *lpSrc, UBINT Count);
	//convert the 24-bit audio data in [lpSrc] into 32-bit format and store the result into [lpDest]. in-place conversion is supported.

	class StreamFilter{
	protected:
		typedef StreamFilter this_type;

		struct Interface{
			UBINT(*_StreamIn)(this_type * const, const float *, UBINT);
			UBINT(*_ZeroFill)(this_type * const, UBINT);
			void(*_Flush)(this_type * const);
			void(*_Reset)(this_type * const);
			void(*_Destroy)(this_type * const);
		};

		Interface *lpInterface;

		float *lpDest;
		UBINT OutputLen;
		UBINT OutputCounter;

		StreamFilter();
	public:
		inline bool isFull() const{ return this->OutputCounter >= this->OutputLen; }

		inline void SetOutput(float *lpDest, UBINT OutputSampleCount);
		inline UBINT StreamIn(const float *lpSrc, UBINT InputSampleCount);
		//return the number of samples consumed by theis operation.
		inline UBINT ZeroFill(UBINT InputSampleCount);
		//return the number of samples consumed by theis operation.
		inline void Flush();
		//flush the filter with zero till the output buffer is full.
		inline void Reset();
		inline void Destroy();

		inline ~StreamFilter();
	};

	class MultiStreamFilter{
	protected:
		typedef StreamFilter this_type;

		struct Interface{
			UBINT(*_StreamIn)(this_type * const, const float *, UBINT);
			UBINT(*_ZeroFill)(this_type * const, UBINT);
			void(*_Flush)(this_type * const);
			void(*_Reset)(this_type * const);
			void(*_Destroy)(this_type * const);
		};

		Interface *lpInterface;

		float *lpDest;
		UBINT OutputLen;
		UBINT OutputCounter;

		MultiStreamFilter();
	public:
		inline bool isFull() const{ return this->OutputCounter >= this->OutputLen; }

		inline void SetOutput(float **lpDest, UBINT StreamCnt, UBINT OutputSampleCount);
		inline UBINT StreamIn(const float **lpSrc, UBINT StreamCnt, UBINT InputSampleCount);
		//return the number of samples consumed by theis operation.
		inline UBINT ZeroFill(UBINT InputSampleCount);
		//return the number of samples consumed by theis operation.
		inline void Flush();
		//flush the filter with zero till the output buffer is full.
		inline void Reset();
		inline void Destroy();

		inline ~MultiStreamFilter();
	};

	class OutputStream;
	class OutputDevice{
	protected:
		typedef OutputDevice this_type;

	private:
		this_type(const this_type &) = delete; //not copyable
		this_type & operator =(const this_type &) = delete; //not copyable

	protected:
		struct Interface{
			OutputStream *(*_CreateStream_Output)(this_type * const, UBINT SampleTypeID, UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec);
			void(*_DestroyStream_Output)(this_type * const, OutputStream * const);
			void(*_Destroy)(this_type * const);
		};

		Interface *lpInterface;
		OutputDevice(){};
	public:
		template <typename T> inline OutputStream *CreateStream_Output(UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec); // Channels should be 1 or 2, BitsPerSamples should be 8 or 16.
		inline void DestroyStream(OutputStream *lpStream);
		inline void Destroy();
		
		inline ~OutputDevice();
	};

	class OutputStream{
	protected:
		typedef OutputStream this_type;

	private:
		OutputStream(const OutputStream &) = delete; //not copyable
		OutputStream & operator =(const OutputStream &) = delete; //not copyable
	protected:
		struct Interface{
			void(*_Play)(this_type * const);
			void(*_Pause)(this_type * const);
			void(*_Stop)(this_type * const);
			void *(*_WaitFrame)(this_type * const);
			void(*_SubmitFrame)(this_type * const);
		};

		Interface *lpInterface;
		bool _Playing; //readonly

		OutputStream(){ this->_Playing = false; };
	public:
		inline bool isPlaying() const { return this->_Playing; }

		inline void Play();
		inline void Pause();
		inline void Stop();
		inline void *WaitFrame();
		inline void SubmitFrame();
	};

	/*-------------------------------- IMPLEMENTATION --------------------------------*/

	template < typename T > struct __Audio_SigInvert<T, true>{
		static void Func(T &rhs){ rhs ^= nsMath::NumericTrait<T>::HighestBit; }
	};
	template < typename T > struct __Audio_SigInvert<T, false>{
		static void Func(T &rhs){}
	};
	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Int<T1, T2, Sig1, Sig2, 1>{
		static inline void Func(T1 &lhs, T2 &rhs){
			__Audio_SigInvert<T2, Sig2>::Func(rhs);
			lhs = (T1)rhs;
			for (UBINT i = 1; i < nsMath::log2intsim_t<sizeof(T1) / sizeof(T2)>::Value; i++){
				lhs += lhs << ((sizeof(T1) * 8) >> (nsMath::log2intsim_t<sizeof(T1) / sizeof(T2)>::Value - i));
			}
			__Audio_SigInvert<T1, Sig1>::Func(lhs);
		}
	};
	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Int<T1, T2, Sig1, Sig2, -1>{
		static inline void Func(T1 &lhs, T2 &rhs){
			lhs = (T1)(rhs >> ((sizeof(T2)-sizeof(T1)) * 8));
			__Audio_SigInvert<T1, Sig1 ^ Sig2>::Func(lhs);
		}
	};
	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Int<T1, T2, Sig1, Sig2, 0>{
		// We always have Sig1 != Sig2.
		static inline void Func(T1 &lhs, T2 &rhs){
			lhs = (T1)(rhs) ^ nsMath::NumericTrait<T1>::HighestBit;
		}
	};

	template <typename T1, typename T2> struct __Audio_Convert_Float<T1, T2, nsMath::TypeID_INT, nsMath::TypeID_FLOAT>{
		static inline void Func(T1 &lhs, T2 &rhs){
			T2 TmpFloat = (rhs + (T2)1) / (T2)2;
			nsMath::NumericTrait<T1>::uint_type TmpUInt;
			TmpUInt = (decltype(TmpUInt))(TmpFloat * (T2)nsMath::NumericTrait<decltype(TmpUInt)>::Max);
			TmpUInt ^= nsMath::NumericTrait<decltype(TmpUInt)>::HighestBit;
			lhs = (T1)TmpUInt;
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_INT>{
		static inline void Func(T1 &lhs, T2 &rhs){
			nsMath::NumericTrait<T2>::uint_type TmpUInt;
			TmpUInt = (decltype(TmpUInt))rhs ^ nsMath::NumericTrait<decltype(TmpUInt)>::HighestBit;
			T1 TmpFloat = (T1)TmpUInt / (T1)nsMath::NumericTrait<decltype(TmpUInt)>::Max;
			lhs = TmpFloat * (T1)2 - (T1)1;
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Float<T1, T2, nsMath::TypeID_UINT, nsMath::TypeID_FLOAT>{
		static inline void Func(T1 &lhs, T2 &rhs){
			T2 TmpFloat = (rhs + (T2)1) / (T2)2;
			lhs = (T1)(TmpFloat * (T2)nsMath::NumericTrait<T1>::Max);
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_UINT>{
		static inline void Func(T1 &lhs, T2 &rhs){
			T1 TmpFloat = (T1)rhs / (T1)nsMath::NumericTrait<T2>::Max;
			lhs = TmpFloat * (T1)2 - (T1)1;
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_FLOAT>{
		static void Func(T1 &lhs, T2 &rhs){
			lhs = (T1)rhs;
		}
	};

	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Bulk_Int<T1, T2, Sig1, Sig2, 1>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T2 *lpDest_T = (T2 *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++){
				T2 TmpInt = lpSrc_T[i];
				__Audio_SigInvert<T2, Sig2>::Func(TmpInt);
				for (UBINT j = 0; j < sizeof(T1) / sizeof(T2) - 1; j++)lpDest_T[i*(sizeof(T1) / sizeof(T2)) + j] = TmpInt;
				__Audio_SigInvert<T2, Sig1>::Func(TmpInt);
				lpDest_T[(i + 1)*(sizeof(T1) / sizeof(T2)) - 1] = TmpInt;
			}
		}
	};
	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Bulk_Int<T1, T2, Sig1, Sig2, -1>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T1 *lpDest_T = (T1 *)lpDest;
			T1 *lpSrc_T = (T1 *)lpSrc;
#if defined LIBENV_SYS_INTELX86 || defined LIBENV_SYS_INTELX64
			for (UBINT i = 0; i < Count; i++){
				lpDest_T[i] = lpSrc_T[(i + 1)*(sizeof(T2) / sizeof(T1)) - 1];
				__Audio_SigInvert<T1, Sig1 ^ Sig2>::Func(lpDest_T[i]);
			}
#else
			// on big-endian machine
			//for (UBINT i = 0; i < Count; i++){
			//	lpDest[i] = lpSrc[i*(sizeof(T2) / sizeof(T1))];
			//	__Audio_SigInvert<T1, Sig1 ^ Sig2>::Func(lpDest_T[i]);
			//}
#endif
		}
	};
	template <typename T1, typename T2, bool Sig1, bool Sig2> struct __Audio_Convert_Bulk_Int<T1, T2, Sig1, Sig2, 0>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T2 *lpDest_T = (T2 *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++)lpDest_T[i] = lpSrc_T[i] ^ nsMath::NumericTrait<T2>::HighestBit;
		}
	};
	
	template <typename T1, typename T2> struct __Audio_Convert_Bulk_Float<T1, T2, nsMath::TypeID_INT, nsMath::TypeID_FLOAT>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			nsMath::NumericTrait<T1>::uint_type *lpDest_T = (nsMath::NumericTrait<T1>::uint_type *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++){
				T2 TmpFloat = (lpSrc_T[i] + (T2)1) / (T2)2;
				lpDest_T[i] = (T1)(TmpFloat * (T2)nsMath::NumericTrait<nsMath::NumericTrait<T1>::uint_type>::Max);
				lpDest_T[i] ^= nsMath::NumericTrait<T1>::HighestBit;
			}
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Bulk_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_INT>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T1 *lpDest_T = (T1 *)lpDest;
			nsMath::NumericTrait<T2>::uint_type *lpSrc_T = (nsMath::NumericTrait<T2>::uint_type *)lpSrc;
			for (UBINT i = 0; i < Count; i++){
				T1 TmpFloat = (T1)(lpSrc_T[i] ^ nsMath::NumericTrait<std::remove_reference<decltype(lpSrc_T[i])>::type>::HighestBit) / (T1)nsMath::NumericTrait<std::remove_reference<decltype(lpSrc_T[i])>::type>::Max;
				lpDest_T[i] = TmpFloat * (T1)2 - (T1)1;
			}
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Bulk_Float<T1, T2, nsMath::TypeID_UINT, nsMath::TypeID_FLOAT>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T1 *lpDest_T = (T1 *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++){
				T2 TmpFloat = (lpSrc_T[i] + (T2)1) / (T2)2;
				lpDest_T[i] = (T1)(TmpFloat * (T2)nsMath::NumericTrait<T1>::Max);
			}
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Bulk_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_UINT>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T1 *lpDest_T = (T1 *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++){
				T1 TmpFloat = (T1)lpSrc_T[i] / (T1)nsMath::NumericTrait<T2>::Max;
				lpDest_T[i] = TmpFloat * (T1)2 - (T1)1;
			}
		}
	};
	template <typename T1, typename T2> struct __Audio_Convert_Bulk_Float<T1, T2, nsMath::TypeID_FLOAT, nsMath::TypeID_FLOAT>{
		static void Func(void *lpDest, void *lpSrc, UBINT Count){
			T1 *lpDest_T = (T1 *)lpDest;
			T2 *lpSrc_T = (T2 *)lpSrc;
			for (UBINT i = 0; i < Count; i++)lpDest_T[i] = (T1)lpSrc_T[i];
		}
	};

	template < typename T1, typename T2 > void _Audio_BulkChannelMerge(void *lpDest, void *lpSrc, UBINT Channels, UBINT Count){
		T2 TmpVal;
		for (UBINT i = 0; i < Count; i++){
			TmpVal = (T2)0;
			for (UBINT j = 0; j < Channels; j++){
				TmpVal += ((T2 *)lpSrc)[i*Channels + j] / (T2)Channels;
			}
			Audio_Convert(((T1 *)lpDest)[i], TmpVal);
		}
	}

	void Audio_Expand24BitTo32Bit(void *lpDest, void *lpSrc, UBINT Count){
		UBINT LoopCount = Count / 4;
		unsigned char *lpDest_Unpacked = (unsigned char *)lpDest + 4 * Count;
		unsigned char *lpSrc_Unpacked = (unsigned char *)lpDest + 3 * Count;
		for (UBINT i = 0; i < LoopCount; i++){
			lpDest_Unpacked -= 16;
			lpSrc_Unpacked -= 12;
			
			UINT4b TmpInt[4];
			TmpInt[0] = ((UINT4b *)lpSrc_Unpacked)[0];
			TmpInt[1] = ((UINT4b *)lpSrc_Unpacked)[1];
			TmpInt[2] = ((UINT4b *)lpSrc_Unpacked)[2];

			TmpInt[3] = TmpInt[2] & 0xFFFFFF00;
			TmpInt[2] = (TmpInt[2] << 24) + ((TmpInt[1] >> 8) & 0xFFFF00);
			TmpInt[1] = (TmpInt[1] << 16) + (TmpInt[0] >> 24);
			TmpInt[0] <<= 8;

			((UINT4b *)lpDest_Unpacked)[0] = TmpInt[0];
			((UINT4b *)lpDest_Unpacked)[1] = TmpInt[1];
			((UINT4b *)lpDest_Unpacked)[2] = TmpInt[2];
			((UINT4b *)lpDest_Unpacked)[3] = TmpInt[3];
		}

		Count -= LoopCount * 4;
		for (UBINT i = 1; i < Count; i++){
			lpDest_Unpacked -= 4;
			lpSrc_Unpacked -= 3;
			lpDest_Unpacked[0] = 0;
			lpDest_Unpacked[1] = lpSrc_Unpacked[0];
			lpDest_Unpacked[2] = lpSrc_Unpacked[1];
			lpDest_Unpacked[3] = lpSrc_Unpacked[2];
		}
	}

	template <typename T> OutputStream *OutputDevice::CreateStream_Output(UBINT Channels, UBINT SamplePerFrame, UBINT SamplePerSec){
		OutputStream *RetValue = this->lpInterface->_CreateStream_Output(this, __typeid(T), Channels, SamplePerFrame, SamplePerSec);
		if (nullptr == RetValue)throw std::exception("Failed to create new output stream.");
		return RetValue;
	}

	StreamFilter::StreamFilter(){ this->lpDest = nullptr; this->OutputLen = 0; this->OutputCounter = 0; }
	void StreamFilter::SetOutput(float *lpDest, UBINT OutputSampleCount){ this->lpDest = lpDest; this->OutputLen = OutputSampleCount; this->OutputCounter = 0; }
	UBINT StreamFilter::StreamIn(const float *lpSrc, UBINT InputSampleCount){
		if (nullptr == this->lpDest || this->OutputLen <= this->OutputCounter || 0 == InputSampleCount)return 0;
		else return this->lpInterface->_StreamIn(this, lpSrc, InputSampleCount);
	}
	UBINT StreamFilter::ZeroFill(UBINT InputSampleCount){
		if (nullptr == this->lpDest || this->OutputLen <= this->OutputCounter || 0 == InputSampleCount)return 0;
		else return this->lpInterface->_ZeroFill(this, InputSampleCount);
	}
	void StreamFilter::Flush(){ if (nullptr != this->lpDest && this->OutputLen > this->OutputCounter){ this->lpInterface->_Flush(this); this->OutputCounter = this->OutputLen; } }
	void StreamFilter::Reset(){ this->lpInterface->_Reset(this); }
	void StreamFilter::Destroy(){ this->lpInterface->_Destroy(this); }
	StreamFilter::~StreamFilter(){ this->Destroy(); }

	void OutputDevice::DestroyStream(OutputStream *lpStream){ this->lpInterface->_DestroyStream_Output(this, lpStream); }
	void OutputDevice::Destroy(){ this->lpInterface->_Destroy(this); }
	OutputDevice::~OutputDevice(){ this->Destroy(); }

	void OutputStream::Play(){ this->lpInterface->_Play(this); this->_Playing = true; }
	void OutputStream::Pause(){ this->lpInterface->_Pause(this); }
	void OutputStream::Stop(){ this->lpInterface->_Stop(this); this->_Playing = false; }
	void *OutputStream::WaitFrame(){ return this->lpInterface->_WaitFrame(this); }
	void OutputStream::SubmitFrame(){ this->lpInterface->_SubmitFrame(this); }
}
#endif