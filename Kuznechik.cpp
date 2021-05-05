using namespace System;

#include "Kuznechik.h"

using namespace Windows::Foundation;
using namespace System::Text;

namespace Кузнечик
{

	Platform::Array<uint8> ^Kuznechik::KuzX(Platform::Array<uint8> ^input1, Platform::Array<uint8> ^input2)
	{
		Platform::Array<uint8> ^output = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i < 16; i++)
		{
			output[i] = Convert::ToByte(input1[i] ^ input2[i]);
		}
		return output;
	}

	void Kuznechik::KuzF(Platform::Array<uint8> ^input1, Platform::Array<uint8> ^input2, Platform::Array<uint8> ^%output1, Platform::Array<uint8> ^%output2, Platform::Array<uint8> ^round_C)
	{
		Platform::Array<uint8> ^state = ref new Platform::Array<uint8>(16);
		state = KuzX(input1, round_C);
		state = KuzS(state);
		state = KuzL(state);
		output1 = KuzX(state, input2);
		output2 = input1;
	}

	void Kuznechik::KuzKeyGen(Platform::Array<uint8> ^mas_key)
	{
		#pragma region Генерация_раундовых_констант

		Platform::Array<Platform::Array<uint8>^> ^iterNum = ref new Platform::Array<Platform::Array<uint8>^>(32);
		for (int32 i = 0; i < 32; i++)
		{
			iterNum[i] = ref new Platform::Array<uint8> {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Convert::ToByte(i + 1)};
			iterC[i] = KuzL(iterNum[i]);
		}

		#pragma endregion

		#pragma region Генерация_первых_2-х_ключей

		Platform::Array<uint8> ^A = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i < 16; i++)
		{
			A[i] = mas_key[i];
		}
		Platform::Array<uint8> ^B = ref new Platform::Array<uint8>(16);
		int32 j = 0;
		for (int32 i = 16; i < 32; i++)
		{
			B[j] = mas_key[i];
			j++;
		}
		j = 0;
		iterK[0] = B;
		iterK[1] = A;

		Platform::Array<uint8> ^C = ref new Platform::Array<uint8>(16);
		Platform::Array<uint8> ^D = ref new Platform::Array<uint8>(16);
		#pragma endregion

		#pragma region Генерация_остальных_ключей

		for (int32 i = 0; i < 4; i++)
		{
			KuzF(A, B, C, D, iterC[0 + 8 * i]);
			KuzF(C, D, A, B, iterC[1 + 8 * i]);
			KuzF(A, B, C, D, iterC[2 + 8 * i]);
			KuzF(C, D, A, B, iterC[3 + 8 * i]);
			KuzF(A, B, C, D, iterC[4 + 8 * i]);
			KuzF(C, D, A, B, iterC[5 + 8 * i]);
			KuzF(A, B, C, D, iterC[6 + 8 * i]);
			KuzF(C, D, A, B, iterC[7 + 8 * i]);
			iterK[2 * i + 2] = A;
			iterK[2 * i + 3] = B;
		}

		#pragma endregion

	}

	Platform::Array<uint8> ^Kuznechik::KuzEncript(Platform::Array<uint8> ^file, Platform::Array<uint8> ^masterKey)
	{
		masterKey = Encoding::Default->GetBytes(LengthTo32Bytes(Encoding::Default->GetString(masterKey)));
		KuzKeyGen(masterKey);
		int32 NumOfBlocks; // Определение кол-ва блоков по 16 байт
		int32 NumberOfNull; // Определение кол-ва недостающих байт последнего блока
		Platform::Array<uint8> ^OriginText = file;
		Platform::Array<uint8> ^encrText = ref new Platform::Array<uint8>(0); // Массив для хранения зашифрованных байтов
		if ((file->Length % 16) == 0)
		{
			NumOfBlocks = file->Length / 16;
			Array::Resize(encrText, file->Length);
		}
		else
		{
			NumOfBlocks = (file->Length / 16) + 1;
			NumberOfNull = NumOfBlocks * 16 - file->Length;
			int32 StartLength = file->Length;
			Array::Resize(OriginText, OriginText->Length + NumberOfNull);
			Array::Resize(encrText, OriginText->Length);
			if (NumberOfNull == 1)
			{
				OriginText[OriginText->Length - 1] = 0x80;
			}
			else
			{
				for (int32 i = OriginText->Length - 1; i >= 0; i--)
				{
					if (i == OriginText->Length - 1)
					{
						OriginText[OriginText->Length - 1] = 0x81;
					}
					else if (OriginText[i] != 0)
					{
						OriginText[i + 1] = 0x01;
						break;
					}
				}
			}
		}
		for (int32 i = 0; i < NumOfBlocks; i++) // Операция зашифровки
		{
			Platform::Array<uint8> ^block = ref new Platform::Array<uint8>(16);
			for (int32 j = 0; j < 16; j++)
			{
				block[j] = OriginText[i * 16 + j];
			}
			for (int32 j = 0; j < 9; j++)
			{
				block = KuzX(block, iterK[j]);
				block = KuzS(block);
				block = KuzL(block);
			}
			block = KuzX(block, iterK[9]);
			for (int32 j = 0; j < 16; j++)
			{
				encrText[i * 16 + j] = block[j];
			}
		}
		return encrText;
	} // Функция зашифрования

	Platform::Array<uint8> ^Kuznechik::KuzDecript(Platform::Array<uint8> ^file, Platform::Array<uint8> ^masterKey)
	{
		masterKey = Encoding::Default->GetBytes(LengthTo32Bytes(Encoding::Default->GetString(masterKey)));
		KuzKeyGen(masterKey);
		int32 NumOfBlocks = file->Length / 16; // Определение кол-ва блоков по 16 байт
		Platform::Array<uint8> ^OriginText = file;
		Platform::Array<uint8> ^decrText = ref new Platform::Array<uint8>(file->Length); // Массив для хранения зашифрованных байтов
		for (int32 i = 0; i < NumOfBlocks; i++)
		{
			Platform::Array<uint8> ^block = ref new Platform::Array<uint8>(16);
			for (int32 j = 0; j < 16; j++)
			{
				block[j] = OriginText[i * 16 + j];
			}
			block = KuzX(block, iterK[9]);
			for (int32 j = 8; j >= 0; j--)
			{
				block = KuzLReverse(block);
				block = KuzSReverse(block);
				block = KuzX(block, iterK[j]);
			}
			for (int32 j = 0; j < 16; j++)
			{
				decrText[i * 16 + j] = block[j];
			}
			if (i == NumOfBlocks - 1 && (decrText[decrText->Length - 1] == 0x81 || decrText[decrText->Length - 1] == 0x80))
			{
				if (decrText[decrText->Length - 1] == 0x81)
				{
					int32 Zeros = 0;
					for (int32 j = decrText->Length - 1; j > 0; j--)
					{
						if (decrText[j] == 0x81 || decrText[j] == 0x01 || decrText[j] == 0)
						{
							Zeros++;
						}
						else
						{
							break;
						}
					}
					Array::Resize(decrText, decrText->Length - Zeros);
				}
				if (decrText[decrText->Length - 1] == 0x80)
				{
					Array::Resize(decrText, decrText->Length - 1);
				}
			}
		}
		return decrText;
	}

	Platform::Array<uint8> ^Kuznechik::KuzS(Platform::Array<uint8> ^input)
	{
		Platform::Array<uint8> ^output = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i < 16; i++)
		{
			output[i] = Pi[input[i]];
		}
		return output;
	}

	Platform::Array<uint8> ^Kuznechik::KuzSReverse(Platform::Array<uint8> ^input)
	{
		Platform::Array<uint8> ^output = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i < 16; i++)
		{
			output[i] = Pi_Reverse[input[i]];
		}
		return output;
	}

	uint8 Kuznechik::KuzMulInGF(uint8 a, uint8 b)
	{
		uint8 p = 0;
		uint8 counter;
		uint8 hi_bit_set;
		for (counter = 0; counter < 8 && a != 0 && b != 0; counter++)
		{
			if ((b & 1) != 0)
			{
				p ^= a;
			}
			hi_bit_set = safe_cast<uint8>(a & 0x80);
			a <<= 1;
			if (hi_bit_set != 0)
			{
				a ^= 0xc3; // x^8 + x^7 + x^6 + x + 1
			}
			b >>= 1;
		}
		return p;
	}

	Platform::Array<uint8> ^Kuznechik::KuzR(Platform::Array<uint8> ^input)
	{
		uint8 a_15 = 0;
		Platform::Array<uint8> ^state = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i <= 15; i++)
		{
			a_15 ^= KuzMulInGF(input[i], LVec[i]);
		}
		for (int32 i = 15; i > 0; i--)
		{
			state[i] = input[i - 1];
		}
		state[0] = a_15;
		return state;
	}

	Platform::Array<uint8> ^Kuznechik::KuzL(Platform::Array<uint8> ^input)
	{
		Platform::Array<uint8> ^state = input;
		for (int32 i = 0; i < 16; i++)
		{
			state = KuzR(state);
		}
		return state;
	}

	Platform::Array<uint8> ^Kuznechik::KuzRReverse(Platform::Array<uint8> ^input)
	{
		uint8 a_15 = input[0];
		Platform::Array<uint8> ^state = ref new Platform::Array<uint8>(16);
		for (int32 i = 0; i < 15; i++)
		{
			state[i] = input[i + 1];
		}
		for (int32 i = 15; i >= 0; i--)
		{
			a_15 ^= KuzMulInGF(state[i], LVec[i]);
		}
		state[15] = a_15;
		return state;
	}

	Platform::Array<uint8> ^Kuznechik::KuzLReverse(Platform::Array<uint8> ^input)
	{
		Platform::Array<uint8> ^state = input;
		for (int32 i = 0; i < 16; i++)
		{
			state = KuzRReverse(state);
		}
		return state;
	}

	Platform::String ^Kuznechik::LengthTo32Bytes(Platform::String ^str)
	{
		if (str->Length < 32)
		{
			int32 diff = 32 - str->Length;
			int32 j = 0;
			for (int32 i = str->Length; i < 32; i++)
			{
				str += str->Substring(j, 1);
				if (j == str->Length - 1)
				{
					j = 0;
				}
				else
				{
					j++;
				}
			}
			return str;
		}
		else if (str->Length > 32)
		{
			return str = str->Substring(0, 32);
		}
		else
		{
			return str;
		}
	}
}
