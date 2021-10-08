/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef AUTHENTICATION_SERVER_NET_DEVICE_H
#define AUTHENTICATION_SERVER_NET_DEVICE_H

#include "basic-net-device.h"

namespace ns3
{
    class HomeAuthenticationServerNetDevice : public BasicNetDevice
    {
    public:
        static TypeId GetTypeId(void);
        HomeAuthenticationServerNetDevice();
        
    private:
        uint32_t hAuS = 20;
        std::map<uint32_t, std::vector<uint32_t>> MobileNodeKeys{{16, {1, 2}}}; // {ID,{X,Y}}
        virtual bool HandleSignalingMessage(Ptr<const Packet> packet);
    };
}

#endif /* AUTHENTICATION_SERVER_NET_DEVICE_H */