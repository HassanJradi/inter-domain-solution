/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef BASIC_NET_DEVICE_H
#define BASIC_NET_DEVICE_H

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/simulator.h"
#include "ns3/traced-callback.h"

#include "utils.h"

#define idLength 4
#define timestampLength 10
#define hashLength 32

#define hashTime 163.65
#define xorTime 1
#define concatenationTime 1

namespace ns3
{
    enum CommandID : uint8_t
    {
        AUTH_REQ = 0xAA,
        ROAMING_AUTH_RESP,
        ATTACH_EVENT,
        ATTACH_VERIFIER,
        AUTH_RESP,
        RTR_SOL,
        RTR_ADV,
        AUTH_SUCC,
        TRIGGER
    };

    class BasicNetDevice : public NetDevice
    {
    public:
        static TypeId GetTypeId(void);
        Ptr<NetDevice> GetNetDevice() const;
        void SetNetDevice(Ptr<NetDevice> device);

        virtual void AddLinkChangeCallback(Callback<void> callback);
        virtual Address GetAddress(void) const;
        virtual Address GetBroadcast(void) const;
        virtual Ptr<Channel> GetChannel(void) const;
        virtual uint32_t GetIfIndex(void) const;
        virtual uint16_t GetMtu(void) const;
        virtual Address GetMulticast(Ipv4Address multicastGroup) const;
        virtual Address GetMulticast(Ipv6Address addr) const;
        virtual Ptr<Node> GetNode(void) const;
        virtual bool IsBridge(void) const;
        virtual bool IsBroadcast(void) const;
        virtual bool IsLinkUp(void) const;
        virtual bool IsMulticast(void) const;
        virtual bool IsPointToPoint(void) const;
        virtual bool NeedsArp(void) const;
        virtual bool Send(Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber);
        virtual bool SendFrom(Ptr<Packet> packet, const Address &source, const Address &dest, uint16_t protocolNumber);
        virtual void SetAddress(Address address);
        virtual void SetIfIndex(const uint32_t index);
        virtual bool SetMtu(const uint16_t mtu);
        virtual void SetNode(Ptr<Node> node);
        virtual void SetPromiscReceiveCallback(PromiscReceiveCallback cb);
        virtual void SetReceiveCallback(ReceiveCallback cb);
        virtual bool SupportsSendFrom(void) const;

    protected:
        uint16_t m_protocolNumber;
        uint32_t m_ifIndex;
        Ptr<Node> m_node;
        Ptr<NetDevice> m_netDevice;

        NetDevice::ReceiveCallback m_rxCallback;
        NetDevice::PromiscReceiveCallback m_promiscRxCallback;
        TracedCallback<> m_timeTrace;

        virtual void ReceiveFromDevice(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t protocol, Address const &source, Address const &destination, PacketType packetType);
        virtual bool HandleSignalingMessage(Ptr<const Packet> packet) = 0;
        SignalingMessage *ExtractSignalingMessage(Ptr<const Packet> packet);
        bool SendSignalingMessage(uint8_t command, uint8_t *data, uint16_t dataLength);
    };

}

#endif /* BASIC_NET_DEVICE_H */
