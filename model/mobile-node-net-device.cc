/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mobile-node-net-device.h"

NS_LOG_COMPONENT_DEFINE("MobileNodeNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(MobileNodeNetDevice);

    TypeId MobileNodeNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::MobileNodeNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<MobileNodeNetDevice>();

        return tid;
    }

    MobileNodeNetDevice::MobileNodeNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    bool Test()
    {
        return true;
    }

    void MobileNodeNetDevice::LaunchAuthentication()
    {
        isAccessRequestSent = true;

        uint32_t T1 = rand() % 0xFFFFFFFF;
        hashKey = Hash32(ConvertUint32ToString(X)) ^ Hash32(ConvertUint32ToString(Y));
        uint32_t MIC = Hash32(ConvertUint32ToString(ID) + ConvertUint32ToString(hAuS) + ConvertUint32ToString(T1) + ConvertUint32ToString(hashKey));

        uint8_t *message = new uint8_t[2 * idLength + timestampLength + hashLength];
        memcpy(message, ConvertUint32ToPointerUint8(ID), idLength);
        memcpy(message + idLength, ConvertUint32ToPointerUint8(hAuS), idLength);
        memcpy(message + 2 * idLength, ConvertUint32ToPointerUint8(T1), timestampLength);
        memcpy(message + 2 * idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

        NS_LOG_INFO("[*] MN -> AUTH_REQ : "
                    << "ID = " << ID << ", "
                    << "hAuS = " << hAuS << ", "
                    << "T1 = " << T1 << ", "
                    << "MIC = " << MIC << " "
                    << "with Hashkey = " << hashKey);

        uint32_t proccessingTime = 3 * hashTime + xorTime + 7 * concatenationTime;
        Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&MobileNodeNetDevice::SendSignalingMessage, this, AUTH_REQ, message, 2 * idLength + timestampLength + hashLength));
    }

    bool MobileNodeNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        if (!isAccessRequestSent)
            LaunchAuthentication();

        return true;
    }

    bool MobileNodeNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;

        switch (signalingMessage->command)
        {

        case ROAMING_AUTH_RESP:
        {
            uint32_t vAuS = ConvertPointerUint8ToUint32(data);
            uint32_t nodeID = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t N = ConvertPointerUint8ToUint32(data + 2 * idLength);
            uint32_t T2 = ConvertPointerUint8ToUint32(data + 2 * idLength + hashLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + 2 * idLength + hashLength + timestampLength);

            NS_LOG_INFO("[*] MN <- ROAMING_AUTH_RESP : "
                        << "vAuS = " << vAuS << ", "
                        << "ID = " << nodeID << ", "
                        << "N = " << N << ", "
                        << "T2 = " << T2 << ", "
                        << "MIC = " << MIC << " "
                        << "with Hashkey = " << hashKey);

            if (nodeID != ID)
                return false;

            uint32_t expectedMIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(N) + ConvertUint32ToString(T2) + ConvertUint32ToString(hashKey));

            if (expectedMIC == MIC)
            {
                vX = Hash32(ConvertUint32ToString(X ^ N));
                vY = Hash32(ConvertUint32ToString(Y ^ N));
             
                return true;
            }
        }

        case TRIGGER:
        {
            visitedHashKey = Hash32(ConvertUint32ToString(vX)) ^ Hash32(ConvertUint32ToString(vY));
            uint32_t T3 = rand() % 0xFFFFFFFF;
            uint32_t MIC = Hash32(ConvertUint32ToString(ID) + ConvertUint32ToString(T3) + ConvertUint32ToString(visitedHashKey));

            uint8_t *message = new uint8_t[idLength + timestampLength + hashLength];
            memcpy(message, ConvertUint32ToPointerUint8(ID), idLength);
            memcpy(message + idLength, ConvertUint32ToPointerUint8(T3), timestampLength);
            memcpy(message + idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

            NS_LOG_INFO("[*] MN -> RTR_SOL : "
                        << "ID = " << ID << ", "
                        << "T3 = " << T3 << ", "
                        << "MIC = " << MIC << " "
                        << "with VisitedHashkey = " << visitedHashKey);

            uint32_t proccessingTime = 3 * hashTime + xorTime + 4 * concatenationTime;
            Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&MobileNodeNetDevice::SendSignalingMessage, this, RTR_SOL, message, idLength + timestampLength + hashLength));

            return true;
        }

        case RTR_ADV:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t HNP = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t T4 = ConvertPointerUint8ToUint32(data + 2 * idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + 2 * idLength + timestampLength);

            if (nodeID != ID)
                return false;

            NS_LOG_INFO("[*] MN <- RTR_ADV : "
                        << "ID = " << nodeID << ", "
                        << "HNP = " << HNP << ", "
                        << "T4 = " << T4 << ", "
                        << "MIC = " << MIC);

            uint32_t expectedMIC = Hash32(ConvertUint32ToString(ID) + ConvertUint32ToString(HNP) + ConvertUint32ToString(T4) + ConvertUint32ToString(visitedHashKey));

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] MAG authenticated by MN");
                NS_LOG_INFO("[*] Configuring network interface using HNP");
                NS_LOG_INFO("[*] Updating MN keys in the MN database");
                vX = Hash32(ConvertUint32ToString(vX));
                vY = Hash32(ConvertUint32ToString(vY));
                
                m_timeTrace();
                return true;
            }
        }
        }

        return false;
    }
}