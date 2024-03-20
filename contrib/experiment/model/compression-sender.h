#ifndef COMPRESSION_SENDER_H
#define COMPRESSION_SENDER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-address.h"
#include "ns3/applications-module.h"

#include <string>

namespace ns3 {

class Socket;
class Packet;
class CompressionPacketGenerator;

/**
 * \sends probe packets to CompressionReceiver and analyzes the results it recieves
 */
class CompressionSender : public Application
{
public:
  static TypeId GetTypeId (void);
  CompressionSender ();
  virtual ~CompressionSender ();

  /**
   * \brief set the remote address and port
   * \param ip remote IP address
   * \param port remote port
   */
  void SetRemote (Address ip, uint16_t port);
  void SetRemote (Address addr);
  void SetTcpPort (uint16_t port_head, uint16_t port_tail);

  // Set the number of packets, inter packet departure spacing, packet length, entropy, and filename
  void SetNumPackets (uint32_t count);
  void SetInterval (Time interval);
  void SetPacketLen (uint32_t size);
  void SetEntropy (uint8_t entropy);
  void SetLogFileName (std::string name);
  void SendTcpTailPacket (void);

protected:
  virtual void DoDispose (void);

private:

//
// Private Member Functions
//

  // If mode 0, sends request for TCP connection
  // If mode 1, schedule SendICMP()
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void Send (void);  

  // // Record the time when a RST packet is received
  // void HandleRstPacket (Ptr<Socket> socket);
  // Maximum number of probe packets in the train
  uint32_t m_count;

  // Time interval between probe packets in the train
  Time m_interval;

  // Size of probe packets
  uint32_t m_size;

  // Tracks the number of probe packets already sent
  uint32_t m_sent;

  // Entropy of probe packet data
  uint8_t m_entropy;

  // Pointer to the socket to send probe packets to
  Ptr<Socket> m_socket;
  Ptr<Socket> m_socket_tcp;

  Address m_peerAddress;
  // Port number for probe packets
  uint16_t m_peerPort;
  
  // Port number for tcp head and tail packets
  uint16_t m_tcpPort_head;
  uint16_t m_tcpPort_tail;
  Time m_tcp_head_rstReceivedTime;   // Time when the head packet's RST is received
  Time m_tcp_tail_rstReceivedTime;   // Time when the tail packet's RST is received

  // Next scheduled event
  EventId m_sendEvent;

  // Name of the output file
  std::string m_name;

  // Generates packets filled with compressable data
  CompressionPacketGenerator m_gen;

  uint32_t m_initialPacketTrainLength;
  
  }; // CompressionSender
  
} // namespace ns3

#endif /* COMPRESSION_SENDER_H */
