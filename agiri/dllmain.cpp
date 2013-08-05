#pragma region �����J�ݒ�

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

#pragma comment(lib, "ws2_32.lib")

#pragma endregion

#pragma region �w�b�_�̃C���N���[�h

// Windows.h��Winsock2.h�̗������C���N���[�h�������̒�`�Փ˂��������s�v�c�̎���
#define WIN32_LEAN_AND_MEAN

// NCodeHook�̒���Windows.h�������ɃC���N���[�h�����
#include "NCodeHook/NCodeHookInstantiation.h"

#include "WinsockFunctionTypedef.h"
#include "Server.h"
#include "SimpleThread.h"

#pragma endregion

#pragma region �G�N�X�|�[�g�֐�

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region connect�̃t�b�N

CONNECT_FUNC originalConnect = nullptr;
int WINAPI hookedConnect(SOCKET socket, const sockaddr* toAddress, int addressLength)
{
	const char* ipAddressText = inet_ntoa(reinterpret_cast<const sockaddr_in*>(toAddress)->sin_addr);
	return originalConnect(socket, toAddress, addressLength);
}

#pragma endregion

#pragma region �t�b�N�̎��s

// hook�͕K���O���[�o���ϐ��ɂ��邱��
NCodeHookIA32 hook;

void createHooks()
{
	originalConnect = hook.createHookByName("ws2_32.dll", "connect", hookedConnect);
}

#pragma endregion

#pragma region �t�b�N��̏���

// serverThread�͕K���O���[�o���ϐ��ɂ��邱��
SimpleThread* serverThread = nullptr;
void afterHook()
{
	// TODO: �����ɃX���b�h�N���̏���������
	serverThread = new SimpleThread(receiveForever);
	serverThread->Start();
}

#pragma endregion

#pragma region ���C���֐�

BOOL APIENTRY DllMain(HMODULE module, DWORD  reason, LPVOID reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		createHooks();
		afterHook();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#pragma endregion