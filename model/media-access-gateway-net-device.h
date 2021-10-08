/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NETWORK_ACCESS_SERVER_NET_DEVICE_H
#define NETWORK_ACCESS_SERVER_NET_DEVICE_H

#include "basic-net-device.h"

namespace ns3
{
    class MediaAccessGatewayNetDevice : public BasicNetDevice
    {
    public:
        static TypeId GetTypeId(void);
        MediaAccessGatewayNetDevice();
        void SetAuxNetDevice(Ptr<NetDevice> device);
        
    private:
        std::map<uint32_t, uint32_t> MobileNodeVisitedKey;
        Ptr<NetDevice> m_auxNetDevice;
        Ptr<NetDevice> m_iniNetDevice;

        virtual bool HandleSignalingMessage(Ptr<const Packet> packet);
    };
}

#endif /* NETWORK_ACCESS_SERVER_NET_DEVICE_H */