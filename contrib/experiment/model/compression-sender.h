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

  // Set the number of packets, inter packet departure spacing, packet length, entropy, and filename
  void SetNumPackets (uint32_t count);
  void SetInterval (Time interval);
  void SetPacketLen (uint32_t size);
  void SetEntropy (uint8_t entropy);
  void SetLogFileName (std::string name);
  void SetV4Ping (ApplicationContainer* v4ping_1, ApplicationContainer* v4ping_2);
  int64_t GetV4Ping1Start (void);
  int64_t GetV4Ping2Start (void);
  // void SetIncludeTs (uint8_t includeTs);

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

  Address m_peerAddress;

  // Port number for probe packets
  uint16_t m_peerPort;

  // Next scheduled event
  EventId m_sendEvent;


  // Name of the output file
  std::string m_name;

  // Generates packets filled with compressable data
  CompressionPacketGenerator m_gen;

  uint32_t m_initialPacketTrainLength;
  
  ApplicationContainer* m_v4ping_1;  // ping application sent at the beginning
  ApplicationContainer* m_v4ping_2;  // ping application sent at the end
  int64_t m_v4ping_1_start;
  int64_t m_v4ping_2_start;
  }; // CompressionSender
  
  
} // namespace ns3

#endif /* COMPRESSION_SENDER_H */
