/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "visited-authentication-server-net-device.h"

NS_LOG_COMPONENT_DEFINE("VisitedAuthenticationServerNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(VisitedAuthenticationServerNetDevice);

    TypeId VisitedAuthenticationServerNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::VisitedAuthenticationServerNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<VisitedAuthenticationServerNetDevice>();
        return tid;
    }

    VisitedAuthenticationServerNetDevice::VisitedAuthenticationServerNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void VisitedAuthenticationServerNetDevice::SetAuxNetDevice(Ptr<NetDevice> device)
    {
        NS_LOG_FUNCTION(this << device);
        NS_ASSERT_MSG(m_netDevice != 0, "VisitedAuthenticationServerNetDevice: set default net device before assigning auxiliary net device " << m_netDevice);

        m_iniNetDevice = m_netDevice;
        m_auxNetDevice = device;
        m_node->RegisterProtocolHandler(MakeCallback(&VisitedAuthenticationServerNetDevice::ReceiveFromDevice, this), m_protocolNumber, device, false);
    }

    bool VisitedAuthenticationServerNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;

        switch (signalingMessage->command)
        {

        case AUTH_REQ:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t hAuS = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t T1 = ConvertPointerUint8ToUint32(data + 2 * idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + 2 * idLength + timestampLength);

            NS_LOG_INFO("[*] vAuS <- AUTH_REQ : "
                        << "ID = " << nodeID << ", "
                        << "hAuS = " << hAuS << ", "
                        << "T1 = " << T1 << ", "
                        << "MIC = " << MIC);

            // if (hAuS == vAuS)
            // {
            //     NS_LOG_INFO("[*] vAuS <- AUTH_REQ : "
            //                 << "ID = " << nodeID << ", "
            //                 << "requestedAuS = " << vAuS << ", "
            //                 << "T3 = " << T << ", "
            //                 << "MIC = " << MIC);

            //     uint32_t mobileNodeVisitedHashKey = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][0])) ^ Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][1]));
            //     uint32_t expectedMIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(vAuS) + ConvertUint32ToString(T) + ConvertUint32ToString(mobileNodeVisitedHashKey));

            //     if (expectedMIC == MIC)
            //     {
            //         uint32_t V = rand() % 0xFFFFFFFF;
            //         uint32_t T4 = rand() % 0xFFFFFFFF;
            //         uint32_t W = V ^ Hash32(ConvertUint32ToString(mobileNodeVisitedHashKey));
            //         MIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(W) + ConvertUint32ToString(T4) + ConvertUint32ToString(mobileNodeVisitedHashKey));

            //         NS_LOG_INFO("[*] vAuS generates V = " << V << " and calculates W = " << W);

            //         uint8_t *message = new uint8_t[idLength + 3 * hashLength + timestampLength];
            //         memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
            //         memcpy(message + idLength, ConvertUint32ToPointerUint8(V), hashLength);
            //         memcpy(message + idLength + hashLength, ConvertUint32ToPointerUint8(W), hashLength);
            //         memcpy(message + idLength + 2 * hashLength, ConvertUint32ToPointerUint8(T4), timestampLength);
            //         memcpy(message + idLength + 2 * hashLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

            //         NS_LOG_INFO("[*] vAuS -> AUTH_RESP : "
            //                     << "ID = " << nodeID << ", "
            //                     << "V = " << V << ", "
            //                     << "W = " << W << ", "
            //                     << "T4 = " << T4 << ", "
            //                     << "MIC = " << MIC);

            //         NS_LOG_INFO("[*] Updating MN keys in the vAuS database");
            //         NS_LOG_INFO("[*] Mobile node key vX was = " << VisitedMobileNodeKeys[nodeID][0] << " and key vY was = " << VisitedMobileNodeKeys[nodeID][1]);
            //         VisitedMobileNodeKeys[nodeID][0] = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][0]));
            //         VisitedMobileNodeKeys[nodeID][1] = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][1]));
            //         NS_LOG_INFO("[*] Mobile node key vX becomes = " << VisitedMobileNodeKeys[nodeID][0] << " and key vY becomes = " << VisitedMobileNodeKeys[nodeID][1]);

            //         uint32_t proccessingTime = 7 * hashTime + 2 * xorTime + 11 * concatenationTime;
            //         Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&VisitedAuthenticationServerNetDevice::SendSignalingMessage, this, AUTH_RESP, message, idLength + 3 * hashLength + timestampLength));
            //     }
            // }
            // else
            // {

            uint8_t *message = new uint8_t[idLength + timestampLength + hashLength];
            memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
            memcpy(message + idLength, ConvertUint32ToPointerUint8(T1), timestampLength);
            memcpy(message + idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

            m_netDevice = m_auxNetDevice;
            Simulator::Schedule(MicroSeconds(0), MakeEvent(&VisitedAuthenticationServerNetDevice::SendSignalingMessage, this, AUTH_REQ, message, idLength + timestampLength + hashLength));
            // }

            return true;
        }

        case ROAMING_AUTH_RESP:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t vX = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t vY = ConvertPointerUint8ToUint32(data + idLength + hashLength);
            uint32_t N = ConvertPointerUint8ToUint32(data + idLength + 2 * hashLength);
            uint32_t T2 = ConvertPointerUint8ToUint32(data + idLength + 3 * hashLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + idLength + 3 * hashLength + timestampLength);

            NS_LOG_INFO("[*] vAuS <-> ROAMING_AUTH_RESP : "
                        << "ID = " << nodeID << ", "
                        << "vX = " << vX << ", "
                        << "vY = " << vY << ", "
                        << "N = " << N << ", "
                        << "T2 = " << T2 << ", "
                        << "MIC = " << MIC);

            VisitedMobileNodeKeys.insert(pair<uint32_t, std::vector<uint32_t>>(nodeID, {vX, vY}));

            uint8_t *message = new uint8_t[2 * idLength + 2 * hashLength + timestampLength];

            memcpy(message, ConvertUint32ToPointerUint8(vAuS), idLength);
            memcpy(message + idLength, ConvertUint32ToPointerUint8(nodeID), idLength);
            memcpy(message + 2 * idLength, ConvertUint32ToPointerUint8(N), hashLength);
            memcpy(message + 2 * idLength + hashLength, ConvertUint32ToPointerUint8(T2), timestampLength);
            memcpy(message + 2 * idLength + hashLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

            m_netDevice = m_iniNetDevice;
            Simulator::Schedule(MicroSeconds(0), MakeEvent(&VisitedAuthenticationServerNetDevice::SendSignalingMessage, this, ROAMING_AUTH_RESP, message, 2 * idLength + 2 * hashLength + timestampLength));

            return true;
        }

        case ATTACH_EVENT:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);

            NS_LOG_INFO("[*] vAuS <- ATTACH_EVENT : "
                        << "ID = " << nodeID);

            uint32_t mobileNodeVisitedHashKey = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][0])) ^ Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][1]));

            uint8_t *message = new uint8_t[idLength + hashLength];

            memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
            memcpy(message + idLength, ConvertUint32ToPointerUint8(mobileNodeVisitedHashKey), hashLength);

            NS_LOG_INFO("[*] vAuS -> ATTACH_VERIFIER : "
                        << "ID = " << nodeID << ", "
                        << "MobileNodeVisitedHashKey = " << mobileNodeVisitedHashKey);

            m_netDevice = m_iniNetDevice;

            uint32_t proccessingTime = 2 * concatenationTime + 2 * hashTime + xorTime;
            Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&VisitedAuthenticationServerNetDevice::SendSignalingMessage, this, ATTACH_VERIFIER, message, idLength + hashLength));

            return true;        
        }

        case AUTH_SUCC:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);

            NS_LOG_INFO("[*] vAuS <- AUTH_SUCC : "
                        << "ID = " << nodeID);
            NS_LOG_INFO("[*] Updating MN keys in the vAuS database");
            NS_LOG_INFO("[*] Mobile node key vX was = " << VisitedMobileNodeKeys[nodeID][0] << " and key vY was = " << VisitedMobileNodeKeys[nodeID][1]);

            VisitedMobileNodeKeys[nodeID][0] = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][0]));
            VisitedMobileNodeKeys[nodeID][1] = Hash32(ConvertUint32ToString(VisitedMobileNodeKeys[nodeID][1]));

            NS_LOG_INFO("[*] Mobile node key vX becomes = " << VisitedMobileNodeKeys[nodeID][0] << " and key vY becomes = " << VisitedMobileNodeKeys[nodeID][1]);
            
            return true;        
        }
        }

        return false;
    }
}