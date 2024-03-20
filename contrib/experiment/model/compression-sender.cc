
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/seq-ts-header.h"
#include "ns3/pointer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/exp-data-header.h"
#include "ns3/seq-header.h"

#include "compression-app-layer.h"
#include "compression-packet-gen.h"
#include "compression-sender.h"
#include "custom-tcp-socket-base.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CompressionSender");
NS_OBJECT_ENSURE_REGISTERED (CompressionSender);

TypeId
CompressionSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CompressionSender")
    .SetParent<Application> ()
    .AddConstructor<CompressionSender> ()
    .AddAttribute ("MaxPackets",
                   "Number of probe packets in the train",
                   UintegerValue (100),
                   MakeUintegerAccessor (&CompressionSender::m_count),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Interval",
                   "The time to wait between probe packets in the train", 
		   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&CompressionSender::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("PacketSize",
                   "Size of packets generated. If less than 12 the timestamp will be ommitted. The minimum packet size is 2 for just the sequence header",
                   UintegerValue (1024),
                   MakeUintegerAccessor (&CompressionSender::m_size),
                   MakeUintegerChecker<uint32_t> (12,65507))
    .AddAttribute ("RemoteAddress",
                   "The Address of the CompressionReceiver",
                   AddressValue (),
                   MakeAddressAccessor (&CompressionSender::m_peerAddress),
                   MakeAddressChecker ())
    .AddAttribute ("RemotePort", "The destination port of the outbound probe packets",
                   UintegerValue (9),
                   MakeUintegerAccessor (&CompressionSender::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
  //  .AddAttribute ("RemoteTcpPort", "The destination port of the outbound TCP connections",
  //                 UintegerValue (10),
  //                 MakeUintegerAccessor (&CompressionSender::m_peerTcpPort),
  //                 MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("InitialPacketTrainLength",
                         "Number of non-apt priority probe packets initially sent to saturate the queue",
                         UintegerValue (0),
                         MakeUintegerAccessor (&CompressionSender::m_initialPacketTrainLength),
                         MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

CompressionSender::CompressionSender ()
{
    NS_LOG_FUNCTION (this);//  m_initialized = 0;
  m_sent = 0;
  m_socket = 0;
//  m_tcpSocket = 0;
  m_sendEvent = EventId ();
  m_initialPacketTrainLength=0;
//   m_v4ping_1 = 0;
//   m_v4ping_2 = 0;
}

CompressionSender::~CompressionSender ()
{
    NS_LOG_FUNCTION (this);
}

    void
    CompressionSender::SetRemote (Address ip, uint16_t port)
    {
        NS_LOG_FUNCTION (this << ip << port);
        m_peerAddress = ip;
        m_peerPort = port;
    }

    void
    CompressionSender::SetRemote (Address addr)
    {
        NS_LOG_FUNCTION (this << addr);
        m_peerAddress = addr;
    }

void 
CompressionSender::SetNumPackets (uint32_t count)
{
    NS_LOG_FUNCTION (this);
  m_count = count;
}

void 
CompressionSender::SetInterval (Time interval)
{
    NS_LOG_FUNCTION (this);
  m_interval = interval;
}

void 
CompressionSender::SetPacketLen (uint32_t size)
{
    NS_LOG_FUNCTION (this);
  m_size = size;
}

void 
CompressionSender::SetEntropy (uint8_t entropy)
{
    NS_LOG_FUNCTION (this);
  m_entropy = entropy;
}

void
CompressionSender::SetLogFileName (std::string name)
{
    NS_LOG_FUNCTION (this);
  m_name = name;
}

// void 
// CompressionSender::SetV4Ping (ApplicationContainer* v4ping_1, ApplicationContainer* v4ping_2)
// {
//     NS_LOG_FUNCTION (this);
//     m_v4ping_1 = v4ping_1;
//     m_v4ping_2 = v4ping_2;
// }


void
CompressionSender::DoDispose (void)
{
    NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void
CompressionSender::StartApplication (void)
{
    NS_LOG_FUNCTION (this);

  // Set up packet generator
  if (m_size >= 12) {
      m_gen.SetSize(m_size - 12);
  }else {
      m_gen.SetSize(m_size - 2);
      std::cout<<"In CompressionSender::StartApplication:Size is less than 12"<<m_size<<std::endl;
  }
  m_gen.SetEntropy(m_entropy);
  m_gen.Initialize();


    if (m_socket == 0)
    {
        TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
        m_socket = Socket::CreateSocket (GetNode (), tid);
        if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
        {
            if (m_socket->Bind () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
        else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
        {
            if (m_socket->Bind6 () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_peerPort));
        }
        else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
        {
            if (m_socket->Bind () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket->Connect (m_peerAddress);
        }
        else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
        {
            if (m_socket->Bind6 () == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
            m_socket->Connect (m_peerAddress);
        }
        else
        {
            NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
        }
    }

    if (m_socket_tcp == 0)
        {
            TypeId tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
            m_socket_tcp = Socket::CreateSocket (GetNode (), tid);
            if (Ipv4Address::IsMatchingType(m_peerAddress) == true)
            {
                if (m_socket_tcp->Bind () == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                m_socket_tcp->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_tcpPort_head));
            }
            else if (Ipv6Address::IsMatchingType(m_peerAddress) == true)
            {
                if (m_socket_tcp->Bind6 () == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                m_socket_tcp->Connect (Inet6SocketAddress (Ipv6Address::ConvertFrom(m_peerAddress), m_tcpPort_head));
            }
            else if (InetSocketAddress::IsMatchingType (m_peerAddress) == true)
            {
                if (m_socket_tcp->Bind () == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                m_socket_tcp->Connect (m_peerAddress);
            }
            else if (Inet6SocketAddress::IsMatchingType (m_peerAddress) == true)
            {
                if (m_socket_tcp->Bind6 () == -1)
                {
                    NS_FATAL_ERROR ("Failed to bind socket");
                }
                m_socket_tcp->Connect (m_peerAddress);
            }
            else
            {
                NS_ASSERT_MSG (false, "Incompatible address type: " << m_peerAddress);
            }
        }


    // if (m_v4ping_1 != 0)
    // {
    //     m_v4ping_1->Start (Now());
    //     m_v4ping_1_start = Simulator::Now().GetMilliSeconds();
    //     // std::cout<<"In CompressionSender::StartApplication:Starting v4ping at " << Now() << std::endl;
    // }

    m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    m_socket->SetAllowBroadcast (true);                 
                                    
    m_sendEvent = Simulator::Schedule (Seconds (0.0), &CompressionSender::Send, this);
    Simulator::Schedule (Seconds(2.8), &Socket::Close, m_socket_tcp);
}


void
CompressionSender::StopApplication ()
{
    NS_LOG_FUNCTION (this);
  Simulator::Cancel (m_sendEvent);
}


// int64_t CompressionSender::GetV4Ping1Start() {
//     return m_v4ping_1_start;
// }

// int64_t CompressionSender::GetV4Ping2Start() {
//     return m_v4ping_2_start;
// }

void
CompressionSender::Send (void)
{

    NS_LOG_FUNCTION (this);
  NS_ASSERT (m_sendEvent.IsExpired ());

  // Generate compressable data
//  uint32_t size = m_size;
  Ptr<Packet> p;
  p = m_gen.GeneratePacket();

      SeqTsHeader seqTs;
      seqTs.SetSeq (m_sent);
      p->AddHeader (seqTs);

  // Covert m_peerAddress for logging
    std::stringstream peerAddressStringStream;
    if (Ipv4Address::IsMatchingType (m_peerAddress))
    {
        peerAddressStringStream << Ipv4Address::ConvertFrom (m_peerAddress);
    }
    else if (Ipv6Address::IsMatchingType (m_peerAddress))
    {
        peerAddressStringStream << Ipv6Address::ConvertFrom (m_peerAddress);
    }

    if ((m_socket->Send (p)) >= 0)
    {
        ++m_sent;
        NS_LOG_INFO ("TraceDelay TX " << m_size << " bytes to "
                                      << peerAddressStringStream.str () << " Uid: "
                                      << p->GetUid () << " Time: "
                                      << (Simulator::Now ()).GetSeconds ());

    }
    else
    {
        NS_LOG_INFO ("Error while sending " << m_size << " bytes to "
                                            << peerAddressStringStream.str ());
    }
    
    if (m_sent < m_count + m_initialPacketTrainLength)
    {   
        m_sendEvent = Simulator::Schedule (m_interval, &CompressionSender::Send, this);
    }else {
        Simulator::Schedule (m_interval, &CompressionSender::SendTcpTailPacket, this);
    }
}

void CompressionSender::SetTcpPort (uint16_t port_head, uint16_t port_tail){
    m_tcpPort_head = port_head;
    m_tcpPort_tail = port_tail;
}

void
CompressionSender::SendTcpTailPacket (){
    m_socket_tcp->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_peerAddress), m_tcpPort_tail));
}


} // Namespace ns3
