/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "ns3/csma-helper.h"
#include "ns3/core-module.h"
#include "ns3/trace-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/mobile-node-net-device.h"
#include "ns3/media-access-gateway-net-device.h"
#include "ns3/visited-authentication-server-net-device.h"
#include "ns3/home-authentication-server-net-device.h"
#include "ns3/basic-net-device-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SecureMobilitySolutionExample");

int main(int argc, char *argv[])
{
  bool verbose = true;
  uint64_t dataRate = 100;
  uint64_t delay = 0;

  CommandLine cmd;
  cmd.AddValue("verbose", "Tell application to log if true", verbose);
  cmd.AddValue("data-rate", "Channel data rate (kbps)", dataRate);
  cmd.AddValue("delay", "Channel delay (ms)", delay);

  cmd.Parse(argc, argv);

  if (verbose)
  {
    LogComponentEnable("SecureMobilitySolutionExample", LOG_INFO);
    LogComponentEnable("BasicNetDevice", LOG_INFO);
    LogComponentEnable("MobileNodeNetDevice", LOG_INFO);
    LogComponentEnable("MediaAccessGatewayNetDevice", LOG_INFO);
    LogComponentEnable("VisitedAuthenticationServerNetDevice", LOG_INFO);
    LogComponentEnable("HomeAuthenticationServerNetDevice", LOG_INFO);
    LogComponentEnable("BasicNetDeviceHelper", LOG_INFO);
  }

  NS_LOG_INFO("Create nodes.");

  Ptr<Node> mobileNode = CreateObject<Node>();
  Ptr<Node> mediaAccessGateway = CreateObject<Node>();
  Ptr<Node> visitedAuthenticationServerNode = CreateObject<Node>();
  Ptr<Node> homeAuthenticationServerNode = CreateObject<Node>();

  NodeContainer radioNetwork(mobileNode, mediaAccessGateway);
  NodeContainer IPNetwork1(mediaAccessGateway, visitedAuthenticationServerNode);
  NodeContainer IPNetwork2(visitedAuthenticationServerNode, homeAuthenticationServerNode);

  NS_LOG_INFO("Create channels.");

  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate", StringValue(to_string(dataRate) + "kbps"));
  csma.SetChannelAttribute("Delay", StringValue(to_string(delay) + "ms"));
  NetDeviceContainer csmaNetDeviceContainer = csma.Install(radioNetwork);

  PointToPointHelper pointToPoint;
  pointToPoint.SetChannelAttribute("Delay", StringValue("10ms"));
  NetDeviceContainer pointToPointNetDeviceContainer1 = pointToPoint.Install(IPNetwork1);
  NetDeviceContainer pointToPointNetDeviceContainer2 = pointToPoint.Install(IPNetwork2);

  NS_LOG_INFO("Create mobile node device.");
  Ptr<MobileNodeNetDevice> mobileNodeNetDevice = CreateObject<MobileNodeNetDevice>();
  mobileNode->AddDevice(mobileNodeNetDevice);
  Ptr<NetDevice> mobileNodeCsmaDevice = csmaNetDeviceContainer.Get(0);
  mobileNodeNetDevice->SetNetDevice(mobileNodeCsmaDevice);

  NS_LOG_INFO("Create media access gateway device.");
  Ptr<MediaAccessGatewayNetDevice> mediaAccessGatewayNetDevice = CreateObject<MediaAccessGatewayNetDevice>();
  mediaAccessGateway->AddDevice(mediaAccessGatewayNetDevice);
  Ptr<NetDevice> mediaAccessGatewayCsmaDevice = csmaNetDeviceContainer.Get(1);
  mediaAccessGatewayNetDevice->SetNetDevice(mediaAccessGatewayCsmaDevice);
  Ptr<NetDevice> mediaAccessGatewayP2PDevice = pointToPointNetDeviceContainer1.Get(0);
  mediaAccessGatewayNetDevice->SetAuxNetDevice(mediaAccessGatewayP2PDevice);

  NS_LOG_INFO("Create visited authentication server device.");
  Ptr<VisitedAuthenticationServerNetDevice> visitedAuthenticationServerNetDevice = CreateObject<VisitedAuthenticationServerNetDevice>();
  visitedAuthenticationServerNode->AddDevice(visitedAuthenticationServerNetDevice);
  Ptr<NetDevice> visitedAuthenticationServerP2PDevice1 = pointToPointNetDeviceContainer1.Get(1);
  visitedAuthenticationServerNetDevice->SetNetDevice(visitedAuthenticationServerP2PDevice1);
  Ptr<NetDevice> visitedAuthenticationServerP2PDevice2 = pointToPointNetDeviceContainer2.Get(0);
  visitedAuthenticationServerNetDevice->SetAuxNetDevice(visitedAuthenticationServerP2PDevice2);

  NS_LOG_INFO("Create home authentication server device.");
  Ptr<HomeAuthenticationServerNetDevice> homeAuthenticationServerNetDevice = CreateObject<HomeAuthenticationServerNetDevice>();
  Ptr<NetDevice> authenticationServerP2PDevice = pointToPointNetDeviceContainer2.Get(1);
  homeAuthenticationServerNode->AddDevice(homeAuthenticationServerNetDevice);
  homeAuthenticationServerNetDevice->SetNetDevice(authenticationServerP2PDevice);

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream(to_string(delay) + "ms" + ".dat", std::ios_base::app);
  
  if(RngSeedManager::GetRun() == 1)
    *stream->GetStream() << std::endl;

  BasicNetDeviceHelper basicNetDeviceHelper;
  basicNetDeviceHelper.EnableAscii(stream, mobileNodeNetDevice);
  // basicNetDeviceHelper.EnableAscii(stream, mediaAccessGatewayNetDevice);
  // basicNetDeviceHelper.EnableAscii(stream, visitedAuthenticationServerNetDevice);
  // basicNetDeviceHelper.EnableAscii(stream, homeAuthenticationServerNetDevice);

  Simulator::Schedule(Seconds(0), MakeEvent(&MobileNodeNetDevice::LaunchAuthentication, mobileNodeNetDevice));

  NS_LOG_INFO("Run Simulation.");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO("Done.");

}