/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file tcpStream.cpp
    \brief Implementation of the TCPStream class.

*/

#include "../include/imebra/tcpStream.h"
#include "../include/imebra/tcpAddress.h"
#include "../implementation/tcpSequenceStreamImpl.h"

namespace imebra
{

TCPStream::TCPStream(const TCPActiveAddress& address):
    BaseStreamInput(std::make_shared<implementation::tcpSequenceStream>(address.m_pAddress)),
    BaseStreamOutput(std::static_pointer_cast<implementation::tcpSequenceStream>(BaseStreamInput::m_pInputStream))
{
}

TCPStream::TCPStream(std::shared_ptr<imebra::implementation::tcpSequenceStream> pTcpStream):
    BaseStreamInput(pTcpStream),
    BaseStreamOutput(pTcpStream)
{

}

TCPAddress* TCPStream::getPeerAddress() const
{
    return new TCPAddress((std::static_pointer_cast<implementation::tcpSequenceStream>(m_pInputStream))->getPeerAddress());
}

void TCPStream::terminate()
{
    (std::static_pointer_cast<implementation::tcpSequenceStream>(m_pInputStream))->terminate();
}

}

