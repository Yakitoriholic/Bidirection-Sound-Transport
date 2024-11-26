#include "lGeneral.hpp"
#include <cstdio>
#include <cmath>
#include <ctime>

#include "lMath_NT.hpp"
#include "lText.hpp"
#include "lText_Process.hpp"

#if defined LIBENV_OS_LINUX
#include <curses.h>
#else
#include <conio.h>
#endif

extern void Util_DeBrujinPath(){
	//print a number where all possible bit combinations of size log2(n) could be found inside.
#if defined LIBENV_SYS_INTELX64
	const UBINT BitCnt = 64, logCnt = 6, BitMask = 0x1F;
	const char ConsoleStr[] = "\nHexadecimal:0x%llX";
#else
	const UBINT BitCnt = 32, logCnt = 5, BitMask = 0xF;
	const char ConsoleStr[] = "\nHexadecimal:0x%lX";
#endif
	UBINT EdgeStat[BitCnt], PointStat[BitCnt >> 1], PointQueue[BitCnt >> 1], Ptr1 = 0, Ptr2, QHead, QTail, HexValue = 0;
	for (UBINT i = 0; i<BitCnt; i++)EdgeStat[i] = 0;
	printf("Binary:");
	for (UBINT i = 0; i<logCnt; i++)printf("0");
	for (UBINT i = 0; i<logCnt; i++){
		printf("1");
		HexValue = (HexValue << 1) + 1;
	}
	for (UBINT i = 0; i<logCnt + 1; i++){
		EdgeStat[Ptr1] = 1;
		Ptr1 = (Ptr1 << 1) + 1;
	}
	Ptr1 = BitMask;
	for (UBINT i = 0; i<BitCnt - 2 * logCnt; i++){
		if (1 == EdgeStat[Ptr1 << 1]){
			EdgeStat[(Ptr1 << 1) + 1] = 1;
			Ptr1 = ((Ptr1 << 1)&BitMask) + 1;
			printf("1");
			HexValue = (HexValue << 1) + 1;
		}
		else if (1 == EdgeStat[(Ptr1 << 1) + 1]){
			EdgeStat[Ptr1 << 1] = 1;
			Ptr1 = (Ptr1 << 1)&BitMask;
			printf("0");
			HexValue <<= 1;
		}
		else{
			EdgeStat[Ptr1 << 1] = 1;
			Ptr2 = (Ptr1 << 1)&BitMask;
			//Connectivity - backward
			for (UBINT j = 0; j<BitCnt >> 1; j++)PointStat[j] = 0;
			PointQueue[0] = Ptr2;
			PointStat[Ptr2] = 1;
			QHead = 0, QTail = 1;
			while (QHead<QTail){
				if (0 == EdgeStat[PointQueue[QHead] << 1] && 0 == PointStat[(PointQueue[QHead] << 1)&BitMask]){
					PointQueue[QTail] = (PointQueue[QHead] << 1)&BitMask;
					PointStat[(PointQueue[QHead] << 1)&BitMask] = 1;
					if (PointQueue[QTail] == Ptr1)break;
					QTail++;
				}
				if (0 == EdgeStat[(PointQueue[QHead] << 1) + 1] && 0 == PointStat[((PointQueue[QHead] << 1)&BitMask) + 1]){
					PointQueue[QTail] = ((PointQueue[QHead] << 1)&BitMask) + 1;
					PointStat[((PointQueue[QHead] << 1)&BitMask) + 1] = 1;
					if (PointQueue[QTail] == Ptr1)break;
					QTail++;
				}
				QHead++;
			}
			if (QHead<QTail){
				Ptr1 = Ptr2;
				printf("0");
				HexValue <<= 1;
			}
			else{
				EdgeStat[Ptr1 << 1] = 0;
				EdgeStat[(Ptr1 << 1) + 1] = 1;
				Ptr1 = Ptr2 + 1;
				printf("1");
				HexValue = (HexValue << 1) + 1;
			}
		}
	}
	printf(ConsoleStr, HexValue);
}
extern void Util_HashPrime(){
	//print the largest prime number which is smaller than 2^n.
#if defined LIBENV_SYS_INTELX64
	const char ConsoleStr[] = "0x%llX, ";
#else
	const char ConsoleStr[] = "0x%lX, ";
#endif
	UBINT i = 4;
	while (i > 0){
		printf(ConsoleStr, nsMath::getprime_leq(i - 1));
		i <<= 1;
	}
	printf(ConsoleStr, nsMath::getprime_leq((UBINT)-1));
}

