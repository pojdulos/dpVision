/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file pipe.cpp
    \brief Implementation of the DataSink and DataSource classes.

*/

#include "../include/imebra/pipe.h"
#include "../include/imebra/readWriteMemory.h"
#include "../implementation/pipeImpl.h"

namespace imebra
{

Pipe::Pipe(size_t circularBufferSize):
    BaseStreamInput(std::make_shared<implementation::pipeSequenceStream>(circularBufferSize)),
    BaseStreamOutput((std::static_pointer_cast<implementation::pipeSequenceStream>(m_pInputStream)))
{
}

void Pipe::feed(const ReadMemory& buffer)
{
    size_t dataSize(0);
    const char* pData(buffer.data(&dataSize));
    (std::static_pointer_cast<implementation::pipeSequenceStream>(m_pInputStream))->write((std::uint8_t*)pData, dataSize);
}


size_t Pipe::sink(ReadWriteMemory& buffer)
{
    size_t bufferSize;
    char* pData(buffer.data(&bufferSize));
    return (std::static_pointer_cast<implementation::pipeSequenceStream>(m_pOutputStream))->read((std::uint8_t*)pData, bufferSize);

}


void Pipe::close(unsigned int timeoutMilliseconds)
{
     (std::static_pointer_cast<implementation::pipeSequenceStream>(m_pOutputStream))->close(timeoutMilliseconds);
}

void Pipe::terminate()
{
     (std::static_pointer_cast<implementation::pipeSequenceStream>(m_pInputStream))->terminate();
}

}

