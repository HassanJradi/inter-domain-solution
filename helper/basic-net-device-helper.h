/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef BASIC_NET_DEVICE_HELPER_H
#define BASIC_NET_DEVICE_HELPER_H

#include "ns3/trace-helper.h"
#include "ns3/basic-net-device.h"

namespace ns3
{
    class BasicNetDeviceHelper : public AsciiTraceHelperForDevice
    {
    public:
        // BasicNetDeviceHelper();
        // void SetDeviceAttribute(std::string n1, const AttributeValue &v1);
        // NetDeviceContainer Install(const NetDeviceContainer c);
        virtual void EnableAsciiInternal(Ptr<OutputStreamWrapper> stream, std::string prefix, Ptr<NetDevice> nd, bool explicitFilename);

        // private:
        //     ObjectFactory m_deviceFactory;
    };
}

#endif /* BASIC_NET_DEVICE_HELPER_H */