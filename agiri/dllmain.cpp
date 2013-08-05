#pragma region �����J�ݒ�

#ifdef DEBUG
#pragma comment(lib, "NCodeHook/distorm_dbg.lib")
#else
#pragma comment(lib, "NCodeHook/distorm.lib")
#endif

#pragma comment(lib, "jansson/jansson.lib")

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
#include "TcpSocketList.h"

#pragma endregion

#pragma region �G�N�X�|�[�g�֐�

// NOTE: every injected dll has to export at least one symbol - otherwise
// the OS loader will fail with STATUS_INVALID_IMAGE_FORMAT (0x0C000007B)
__declspec(dllexport) void dummyExport() {}

#pragma endregion

#pragma region �t�b�N�֐�

/**
 * ���ݐڑ����̂��ׂẴ\�P�b�g��ێ����郊�X�g
 */
TcpSocketList socketList;

#pragma region connect�̃t�b�N

CONNECT_FUNC originalConnect = nullptr;
int WINAPI hookedConnect(SOCKET socket, const sockaddr* toAddress, int addressLength)
{
	int result = originalConnect(socket, toAddress, addressLength);
	if (result == 0) {
		socketList.add(socket);
	}
	return result;
}

#pragma endregion

#pragma region WSAConnect�̃t�b�N

WSA_CONNECT_FUNC originalWSAConnect = nullptr;
int WINAPI hookedWSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
	LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos)
{
	int result = originalWSAConnect(socket, toAddress, addressLength, caller, callee, sqos, gqos);
	if (result == 0) {
		socketList.add(socket);
	}
	return result;
}

#pragma endregion

#pragma region send�̃t�b�N

SEND_FUNC originalSend = nullptr;

int WINAPI hookedSend(SOCKET socket, char* buf, int len, int flags)
{
	return originalSend(socket, buf, len, flags);
}

#pragma endregion

#pragma endregion

#pragma region �t�b�N�̎��s

// hook�͕K���O���[�o���ϐ��ɂ��邱��
NCodeHookIA32 hook;

void createHooks()
{
	originalWSAConnect = hook.createHookByName("ws2_32.dll", "WSAConnect", hookedWSAConnect);
	originalConnect = hook.createHookByName("ws2_32.dll", "connect", hookedConnect);
	originalSend = hook.createHookByName("ws2_32.dll", "send", hookedSend);
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