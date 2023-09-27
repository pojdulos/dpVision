/*
Copyright 2005 - 2017 by Paolo Brandoli/Binarno s.p.

Imebra is available for free under the GNU General Public License.

The full text of the license is available in the file license.rst
 in the project root folder.

If you do not want to be bound by the GPL terms (such as the requirement
 that your application must also be GPL), you may purchase a commercial
 license for Imebra from the Imebra’s website (http://imebra.com).
*/

/*! \file acse.cpp
    \brief Implementation of the the ACSE classes.
*/

#include "../include/imebra/acse.h"
#include "../include/imebra/streamReader.h"
#include "../include/imebra/streamWriter.h"
#include "../include/imebra/dataSet.h"
#include "../implementation/acseImpl.h"

namespace imebra
{

PresentationContext::PresentationContext(const std::string& abstractSyntax):
    m_pPresentationContext(std::make_shared<implementation::presentationContext>(abstractSyntax))
{
}

PresentationContext::PresentationContext(const std::string& abstractSyntax, bool bSCURole, bool bSCPRole):
    m_pPresentationContext(std::make_shared<implementation::presentationContext>(abstractSyntax, bSCURole, bSCPRole))
{
}

void PresentationContext::addTransferSyntax(const std::string& transferSyntax)
{
    m_pPresentationContext->addTransferSyntax(transferSyntax);

}

PresentationContext::~PresentationContext()
{
}

PresentationContexts::PresentationContexts():
    m_pPresentationContexts(std::make_shared<implementation::presentationContexts>())
{
}

PresentationContexts::~PresentationContexts()
{

}

void PresentationContexts::addPresentationContext(const PresentationContext& presentationContext)
{
    m_pPresentationContexts->m_presentationContexts.push_back(presentationContext.m_pPresentationContext);
}


AssociationMessage::AssociationMessage()
{
}


AssociationMessage::AssociationMessage(std::shared_ptr<implementation::associationMessage> pMessage):
    m_pMessage(pMessage)
{
}

AssociationMessage::AssociationMessage(const std::string& abstractSyntax):
    m_pMessage(std::make_shared<implementation::associationMessage>(abstractSyntax))
{
}

std::string AssociationMessage::getAbstractSyntax() const
{
    return m_pMessage->getAbstractSyntax();
}

DataSet* AssociationMessage::getCommand() const
{
    return new DataSet(m_pMessage->getCommandDataSet());
}

DataSet* AssociationMessage::getPayload() const
{
    IMEBRA_FUNCTION_START();

    std::shared_ptr<implementation::dataSet> pDataSet(m_pMessage->getPayloadDataSetNoThrow());
    if(pDataSet == nullptr)
    {
        IMEBRA_THROW(AcseNoPayloadError, "Payload not available");
    }
    return new DataSet(pDataSet);

    IMEBRA_FUNCTION_END();
}

bool AssociationMessage::hasPayload() const
{
    return m_pMessage->getPayloadDataSetNoThrow() != nullptr;
}

void AssociationMessage::addDataSet(const DataSet& dataSet)
{
    m_pMessage->addDataset(dataSet.m_pDataSet);
}



AssociationBase::AssociationBase()
{

}

AssociationBase::~AssociationBase()
{

}

AssociationMessage* AssociationBase::getCommand()
{
    return new AssociationMessage(m_pAssociation->getCommand());
}

AssociationMessage* AssociationBase::getResponse(std::uint16_t messageId)
{
    return new AssociationMessage(m_pAssociation->getResponse(messageId));
}

void AssociationBase::sendMessage(const AssociationMessage& messageDataSet)
{
    m_pAssociation->sendMessage(messageDataSet.m_pMessage);
}

void AssociationBase::release()
{
    m_pAssociation->release();
}

void AssociationBase::abort()
{
    m_pAssociation->abort(implementation::acsePDUAAbort::reason_t::serviceUser);
}

std::string AssociationBase::getThisAET() const
{
    return m_pAssociation->getThisAET();
}

std::string AssociationBase::getOtherAET() const
{
    return m_pAssociation->getOtherAET();
}

std::string AssociationBase::getTransferSyntax(const std::string &abstractSyntax) const
{
    return m_pAssociation->getPresentationContextTransferSyntax(abstractSyntax);
}


AssociationSCU::AssociationSCU(
        const std::string& thisAET,
        const std::string& otherAET,
        std::uint32_t invokedOperations,
        std::uint32_t performedOperations,
        const PresentationContexts& presentationContexts,
        StreamReader& pInput,
        StreamWriter& pOutput,
        std::uint32_t dimseTimeoutSeconds)
{
    m_pAssociation = std::make_shared<implementation::associationSCU>(
                presentationContexts.m_pPresentationContexts,
                thisAET,
                otherAET,
                invokedOperations,
                performedOperations,
                pInput.m_pReader,
                pOutput.m_pWriter,
                dimseTimeoutSeconds);
}


AssociationSCP::AssociationSCP(
        const std::string& thisAET,
        std::uint32_t invokedOperations,
        std::uint32_t performedOperations,
        const PresentationContexts& presentationContexts,
        StreamReader& pInput,
        StreamWriter& pOutput,
        std::uint32_t dimseTimeoutSeconds,
        std::uint32_t artimTimeoutSeconds)
{
    m_pAssociation = std::make_shared<implementation::associationSCP>(
                presentationContexts.m_pPresentationContexts,
                thisAET,
                invokedOperations,
                performedOperations,
                pInput.m_pReader,
                pOutput.m_pWriter,
                dimseTimeoutSeconds,
                artimTimeoutSeconds);
}




}
