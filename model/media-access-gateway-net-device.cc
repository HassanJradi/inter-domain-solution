/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "media-access-gateway-net-device.h"

NS_LOG_COMPONENT_DEFINE("MediaAccessGatewayNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(MediaAccessGatewayNetDevice);

    TypeId MediaAccessGatewayNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::MediaAccessGatewayNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<MediaAccessGatewayNetDevice>();
        return tid;
    }

    MediaAccessGatewayNetDevice::MediaAccessGatewayNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    void MediaAccessGatewayNetDevice::SetAuxNetDevice(Ptr<NetDevice> device)
    {
        NS_LOG_FUNCTION(this << device);
        NS_ASSERT_MSG(m_netDevice != 0, "MediaAccessGatewayNetDevice: set default net device before assigning auxiliary net device " << m_netDevice);

        m_iniNetDevice = m_netDevice;
        m_auxNetDevice = device;
        m_node->RegisterProtocolHandler(MakeCallback(&MediaAccessGatewayNetDevice::ReceiveFromDevice, this), m_protocolNumber, device, false);
    }

    bool MediaAccessGatewayNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;

        switch (signalingMessage->command)
        {

        case AUTH_REQ:
        {
            NS_LOG_INFO("[*] MAG <-> AUTH_REQ");

            m_netDevice = m_auxNetDevice;
            SendSignalingMessage(signalingMessage->command, signalingMessage->data, signalingMessage->dataLength);
            return true;
        }

        case ROAMING_AUTH_RESP:
        {
            NS_LOG_INFO("[*] MAG <-> ROAMING_AUTH_RESP");
            uint32_t nodeID = ConvertPointerUint8ToUint32(data + idLength);

            m_netDevice = m_iniNetDevice;
            SendSignalingMessage(signalingMessage->command, signalingMessage->data, signalingMessage->dataLength);

            NS_LOG_INFO("[*] MAG -> ATTACH_EVENT : "
                        << "ID = " << nodeID);

            uint8_t *message = new uint8_t[idLength];
            memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);

            m_netDevice = m_auxNetDevice;
            SendSignalingMessage(ATTACH_EVENT, message, idLength);

            return true;
        }

        case ATTACH_VERIFIER:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t mobileNodeVisitedHashKey = ConvertPointerUint8ToUint32(data + idLength);

            NS_LOG_INFO("[*] MAG <- ATTACH_VERIFIER : "
                        << "ID = " << nodeID << ", "
                        << "MobileNodeVisitedHashKey = " << mobileNodeVisitedHashKey);

            MobileNodeVisitedKey.insert(pair<uint32_t, uint32_t>(nodeID, mobileNodeVisitedHashKey));

            m_netDevice = m_iniNetDevice;
            SendSignalingMessage(TRIGGER, nullptr, 0);

            return true;
        }

        case RTR_SOL:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t T3 = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + idLength + timestampLength);

            NS_LOG_INFO("[*] MAG <- RTR_SOL : "
                        << "ID = " << nodeID << ", "
                        << "T3 = " << T3 << ", "
                        << "MIC = " << MIC);

            uint32_t expectedMIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(T3) + ConvertUint32ToString(MobileNodeVisitedKey[nodeID]));

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] MN authenticated by MAG");

                uint32_t T4 = rand() % 0xFFFFFFFF;
                uint32_t HNP = rand() % 0xFFFFFFFF;
                uint32_t MIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(HNP) + ConvertUint32ToString(T4) + ConvertUint32ToString(MobileNodeVisitedKey[nodeID]));

                uint8_t *message = new uint8_t[2 * idLength + timestampLength + hashLength];
                memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
                memcpy(message + idLength, ConvertUint32ToPointerUint8(HNP), timestampLength);
                memcpy(message + 2 * idLength, ConvertUint32ToPointerUint8(T4), timestampLength);
                memcpy(message + 2 * idLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

                NS_LOG_INFO("[*] MAG -> RTR_ADV : "
                            << "ID = " << nodeID << ", "
                            << "HNP = " << HNP << ", "
                            << "T4 = " << T4 << ", "
                            << "MIC = " << MIC);

                uint32_t proccessingTime = 2 * hashTime + 8 * concatenationTime;

                m_netDevice = m_iniNetDevice;
                Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&MediaAccessGatewayNetDevice::SendSignalingMessage, this, RTR_ADV, message, 2 * idLength + timestampLength + hashLength));

                NS_LOG_INFO("[*] MAG -> AUTH_SUCC : "
                            << "ID = " << nodeID);

                message = new uint8_t[idLength];
                memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);

                //This step should be delayed
                //m_netDevice = m_auxNetDevice;
                Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&MediaAccessGatewayNetDevice::SendSignalingMessage, this, AUTH_SUCC, message, idLength));

                return true;
            }
        }
        }

        return false;
    }
}