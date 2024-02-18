#ifndef COMPRESSION_RECEIVER_H
#define COMPRESSION_RECEIVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include <string>
#include "ns3/traced-callback.h"
#include "ns3/packet-loss-counter.h"

namespace ns3 {

class CompressionReceiver : public Application
{
public:
  static TypeId GetTypeId (void);
  CompressionReceiver ();
  virtual ~CompressionReceiver ();

/**
   * \brief Returns the number of lost packets
   * \return the number of lost packets
   */
    uint32_t GetLost (void) const;

    /**
     * \brief Returns the number of received packets
     * \return the number of received packets
     */
    uint64_t GetReceived (void) const;

    /**
     * \brief Returns the size of the window used for checking loss.
     * \return the size of the window used for checking loss.
     */
    uint16_t GetPacketWindowSize () const;

    void SetLogFileName (std::string name);

    /**
   * \brief Set the size of the window used for checking loss. This value should
   *  be a multiple of 8
   * \param size the size of the window used for checking loss. This value should
   *  be a multiple of 8
   */
    void SetPacketWindowSize (uint16_t size);

    void Process (void);
protected:
    virtual void DoDispose (void);

private:

    virtual void StartApplication (void);
    virtual void StopApplication (void);

    /**
     * \brief Handle a packet reception.
     *
     * This function is called by lower layers.
     *
     * \param socket the socket the packet was received to.
     */
    void HandleRead (Ptr<Socket> socket);
    void Initialize (void);

    uint16_t m_port; //!< Port on which we listen for incoming packets.
    Ptr<Socket> m_socket; //!< IPv4 Socket
    Ptr<Socket> m_socket6; //!< IPv6 Socket
    uint64_t m_received; //!< Number of received packets
    PacketLossCounter m_lossCounter; //!< Lost packet counter

// Name of the output file
        std::string m_name;
        uint32_t m_numPackets;
        Time m_interPacketTime;
        uint32_t m_probePacketLen;
        int64_t* m_results;

        /// Callbacks for tracing the packet Rx events
//  TracedCallback<Ptr<const Packet> > m_rxTrace;

        /// Callbacks for tracing the packet Rx events, includes source and destination addresses
//  TracedCallback<Ptr<const Packet>, const Address &, const Address &> m_rxTraceWithAddresses;

};

} // namespace ns3

#endif /* COMPRESSION_RECEIVER_H */
