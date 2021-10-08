/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef VISITED_AUTHENTICATION_SERVER_NET_DEVICE_H
#define VISITED_AUTHENTICATION_SERVER_NET_DEVICE_H

#include "basic-net-device.h"

namespace ns3
{
    class VisitedAuthenticationServerNetDevice : public BasicNetDevice
    {
    public:
        static TypeId GetTypeId(void);
        VisitedAuthenticationServerNetDevice();
        void SetAuxNetDevice(Ptr<NetDevice> device);

    protected:
        Ptr<NetDevice> m_auxNetDevice;
        Ptr<NetDevice> m_iniNetDevice;
        uint32_t vAuS = 30;
        std::map<uint32_t, std::vector<uint32_t>> VisitedMobileNodeKeys; // {ID,{vX,vY}}

        virtual bool HandleSignalingMessage(Ptr<const Packet> packet);
    };
}

#endif /* VISITED_AUTHENTICATION_SERVER_NET_DEVICE_H */