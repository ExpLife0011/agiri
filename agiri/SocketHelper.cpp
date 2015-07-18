#include "precompiled_header.hpp"
#include "SocketHelper.hpp"

bool SocketHelper::isSocket(const uint32_t& handle)
{
    return GetFileType(reinterpret_cast<HANDLE>(handle)) == FILE_TYPE_PIPE;
}

bool SocketHelper::isTcpSocket(const socket_t& sock)
{
    int socketType;
    int socketTypeLength = sizeof(socketType);
    return getsockopt(sock, SOL_SOCKET, SO_TYPE, reinterpret_cast<char*>(&socketType), &socketTypeLength) != SOCKET_ERROR
        && socketType == SOCK_STREAM;
}

bool SocketHelper::getPeerEndPoint(const socket_t& sock, uint32_t& ipAddress, uint16_t& port)
{
    sockaddr_in addr;
    int addrLen = sizeof(addr);
    if (getpeername(sock, reinterpret_cast<sockaddr*>(&addr), &addrLen) != SOCKET_ERROR) {
        ipAddress = (uint32_t) (addr.sin_addr.S_un.S_addr);
        port = ntohs(addr.sin_port);
        return true;
    }
    return false;
}

// WinAPI�ɂ̓v���Z�X���̂��ׂẴ\�P�b�g���n���h��ID�Ƌ��ɗ񋓂���֐���������Ȃ������̂�
// 1�`1024�͈̔͂�S�T�����ă\�P�b�g��T��
int SocketHelper::getAllSockets(SocketInfo result[])
{
    int socketsCount = 0;
    socket_t i = 0;
    while (i <= MaxSocketsCount) {
        uint32_t ipAddress;
        uint16_t port;
        if (SocketHelper::isSocket(i) &&
            SocketHelper::isTcpSocket(i) &&
            SocketHelper::getPeerEndPoint(i, ipAddress, port)) {
            result[socketsCount].id = i;
            result[socketsCount].host = ipAddress;
            result[socketsCount].port = port;
            socketsCount++;
            i += 4; // Winsock creates consequtive four handles, so four consequtive socket handles are the same socket.
        }
        i++;
    }
    return socketsCount;
}