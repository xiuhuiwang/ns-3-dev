#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/seq-ts-header.h"

#include <string>
#include <fstream>

#include "ns3/exp-data-header.h"
#include "ns3/seq-header.h"

#include "compression-receiver.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CompressionReceiver");
NS_OBJECT_ENSURE_REGISTERED (CompressionReceiver);

TypeId
CompressionReceiver::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CompressionReceiver")
    .SetParent<Application> ()
    .AddConstructor<CompressionReceiver> ()
    .AddAttribute ("Port",
                   "Port on which we listen for incoming probe packets.",
                   UintegerValue (100),
                   MakeUintegerAccessor (&CompressionReceiver::m_port),
                   MakeUintegerChecker<uint16_t> ())
          .AddAttribute ("PacketWindowSize",
                         "The size of the window used to compute the packet loss. This value should be a multiple of 8.",
                         UintegerValue (32),
                         MakeUintegerAccessor (&CompressionReceiver::GetPacketWindowSize,
                                               &CompressionReceiver::SetPacketWindowSize),
                         MakeUintegerChecker<uint16_t> (8,256))
//    .AddAttribute ("TcpPort",
//                   "TcpPort on which we listen for incoming tcp connections.",
//                   UintegerValue (10),
//                   MakeUintegerAccessor (&CompressionReceiver::m_tcpPort),
//                   MakeUintegerChecker<uint16_t> ())
     .AddAttribute("NumPackets",
                   "Number of probe packets in the train",
                   UintegerValue (100),
                   MakeUintegerAccessor (&CompressionReceiver::m_numPackets),
                   MakeUintegerChecker<uint32_t> ())
      .AddAttribute("Interval",
                    "The time to wait between probe packets in the train",
                    UintegerValue (100),
                    MakeTimeAccessor (&CompressionReceiver::m_interPacketTime),
                    MakeTimeChecker ())
       .AddAttribute("PacketSize",
                     "Size of packets generated. If less than 12 the timestamp will be ommitted. The minimum packet size is 2 for just the sequence header",
                     UintegerValue (1024),
                     MakeUintegerAccessor (&CompressionReceiver::m_probePacketLen),
                     MakeUintegerChecker<uint32_t> (2, 10000))
  ;
  return tid;
}

CompressionReceiver::CompressionReceiver()
: m_lossCounter (0)
{
  NS_LOG_FUNCTION (this);
  m_received=0;
  m_results=NULL;
}

