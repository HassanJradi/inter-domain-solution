/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_H
#define UTILS_H

#include "ns3/ipv6-header.h"

using namespace std;

namespace ns3
{
	struct SignalingMessage
	{
		uint8_t command;
		uint8_t *data;
		uint16_t dataLength;

		static uint8_t *Serialize(SignalingMessage *signalingMessage, uint16_t &length);
		static SignalingMessage *Deserialize(uint8_t *serializedSignalingMessage, uint16_t length);
	};

	uint8_t *SerializeHeader(Ipv6Header *header, uint16_t &length);
    Ipv6Header *DeserializeHeader(uint8_t *serializedHeader, uint16_t length);

    string ConvertPointerUint8ToString(uint8_t *serializedNumber, uint16_t length);
	uint8_t *ConvertStringToPointerUint8(string s, uint16_t length);

	uint8_t *ConvertUint64ToPointerUint8(uint64_t number);
	uint64_t ConvertPointerUint8ToUint64(uint8_t *serializedNumber);

	uint64_t ConvertStringToUint64(string s, uint16_t length);
	string ConvertUint64ToString(uint64_t number);

	uint8_t *ConvertUint32ToPointerUint8(uint32_t number);
	uint32_t ConvertPointerUint8ToUint32(uint8_t *serializedNumber);

	uint32_t ConvertStringToUint32(string s, uint16_t length);
	string ConvertUint32ToString(uint32_t number);

    void ReadString(Buffer::Iterator *iterator, string *s);
    void WriteString(Buffer::Iterator *iterator, string s);
}

#endif /* UTILS_H */