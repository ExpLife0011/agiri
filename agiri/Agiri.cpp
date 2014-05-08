// Windows.h��Winsock2.h�̗������C���N���[�h�������̒�`�Փ˂��������s�v�c�̎���
#define WIN32_LEAN_AND_MEAN

// WinAPI�t�b�N�̃��C�u���� NCodeHook
// NCodeHook�̒���Windows.h�������ɃC���N���[�h�����
#include "NCodeHook/NCodeHookInstantiation.h"

#include <WinSock2.h>

#include "agiri.h"
#include "SimpleThread.h"
#include "server.h"

namespace agiri
{
    /**
     * API�t�b�N�{��
     */
    NCodeHookIA32 hooker;

    const SimpleThread* serverThread = nullptr;

    /**
     * threadFunc�ŕK�v�ƂȂ�IP�A�h���X
     */
    const char* serverIpAddress = nullptr;

    /**
     * threadFunc�ŕK�v�ƂȂ�|�[�g�ԍ�
     */
    unsigned short serverPort = 0;

#pragma region ���̊֐��ւ̎Q��

    namespace original
    {
        int (WINAPI *connect)(SOCKET socket, const sockaddr* toAddress, int addressLength);
        int (WINAPI *WSAConnect)(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos);
        int (WINAPI *send)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
        int (WINAPI *recv)(SOCKET socket, char* recvBuffer, int recvLength, int flags);
    }

#pragma endregion

#pragma region �t�b�N�֐�

    namespace hooked
    {
        int WINAPI connect(SOCKET socket, const sockaddr* toAddress, int addressLength)
        {
            int result = original::connect(socket, toAddress, addressLength);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI WSAConnect(SOCKET socket, const sockaddr* toAddress, int addressLength,
            LPWSABUF caller, LPWSABUF callee, LPQOS sqos, LPQOS gqos)
        {
            int result = original::WSAConnect(socket, toAddress, addressLength, caller, callee, sqos, gqos);
            if (result == 0) {
                // socketList.add(socket);
            }
            return result;
        }

        int WINAPI send(SOCKET socket, char* buf, int len, int flags)
        {
            return original::send(socket, buf, len, flags);
        }
    }

#pragma endregion

    void createHooks()
    {
        original::WSAConnect = hooker.createHookByName("ws2_32.dll", "WSAConnect", hooked::WSAConnect);
        original::connect = hooker.createHookByName("ws2_32.dll", "connect", hooked::connect);
        original::send = hooker.createHookByName("ws2_32.dll", "send", hooked::send);
    }

    void start()
    {
        createHooks();
    }

    /**
    * �ʃX���b�h�Ŏ��s����֐�
    * startServer�����̂܂�SimpleThread�ɓn�������������C
    * SimpleThread���󂯕t����͈̂����Ȃ��̊֐������Ȃ̂ŁC
    * this�|�C���^������Ŏ󂯎���Ă��郁���o�֐��͓n���Ȃ�
    */
    void threadFunc()
    {
        // �T�[�o�[�𗧂Ă�
        startServer(serverIpAddress, serverPort);
    }

    /**
    * �V���������肳������
    *
    * @param serverIpAddress ��������send���󂯎��T�[�o�[�̑҂���IP�A�h���X
    * @param serverPort ��������send���󂯎��T�[�o�[�̑Ҏ�|�[�g�ԍ�
    */
    Agiri::Agiri(const char* serverIpAddress, const unsigned short serverPort)
    {
        agiri::serverIpAddress = serverIpAddress;
        agiri::serverPort = serverPort;
        agiri::serverThread = new SimpleThread(agiri::threadFunc);
        agiri::serverThread->Start();
    }
}