CompressionReceiver::~CompressionReceiver()
{
    NS_LOG_FUNCTION (this);
}


    uint16_t
    CompressionReceiver::GetPacketWindowSize () const
    {
        NS_LOG_FUNCTION (this);
       // return 0;
  return m_lossCounter.GetBitMapSize ();
    }

    void
    CompressionReceiver::SetPacketWindowSize (uint16_t size)
    {
        NS_LOG_FUNCTION (this << size);
      m_lossCounter.SetBitMapSize (size);
    }

    uint32_t
    CompressionReceiver::GetLost (void) const
    {
        NS_LOG_FUNCTION (this);
       // return 0;
      return m_lossCounter.GetLost ();
    }

    uint64_t
    CompressionReceiver::GetReceived (void) const
    {
        NS_LOG_FUNCTION (this);
        return m_received;
    }

    void
    CompressionReceiver::SetLogFileName (std::string name)
    {
        NS_LOG_FUNCTION (name);
        m_name = name;
    }

    void
    CompressionReceiver::DoDispose (void)
    {
        NS_LOG_FUNCTION (this);
        Application::DoDispose ();
    }

    void
    CompressionReceiver::StartApplication (void)
    {
        NS_LOG_FUNCTION (this);

        if (m_socket == 0)
        {
            TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
            m_socket = Socket::CreateSocket (GetNode (), tid);
            InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (),
                                                         m_port);
            if (m_socket->Bind (local) == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
        }

        m_socket->SetRecvCallback (MakeCallback (&CompressionReceiver::HandleRead, this));

        if (m_socket6 == 0)
        {
            TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
            m_socket6 = Socket::CreateSocket (GetNode (), tid);
            Inet6SocketAddress local = Inet6SocketAddress (Ipv6Address::GetAny (),
                                                           m_port);
            if (m_socket6->Bind (local) == -1)
            {
                NS_FATAL_ERROR ("Failed to bind socket");
            }
        }

        m_socket6->SetRecvCallback (MakeCallback (&CompressionReceiver::HandleRead, this));

        Initialize();
    }

    void
    CompressionReceiver::StopApplication ()
    {
        NS_LOG_FUNCTION (this);

        if (m_socket != 0)
        {
            m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
        }
    }

    void
    CompressionReceiver::HandleRead (Ptr<Socket> socket)
    {
        NS_LOG_FUNCTION (this << socket);
        Ptr<Packet> packet;
        Address from;
//  Address localAddress;
        while ((packet = socket->RecvFrom (from)))
        {
//      socket->GetSockName (localAddress);
//      m_rxTrace (packet);
//      m_rxTraceWithAddresses (packet, from, localAddress);
            if (packet->GetSize () > 0)
            {
                SeqTsHeader seqTs;
                packet->RemoveHeader (seqTs);
                uint32_t currentSequenceNumber = seqTs.GetSeq ();
                uint64_t ts = Simulator::Now().GetTimeStep();
                if (InetSocketAddress::IsMatchingType (from))
                {
                    NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
                                                   " bytes from "<< InetSocketAddress::ConvertFrom (from).GetIpv4 () <<
                                                   " Sequence Number: " << currentSequenceNumber <<
                                                   " Uid: " << packet->GetUid () <<
                                                   " TXtime: " << seqTs.GetTs () <<
                                                   " RXtime: " << Simulator::Now () <<
                                                   " Delay: " << Simulator::Now () - seqTs.GetTs ());
                }
                else if (Inet6SocketAddress::IsMatchingType (from))
                {
                    NS_LOG_INFO ("TraceDelay: RX " << packet->GetSize () <<
                                                   " bytes from "<< Inet6SocketAddress::ConvertFrom (from).GetIpv6 () <<
                                                   " Sequence Number: " << currentSequenceNumber <<
                                                   " Uid: " << packet->GetUid () <<
                                                   " TXtime: " << seqTs.GetTs () <<
                                                   " RXtime: " << Simulator::Now () <<
                                                   " Delay: " << Simulator::Now () - seqTs.GetTs ());
                }

//          m_lossCounter.NotifyReceived (currentSequenceNumber);
                m_received++;
                m_results[currentSequenceNumber] = ts;
            }
        }
    }


    void
    CompressionReceiver::Initialize (void)
    {
        m_results = new int64_t[m_numPackets];
        for (uint32_t i = 0; i < m_numPackets; i++)
            m_results[i] = -1;
    }


    void
    CompressionReceiver::Process (void)
    {
        NS_LOG_FUNCTION (this);
        NS_LOG_INFO ("Processing experiment results");

        std::stringstream ss;
        std::string results = "";
        std::ofstream output;
        output.open(m_name.c_str());

        uint32_t chunkSize = m_numPackets;
        for (uint32_t chunk = 0; chunk < chunkSize; chunk++)
        {
            ss << chunk;
            ss << "\t";
            ss << m_results[chunk];
            ss << "\n";
            results = ss.str();
            output << results;
            ss.str("");
        }
        output.close();
        delete [] m_results;

        NS_LOG_INFO ("Saved results to: " << m_name);
        std::cout<<"Packets Lost:"<<GetLost()<<std::endl;
        std::cout<<"ChunckSize:"<<chunkSize<<std::endl;
        std::cout<<"Recieved:"<<m_received<<std::endl;
        std::cout<<"Saved results to: "<<m_name<<std::endl;
        return;
    }
} // Namespace ns3

