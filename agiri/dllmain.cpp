#pragma region �����J�ݒ�

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

#pragma comment(lib, "ws2_32.lib")

#pragma endregion

#include <Windows.h>
#include "agiri.h"

#pragma region �G�N�X�|�[�g�֐�

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region ���C���֐�

// �����肳��{��
// �O���[�o���̈�ɂ���̂ŁC�v���Z�X�������Ă���Ԃ͂����ƕێ������
agiri::Agiri ag("127.0.0.1", 10800);

BOOL APIENTRY DllMain(HMODULE module, DWORD  reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
        agiri::start();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#pragma endregion