extern void Util_Grisu_Table_Pow_of_10(int LowerBound, int UpperBound, nsBasic::Stream_W<UINT4b> *lpOutputStream){
	//We suppose that the lower bound is negative and the upper bound is positive.
	auto MyTable = nsBasic::MemAlloc_Arr<nsText::__FloatTable_Entry>(UpperBound - LowerBound + 1);
	UBINT ZeroIndex = - LowerBound;
	
	INT4b Exponent;
	UINT8b Mantissa[2];

	MyTable[ZeroIndex] = { -63, nsMath::NumericTrait<UINT4b>::HighestBit, nsMath::NumericTrait<UINT8b>::HighestBit };

	Exponent = -63;
	Mantissa[0] = nsMath::NumericTrait<UINT8b>::HighestBit;
	Mantissa[1] = 0;
	for (UBINT i = -1; i >= LowerBound; i--){
		UINT8b TmpVal0 = Mantissa[0] % 10;
		Mantissa[0] /= 10;
		Mantissa[1] /= 10;
		Mantissa[1] += 0x1999999999999999 * TmpVal0 + 3 * TmpVal0 / 5;

		UBINT TmpVal1 = 64 - nsMath::log2intsim(Mantissa[0]);
		Mantissa[0] <<= TmpVal1;
		Mantissa[0] += Mantissa[1] >> (64 - TmpVal1);
		Mantissa[1] <<= TmpVal1;
		Exponent -= (INT4b)TmpVal1;

		MyTable[ZeroIndex + i].Exponent = Exponent;

		if ((Mantissa[0] & (((UINT8b)nsMath::NumericTrait<UINT4b>::HighestBit) << 1)) > 0)MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + nsMath::NumericTrait<UINT4b>::HighestBit) >> 32);
		else MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + nsMath::NumericTrait<UINT4b>::HighestBit - 1) >> 32);

		if ((Mantissa[0] & 1) > 0){
			if (Mantissa[1] >= nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0] + 1;
			else MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0];
		}
		else{
			if (Mantissa[1] > nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0] + 1;
			else MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0];
		}
	}

	Exponent = -62;
	Mantissa[0] = nsMath::NumericTrait<UINT8b>::HighestBit >> 1;
	Mantissa[1] = 0;
	for (UBINT i = 1; i <= UpperBound; i++){
		UINT8b TmpVal0 = Mantissa[0] >> 2;
		UINT8b TmpVal1 = (Mantissa[0] << 62) + (Mantissa[1] >> 2);
		Mantissa[0] += TmpVal0;
		if ((Mantissa[1] += TmpVal1) < TmpVal1)Mantissa[0]++;
		Exponent += 3;

		if ((Mantissa[0] & nsMath::NumericTrait<UINT8b>::HighestBit) > 0){
			MyTable[ZeroIndex + i].Exponent = Exponent;

			if ((Mantissa[0] & (((UINT8b)nsMath::NumericTrait<UINT4b>::HighestBit) << 1)) > 0)MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + nsMath::NumericTrait<UINT4b>::HighestBit) >> 32);
			else MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + nsMath::NumericTrait<UINT4b>::HighestBit - 1) >> 32);

			if ((Mantissa[0] & 1) > 0){
				if (Mantissa[1] >= nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0] + 1;
				else MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0];
			}
			else{
				if (Mantissa[1] > nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0] + 1;
				else MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0];
			}

			Mantissa[1] >>= 1;
			if ((Mantissa[0] & 1) > 0)Mantissa[1] += nsMath::NumericTrait<UINT8b>::HighestBit;
			Mantissa[0] >>= 1;
			Exponent++;
		}
		else{
			MyTable[ZeroIndex + i].Exponent = Exponent - 1;

			if ((Mantissa[0] & ((UINT8b)nsMath::NumericTrait<UINT4b>::HighestBit)) > 0)MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + (nsMath::NumericTrait<UINT4b>::HighestBit >> 1)) >> 31);
			else MyTable[ZeroIndex + i].Mantissa_Float = (UINT4b)((Mantissa[0] + (nsMath::NumericTrait<UINT4b>::HighestBit >> 2) - 1) >> 31);

			if ((Mantissa[1] & nsMath::NumericTrait<UINT8b>::HighestBit) > 0){
				if ((Mantissa[1] << 1) >= nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = (Mantissa[0] << 1) + 2;
				else MyTable[ZeroIndex + i].Mantissa_Double = (Mantissa[0] << 1) + 1;
			}
			else{
				if ((Mantissa[1] << 1) > nsMath::NumericTrait<UINT8b>::HighestBit)MyTable[ZeroIndex + i].Mantissa_Double = (Mantissa[0] << 1) + 1;
				else MyTable[ZeroIndex + i].Mantissa_Double = Mantissa[0] << 1;
			}
		}
	}
	UINT4b Chars[6] = { (UINT4b)'{', (UINT4b)'}', (UINT4b)'\n', (UINT4b)',', (UINT4b)'0', (UINT4b)'x' };
	lpOutputStream->Write(&Chars[0]); lpOutputStream->Write(&Chars[2]);
	for (UBINT i = 0; i <= UpperBound - LowerBound; i++){
		lpOutputStream->Write(&Chars[0]);
		lpOutputStream->WriteBulk(&Chars[4], 2);
		nsText::WriteInt4b_Hex(lpOutputStream, MyTable[i].Exponent);
		lpOutputStream->WriteBulk(&Chars[3], 3);
		nsText::WriteUInt4b_Hex(lpOutputStream, MyTable[i].Mantissa_Float);
		lpOutputStream->WriteBulk(&Chars[3], 3);
		nsText::WriteUInt8b_Hex(lpOutputStream, MyTable[i].Mantissa_Double);
		lpOutputStream->Write(&Chars[1]);
		if (3 == (i % 4)){
			if (i != UpperBound - LowerBound)lpOutputStream->Write(&Chars[3]);
			lpOutputStream->Write(&Chars[2]);
		}
		else{
			if (i == UpperBound - LowerBound)lpOutputStream->Write(&Chars[2]); else lpOutputStream->Write(&Chars[3]);
		}
	}
	lpOutputStream->Write(&Chars[1]);
	printf("\nValidation:\n");
	for (UBINT i = 0; i <= UpperBound - LowerBound; i++){
		UINT8b TmpVal = 0;
		TmpVal += (MyTable[i].Mantissa_Double ^ nsMath::NumericTrait<UINT8b>::HighestBit) >> (nsMath::NumericTrait<double>::Exponent_Bit_Count);
		TmpVal |= ((UINT8b)MyTable[i].Exponent + nsMath::NumericTrait<double>::Exponent_Bias + 63) << nsMath::NumericTrait<double>::Mantissa_Bit_Count;
		printf("%llf\n",*(double *)&TmpVal);
	}
	nsBasic::MemFree_Arr<nsText::__FloatTable_Entry>(MyTable, UpperBound - LowerBound + 1);
}
extern int Util_Grisu_K_Double(int e, int alpha){
	return (int)ceil((alpha - e + 63) * (log(2) / log(10)));
}
extern int Util_Grisu_K_Float(int e, int alpha){
	return (int)ceil((alpha - e + 31) * (log(2) / log(10)));
}

