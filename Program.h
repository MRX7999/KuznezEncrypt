#pragma once

using namespace System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace System::IO;
using namespace System::Linq;
using namespace System::Text;
using namespace System::Threading::Tasks;
using namespace Кузнечик;

namespace Кузнечик
{
	private ref class Program
	{
		static void Main(Platform::Array<Platform::String^> ^args);

	private:
		static void EncryptFile();

		static void EncryptText();
	};
}
