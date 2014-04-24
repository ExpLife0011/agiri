#pragma once

#pragma region �w�b�_�C���N���[�h

// Windows.h��Winsock2.h�̗������C���N���[�h�������̒�`�Փ˂��������s�v�c�̎���
#define WIN32_LEAN_AND_MEAN

// WinAPI�t�b�N�̃��C�u���� NCodeHook
// NCodeHook�̒���Windows.h�������ɃC���N���[�h�����
#include "NCodeHook/NCodeHookInstantiation.h"

#pragma endregion

namespace agiri {

    class SimpleThread;

    class Agiri
    {
    public:
        Agiri(const char* serverIpAddress, const unsigned short serverPort);
        ~Agiri();
        void start();

    private:
        SimpleThread* serverThread;
        NCodeHookIA32 hook;

        void createHooks();
    };
}