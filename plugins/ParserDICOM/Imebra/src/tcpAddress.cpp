/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file tcpAddress.cpp
    \brief Implementation of the TCPAddress class.

*/

#include "../include/imebra/tcpAddress.h"
#include "../implementation/tcpSequenceStreamImpl.h"

namespace imebra
{

TCPAddress::TCPAddress(std::shared_ptr<implementation::tcpAddress> pAddress):
    m_pAddress(pAddress)
{
}

std::string TCPAddress::getNode() const
{
    return m_pAddress->getNode();
}

std::string TCPAddress::getService() const
{
    return m_pAddress->getService();
}

TCPAddress::~TCPAddress()
{
}


TCPActiveAddress::TCPActiveAddress(const std::string& node, const std::string& service):
    TCPAddress(std::make_shared<implementation::tcpAddress>(node, service, implementation::tcpAddress::passiveSocket_t::active))
{
}

TCPPassiveAddress::TCPPassiveAddress(const std::string& node, const std::string& service):
    TCPAddress(std::make_shared<implementation::tcpAddress>(node, service, implementation::tcpAddress::passiveSocket_t::passive))
{
}

}
