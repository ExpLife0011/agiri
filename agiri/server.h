#pragma once

#include <WinSock2.h>
#include <list>

typedef void(*ONRECV)(const SOCKET socket, const char* data, const int data_length);
typedef void(*ONERROR)(const char* message, const int error_code);
typedef int (WINAPI *SEND_FUNC)(SOCKET socket, char* sendBuffer, int sendLength, int flags);
typedef int (WINAPI *RECV_FUNC)(SOCKET socket, char* recvBuffer, int recvLength, int flags);

static enum class PacketType: unsigned char
{
    ListSocket = 0x01,
    InjectSend = 0x03
};

void receiveCompletely(RECV_FUNC originalRecv, SOCKET sock, const int length, char* outputBuffer)
{
    int receivedLength = 0;
    while (receivedLength < length) {
        originalRecv(sock, outputBuffer + receivedLength, length - receivedLength, 0);
    }
}

char receiveChar(RECV_FUNC originalRecv, SOCKET sock)
{
    char result;
    receiveCompletely(originalRecv, sock, sizeof(char), &result);
    return result;
}

int receiveInt32(RECV_FUNC originalRecv, SOCKET sock)
{
    int result;
    receiveCompletely(originalRecv, sock, 4, reinterpret_cast<char*>(&result));
    return result;
}

/**
 * ����\�P�b�g����1�̃p�P�b�g����M����
 * �p�P�b�g�̎�ނ���肵�C�Ԃ�l�Ƃ���
 * �p�P�b�g�̒��g�͈���packetData�ɓ���
 * ���g�̑傫����packetDataLength�ɓ���
 */
PacketType receivePacket(RECV_FUNC originalRecv, SOCKET sock, __out char* packetData, __out int* packetDataLength)
{
    // ��������p�P�b�g�Ȃ̂���ǂݎ��
    PacketType packetType = (PacketType) receiveChar(originalRecv, sock);

    // �f�[�^����ǂݎ��
    *packetDataLength = receiveInt32(originalRecv, sock);

    // �p�P�b�g�f�[�^��ǂݎ��
    receiveCompletely(originalRecv, sock, *packetDataLength, packetData);
}

void startServer(const char* ipAddress, const unsigned short port,
    ONRECV on_recv, ONERROR on_error, RECV_FUNC originalRecv)
{
    // winsock ������
    WSADATA d;
    WSAStartup(MAKEWORD(2, 0), &d);

    // �T�[�o�[�p�\�P�b�g�쐬
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.S_un.S_addr = inet_addr(ipAddress);
    server_address.sin_port = htons(port);
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) != 0) {
        on_error("socket bind failed", GetLastError());
        return;
    }
    if (listen(server_socket, 5) != 0) {
        on_error("socket listen failed", GetLastError());
    }
    // �m���u���b�L���O�ɂ���
    u_long socket_mode = 1;
    ioctlsocket(server_socket, FIONBIO, &socket_mode);

    // �N���C�A���g���X�g
    std::list<SOCKET> clients;

    while (true) {
        // �V�����N���C�A���g�̎󂯓���
        sockaddr_in client_address = { 0 };
        int client_address_length = sizeof(client_address);
        SOCKET client = accept(server_socket, (sockaddr*)&client_address, &client_address_length);
        if (client != INVALID_SOCKET) {
            clients.push_back(client);
        }

        // �ڑ��ς̃N���C�A���g����̎�M
        for (SOCKET client : clients) {
            char buffer[0xffff] = { 0 };
            int received_length = originalRecv(client, buffer, 0xffff, 0);
            if (received_length < 1 && GetLastError() != WSAEWOULDBLOCK) {
                clients.remove(client);
                break;
            }
            if (received_length > 0) {
                PacketType packetType = receivePacket()
            }
        }
        Sleep(10);
    }

    WSACleanup();
}