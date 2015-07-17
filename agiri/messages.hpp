/*
 * inject���ꂽagiri.dll�ƊO����agiri�Ή��v���O���� (mediator) �����Ƃ肷�郁�b�Z�[�W���`
 */
#pragma once
#include <cstdint>
#include "types.hpp"

const int MaxDataSize = 0xffff;

enum struct Command : uint8_t
{
    PingRequest = 0x00,
    PongResponse = 0x01,
    ListSocketRequest = 0x02,
    ListSocketResponse = 0x03,
    InjectOutgoingPacketRequest = 0x04,
};

#pragma pack(1)
struct Message
{
    Command command;
    uint32_t dataSize;
    byte_t data[MaxDataSize];
    inline uint32_t getSize() const
    {
        return sizeof(command) + sizeof(data[0]) * dataSize;
    }
};

#pragma region Requests
struct InjectOutgoingPacketRequest
{
    socket_t targetSocket;
    uint32_t packetSize;
    byte_t packetData[MaxDataSize];
};
#pragma endregion

#pragma region Responses
struct ListSocketResponse
{
    uint32_t socketsCount;
    SocketInfo sockets[MaxSocketsCount];

    uint32_t getSize() const
    {
        return sizeof(socketsCount) + sizeof(sockets[0]) * socketsCount;
    }
};
#pragma endregion

#pragma pack()