nsText::String _SH_Type(){ return nsText::String("T"); }
nsText::String _SH_Add(nsText::String &lhs, nsText::String &rhs){ return lhs + " + " + rhs; }
nsText::String _SH_Sub(nsText::String &lhs, nsText::String &rhs){ return lhs + " - " + rhs; }
nsText::String _SH_Mul(nsText::String &lhs, nsText::String &rhs){ return lhs + " * " + rhs; }
nsText::String _SH_Bracket(nsText::String &lhs){ return "(" + lhs + ")"; }
nsText::String _SH_Const(double Val){ return "(T)" + nsText::ToString(Val); }

extern nsText::String Util_SH_Generation(
	int Order,
	nsText::String(*Func_Type)() = _SH_Type,
	nsText::String(*Func_Add)(nsText::String &, nsText::String &) = _SH_Add,
	nsText::String(*Func_Sub)(nsText::String &, nsText::String &) = _SH_Sub,
	nsText::String(*Func_Mul)(nsText::String &, nsText::String &) = _SH_Mul,
	nsText::String(*Func_Bracket)(nsText::String &) = _SH_Bracket,
	nsText::String(*Func_Const)(double) = _SH_Const){

	//generate a string containing the code for evaluating spherical harmonic bases.

	nsText::String RetValue = Func_Type() + " c0,c1,s0,s1,p0,p1,p2,zsqr;\n";
	RetValue += "c0 = x; s0 = y; zsqr = " + Func_Mul(nsText::String("z"), nsText::String("z")) + ";\n";
	char TmpStr0[] = "p0", TmpStr1[] = "s0", TmpStr2[] = "c0";
	double K_Prev[2];

	double LegendreRoot = 1.0;
	for (int m = 0; m < Order; m++){
		//preparation for calculating K
		double Factorial = 1.0;
		for (UBINT i = 2; i <= 2 * m; i++)Factorial *= (double)i;
		if (0 == m){
			Factorial = 1.0 / Factorial;
			for (int l = m; l < Order; l++){
				double K = sqrt((double)(2 * l + 1) * Factorial / (4 * M_PI));
				RetValue += "result[";
				RetValue += nsText::ToString((UBINT)((l + 1) * (l + 1) - (l + m) - 1));
				RetValue += "] = ";
				switch (l - m){
				case 0:
					RetValue += Func_Const(K * LegendreRoot);
					break;
				case 1:
					RetValue += Func_Mul(nsText::String("z"), Func_Const((double)(2 * m + 1) * K * LegendreRoot));
					break;
				case 2:
					RetValue += Func_Add(Func_Mul(nsText::String("zsqr"), Func_Const((double)((2 * m + 3) * (2 * m + 1)) / 2.0 * K * LegendreRoot)), Func_Const((double)(2 * m + 1) / -2.0 * K * LegendreRoot));
					K_Prev[1] = K;
					break;
				case 3:
					RetValue += Func_Mul(nsText::String("z"), Func_Bracket(Func_Add(Func_Mul(nsText::String("zsqr"), Func_Const((double)((2 * m + 5) * (2 * m + 3) * (2 * m + 1)) / 6.0 * K * LegendreRoot)), Func_Const((double)(4 * m * m + 8 * m + 3) / -2.0 * K * LegendreRoot))));
					K_Prev[0] = K;
					break;
				default:
				{
					nsText::String T1 = "result[" + nsText::ToString((UBINT)(l * l - (l + m))) + "]";
					nsText::String T2 = "result[" + nsText::ToString((UBINT)((l - 1) * (l - 1) - (l + m) + 1)) + "]";
					TmpStr0[1] = '0' + (l - m - 1) % 3;
					T1 = Func_Mul(Func_Mul(nsText::String("z"), T1), Func_Const(K * (double)(2 * l - 1) / (K_Prev[0] * (double)(l - m))));
					TmpStr0[1] = '0' + (l - m - 2) % 3;
					RetValue += Func_Add(T1, Func_Mul(T2, Func_Const(K * (double)(l + m - 1) / (K_Prev[1] * (double)(m - l)))));
					K_Prev[1] = K_Prev[0]; K_Prev[0] = K;
				}
					break;
				}
				RetValue += ";\n";
				Factorial *= (double)(l - m + 1) / (double)(l + m + 1);
			}
		}
		else{
			Factorial = 2.0 / Factorial;
			for (int l = m; l < Order; l++){
				double K = sqrt((double)(2 * l + 1) * Factorial / (4 * M_PI));
				TmpStr0[1] = '0' + (l - m) % 3;
				RetValue.append(TmpStr0, 2);
				RetValue += " = ";
				switch (l - m){
				case 0:
					RetValue += Func_Const(K * LegendreRoot);
					break;
				case 1:
					RetValue += Func_Mul(nsText::String("z"), Func_Const((double)(2 * m + 1) * K * LegendreRoot));
					break;
				case 2:
					RetValue += Func_Add(Func_Mul(nsText::String("zsqr"), Func_Const((double)((2 * m + 3) * (2 * m + 1)) / 2.0 * K * LegendreRoot)), Func_Const((double)(2 * m + 1) / -2.0 * K * LegendreRoot));
					K_Prev[1] = K;
					break;
				case 3:
					RetValue += Func_Mul(nsText::String("z"), Func_Bracket(Func_Add(Func_Mul(nsText::String("zsqr"), Func_Const((double)((2 * m + 5) * (2 * m + 3) * (2 * m + 1)) / 6.0 * K * LegendreRoot)), Func_Const((double)(4 * m * m + 8 * m + 3) / -2.0 * K * LegendreRoot))));
					K_Prev[0] = K;
					break;
				default:
				{
					TmpStr0[1] = '0' + (l - m - 1) % 3;
					nsText::String T1 = Func_Mul(Func_Mul(nsText::String("z"), nsText::String(TmpStr0)), Func_Const(K * (double)(2 * l - 1) / (K_Prev[0] * (double)(l - m))));
					TmpStr0[1] = '0' + (l - m - 2) % 3;
					RetValue += Func_Add(T1, Func_Mul(nsText::String(TmpStr0), Func_Const(K * (double)(l + m - 1) / (K_Prev[1] * (double)(m - l)))));
					K_Prev[1] = K_Prev[0]; K_Prev[0] = K;
				}
					break;
				}
				RetValue += ";\nresult[";
				RetValue += nsText::ToString((UBINT)((l + 1) * (l + 1) - (l + m) - 1));
				RetValue += "] = " + Func_Mul(nsText::String(TmpStr0, 2), nsText::String(TmpStr1, 2));
				RetValue += ";\nresult[";
				RetValue += nsText::ToString((UBINT)((l + 1) * (l + 1) - (l - m) - 1));
				RetValue += "] = " + Func_Mul(nsText::String(TmpStr0, 2), nsText::String(TmpStr2, 2));
				RetValue += ";\n";
				Factorial *= (double)(l - m + 1) / (double)(l + m + 1);
			}
		}
		if (m > 0 && m + 1 < Order){
			nsText::String T1 = Func_Add(Func_Mul(nsText::String("x"), nsText::String(TmpStr1)), Func_Mul(nsText::String("y"), nsText::String(TmpStr2)));
			nsText::String T2 = Func_Sub(Func_Mul(nsText::String("x"), nsText::String(TmpStr2)), Func_Mul(nsText::String("y"), nsText::String(TmpStr1)));
			TmpStr1[1] = '0' + m % 2;
			TmpStr2[1] = '0' + m % 2;
			RetValue.append(TmpStr1, 2);
			RetValue += " = " + T1 + ";\n";
			RetValue.append(TmpStr2, 2);
			RetValue += " = " + T2 + ";\n";
		}
		RetValue += "\n";
		LegendreRoot *= -(double)(2 * m + 1);
	}
	return std::move(RetValue);
}