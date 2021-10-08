/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"

#include "math.h"
#include "utils.h"

NS_LOG_COMPONENT_DEFINE("Utils");

namespace ns3
{
	uint8_t *SignalingMessage::Serialize(SignalingMessage *signalingMessage, uint16_t &length)
	{
		length = signalingMessage->dataLength + 1;
		uint8_t *serializedSignalingMessage = new uint8_t[length];

		if (signalingMessage->dataLength > 0)
		{
			serializedSignalingMessage++;
			serializedSignalingMessage = signalingMessage->data;
			serializedSignalingMessage--;
		}

		*serializedSignalingMessage = signalingMessage->command;

		return serializedSignalingMessage;
	}

	SignalingMessage *SignalingMessage::Deserialize(uint8_t *serializedSignalingMessage, uint16_t length)
	{
		SignalingMessage *signalingMessage = new SignalingMessage();

		if (length > 1)
		{
			signalingMessage->data = serializedSignalingMessage + 1;
			signalingMessage->dataLength = length - 1;
		}

		signalingMessage->command = *serializedSignalingMessage;

		return signalingMessage;
	}

	uint8_t *SerializeHeader(Ipv6Header *header, uint16_t &length)
	{
		uint8_t *serializedHeader;
		Buffer tmp;
		tmp.AddAtStart(header->GetSerializedSize());
		Buffer::Iterator iterator = tmp.Begin();
		length = iterator.GetSize();
		header->Serialize(iterator);
		serializedHeader = new uint8_t[length];
		iterator.Read(serializedHeader, length);
		return serializedHeader;
	}

	Ipv6Header *DeserializeHeader(uint8_t *serializedHeader, uint16_t length)
	{
		Ipv6Header *header = new Ipv6Header();
		Buffer tmp;
		tmp.AddAtStart(header->GetSerializedSize());
		Buffer::Iterator iterator = tmp.Begin();
		iterator.Write(serializedHeader, length);
		iterator = tmp.Begin();
		header->Deserialize(iterator);
		return header;
	}

	string ConvertPointerUint8ToString(uint8_t *arr, uint16_t length)
	{
		string s;

		for (int i = 0; i < length; i++)
		{
			s += static_cast<char>(arr[i]);
		}

		return s;
	}

	uint8_t *ConvertStringToPointerUint8(string s, uint16_t length)
	{
		uint8_t *arr = new uint8_t[length];

		for (int i = 0; i < length; i++)
		{
			arr[i] = static_cast<uint8_t>(s[i]);
		}

		return arr;
	}

	uint8_t *ConvertUint64ToPointerUint8(uint64_t number)
	{
		// NS_LOG_DEBUG("number 	" << number );

		uint8_t *a = new uint8_t[8]{0};
		uint8_t i = 0;

		do
		{
			a[i] = number % 256;
			number -= a[i++];
			number /= 256;
			NS_LOG_DEBUG("sn	" << number << "\t" << (uint)a[i-1] );
		} while (number > 0);
			NS_LOG_DEBUG("done sn");

		return a;
	}

	uint64_t ConvertPointerUint8ToUint64(uint8_t *serializedNumber)
	{
		uint64_t number = 0;

		for (uint8_t i = 0; i < 8; i++)
		{
			number += serializedNumber[i] * pow(256, i);
		}
			NS_LOG_DEBUG("done rc");
		
		// NS_LOG_DEBUG("sn	" << number);

		return number;
	}

	uint64_t ConvertStringToUint64(string s, uint16_t length)
	{
		return ConvertPointerUint8ToUint64(ConvertStringToPointerUint8(s, length));
	}

	string ConvertUint64ToString(uint64_t number)
	{
		return ConvertPointerUint8ToString(ConvertUint64ToPointerUint8(number), 8);
	}

	uint8_t *ConvertUint32ToPointerUint8(uint32_t number)
	{
		// NS_LOG_DEBUG("number 	" << number );

		uint8_t *a = new uint8_t[4]{0};
		uint8_t i = 0;

		do
		{
			a[i] = number % 256;
			number -= a[i++];
			number /= 256;
		} while (number > 0);

		return a;
	}

	uint32_t ConvertPointerUint8ToUint32(uint8_t *serializedNumber)
	{
		uint32_t number = 0;

		for (uint8_t i = 0; i < 4; i++)
		{
			number += serializedNumber[i] * pow(256, i);
		}

		// NS_LOG_DEBUG("sn	" << number);

		return number;
	}

	uint32_t ConvertStringToUint32(string s, uint16_t length)
	{
		return ConvertPointerUint8ToUint32(ConvertStringToPointerUint8(s, length));
	}

	string ConvertUint32ToString(uint32_t number)
	{
		return ConvertPointerUint8ToString(ConvertUint32ToPointerUint8(number), 4);
	}

	void ReadString(Buffer::Iterator *iterator, string *s)
	{
		uint8_t length = iterator->ReadU8();

		for (uint8_t i = 0; i < length; i++)
		{
			*s += static_cast<char>(iterator->ReadU8());
		}
	}

	void WriteString(Buffer::Iterator *iterator, string s)
	{
		uint8_t length = s.size();
		iterator->WriteU8(length);

		for (uint8_t i = 0; i < length; i++)
		{
			iterator->WriteU8(static_cast<uint8_t>(s[i]));
		}
	}

}