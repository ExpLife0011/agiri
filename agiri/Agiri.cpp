#include "Agiri.h"
#include "MiniServer.h"
#include "SimpleThread.h"
#include "original_funcs.h"
#include "hooked_funcs.h"

namespace agiri
{

    /**
     * threadFunc�ŕK�v�ƂȂ�IP�A�h���X
     * threadFunc��static�֐��Ȃ̂ŁC��������A�N�Z�X�ł���悤�ɂ���ɂ�
     * ���̂悤�ɕϐ����̂�static�ɂ��Ă��������Ȃ��D
     * �ł����Agiri�N���X�̃R���X�g���N�^����n�������������E�E
     */
    static const char* serverIpAddress = nullptr;

    /**
     * threadFunc�ŕK�v�ƂȂ�|�[�g�ԍ�
     * IP�A�h���X�Ɠ������R�ɂ��static�ł���
     */
    static unsigned short serverPort = 0;

    /**
     * �ʃX���b�h�Ŏ��s����֐�
     * agiri::server::MiniServer::receiveForever�����̂܂�SimpleThread�ɓn�������������C
     * SimpleThread���󂯕t����͈̂����Ȃ��̊֐������Ȃ̂ŁC
     * this�|�C���^������Ŏ󂯎���Ă��郁���o�֐��͓n���Ȃ�
     * ������C������static�ȃX���b�h���s�p�̊֐������C
     * ���g�ł̓T�[�o�[�𗧂Ăĉi���҂��󂯂��邾���̏��������Ă���
     */
    static void threadFunc()
    {
        // �T�[�o�[�𗧂Ă�
        server::MiniServer(serverIpAddress, serverPort).receiveForever();
    }

    /**
     * �V���������肳������
     *
     * @param serverIpAddress ��������send���󂯎��T�[�o�[�̑҂���IP�A�h���X
     * @param serverPort ��������send���󂯎��T�[�o�[�̑Ҏ�|�[�g�ԍ�
     */
    Agiri::Agiri(const char* serverIpAddress, const unsigned short serverPort)
        : serverThread(new SimpleThread(agiri::threadFunc)), hook()
    {
        agiri::serverIpAddress = serverIpAddress;
        agiri::serverPort = serverPort;
        this->serverThread->Start();
    }


    Agiri::~Agiri()
    {
        if (this->serverThread != nullptr) {
            delete this->serverThread;
        }
    }

    void Agiri::start()
    {
        this->createHooks();
    }


    void Agiri::createHooks()
    {
        hook::original::WSAConnect = this->hook.createHookByName("ws2_32.dll", "WSAConnect", hook::WSAConnect);
        hook::original::connect = this->hook.createHookByName("ws2_32.dll", "connect", hook::connect);
        hook::original::send = this->hook.createHookByName("ws2_32.dll", "send", hook::send);
    }
}