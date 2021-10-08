/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/node.h"
#include "ns3/net-device.h"

#include "basic-net-device-helper.h"

namespace ns3
{
    NS_LOG_COMPONENT_DEFINE("BasicNetDeviceHelper");

    /*BasicNetDeviceHelper::BasicNetDeviceHelper()
    {
        NS_LOG_FUNCTION(this);
        m_deviceFactory.SetTypeId("ns3::BasicNetDevice");
    }

    void BasicNetDeviceHelper::SetDeviceAttribute(std::string n1, const AttributeValue &v1)
    {
        NS_LOG_FUNCTION(this);
        m_deviceFactory.Set(n1, v1);
    }

    NetDeviceContainer BasicNetDeviceHelper::Install(const NetDeviceContainer c)
    {
        NS_LOG_FUNCTION(this);
        NetDeviceContainer devs;

        for (uint32_t i = 0; i < c.GetN(); ++i)
        {
            Ptr<NetDevice> device = c.Get(i);
            NS_ASSERT_MSG(device != 0, "No NetDevice found in the node " << int(i));

            Ptr<Node> node = device->GetNode();
            NS_LOG_LOGIC("**** Install BasicNetDevice on node " << node->GetId());

            Ptr<BasicNetDevice> dev = m_deviceFactory.Create<BasicNetDevice>();

            devs.Add(dev);
            node->AddDevice(dev);
            dev->SetNetDevice(device);
        }

        return devs;
    }*/

    static void WriteTime(Ptr<OutputStreamWrapper> stream)
    {
        *stream->GetStream() << Simulator::Now().GetSeconds() << ' ' << std::flush;
    }

    void BasicNetDeviceHelper::EnableAsciiInternal(Ptr<OutputStreamWrapper> stream, std::string prefix, Ptr<NetDevice> nd, bool explicitFilename)
    {
        Ptr<BasicNetDevice> device = nd->GetObject<BasicNetDevice>();
        if (device == 0)
        {
            NS_LOG_INFO("BasicNetDeviceHelper::EnableAsciiInternal(): Device " << device << " not of type ns3::BasicNetDevice");
            return;
        }

        AsciiTraceHelper asciiTraceHelper;
        std::string filename;
        if (stream == 0)
        {
            if (explicitFilename)
            {
                filename = prefix;
            }
            else
            {
                filename = asciiTraceHelper.GetFilenameFromDevice(prefix, device);
            }

            stream = asciiTraceHelper.CreateFileStream(filename, std::ios_base::app);
        }

        device->TraceConnectWithoutContext("TimeTrace", MakeBoundCallback(&WriteTime, stream));
    }
}