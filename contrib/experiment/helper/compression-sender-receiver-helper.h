#ifndef COMPRESSION_SENDER_RECEIVER_HELPER_H
#define COMPRESSION_SENDER_RECEIVER_HELPER_H

#include <stdint.h>
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/compression-sender.h"
#include "ns3/compression-receiver.h"

namespace ns3 {

class CompressionReceiverHelper
{
public:
  CompressionReceiverHelper ();

  /**
   * Record an attribute to be set in each Application after it is is created.
   *
   * \param name the name of the attribute to set
   * \param value the value of the attribute to set
   */


  CompressionReceiverHelper (uint16_t port);


  void SetAttribute (std::string name, const AttributeValue &value);

  /**
   * Create one compression receiver application on each of the Nodes in the
   * NodeContainer.
   *
   * \param c The nodes on which to create the Applications.  The nodes
   *          are specified by a NodeContainer.
   * \returns The applications created, one Application per Node in the
   *          NodeContainer.
   */
  ApplicationContainer Install (NodeContainer c);

  Ptr<CompressionReceiver> GetReceiver (void);

private:
  ObjectFactory m_factory;
  Ptr<CompressionReceiver> m_receiver;
};


class CompressionSenderHelper
{
public:
  CompressionSenderHelper ();

  CompressionSenderHelper (Address ip, uint16_t port);

  CompressionSenderHelper (Address addr);

  //Record an attribute to be set in each Application after it is is created.
  void SetAttribute (std::string name, const AttributeValue &value);

  // Create one compression sender application on each of the input nodes,
  // returns the applications created
  ApplicationContainer Install (NodeContainer c);

  // Return a pointer to m_sender
  Ptr<CompressionSender> GetSender (void);

private:
  ObjectFactory m_factory;
  Ptr<CompressionSender> m_sender;

};

class UdpTraceClientHelperC
{
    public:
        /**
         * Create UdpTraceClientHelper which will make life easier for people trying
         * to set up simulations with udp-client-server.
         *
         */
        UdpTraceClientHelperC ();

        /**
         * Create UdpTraceClientHelper which will make life easier for people trying
         * to set up simulations with udp-client-server. Use this variant with
         * addresses that do not include a port value (e.g., Ipv4Address and
         * Ipv6Address).
         *
         * \param ip The IP address of the remote UDP server
         * \param port The port number of the remote UDP server
         * \param filename the file from which packet traces will be loaded
         */
        UdpTraceClientHelperC (Address ip, uint16_t port, std::string filename);
        /**
         * Create UdpTraceClientHelper which will make life easier for people trying
         * to set up simulations with udp-client-server. Use this variant with
         * addresses that do include a port value (e.g., InetSocketAddress and
         * Inet6SocketAddress).
         *
         * \param addr The address of the remote UDP server
         * \param filename the file from which packet traces will be loaded
         */
        UdpTraceClientHelperC (Address addr, std::string filename);

        /**
          * Record an attribute to be set in each Application after it is is created.
          *
          * \param name the name of the attribute to set
          * \param value the value of the attribute to set
          */
        void SetAttribute (std::string name, const AttributeValue &value);

        /**
          * \param c the nodes
          *
          * Create one UDP trace client application on each of the input nodes
          *
          * \returns the applications created, one application per input node.
          */
        ApplicationContainer Install (NodeContainer c);

    private:
        ObjectFactory m_factory; //!< Object factory.
};
} // namespace ns3

#endif /* COMPRESSION_SENDER_RECEIVER_HELPER_H */
