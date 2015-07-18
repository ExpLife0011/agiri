// �����̃t�@�C�����狤�p����O���[�o���ϐ����`����
// �O���[�o���ϐ���global�Ƃ������O��Ԃɕ����߁C���O�������Ȃ��悤�ɂ���
// 
// �{���w�b�_�ŃO���[�o���ϐ��͒�`�ł��Ȃ����CMicrosoft�Ǝ���selectany�ɂ��
// �����J��1�x������`��F�������邱�Ƃɂ��C��d��`�̃G���[��������O���[�o���ϐ����`���Ă���
// �ڍׂ� http://msdn.microsoft.com/ja-jp/library/5tkz6s71.aspx

#pragma once
#include "precompiled_header.hpp"
#include "RequestHandler.hpp"

// C++11����using��typedef�̑���Ɏg����悤�ɂȂ��Ă���
// �Ή���VS2013����H
using RecvFunc = int (WINAPI*) (SOCKET socket, char* recvBuffer, int recvLength, int flags);
using SendFunc = int (WINAPI*) (SOCKET socket, char* sendBuffer, int sendLength, int flags);
using ConnectFunc = int (WINAPI*) (SOCKET socket, const sockaddr* address, int addressLength);

namespace global {
	// �t�b�N����Winsock�̌��̊֐�
	namespace original_api {
		__declspec(selectany) RecvFunc recv;
		__declspec(selectany) SendFunc send;
		__declspec(selectany) ConnectFunc connect;
	}

    // ���̃v���Z�X��recv�����p�P�b�g���O����sniffer�ɗ������ǂ���
    // ���̃Z�b�g�Ƀ\�P�b�gID-key���܂܂�Ă���ꍇ��value��RequestHandler�ɗ���
    __declspec(selectany) std::unordered_map<socket_t, const RequestHandler*> incomingPacketSniffers;
}