#include "global.h"
#include "server.h"
#include <list>
#include "SimpleThread.h"

// server.cpp�̓��������Ŏg���O���[�o���ϐ�
// �������O��Ԃɂ���Ƒ��̃t�@�C������A�N�Z�X����Ȃ��Ȃ�
namespace {
	// �T�[�o�[�̑҂��󂯃\�P�b�g
	SOCKET listenSocket;

	// �N���C�A���g���X�g
	std::list<SOCKET> clients;
}

/**
 * �w�肵���\�P�b�g���m���u���b�L���O�ɂ���
 */
void toNonBlocking(SOCKET socket)
{
	// �m���u���b�L���O�ɂ���
	u_long socketMode = 1;
	ioctlsocket(socket, FIONBIO, &socketMode);
}

/**
 * 1�N���C�A���g����������
 *
 * �N���C�A���g���ؒf������Ȃǂ̃G���[�������false��Ԃ�
 */
bool processClient(SOCKET client)
{
	char buffer[0xffff] = { 0 };
	int receivedLength = global::original_api::recv(client, buffer, 0xffff, 0);
	// �N���C�A���g�\�P�b�g�̓m���u���b�L���O���[�h�ɂȂ��Ă���̂ŁC
	// WSAEWOULDBLOCK�̏ꍇ�̓G���[�ł͂Ȃ��D
	if (receivedLength < 1 && GetLastError() != WSAEWOULDBLOCK) {
		return false;
	}

	if (global::targetSocket != INVALID_SOCKET) {
		// �N���C�A���g���瑗���Ă������̂����̂܂�send����
		global::original_api::send(global::targetSocket, buffer, receivedLength, 0);
	}
	return true;
}

/**
 * �O���N���C�A���g�҂��󂯃T�[�o�[�����
 *
 * @param listenIpAddress �҂���IP�A�h���X�iipv4������)
 * @param listenPort �҂��󂯃|�[�g�ԍ�
 * @param onError ���s���ɌĂяo���R�[���o�b�N
 * @return �҂���(listen)����\�P�b�g
 */
SOCKET prepareServer(const char* listenIpAddress, const unsigned short listenPort, ErrorCallbackFunc onError)
{
	// winsock ������
	WSADATA d;
	WSAStartup(MAKEWORD(2, 0), &d);

	// �T�[�o�[�p�\�P�b�g�쐬
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.S_un.S_addr = inet_addr(listenIpAddress);
	server_address.sin_port = htons(listenPort);
	if (bind(listenSocket, (sockaddr*) &server_address, sizeof(server_address)) != 0) {
		onError("socket bind failed", GetLastError());
		return INVALID_SOCKET;
	}
	if (listen(listenSocket, 5) != 0) {
		onError("socket listen failed", GetLastError());
		return INVALID_SOCKET;
	}
	toNonBlocking(listenSocket);

	return listenSocket;
}

/**
 * �ʃX���b�h�Ŏ��s�����T�[�o�[�֐�
 */
void serverFunc()
{
	while (true) {
		// �V�����N���C�A���g�����}����
		sockaddr_in clientAddress = { 0 };
		int clientAddressLength = sizeof(clientAddress);
		SOCKET newClient = accept(listenSocket, (sockaddr*) &clientAddress, &clientAddressLength);
		if (newClient != INVALID_SOCKET) {
			toNonBlocking(newClient);
			clients.push_back(newClient);
		}

		// �N���C�A���g����f�[�^�󂯎��
		for (SOCKET client : clients) {
			if (processClient(client) == false) {
				clients.remove(client);
				break;
			}
		}

		// �����X���[�v���Ă���
		Sleep(10);
	}
}

/**
 * �T�[�o�[���n�߂�
 */
void startServer(const char* ipAddress, const unsigned short port, ErrorCallbackFunc onError)
{
	// �T�[�o�[�̗p�ӂ���
	listenSocket = prepareServer(ipAddress, port, onError);

	// �T�[�o�[��ʃX���b�h�ŊJ�n
	// �����̏�����dllmain�̒��Ȃ̂ŕʃX���b�h�ɂ��Ȃ���dll��attach�őΏۃv���Z�X���~�܂��Ă��܂����߁D
	// ����SimpleThread�C���X�^���X��dll��detach����܂Ŏc���Ă������ƁD�łȂ���access violation���o���D
	// TODO: new�����܂�delete���Ă��Ȃ�
	(new SimpleThread(serverFunc))->Start();
}