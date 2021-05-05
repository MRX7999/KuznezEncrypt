using namespace System;

#include "Program.h"
#include "Kuznechik.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace System::IO;
using namespace System::Linq;
using namespace System::Text;
using namespace System::Threading::Tasks;
using namespace Кузнечик;

namespace Кузнечик
{

	void Program::Main(Platform::Array<Platform::String^> ^args)
	{
		do
		{
			Console::WriteLine("\n\nДля Шифрования файла 1, а для шифрования текста - 2");
			switch (Convert::ToInt32(Console::ReadLine()))
			{
				case 1:
					EncryptFile();
					break;
				case 2:
					EncryptText();
					break;
			}
		} while (true);

	}

	void Program::EncryptFile()
	{
		Kuznechik ^Kuz = ref new Kuznechik(); //Создание экземпляра класса Кузнечик
		Console::Write("Введите путь к файлу: ");
		Platform::Array<uint8> ^fileToEncrypt = File::ReadAllBytes(Console::ReadLine());
		Console::Write("Введите пароль: ");
		Platform::Array<uint8> ^password = File::ReadAllBytes(Console::ReadLine()); //Пароль должен быть 256 бит (32 символа)
		Platform::Array<uint8> ^EncryptedFile = Kuz->KuzEncript(fileToEncrypt, password); //Получение массива байт зашифрованного файла
		Platform::Array<uint8> ^DecryptedFile = Kuz->KuzDecript(EncryptedFile, password); //Получение массива байт расшифрованного файла
	}

	void Program::EncryptText()
	{
		Kuznechik ^Kuz = ref new Kuznechik(); //Создание экземпляра класса Кузнечик
		Console::Write("Введите открытый текст: ");
		Platform::String ^textToEncrypt = Console::ReadLine();
		Platform::String ^password = "01234567890123456789012345678901"; //Пароль должен быть 256 бит (32 символа)
		Platform::Array<uint8> ^EncryptedText = Kuz->KuzEncript(Encoding::Default->GetBytes(textToEncrypt), Encoding::Default->GetBytes(password)); //Получение массива байт зашифрованного файла
		Platform::String ^EncrText = Encoding::Default->GetString(EncryptedText);
		Console::WriteLine("Шифро текст: " + EncrText);
		Platform::Array<uint8> ^DecryptedFile = Kuz->KuzDecript(EncryptedText, Encoding::Default->GetBytes(password)); //Получение массива байт расшифрованного файла
		Platform::String ^DecryptedText = Encoding::Default->GetString(DecryptedFile);
		Console::WriteLine("Расшифрованное сообщение: " + DecryptedText);
	}
}
