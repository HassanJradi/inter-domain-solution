/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/channel.h"
#include "ns3/uinteger.h"
#include "ns3/ipv6-l3-protocol.h"

#include "basic-net-device.h"

NS_LOG_COMPONENT_DEFINE("BasicNetDevice");

namespace ns3
{

    NS_OBJECT_ENSURE_REGISTERED(BasicNetDevice);

    /* Public Functions */

    TypeId BasicNetDevice::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::BasicNetDevice")
                                .SetParent<NetDevice>()
                                .SetGroupName("BasicNetDevice")
                                .AddAttribute("ProtocolNumber",
                                              "The specific Protocol Number to be used in L3 packets.",
                                              UintegerValue(Ipv6L3Protocol::PROT_NUMBER),
                                              MakeUintegerAccessor(&BasicNetDevice::m_protocolNumber),
                                              MakeUintegerChecker<uint16_t>())
                                .AddTraceSource("TimeTrace",
                                                "Send - packet (including BasicNetDevice header), ",
                                                MakeTraceSourceAccessor(&BasicNetDevice::m_timeTrace),
                                                "ns3::Time::TracedCallback");

        return tid;
    }

    Ptr<NetDevice> BasicNetDevice::GetNetDevice() const
    {
        //NS_LOG_FUNCTION(this);
        return m_netDevice;
    }

    void BasicNetDevice::SetNetDevice(Ptr<NetDevice> device)
    {
        NS_LOG_FUNCTION(this << device);

        m_netDevice = device;
        m_node->RegisterProtocolHandler(MakeCallback(&BasicNetDevice::ReceiveFromDevice, this), m_protocolNumber, device, false);
    }

    /* Inherited Functions */

    void BasicNetDevice::AddLinkChangeCallback(Callback<void> callback)
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->AddLinkChangeCallback(callback);
    }

    Address BasicNetDevice::GetAddress(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetAddress();
    }

    Address BasicNetDevice::GetBroadcast(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetBroadcast();
    }

    Ptr<Channel> BasicNetDevice::GetChannel(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetChannel();
    }

    uint32_t BasicNetDevice::GetIfIndex(void) const
    {
        //NS_LOG_FUNCTION(this);
        return m_ifIndex;
    }

    uint16_t BasicNetDevice::GetMtu(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetMtu();
    }

    Address BasicNetDevice::GetMulticast(Ipv4Address multicastGroup) const
    {
        //NS_LOG_FUNCTION(this << multicastGroup);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetMulticast(multicastGroup);
    }

    Address BasicNetDevice::GetMulticast(Ipv6Address addr) const
    {
        //NS_LOG_FUNCTION(this << addr);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->GetMulticast(addr);
    }

    Ptr<Node> BasicNetDevice::GetNode(void) const
    {
        //NS_LOG_FUNCTION(this);
        return m_node;
    }

    bool BasicNetDevice::IsBridge(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->IsBridge();
    }

    bool BasicNetDevice::IsBroadcast(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->IsBroadcast();
    }

    bool BasicNetDevice::IsLinkUp(void) const
    {
        // NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->IsLinkUp();
    }

    bool BasicNetDevice::IsMulticast(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->IsMulticast();
    }

    bool BasicNetDevice::IsPointToPoint(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->IsPointToPoint();
    }

    bool BasicNetDevice::NeedsArp(void) const
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->NeedsArp();
    }

    bool BasicNetDevice::Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
    {
        NS_LOG_FUNCTION(this);
        return true;
    }

    bool BasicNetDevice::SendFrom(Ptr<Packet> packet, const Address &src, const Address &dest, uint16_t protocolNumber)
    {
        NS_LOG_FUNCTION(this);
        return false;
    }

    void BasicNetDevice::SetAddress(Address address)
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        m_netDevice->SetAddress(address);
    }

    void BasicNetDevice::SetIfIndex(const uint32_t index)
    {
        //NS_LOG_FUNCTION(this);
        m_ifIndex = index;
    }

    bool BasicNetDevice::SetMtu(const uint16_t mtu)
    {
        //NS_LOG_FUNCTION(this);
        NS_ASSERT_MSG(m_netDevice != 0, "BasicNetDevice: can't find any lower-layer protocol " << m_netDevice);

        return m_netDevice->SetMtu(mtu);
    }

    void BasicNetDevice::SetNode(Ptr<Node> node)
    {
        NS_LOG_FUNCTION(this << node);
        m_node = node;
    }

    void BasicNetDevice::SetPromiscReceiveCallback(NetDevice::PromiscReceiveCallback cb)
    {
        //NS_LOG_FUNCTION(this);
        m_promiscRxCallback = cb;
    }

    void BasicNetDevice::SetReceiveCallback(NetDevice::ReceiveCallback cb)
    {
        //NS_LOG_FUNCTION(this);
        m_rxCallback = cb;
    }

    bool BasicNetDevice::SupportsSendFrom() const
    {
        //NS_LOG_FUNCTION(this);
        return false;
    }

    /* Protected Functions*/

    void BasicNetDevice::ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocolNumber, Address const &source, Address const &destination, PacketType packetType)
    {
        NS_LOG_FUNCTION(this);
        HandleSignalingMessage(packet);
    }

    SignalingMessage *BasicNetDevice::ExtractSignalingMessage(Ptr<const Packet> packet)
    {
        uint16_t length = packet->GetSize();
        uint8_t *serializedSignalingMessage = new uint8_t[length];
        packet->CopyData(serializedSignalingMessage, length);
        return SignalingMessage::Deserialize(serializedSignalingMessage, length);
    }

    bool BasicNetDevice::SendSignalingMessage(uint8_t command, uint8_t *data, uint16_t dataLength)
    {
        SignalingMessage *message = new SignalingMessage();

        message->command = command;
        message->data = data;
        message->dataLength = dataLength;

        uint16_t length;
        uint8_t *serializedSignalingMessage = SignalingMessage::Serialize(message, length);

        Ptr<Packet> packet = new Packet(serializedSignalingMessage, length);
        return m_netDevice->Send(packet, GetBroadcast(), Ipv6L3Protocol::PROT_NUMBER);
    }

}