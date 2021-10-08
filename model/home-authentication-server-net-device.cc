/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "home-authentication-server-net-device.h"

NS_LOG_COMPONENT_DEFINE("HomeAuthenticationServerNetDevice");

namespace ns3
{
    NS_OBJECT_ENSURE_REGISTERED(HomeAuthenticationServerNetDevice);

    TypeId HomeAuthenticationServerNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::HomeAuthenticationServerNetDevice")
                                .SetParent<BasicNetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddConstructor<HomeAuthenticationServerNetDevice>();

        return tid;
    }

    HomeAuthenticationServerNetDevice::HomeAuthenticationServerNetDevice()
    {
        NS_LOG_FUNCTION(this);
    }

    bool HomeAuthenticationServerNetDevice::HandleSignalingMessage(Ptr<const Packet> packet)
    {
        NS_LOG_FUNCTION(this);

        SignalingMessage *signalingMessage = ExtractSignalingMessage(packet);
        uint8_t *data = signalingMessage->data;

        switch (signalingMessage->command)
        {

        case AUTH_REQ:
        {
            uint32_t nodeID = ConvertPointerUint8ToUint32(data);
            uint32_t T1 = ConvertPointerUint8ToUint32(data + idLength);
            uint32_t MIC = ConvertPointerUint8ToUint32(data + idLength + timestampLength);

            NS_LOG_INFO("[*] hAuS <- AUTH_REQ : "
                        << "ID = " << nodeID << ", "
                        << "T1 = " << T1 << ", "
                        << "MIC = " << MIC);

            uint32_t mobileNodeHashKey = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][0])) ^ Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][1]));
            uint32_t expectedMIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(hAuS) + ConvertUint32ToString(T1) + ConvertUint32ToString(mobileNodeHashKey));

            if (expectedMIC == MIC)
            {
                NS_LOG_INFO("[*] MN authenticated by hAuS");

                uint32_t N = rand() % 0xFFFFFFFF;
                uint32_t vX = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][0] ^ N));
                uint32_t vY = Hash32(ConvertUint32ToString(MobileNodeKeys[nodeID][1] ^ N));
                uint32_t T2 = rand() % 0xFFFFFFFF;

                MIC = Hash32(ConvertUint32ToString(nodeID) + ConvertUint32ToString(N) + ConvertUint32ToString(T2) + ConvertUint32ToString(mobileNodeHashKey));

                uint8_t *message = new uint8_t[idLength + 4 * hashLength + timestampLength];
                memcpy(message, ConvertUint32ToPointerUint8(nodeID), idLength);
                memcpy(message + idLength, ConvertUint32ToPointerUint8(vX), hashLength);
                memcpy(message + idLength + hashLength, ConvertUint32ToPointerUint8(vY), hashLength);
                memcpy(message + idLength + 2 * hashLength, ConvertUint32ToPointerUint8(N), hashLength);
                memcpy(message + idLength + 3 * hashLength, ConvertUint32ToPointerUint8(T2), timestampLength);
                memcpy(message + idLength + 3 * hashLength + timestampLength, ConvertUint32ToPointerUint8(MIC), hashLength);

                NS_LOG_INFO("[*] hAuS generates random number N = " << N << ", "
                                                                    << "and calculates vX = " << vX << ", "
                                                                    << " vY = " << vY);

                NS_LOG_INFO("[*] hAuS -> ROAMING_AUTH_RESP : "
                            << "ID = " << nodeID << ", "
                            << "vX = " << vX << ", "
                            << "vY = " << vY << ", "
                            << "N = " << N << ", "
                            << "T2 = " << T2 << ", "
                            << "MIC = " << MIC);

                uint32_t proccessingTime = 6 * hashTime + 3 * xorTime + 10 * concatenationTime;
                Simulator::Schedule(MicroSeconds(proccessingTime), MakeEvent(&HomeAuthenticationServerNetDevice::SendSignalingMessage, this, ROAMING_AUTH_RESP, message, idLength + 4 * hashLength + timestampLength));

                return true;
            }
        }
        }

        return false;
    }
}