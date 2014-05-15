/**
 * Copyright (c) 2014 Luis Saavedra. All rights reserved.
 *
 * This file is part of XBeeNG.
 *
 * XBeeNG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBeeNG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBeeNG.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "XBeeNG.h"

XBeeApiFrame::XBeeApiFrame() { _cmdData = nullptr; }
XBeeApiFrame::~XBeeApiFrame() {
    if (_cmdData != nullptr) delete[] _cmdData;
    _cmdData = nullptr;
}

uint8_t
XBeeApiFrame::getMsbLength() { return _msbLength; }
void
XBeeApiFrame::setMsbLength(const uint8_t& msbLength) { _msbLength = msbLength; }

uint8_t
XBeeApiFrame::getLsbLength() { return _lsbLength; }
void
XBeeApiFrame::setLsbLength(const uint8_t& lsbLength) { _lsbLength = lsbLength; }

uint16_t
XBeeApiFrame::getFrameDataLength() {
    return ((_msbLength << 8) & 0xffff) + (_lsbLength & 0xff);
}
void
XBeeApiFrame::setFrameDataLength(const uint16_t& frameDataLength) {
    _msbLength = 0xff & (frameDataLength >> 8);
    _lsbLength = 0xff & frameDataLength;
}

uint8_t
XBeeApiFrame::getCmdId() { return _cmdId; }
void
XBeeApiFrame::setCmdId(const uint8_t& cmdId) { _cmdId = cmdId; }

uint16_t
XBeeApiFrame::getCmdDataLength() {
    uint16_t frameDataLength = getFrameDataLength();
    if (frameDataLength == 0) return 0;
    return frameDataLength - 1;
}
void
XBeeApiFrame::setCmdDataLength(const uint16_t& cmdDataLength) {
    setFrameDataLength(cmdDataLength + 1);
}

uint8_t*
XBeeApiFrame::getCmdData() { return _cmdData; }
void
XBeeApiFrame::setCmdData(uint8_t* cmdData) { _cmdData = cmdData; }

uint8_t
XBeeApiFrame::getChecksum() { return _checksum; }
void
XBeeApiFrame::setChecksum(const uint8_t& checksum) { _checksum = checksum; }
void
XBeeApiFrame::setChecksum() {
    uint8_t checksum = 0;

    checksum += _cmdId;

    uint16_t length = getCmdDataLength();
    for (uint16_t i = 0; i < length; i++) {
        checksum += _cmdData[i];
    }

    checksum = 0xff - checksum;

    setChecksum(checksum);
}

bool
XBeeApiFrame::isReady() { return _ready; }
void
XBeeApiFrame::setReady(const bool& ready) { _ready = ready; }

bool
XBeeApiFrame::hasErrors() { return _errorCode > 0; }
uint8_t
XBeeApiFrame::getErrorCode() { return _errorCode; }
void
XBeeApiFrame::setErrorCode(const uint8_t& errorCode) { _errorCode = errorCode; }

#ifdef XBEENG_WITH_EXTRAS
void
XBeeApiFrame::printSummary(Stream& strm) {
    strm.print('\r');
    strm.print(F("cmdID: ")); printHex(strm, this->getCmdId()); strm.print('\r');

    // All FrameIdDescription descendants:
    if (_cmdId == TX_64_REQUEST || _cmdId == TX_16_REQUEST ||
        _cmdId == AT_COMMAND || _cmdId == AT_QUEUE_COMMAND ||
        _cmdId == TX_REQUEST || _cmdId == EXPLICIT_TX_REQUEST ||
        _cmdId == REMOTE_AT_COMMAND || _cmdId == AT_COMMAND_RESPONSE ||
        _cmdId == TX_STATUS_RESPONSE ||
        _cmdId == TX_STATUS || _cmdId == REMOTE_AT_COMMAND_RESPONSE) {
        FrameIdDescription* pt = static_cast<FrameIdDescription*>(this);
        strm.print(F("Frame ID: ")); printHex(strm, pt->getFrameId()); strm.print('\r');
        // All TxRxFrameIdDescription descendants:
        if (_cmdId == TX_64_REQUEST || _cmdId == TX_REQUEST || _cmdId == EXPLICIT_TX_REQUEST ||
            _cmdId == REMOTE_AT_COMMAND || _cmdId == REMOTE_AT_COMMAND_RESPONSE) {
            TxRxFrameIdDescription* xpt = static_cast<TxRxFrameIdDescription*>(this);
            strm.print(F("Address64Msb: "));
            uint32_t address64Msb = xpt->getAddress64Msb(); uint8_t* address64MsbPtr = (uint8_t*)&address64Msb;
            for (uint8_t i = 0; i < 4; i++) {
                printHex(strm, address64MsbPtr[i]);
                if (i != 3) strm.print(F(" "));
                else strm.print('\r');
            }
            strm.print(F("Address64Lsb: "));
            uint32_t address64Lsb = xpt->getAddress64Lsb(); uint8_t* address64LsbPtr = (uint8_t*)&address64Lsb;
            for (uint8_t i = 0; i < 4; i++) {
                printHex(strm, address64LsbPtr[i]);
                if (i != 3) strm.print(F(" "));
                else strm.print('\r');
            }
            if (_cmdId != TX_64_REQUEST) {
                strm.print(F("Address16: "));
                uint16_t address16 = xpt->getAddress16(); uint8_t* address16Ptr = (uint8_t*)&address16;
                for (uint8_t i = 0; i < 2; i++) {
                    printHex(strm, address16Ptr[i]);
                    if (i != 1) strm.print(F(" "));
                    else strm.print('\r');
                }
            }
        }
    }
    // All TxRxXBeeApiFrame descendants
    if (_cmdId == RX_64_RESPONSE || _cmdId == RX_RESPONSE || _cmdId == EXPLICIT_RX_RESPONSE ||
        _cmdId == RX_DATA_SAMPLE || _cmdId == RX_NODE_ID) {
        TxRxXBeeApiFrame* pt = static_cast<TxRxXBeeApiFrame*>(this);
        strm.print(F("Address64Msb: "));
        uint32_t address64Msb = pt->getAddress64Msb(); uint8_t* address64MsbPtr = (uint8_t*)&address64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, address64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Address64Lsb: "));
        uint32_t address64Lsb = pt->getAddress64Lsb(); uint8_t* address64LsbPtr = (uint8_t*)&address64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, address64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        if (_cmdId != RX_64_RESPONSE) {
            strm.print(F("Address16: "));
            uint16_t address16 = pt->getAddress16(); uint8_t* address16Ptr = (uint8_t*)&address16;
            for (uint8_t i = 0; i < 2; i++) {
                printHex(strm, address16Ptr[i]);
               if (i != 1) strm.print(F(" "));
                else strm.print('\r');
            }
        }
    }

    switch (_cmdId) {
    case TX_64_REQUEST: {
        Tx64Request* pt = static_cast<Tx64Request*>(this);
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case TX_16_REQUEST: {
        Tx16Request* pt = static_cast<Tx16Request*>(this);
        strm.print(F("Address16: "));
        uint16_t address16 = pt->getAddress16(); uint8_t* address16Ptr = (uint8_t*)&address16;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, address16Ptr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case AT_COMMAND:
    case AT_QUEUE_COMMAND: {
        AtQueueCommand* pt = static_cast<AtQueueCommand*>(this);
        strm.print(F("Command: "));
        uint16_t cmd = pt->getCmd(); uint8_t* cmdPtr = (uint8_t*)&cmd;
        for (uint8_t i = 0; i < 2; i++) {
            strm.print((char)cmdPtr[i]);
        } strm.print('\r');

        uint8_t* param = pt->getParam();
        uint16_t paramLength = pt->getParamLength();
        if (paramLength > 0) strm.print(F("Parameter: "));
        for (int i = 0; i < paramLength; i++) {
            printHex(strm, param[i]);
            if (i != (paramLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case TX_REQUEST: {
        TxRequest* pt = static_cast<TxRequest*>(this);
        strm.print(F("Broadcast: ")); printHex(strm, pt->getBroadcast()); strm.print('\r');
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case EXPLICIT_TX_REQUEST: {
        ExplicitTxRequest* pt = static_cast<ExplicitTxRequest*>(this);
        strm.print(F("Source Endpoint: ")); printHex(strm, pt->getSourceEndpoint()); strm.print('\r');
        strm.print(F("Destination Endpoint: ")); printHex(strm, pt->getDestinationEndpoint()); strm.print('\r');
        strm.print(F("Cluster ID: "));
        uint16_t clusterId = pt->getClusterId(); uint8_t* clusterIdPtr = (uint8_t*)&clusterId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, clusterIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Profile ID: "));
        uint16_t profileId = pt->getProfileId(); uint8_t* profileIdPtr = (uint8_t*)&profileId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, profileIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Broadcast: ")); printHex(strm, pt->getBroadcast()); strm.print('\r');
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case REMOTE_AT_COMMAND: {
        RemoteAtCommand* pt = static_cast<RemoteAtCommand*>(this);
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');
        strm.print(F("Command: "));
        uint16_t cmd = pt->getCmd(); uint8_t* cmdPtr = (uint8_t*)&cmd;
        for (uint8_t i = 0; i < 2; i++) {
            strm.print((char)cmdPtr[i]);
        } strm.print('\r');

        uint8_t* param = pt->getParam();
        uint16_t paramLength = pt->getParamLength();
        if (paramLength > 0) strm.print(F("Parameter: "));
        for (int i = 0; i < paramLength; i++) {
            printHex(strm, param[i]);
            if (i != (paramLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case RX_64_RESPONSE: {
        Rx64Response* pt = static_cast<Rx64Response*>(this);
        strm.print(F("Rssi: ")); printHex(strm, pt->getRssi()); strm.print('\r');
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case RX_16_RESPONSE: {
        Rx16Response* pt = static_cast<Rx16Response*>(this);
        strm.print(F("Address16: "));
        uint16_t address16 = pt->getAddress16(); uint8_t* address16Ptr = (uint8_t*)&address16;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, address16Ptr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Rssi: ")); printHex(strm, pt->getRssi()); strm.print('\r');
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case AT_COMMAND_RESPONSE: {
        AtCommandResponse* pt = static_cast<AtCommandResponse*>(this);
        strm.print(F("Command: "));
        uint16_t cmd = pt->getCmd(); uint8_t* cmdPtr = (uint8_t*)&cmd;
        for (uint8_t i = 0; i < 2; i++) {
            strm.print((char)cmdPtr[i]);
        } strm.print('\r');
        strm.print(F("Status: ")); printHex(strm, pt->getStatus()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case TX_STATUS_RESPONSE: {
        TxStatusResponse* pt = static_cast<TxStatusResponse*>(this);
        strm.print(F("Status: ")); printHex(strm, pt->getStatus()); strm.print('\r');
        strm.print('\r');
    } break;
    case MODEM_STATUS: {
        ModemStatus* pt = static_cast<ModemStatus*>(this);
        strm.print(F("Status: ")); printHex(strm, pt->getStatus()); strm.print('\r');
        strm.print('\r');
    } break;
    case TX_STATUS: {
        TxStatus* pt = static_cast<TxStatus*>(this);
        strm.print(F("Address16: "));
        uint16_t address16 = pt->getAddress16(); uint8_t* address16Ptr = (uint8_t*)&address16;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, address16Ptr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Retry Count: ")); printHex(strm, pt->getRetryCount()); strm.print('\r');
        strm.print(F("Delivery Status: ")); printHex(strm, pt->getDeliveryStatus()); strm.print('\r');
        strm.print(F("Discovery Status: ")); printHex(strm, pt->getDiscoveryStatus()); strm.print('\r');
        strm.print('\r');
    } break;
    case ROUTE_INFORMATION: {
        RouteInformation* pt = static_cast<RouteInformation*>(this);
        strm.print(F("Source Event: ")); printHex(strm, pt->getSourceEvent()); strm.print('\r');
        strm.print(F("Length: ")); printHex(strm, pt->getLength()); strm.print('\r');
        strm.print(F("Timestamp: "));
        uint32_t timestamp = pt->getTimestamp(); uint8_t* timestampPtr = (uint8_t*)&timestamp;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, timestampPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("ACK Timout Count: ")); printHex(strm, pt->getAckTimoutCount()); strm.print('\r');
        strm.print(F("Destination Address64Msb: "));
        uint32_t destinationAddress64Msb = pt->getDestinationAddress64Msb();
        uint8_t* destinationAddress64MsbPtr = (uint8_t*)&destinationAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, destinationAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Destination Address64Lsb: "));
        uint32_t destinationAddress64Lsb = pt->getDestinationAddress64Lsb();
        uint8_t* destinationAddress64LsbPtr = (uint8_t*)&destinationAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, destinationAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Source Address64Msb: "));
        uint32_t sourceAddress64Msb = pt->getSourceAddress64Msb();
        uint8_t* sourceAddress64MsbPtr = (uint8_t*)&sourceAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, sourceAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Source Address64Lsb: "));
        uint32_t sourceAddress64Lsb = pt->getSourceAddress64Lsb();
        uint8_t* sourceAddress64LsbPtr = (uint8_t*)&sourceAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, sourceAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Responder Address64Msb: "));
        uint32_t responderAddress64Msb = pt->getResponderAddress64Msb();
        uint8_t* responderAddress64MsbPtr = (uint8_t*)&responderAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, responderAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Responder Address64Lsb: "));
        uint32_t responderAddress64Lsb = pt->getResponderAddress64Lsb();
        uint8_t* responderAddress64LsbPtr = (uint8_t*)&responderAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, responderAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Receiver Address64Msb: "));
        uint32_t receiverAddress64Msb = pt->getReceiverAddress64Msb();
        uint8_t* receiverAddress64MsbPtr = (uint8_t*)&receiverAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, receiverAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Receiver Address64Lsb: "));
        uint32_t receiverAddress64Lsb = pt->getReceiverAddress64Lsb();
        uint8_t* receiverAddress64LsbPtr = (uint8_t*)&receiverAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, receiverAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case AGGREGATE_ADDRESSING: {
        AggregateAddressing* pt = static_cast<AggregateAddressing*>(this);
        strm.print(F("Format ID: ")); printHex(strm, pt->getFormatId()); strm.print('\r');
        strm.print(F("New Address64Msb: "));
        uint32_t newAddress64Msb = pt->getNewAddress64Msb();
        uint8_t* newAddress64MsbPtr = (uint8_t*)&newAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, newAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("New Address64Lsb: "));
        uint32_t newAddress64Lsb = pt->getNewAddress64Lsb();
        uint8_t* newAddress64LsbPtr = (uint8_t*)&newAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, newAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Old Address64Msb: "));
        uint32_t oldAddress64Msb = pt->getOldAddress64Msb();
        uint8_t* oldAddress64MsbPtr = (uint8_t*)&oldAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, oldAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Old Address64Lsb: "));
        uint32_t oldAddress64Lsb = pt->getOldAddress64Lsb();
        uint8_t* oldAddress64LsbPtr = (uint8_t*)&oldAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, oldAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case RX_RESPONSE: {
        RxResponse* pt = static_cast<RxResponse*>(this);
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case EXPLICIT_RX_RESPONSE: {
        ExplicitRxResponse* pt = static_cast<ExplicitRxResponse*>(this);
        strm.print(F("Source Endpoint: ")); printHex(strm, pt->getSourceEndpoint()); strm.print('\r');
        strm.print(F("Destination Endpoint: ")); printHex(strm, pt->getDestinationEndpoint()); strm.print('\r');
        strm.print(F("Cluster ID: "));
        uint16_t clusterId = pt->getClusterId(); uint8_t* clusterIdPtr = (uint8_t*)&clusterId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, clusterIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Profile ID: "));
        uint16_t profileId = pt->getProfileId(); uint8_t* profileIdPtr = (uint8_t*)&profileId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, profileIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case RX_DATA_SAMPLE: {
        RxDataSample* pt = static_cast<RxDataSample*>(this);
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');
        strm.print(F("Number of Samples: ")); printHex(strm, pt->getNSamples()); strm.print('\r');
        uint16_t digitalMask = pt->getDigitalMask(); uint8_t* digitalMaskPtr = (uint8_t*)&digitalMask;
        strm.print(F("Digital Mask: "));
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, digitalMaskPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Analog Mask: ")); printHex(strm, pt->getAnalogMask()); strm.print('\r');
        strm.print(F("Digital Samples: "));
        uint16_t digitalSamples = pt->getDigitalSamples(); uint8_t* digitalSamplesPtr = (uint8_t*)&digitalSamples;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, digitalSamplesPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }

        uint16_t* analogSamples = pt->getAnalogSamples();
        uint16_t analogSamplesLength = pt->getAnalogSamplesLength();
        if (analogSamplesLength > 0) strm.print(F("Analog Samples: "));
        for (int i = 0; i < analogSamplesLength; i++) {
            uint8_t* analogSample = (uint8_t*)&(analogSamples[i]);
            for (uint8_t j = 0; j < 2; j++) {
                printHex(strm, analogSample[j]);
                if (j != 1) strm.print(F(" "));
            }
            if (i != (analogSamplesLength-1)) strm.print(F("  "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    case RX_NODE_ID: {
        RxNodeId* pt = static_cast<RxNodeId*>(this);
        strm.print(F("Options: ")); printHex(strm, pt->getOptions()); strm.print('\r');
        strm.print(F("Source Address16: "));
        uint16_t sourceAddress16 = pt->getSourceAddress16(); uint8_t* sourceAddress16Ptr = (uint8_t*)&sourceAddress16;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, sourceAddress16Ptr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Network Address64Msb: "));
        uint32_t networkAddress64Msb = pt->getNetworkAddress64Msb(); uint8_t* networkAddress64MsbPtr = (uint8_t*)&networkAddress64Msb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, networkAddress64MsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Network Address64Lsb: "));
        uint32_t networkAddress64Lsb = pt->getNetworkAddress64Lsb(); uint8_t* networkAddress64LsbPtr = (uint8_t*)&networkAddress64Lsb;
        for (uint8_t i = 0; i < 4; i++) {
            printHex(strm, networkAddress64LsbPtr[i]);
            if (i != 3) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("NI: ")); strm.print((char*)(pt->getNi())); strm.print('\r');
        strm.print(F("Parent Address16: "));
        uint16_t parentAddress16 = pt->getParentAddress16(); uint8_t* parentAddress16Ptr = (uint8_t*)&parentAddress16;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, parentAddress16Ptr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Device Type: ")); printHex(strm, pt->getType()); strm.print('\r');
        strm.print(F("Source Event: ")); printHex(strm, pt->getSourceEvent()); strm.print('\r');
        strm.print(F("Digi Profile ID: "));
        uint16_t digiProfileId = pt->getDigiProfileId(); uint8_t* digiProfileIdPtr = (uint8_t*)&digiProfileId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, digiProfileIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print(F("Digi Manufacturer ID: "));
        uint16_t digiManufacturerId = pt->getDigiManufacturerId(); uint8_t* digiManufacturerIdPtr = (uint8_t*)&digiManufacturerId;
        for (uint8_t i = 0; i < 2; i++) {
            printHex(strm, digiManufacturerIdPtr[i]);
            if (i != 1) strm.print(F(" "));
            else strm.print('\r');
        }

        uint32_t dd = pt->getDigiDd();
        if (dd > 0) {
            uint8_t* ddPtr = (uint8_t*)&dd;
            strm.print(F("Digi DD Value: "));
            for (uint8_t i = 0; i < 4; i++) {
                printHex(strm, ddPtr[i]);
                if (i != 3) strm.print(F(" "));
                else strm.print('\r');
            }
        }
        uint8_t rssi = pt->getRssi();
        if (rssi > 0) {
            strm.print(F("Rssi: ")); printHex(strm, rssi); strm.print('\r');
        }
        strm.print('\r');
    } break;
    case REMOTE_AT_COMMAND_RESPONSE: {
        RemoteAtCommandResponse* pt = static_cast<RemoteAtCommandResponse*>(this);
        strm.print(F("Command: "));
        uint16_t cmd = pt->getCmd(); uint8_t* cmdPtr = (uint8_t*)&cmd;
        for (uint8_t i = 0; i < 2; i++) {
            strm.print((char)cmdPtr[i]);
        } strm.print('\r');
        strm.print(F("Status: ")); printHex(strm, pt->getStatus()); strm.print('\r');

        uint8_t* data = pt->getData();
        uint16_t dataLength = pt->getDataLength();
        if (dataLength > 0) strm.print(F("Data: "));
        for (int i = 0; i < dataLength; i++) {
            printHex(strm, data[i]);
            if (i != (dataLength-1)) strm.print(F(" "));
            else strm.print('\r');
        }
        strm.print('\r');
    } break;
    }
}

void
XBeeApiFrame::printHex(Stream& strm, const uint8_t& hex) {
    if (hex < 0x10) strm.print(F("0"));
    strm.print(hex, HEX);
}
#endif


uint32_t
TxRxXBeeApiFrame::getAddress64Msb() { return *((uint32_t*)&(_cmdData[4-CMD_DATA_OFFSET])); }
uint32_t
TxRxXBeeApiFrame::getAddress64Lsb() { return *((uint32_t*)&(_cmdData[8-CMD_DATA_OFFSET])); }
uint16_t
TxRxXBeeApiFrame::getAddress16() { return *((uint16_t*)&(_cmdData[12-CMD_DATA_OFFSET])); }


uint8_t
FrameIdDescription::getFrameId() { return _cmdData[0]; }
void
FrameIdDescription::setFrameId(const uint8_t& frameId) { _cmdData[0] = frameId; }


uint32_t
TxRxFrameIdDescription::getAddress64Msb() { return *((uint32_t*)&(_cmdData[5-CMD_DATA_OFFSET])); }
uint32_t
TxRxFrameIdDescription::getAddress64Lsb() { return *((uint32_t*)&(_cmdData[9-CMD_DATA_OFFSET])); }
void
TxRxFrameIdDescription::setAddress64(const uint32_t& address64Msb, const uint32_t& address64Lsb,
    bool performChecksum) {
    uint8_t* address64 = &(_cmdData[5-CMD_DATA_OFFSET]);
    address64[0] = (address64Msb >> 24) & 0xff;
    address64[1] = (address64Msb >> 16) & 0xff;
    address64[2] = (address64Msb >> 8) & 0xff;
    address64[3] = address64Msb & 0xff;
    address64[4] = (address64Lsb >> 24) & 0xff;
    address64[5] = (address64Lsb >> 16) & 0xff;
    address64[6] = (address64Lsb >> 8) & 0xff;
    address64[7] = address64Lsb & 0xff;
    if (performChecksum) setChecksum();
}
void
TxRxFrameIdDescription::setAddress64(const uint32_t& address64Msb, const uint32_t& address64Lsb) {
    setAddress64(address64Msb, address64Lsb, true);
}

uint16_t
TxRxFrameIdDescription::getAddress16() { return *((uint16_t*)&(_cmdData[13-CMD_DATA_OFFSET])); }
void
TxRxFrameIdDescription::setAddress16(const uint16_t& address16, bool performChecksum) {
    uint8_t* address16ptr = &(_cmdData[13-CMD_DATA_OFFSET]);
    address16ptr[0] = (address16 >> 8) & 0xff;
    address16ptr[1] = address16 & 0xff;
    if (performChecksum) setChecksum();
}
void
TxRxFrameIdDescription::setAddress16(const uint16_t& address16) {
    setAddress16(address16, true);
}


Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const uint8_t* data, const uint16_t& dataLength) {
    _cmdId = TX_64_REQUEST;
    _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setOptions(options, false);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[TX_64_REQUEST_HEAD+i] = data[i];

    setChecksum();
}
Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t* data, const uint16_t& dataLength):
    Tx64Request(frameId, address64Msb, address64Lsb, 0, data, dataLength) {}

Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const char* data) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    _cmdId = TX_64_REQUEST;
    _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setOptions(options, false);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[TX_64_REQUEST_HEAD+i] = data[i];

    setChecksum();
}
Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const char* data):
    Tx64Request(frameId, address64Msb, address64Lsb, 0, data) {}

#ifdef XBEENG_WITH_EXTRAS
Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const std::initializer_list<uint8_t>& data) {
    uint16_t dataLength = data.size();

    _cmdId = TX_64_REQUEST;
    _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setOptions(options, false);

    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[TX_64_REQUEST_HEAD+(i++)] = data_i;

    setChecksum();
}
Tx64Request::Tx64Request(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const std::initializer_list<uint8_t>& data):
    Tx64Request(frameId, address64Msb, address64Lsb, 0, data) {}
#endif

uint8_t
Tx64Request::getOptions() { return _cmdData[13-CMD_DATA_OFFSET]; }
void
Tx64Request::setOptions(const uint8_t& options, const bool& performChecksum) {
    _cmdData[13-CMD_DATA_OFFSET] = options;
    if (performChecksum) setChecksum();
}
void
Tx64Request::setOptions(const uint8_t& options) {
    setOptions(options, true);
}

uint8_t*
Tx64Request::getData() { return &(_cmdData[14-CMD_DATA_OFFSET]); }
void
Tx64Request::setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum) {
    if ((getCmdDataLength() - TX_64_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_64_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[TX_64_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
Tx64Request::setData(const uint8_t* data, const uint16_t& dataLength) {
    setData(data, dataLength, true);
}
void
Tx64Request::setData(const char* data, const bool& performChecksum) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    if ((getCmdDataLength() - TX_64_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_64_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[TX_64_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
Tx64Request::setData(const char* data) {
    setData(data, true);
}
#ifdef XBEENG_WITH_EXTRAS
void
Tx64Request::setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum) {
    uint16_t dataLength = data.size();

    if ((getCmdDataLength() - TX_64_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_64_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_64_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_64_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_64_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }


    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[TX_64_REQUEST_HEAD+(i++)] = data_i;

    if (performChecksum) setChecksum();
}
void
Tx64Request::setData(const std::initializer_list<uint8_t>& data) {
    setData(data, true);
}
#endif
uint16_t
Tx64Request::getDataLength() { return getCmdDataLength() - TX_64_REQUEST_HEAD; }


Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength) {
    _cmdId = TX_16_REQUEST;
    _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress16(address16, false);
    setOptions(options, false);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[TX_16_REQUEST_HEAD+i] = data[i];

    setChecksum();
}
Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const uint8_t* data, const uint16_t& dataLength):
    Tx16Request(frameId, address16, 0, data, dataLength) {}

Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const uint8_t& options, const char* data) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    _cmdId = TX_16_REQUEST;
    _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress16(address16, false);
    setOptions(options, false);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[TX_16_REQUEST_HEAD+i] = data[i];

    setChecksum();
}
Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const char* data):
    Tx16Request(frameId, address16, 0, data) {}

#ifdef XBEENG_WITH_EXTRAS
Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const uint8_t& options,
    const std::initializer_list<uint8_t>& data) {
    uint16_t dataLength = data.size();

    _cmdId = TX_16_REQUEST;
    _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress16(address16, false);
    setOptions(options, false);

    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[TX_16_REQUEST_HEAD+(i++)] = data_i;

    setChecksum();
}
Tx16Request::Tx16Request(const uint8_t& frameId,
    const uint16_t& address16, const std::initializer_list<uint8_t>& data):
    Tx16Request(frameId, address16, 0, data) {}
#endif

uint16_t
Tx16Request::getAddress16() { return *((uint16_t*)&(_cmdData[8-CMD_DATA_OFFSET])); }
void
Tx16Request::setAddress16(const uint16_t& address16, const bool& performChecksum) {
    uint8_t* address16ptr = &(_cmdData[5-CMD_DATA_OFFSET]);
    address16ptr[0] = (address16 >> 8) & 0xff;
    address16ptr[1] = address16 & 0xff;
    if (performChecksum) setChecksum();
}
void
Tx16Request::setAddress16(const uint16_t& address16) {
    setAddress16(address16, true);
}

uint8_t
Tx16Request::getOptions() { return _cmdData[7-CMD_DATA_OFFSET]; }
void
Tx16Request::setOptions(const uint8_t& options, const bool& performChecksum) {
    _cmdData[7-CMD_DATA_OFFSET] = options;
    if (performChecksum) setChecksum();
}
void
Tx16Request::setOptions(const uint8_t& options) {
    setOptions(options, true);
}

uint8_t*
Tx16Request::getData() { return &(_cmdData[8-CMD_DATA_OFFSET]); }
void
Tx16Request::setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum) {
    if ((getCmdDataLength() - TX_16_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_16_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[TX_16_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
Tx16Request::setData(const uint8_t* data, const uint16_t& dataLength) {
    setData(data, dataLength, true);
}
void
Tx16Request::setData(const char* data, const bool& performChecksum) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    if ((getCmdDataLength() - TX_16_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_16_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[TX_16_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
Tx16Request::setData(const char* data) {
    setData(data, true);
}
#ifdef XBEENG_WITH_EXTRAS
void
Tx16Request::setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum) {
    uint16_t dataLength = data.size();

    if ((getCmdDataLength() - TX_16_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_16_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_16_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_16_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_16_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }


    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[TX_16_REQUEST_HEAD+(i++)] = data_i;

    if (performChecksum) setChecksum();
}
void
Tx16Request::setData(const std::initializer_list<uint8_t>& data) {
    setData(data, true);
}
#endif
uint16_t
Tx16Request::getDataLength() { return getCmdDataLength() - TX_16_REQUEST_HEAD; }


AtCommand::AtCommand(const uint8_t& frameId, const uint8_t* data, const uint16_t& dataLength) {
    //assert (dataLength >= 2);

    _cmdId = AT_COMMAND;
    _cmdData = new uint8_t[FRAME_ID_HEAD+dataLength];

    setCmdDataLength(FRAME_ID_HEAD+dataLength);
    setFrameId(frameId);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[FRAME_ID_HEAD+i] = data[i];

    setChecksum();
}
AtCommand::AtCommand(const uint8_t& frameId, const char* data) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    //assert (dataLength >= 2);

    _cmdId = AT_COMMAND;
    _cmdData = new uint8_t[FRAME_ID_HEAD+dataLength];

    setCmdDataLength(FRAME_ID_HEAD+dataLength);
    setFrameId(frameId);

    for (uint16_t i = 0; i < dataLength; i++) _cmdData[FRAME_ID_HEAD+i] = data[i];

    setChecksum();
}

AtCommand::AtCommand(const uint8_t& frameId, const uint16_t& cmd):
    AtCommand(frameId, (uint8_t*)(&cmd), 2) {}
AtCommand::AtCommand(const uint8_t& frameId, const uint16_t& cmd,
    const uint8_t* param, const uint16_t& paramLength) {
    _cmdId = AT_COMMAND;
    _cmdData = new uint8_t[AT_COMMAND_HEAD+paramLength];

    setCmdDataLength(AT_COMMAND_HEAD+paramLength);
    setFrameId(frameId);
    setCmd(cmd, false);
    setParam(param, paramLength, false);
    setChecksum();
}

#ifdef XBEENG_WITH_EXTRAS
AtCommand::AtCommand(const uint8_t& frameId, const std::initializer_list<uint8_t>& data) {
    uint16_t dataLength = data.size();

    _cmdId = AT_COMMAND;
    _cmdData = new uint8_t[FRAME_ID_HEAD+dataLength];

    setCmdDataLength(FRAME_ID_HEAD+dataLength);
    setFrameId(frameId);

    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[FRAME_ID_HEAD+(i++)] = data_i;

    setChecksum();
}
AtCommand::AtCommand(const uint8_t& frameId, const char (&cmd)[3], const std::initializer_list<uint8_t>& param) {
    uint16_t paramLength = param.size();

    _cmdId = AT_COMMAND;
    _cmdData = new uint8_t[AT_COMMAND_HEAD+paramLength];

    setCmdDataLength(AT_COMMAND_HEAD+paramLength);
    setFrameId(frameId);
    setCmd(cmd, false);

    uint16_t i = 0;
    for (uint8_t param_i : param) _cmdData[AT_COMMAND_HEAD+(i++)] = param_i;

    setChecksum();
}
#endif

uint16_t
AtCommand::getCmd() { return *((uint16_t*)&(_cmdData[1])); }
void
AtCommand::setCmd(const uint16_t& cmd, const bool& performChecksum) {
    _cmdData[1] = ((uint8_t*)&cmd)[0];
    _cmdData[2] = ((uint8_t*)&cmd)[1];
    if (performChecksum) setChecksum();
}
void
AtCommand::setCmd(const uint16_t& cmd) {
    setCmd(cmd, true);
}
void
AtCommand::setCmd(const char (&cmd)[3], const bool& performChecksum) {
    _cmdData[1] = cmd[0];
    _cmdData[2] = cmd[1];
    if (performChecksum) setChecksum();
}
void
AtCommand::setCmd(const char (&cmd)[3]) {
    setCmd(cmd, true);
}
#ifdef XBEENG_WITH_EXTRAS
void
AtCommand::setCmd(const char (&cmd)[3], const std::initializer_list<uint8_t>& param,
    const bool& performChecksum) {
    setCmd(cmd, false);
    setParam(param, false);
    if (performChecksum) setChecksum();
}
void
AtCommand::setCmd(const char (&cmd)[3], const std::initializer_list<uint8_t>& param) {
    setCmd(cmd, param, true);
}
#endif

uint8_t*
AtCommand::getParam() { return &(_cmdData[3]); }
void
AtCommand::setParam(const uint8_t* param, const uint16_t& paramLength,
    const bool& performChecksum) {
    if ((getCmdDataLength() - AT_COMMAND_HEAD) != paramLength) {
        uint8_t head[AT_COMMAND_HEAD];
        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[AT_COMMAND_HEAD+paramLength];
        setCmdDataLength(AT_COMMAND_HEAD+paramLength);

        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < paramLength; i++) _cmdData[AT_COMMAND_HEAD+i] = param[i];

    if (performChecksum) setChecksum();
}
void
AtCommand::setParam(const uint8_t* param, const uint16_t& paramLength) {
    setParam(param, paramLength, true);
}
void
AtCommand::setParam(const char* param, const bool& performChecksum) {
    uint16_t paramLength = 0;
    for (; param[paramLength]!='\0'; paramLength++);

    if ((getCmdDataLength() - AT_COMMAND_HEAD) != paramLength) {
        uint8_t head[AT_COMMAND_HEAD];
        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[AT_COMMAND_HEAD+paramLength];
        setCmdDataLength(AT_COMMAND_HEAD+paramLength);

        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < paramLength; i++) {
        if ('9' >= param[i] && param[i] >= '0')
            _cmdData[AT_COMMAND_HEAD+i] = param[i] - '0';
        else
            _cmdData[AT_COMMAND_HEAD+i] = param[i];
    }
    setChecksum();
}
void
AtCommand::setParam(const char* param) {
    setParam(param, true);
}
#ifdef XBEENG_WITH_EXTRAS
void AtCommand::setParam(const std::initializer_list<uint8_t>& param,
    const bool& performChecksum) {
    uint16_t paramLength = param.size();

    if ((getCmdDataLength() - AT_COMMAND_HEAD) != paramLength) {
        uint8_t head[AT_COMMAND_HEAD];
        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[AT_COMMAND_HEAD+paramLength];
        setCmdDataLength(AT_COMMAND_HEAD+paramLength);

        for (uint16_t i = 0; i < AT_COMMAND_HEAD; i++) _cmdData[i] = head[i];
    }


    uint16_t i = 0;
    for (uint8_t param_i : param) _cmdData[AT_COMMAND_HEAD+(i++)] = param_i;

    if (performChecksum) setChecksum();
}
void AtCommand::setParam(const std::initializer_list<uint8_t>& param) {
    setParam(param, true);
}
#endif
uint16_t
AtCommand::getParamLength() { return getCmdDataLength() - AT_COMMAND_HEAD; }


AtQueueCommand::AtQueueCommand(const uint8_t& frameId,
    const uint8_t* data, const uint16_t& dataLength):
    AtCommand(frameId, data, dataLength) {
    _cmdId = AT_QUEUE_COMMAND;
    setChecksum();
}
AtQueueCommand::AtQueueCommand(const uint8_t& frameId, const char* data):
    AtCommand(frameId, data) {
    _cmdId = AT_QUEUE_COMMAND;
    setChecksum();
}
AtQueueCommand::AtQueueCommand(const uint8_t& frameId,
    const uint16_t& cmd):
    AtQueueCommand(frameId, (uint8_t*)(&cmd), 2) {}
AtQueueCommand::AtQueueCommand(const uint8_t& frameId, const uint16_t& cmd,
    const uint8_t* param, const uint16_t& paramLength):
    AtCommand(frameId, cmd, param, paramLength) {
    _cmdId = AT_QUEUE_COMMAND;
    setChecksum();
}


TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength) {
    _cmdId = TX_REQUEST;
    _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, dataLength, false);
    setChecksum();
}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& broadcast, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength):
    TxRequest(frameId, address64Msb, address64Lsb, BROADCAST_ADDRESS16,
    broadcast, options, data, dataLength) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t* data, const uint16_t& dataLength):
    TxRequest(frameId, address64Msb, address64Lsb, 0, 0, data, dataLength) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint8_t* data, const uint16_t& dataLength):
    TxRequest(frameId, 0, BROADCAST_ADDRESS64, data, dataLength) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
    const char* data) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    _cmdId = TX_REQUEST;
    _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, false);
    setChecksum();
}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& broadcast, const uint8_t& options,
    const char* data):
    TxRequest(frameId, address64Msb, address64Lsb, BROADCAST_ADDRESS16,
    broadcast, options, data) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const char* data):
    TxRequest(frameId, address64Msb, address64Lsb, 0, 0, data) {}
TxRequest::TxRequest(const uint8_t& frameId, const char* data):
    TxRequest(frameId, 0, BROADCAST_ADDRESS64, data) {}
#ifdef XBEENG_WITH_EXTRAS
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
    const std::initializer_list<uint8_t>& data) {
    uint16_t dataLength = data.size();

    _cmdId = TX_REQUEST;
    _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, false);
    setChecksum();
}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& broadcast, const uint8_t& options,
    const std::initializer_list<uint8_t>& data):
    TxRequest(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, broadcast, options, data) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const std::initializer_list<uint8_t>& data):
    TxRequest(frameId, address64Msb, address64Lsb, 0, 0, data) {}
TxRequest::TxRequest(const uint8_t& frameId,
    const std::initializer_list<uint8_t>& data):
    TxRequest(frameId, 0, BROADCAST_ADDRESS64, data) {}
#endif

uint8_t
TxRequest::getBroadcast() { return _cmdData[15-CMD_DATA_OFFSET]; }
void
TxRequest::setBroadcast(const uint8_t& broadcast, const bool& performChecksum) {
    uint8_t* broadcastptr = &(_cmdData[15-CMD_DATA_OFFSET]);
    broadcastptr[0] = broadcast;
    if (performChecksum) setChecksum();
}
void
TxRequest::setBroadcast(const uint8_t& broadcast) {
    setBroadcast(broadcast, true);
}

uint8_t
TxRequest::getOptions() { return _cmdData[16-CMD_DATA_OFFSET]; }
void
TxRequest::setOptions(const uint8_t& options, const bool& performChecksum) {
    _cmdData[16-CMD_DATA_OFFSET] = options;
    if (performChecksum) setChecksum();
}
void
TxRequest::setOptions(const uint8_t& options) {
    setOptions(options, true);
}

uint8_t*
TxRequest::getData() { return &(_cmdData[17-CMD_DATA_OFFSET]); }
void
TxRequest::setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum) {
    if ((getCmdDataLength() - TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[TX_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
TxRequest::setData(const uint8_t* data, const uint16_t& dataLength) {
    setData(data, dataLength, true);
}
#ifdef XBEENG_WITH_EXTRAS
void
TxRequest::setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum) {
    uint16_t dataLength = data.size();

    if ((getCmdDataLength() - TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }


    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[TX_REQUEST_HEAD+(i++)] = data_i;

    if (performChecksum) setChecksum();
}
void
TxRequest::setData(const std::initializer_list<uint8_t>& data) {
    setData(data, true);
}
#endif
uint16_t
TxRequest::getDataLength() { return getCmdDataLength() - TX_REQUEST_HEAD; }

void
TxRequest::setData(const char* data, const bool& performChecksum) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    if ((getCmdDataLength() - TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++) {
        if ('9' >= data[i] && data[i] >= '0')
            _cmdData[TX_REQUEST_HEAD+i] = data[i]-'0';
        else
            _cmdData[TX_REQUEST_HEAD+i] = data[i];
    }

    if (performChecksum) setChecksum();
}
void
TxRequest::setData(const char* data) {
    setData(data, true);
}


ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength) {
    _cmdId = EXPLICIT_TX_REQUEST;
    _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setSourceEndpoint(sourceEndpoint, false);
    setDestinationEndpoint(destinationEndpoint, false);
    setClusterId(clusterId, false);
    setProfileId(profileId, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, dataLength, false);
    setChecksum();
}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb, BROADCAST_ADDRESS16,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        broadcast, options, data, dataLength) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t* data, const uint16_t& dataLength):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        0, 0, data, dataLength) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t* data, const uint16_t& dataLength):
    ExplicitTxRequest(frameId, 0, BROADCAST_ADDRESS64,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        data, dataLength) {}

ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const char* data) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    _cmdId = EXPLICIT_TX_REQUEST;
    _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setSourceEndpoint(sourceEndpoint, false);
    setDestinationEndpoint(destinationEndpoint, false);
    setClusterId(clusterId, false);
    setProfileId(profileId, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, false);
    setChecksum();
}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const char* data):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb, BROADCAST_ADDRESS16,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        broadcast, options, data) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const char* data):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        0, 0, data) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const char* data):
    ExplicitTxRequest(frameId, 0, BROADCAST_ADDRESS64,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        data) {}

#ifdef XBEENG_WITH_EXTRAS
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const std::initializer_list<uint8_t>& data) {
    uint16_t dataLength = data.size();

    _cmdId = EXPLICIT_TX_REQUEST;
    _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];

    setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setSourceEndpoint(sourceEndpoint, false);
    setDestinationEndpoint(destinationEndpoint, false);
    setClusterId(clusterId, false);
    setProfileId(profileId, false);
    setBroadcast(broadcast, false);
    setOptions(options, false);
    setData(data, false);
    setChecksum();
}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const uint8_t& broadcast, const uint8_t& options,
    const std::initializer_list<uint8_t>& data):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb, BROADCAST_ADDRESS16,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        broadcast, options, data) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const std::initializer_list<uint8_t>& data):
    ExplicitTxRequest(frameId, address64Msb, address64Lsb,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        0, 0, data) {}
ExplicitTxRequest::ExplicitTxRequest(const uint8_t& frameId,
    const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
    const uint16_t& clusterId, const uint16_t& profileId,
    const std::initializer_list<uint8_t>& data):
    ExplicitTxRequest(frameId, 0, BROADCAST_ADDRESS64,
        sourceEndpoint, destinationEndpoint, clusterId, profileId,
        data) {}
#endif

uint8_t
ExplicitTxRequest::getSourceEndpoint() { return _cmdData[15-CMD_DATA_OFFSET]; }
void
ExplicitTxRequest::setSourceEndpoint(const uint8_t& sourceEndpoint, const bool& performChecksum) {
    uint8_t* sourceEndpointptr = &(_cmdData[15-CMD_DATA_OFFSET]);
    sourceEndpointptr[0] = sourceEndpoint;

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setSourceEndpoint(const uint8_t& sourceEndpoint) {
    setSourceEndpoint(sourceEndpoint, true);
}

uint8_t
ExplicitTxRequest::getDestinationEndpoint() { return _cmdData[16-CMD_DATA_OFFSET]; }
void
ExplicitTxRequest::setDestinationEndpoint(const uint8_t& destinationEndpoint, const bool& performChecksum) {
    uint8_t* destinationEndpointptr = &(_cmdData[16-CMD_DATA_OFFSET]);
    destinationEndpointptr[0] = destinationEndpoint;

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setDestinationEndpoint(const uint8_t& destinationEndpoint) {
    setDestinationEndpoint(destinationEndpoint, true);
}

uint16_t
ExplicitTxRequest::getClusterId() { return *((uint16_t*)&(_cmdData[17-CMD_DATA_OFFSET])); }
void
ExplicitTxRequest::setClusterId(const uint16_t& clusterId, const bool& performChecksum) {
    uint8_t* clusterIdptr = &(_cmdData[17-CMD_DATA_OFFSET]);
    clusterIdptr[0] = (clusterId >> 8) & 0xff;
    clusterIdptr[1] = clusterId & 0xff;
    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setClusterId(const uint16_t& clusterId) {
    setClusterId(clusterId, true);
}

uint16_t
ExplicitTxRequest::getProfileId() { return *((uint16_t*)&(_cmdData[19-CMD_DATA_OFFSET])); }
void
ExplicitTxRequest::setProfileId(const uint16_t& profileId, const bool& performChecksum) {
    uint8_t* profileIdptr = &(_cmdData[19-CMD_DATA_OFFSET]);
    profileIdptr[0] = (profileId >> 8) & 0xff;
    profileIdptr[1] = profileId & 0xff;
    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setProfileId(const uint16_t& profileId) {
    setProfileId(profileId, true);
}

uint8_t
ExplicitTxRequest::getBroadcast() { return _cmdData[21-CMD_DATA_OFFSET]; }
void
ExplicitTxRequest::setBroadcast(const uint8_t& broadcast, const bool& performChecksum) {
    uint8_t* broadcastptr = &(_cmdData[21-CMD_DATA_OFFSET]);
    broadcastptr[0] = broadcast;

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setBroadcast(const uint8_t& broadcast) {
    setBroadcast(broadcast, true);
}

uint8_t
ExplicitTxRequest::getOptions() { return _cmdData[22-CMD_DATA_OFFSET]; }
void
ExplicitTxRequest::setOptions(const uint8_t& options, const bool& performChecksum) {
    _cmdData[22-CMD_DATA_OFFSET] = options;
    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setOptions(const uint8_t& options) {
    setOptions(options, true);
}

uint8_t*
ExplicitTxRequest::getData() { return &_cmdData[23-CMD_DATA_OFFSET]; }
uint16_t
ExplicitTxRequest::getDataLength() { return getCmdDataLength() - EXPLICIT_TX_REQUEST_HEAD; }
void
ExplicitTxRequest::setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum) {
    if ((getCmdDataLength() - EXPLICIT_TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[EXPLICIT_TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++)
        _cmdData[EXPLICIT_TX_REQUEST_HEAD+i] = data[i];

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setData(const uint8_t* data, const uint16_t& dataLength) {
    setData(data, dataLength, true);
}

void
ExplicitTxRequest::setData(const char* data, const bool& performChecksum) {
    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    if ((getCmdDataLength() - EXPLICIT_TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[EXPLICIT_TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < dataLength; i++) {
        if ('9' >= data[i] && data[i] >= '0')
            _cmdData[EXPLICIT_TX_REQUEST_HEAD+i] = data[i]-'0';
        else
            _cmdData[EXPLICIT_TX_REQUEST_HEAD+i] = data[i];
    }

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setData(const char* data) {
    setData(data, true);
}

#ifdef XBEENG_WITH_EXTRAS
void
ExplicitTxRequest::setData(const std::initializer_list<uint8_t>& data,
    const bool& performChecksum) {
    uint16_t dataLength = data.size();

    if ((getCmdDataLength() - EXPLICIT_TX_REQUEST_HEAD) != dataLength) {
        uint8_t head[EXPLICIT_TX_REQUEST_HEAD];
        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[EXPLICIT_TX_REQUEST_HEAD+dataLength];
        setCmdDataLength(EXPLICIT_TX_REQUEST_HEAD+dataLength);

        for (uint16_t i = 0; i < EXPLICIT_TX_REQUEST_HEAD; i++) _cmdData[i] = head[i];
    }


    uint16_t i = 0;
    for (uint8_t data_i : data) _cmdData[EXPLICIT_TX_REQUEST_HEAD+(i++)] = data_i;

    if (performChecksum) setChecksum();
}
void
ExplicitTxRequest::setData(const std::initializer_list<uint8_t>& data) {
    setData(data, true);
}
#endif


RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& options,
    const char* data) {

    uint16_t dataLength = 0;
    for (; data[dataLength]!='\0'; dataLength++);

    // assert(dataLength >= 2);

    _cmdId = REMOTE_AT_COMMAND;
    _cmdData = new uint8_t[(REMOTE_AT_COMMAND_HEAD-2)+dataLength];

    setCmdDataLength((REMOTE_AT_COMMAND_HEAD-2)+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setOptions(options, false);
    setCmd(data, false);

    setChecksum();
}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const char* data):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, options, data) {}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& options,
    const uint8_t* data, const uint16_t& dataLength) {

    // assert(dataLength >= 2);

    _cmdId = REMOTE_AT_COMMAND;
    _cmdData = new uint8_t[(REMOTE_AT_COMMAND_HEAD-2)+dataLength];

    setCmdDataLength((REMOTE_AT_COMMAND_HEAD-2)+dataLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setOptions(options, false);
    setCmd(((uint16_t*)data)[0], false);
    setParam(&(data[2]), dataLength-2, false);

    setChecksum();
}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const uint8_t* data, const uint16_t& dataLength):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, options, data, dataLength) {}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& options,
    const uint16_t& cmd):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        address16, options, (uint8_t*)&cmd, 2) {}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options, const uint16_t& cmd):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, options, cmd) {}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& options,
    const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength) {

    _cmdId = REMOTE_AT_COMMAND;
    _cmdData = new uint8_t[REMOTE_AT_COMMAND_HEAD+paramLength];

    setCmdDataLength(REMOTE_AT_COMMAND_HEAD+paramLength);
    setFrameId(frameId);
    setAddress64(address64Msb, address64Lsb, false);
    setAddress16(address16, false);
    setOptions(options, false);
    setCmd(cmd);
    setParam(param, paramLength, false);
    setChecksum();
}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options,
    const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, options, cmd, param, paramLength) {}
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint16_t& address16, const uint8_t& options,
    const char (&cmd)[3], const uint8_t* param, const uint16_t& paramLength):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        address16, options, ((uint16_t*)cmd)[0], param, paramLength) {} // warning: insecure cast
RemoteAtCommand::RemoteAtCommand(const uint8_t& frameId,
    const uint32_t& address64Msb, const uint32_t& address64Lsb,
    const uint8_t& options,
    const char (&cmd)[3], const uint8_t* param, const uint16_t& paramLength):
    RemoteAtCommand(frameId, address64Msb, address64Lsb,
        BROADCAST_ADDRESS16, options, cmd, param, paramLength) {}

uint8_t
RemoteAtCommand::getOptions() { return _cmdData[15-CMD_DATA_OFFSET]; }
void
RemoteAtCommand::setOptions(const uint8_t& options, const bool& performChecksum) {
    _cmdData[15-CMD_DATA_OFFSET] = options;
    if (performChecksum) setChecksum();
}
void
RemoteAtCommand::setOptions(const uint8_t& options) {
    setOptions(options, true);
}

uint16_t
RemoteAtCommand::getCmd() { return *((uint16_t*)&(_cmdData[16-CMD_DATA_OFFSET])); }
void
RemoteAtCommand::setCmd(const uint16_t& cmd, const bool& performChecksum) {
    uint16_t* cmdptr = (uint16_t*)&(_cmdData[16-CMD_DATA_OFFSET]);
    cmdptr[0] = cmd;
    if (performChecksum) setChecksum();
}
void
RemoteAtCommand::setCmd(const uint16_t& cmd) {
    setCmd(cmd, true);
}
void
RemoteAtCommand::setCmd(const char* cmd, const bool& performChecksum) {
    uint16_t cmdLength = 0;
    for (; cmd[cmdLength]!='\0'; cmdLength++);

    if (cmdLength!=2 && (getCmdDataLength() - (REMOTE_AT_COMMAND_HEAD-2)) != cmdLength) {
        uint8_t head[REMOTE_AT_COMMAND_HEAD-2];
        for (uint16_t i = 0; i < REMOTE_AT_COMMAND_HEAD-2; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[(REMOTE_AT_COMMAND_HEAD-2)+cmdLength];
        setCmdDataLength((REMOTE_AT_COMMAND_HEAD-2)+cmdLength);

        for (uint16_t i = 0; i < REMOTE_AT_COMMAND_HEAD-2; i++) _cmdData[i] = head[i];
    }

    for (int i = 0; i < cmdLength; i++) {
        if ('9' >= cmd[i] && cmd[i] >= '0')
            _cmdData[(REMOTE_AT_COMMAND_HEAD-2)+i] = cmd[i]-'0';
        else
            _cmdData[(REMOTE_AT_COMMAND_HEAD-2)+i] = cmd[i];
    }

    if (performChecksum) setChecksum();
}
void
RemoteAtCommand::setCmd(const char* cmd) {
    setCmd(cmd, true);
}

uint8_t*
RemoteAtCommand::getParam() { return &(_cmdData[18-CMD_DATA_OFFSET]); }
void
RemoteAtCommand::setParam(const uint8_t* param, const uint16_t& paramLength,
    const bool& performChecksum) {
    if ((getCmdDataLength() - REMOTE_AT_COMMAND_HEAD) != paramLength) {
        uint8_t head[REMOTE_AT_COMMAND_HEAD];
        for (uint16_t i = 0; i < REMOTE_AT_COMMAND_HEAD; i++) head[i] = _cmdData[i];

        delete[] _cmdData;
        _cmdData = new uint8_t[REMOTE_AT_COMMAND_HEAD+paramLength];
        setCmdDataLength(REMOTE_AT_COMMAND_HEAD+paramLength);

        for (uint16_t i = 0; i < REMOTE_AT_COMMAND_HEAD; i++) _cmdData[i] = head[i];
    }

    for (uint16_t i = 0; i < paramLength; i++)
        _cmdData[REMOTE_AT_COMMAND_HEAD+i] = param[i];

    if (performChecksum) setChecksum();
}
void
RemoteAtCommand::setParam(const uint8_t* param, const uint16_t& paramLength) {
    setParam(param, paramLength, true);
}
uint16_t
RemoteAtCommand::getParamLength() { return getCmdDataLength() - REMOTE_AT_COMMAND_HEAD; }


uint8_t
Rx64Response::getRssi() { return _cmdData[12-CMD_DATA_OFFSET]; }
uint8_t
Rx64Response::getOptions() { return _cmdData[13-CMD_DATA_OFFSET]; }

uint8_t*
Rx64Response::getData() { return &(_cmdData[14-CMD_DATA_OFFSET]); }
uint16_t
Rx64Response::getDataLength() { return getCmdDataLength() - RX_64_RESPONSE_HEAD; }


uint16_t
Rx16Response::getAddress16() { return *((uint16_t*)&(_cmdData[4-CMD_DATA_OFFSET])); }
uint8_t
Rx16Response::getRssi() { return _cmdData[6-CMD_DATA_OFFSET]; }
uint8_t
Rx16Response::getOptions() { return _cmdData[7-CMD_DATA_OFFSET]; }

uint8_t*
Rx16Response::getData() { return &(_cmdData[8-CMD_DATA_OFFSET]); }
uint16_t
Rx16Response::getDataLength() { return getCmdDataLength() - RX_16_RESPONSE_HEAD; }


uint16_t
AtCommandResponse::getCmd() { return *((uint16_t*)&(_cmdData[5-CMD_DATA_OFFSET])); }

uint8_t
AtCommandResponse::getStatus() { return _cmdData[7-CMD_DATA_OFFSET]; }

uint8_t*
AtCommandResponse::getData() { return &(_cmdData[8-CMD_DATA_OFFSET]); }
uint16_t
AtCommandResponse::getDataLength() { return getCmdDataLength() - AT_COMMAND_RESPONSE_HEAD; }


uint8_t
TxStatusResponse::getStatus() { return _cmdData[5-CMD_DATA_OFFSET]; }


uint8_t
ModemStatus::getStatus() { return _cmdData[4-CMD_DATA_OFFSET]; }


uint16_t
TxStatus::getAddress16() { return *((uint16_t*)&(_cmdData[5-CMD_DATA_OFFSET])); }

uint8_t
TxStatus::getRetryCount() { return _cmdData[7-CMD_DATA_OFFSET]; }

uint8_t
TxStatus::getDeliveryStatus() {
    if (_cmdId == TX_STATUS_RESPONSE) // 802.15.4 case!
        return _cmdData[5-CMD_DATA_OFFSET];
    return _cmdData[8-CMD_DATA_OFFSET];
}

uint8_t
TxStatus::getDiscoveryStatus() { return _cmdData[9-CMD_DATA_OFFSET]; }


uint8_t
RouteInformation::getSourceEvent() { return _cmdData[4-CMD_DATA_OFFSET]; }
uint8_t
RouteInformation::getLength() { return _cmdData[5-CMD_DATA_OFFSET]; }
uint32_t
RouteInformation::getTimestamp() { return *((uint32_t*)&(_cmdData[6-CMD_DATA_OFFSET])); }
uint8_t
RouteInformation::getAckTimoutCount() { return _cmdData[10-CMD_DATA_OFFSET]; }

uint32_t
RouteInformation::getDestinationAddress64Msb() { return *((uint32_t*)&(_cmdData[13-CMD_DATA_OFFSET])); }
uint32_t
RouteInformation::getDestinationAddress64Lsb() { return *((uint32_t*)&(_cmdData[17-CMD_DATA_OFFSET])); }

uint32_t
RouteInformation::getSourceAddress64Msb() { return *((uint32_t*)&(_cmdData[21-CMD_DATA_OFFSET])); }
uint32_t
RouteInformation::getSourceAddress64Lsb() { return *((uint32_t*)&(_cmdData[25-CMD_DATA_OFFSET])); }

uint32_t
RouteInformation::getResponderAddress64Msb() { return *((uint32_t*)&(_cmdData[29-CMD_DATA_OFFSET])); }
uint32_t
RouteInformation::getResponderAddress64Lsb() { return *((uint32_t*)&(_cmdData[33-CMD_DATA_OFFSET])); }

uint32_t
RouteInformation::getReceiverAddress64Msb() { return *((uint32_t*)&(_cmdData[37-CMD_DATA_OFFSET])); }
uint32_t
RouteInformation::getReceiverAddress64Lsb() { return *((uint32_t*)&(_cmdData[41-CMD_DATA_OFFSET])); }


uint8_t
AggregateAddressing::getFormatId() { return _cmdData[4-CMD_DATA_OFFSET]; }

uint32_t
AggregateAddressing::getNewAddress64Msb() { return *((uint32_t*)&(_cmdData[5-CMD_DATA_OFFSET])); }
uint32_t
AggregateAddressing::getNewAddress64Lsb() { return *((uint32_t*)&(_cmdData[9-CMD_DATA_OFFSET])); }

uint32_t
AggregateAddressing::getOldAddress64Msb() { return *((uint32_t*)&(_cmdData[13-CMD_DATA_OFFSET])); }
uint32_t
AggregateAddressing::getOldAddress64Lsb() { return *((uint32_t*)&(_cmdData[17-CMD_DATA_OFFSET])); }


uint8_t
RxResponse::getOptions() { return _cmdData[14-CMD_DATA_OFFSET]; }

uint8_t*
RxResponse::getData() { return &(_cmdData[15-CMD_DATA_OFFSET]); }
uint16_t
RxResponse::getDataLength() { return getCmdDataLength() - RX_RESPONSE_HEAD; }


uint8_t
ExplicitRxResponse::getSourceEndpoint() { return _cmdData[14-CMD_DATA_OFFSET]; }
uint8_t
ExplicitRxResponse::getDestinationEndpoint() { return _cmdData[15-CMD_DATA_OFFSET]; }
uint16_t
ExplicitRxResponse::getClusterId() { return *((uint16_t*)&(_cmdData[16-CMD_DATA_OFFSET])); }
uint16_t
ExplicitRxResponse::getProfileId() { return *((uint16_t*)&(_cmdData[18-CMD_DATA_OFFSET])); }

uint8_t
ExplicitRxResponse::getOptions() { return _cmdData[20-CMD_DATA_OFFSET]; }

uint8_t*
ExplicitRxResponse::getData() { return &(_cmdData[21-CMD_DATA_OFFSET]); }
uint16_t
ExplicitRxResponse::getDataLength() { return getCmdDataLength() - EXPLICIT_RX_RESPONSE_HEAD; }


uint8_t
RxDataSample::getNSamples() { return _cmdData[15-CMD_DATA_OFFSET]; }
uint16_t
RxDataSample::getDigitalMask() { return *((uint16_t*)&(_cmdData[16-CMD_DATA_OFFSET])); }
uint8_t
RxDataSample::getAnalogMask() { return _cmdData[18-CMD_DATA_OFFSET]; }
uint16_t
RxDataSample::getDigitalSamples() {
    if (getDigitalMask() > 0) return *((uint16_t*)&(_cmdData[19-CMD_DATA_OFFSET]));
    return 0;
}
uint16_t*
RxDataSample::getAnalogSamples() {
    if (getDigitalMask() > 0) return ((uint16_t*)&(_cmdData[21-CMD_DATA_OFFSET]));
    return ((uint16_t*)&(_cmdData[19-CMD_DATA_OFFSET]));
}
uint16_t
RxDataSample::getAnalogSamplesLength() {
    if (getDigitalMask() > 0) return (getCmdDataLength() - RX_DATA_SAMPLE_HEAD)/2;
    return (2 + getCmdDataLength() - RX_DATA_SAMPLE_HEAD)/2;
}


uint16_t
RxNodeId::getSourceAddress16() { return *((uint16_t*)&(_cmdData[15-CMD_DATA_OFFSET])); }
uint32_t
RxNodeId::getNetworkAddress64Msb() { return *((uint32_t*)&(_cmdData[17-CMD_DATA_OFFSET])); }
uint32_t
RxNodeId::getNetworkAddress64Lsb() { return *((uint32_t*)&(_cmdData[21-CMD_DATA_OFFSET])); }

uint8_t*
RxNodeId::getNi() { return &(_cmdData[25-CMD_DATA_OFFSET]); }
uint16_t
RxNodeId::getNiLength() {
    uint8_t* ni = getNi();
    uint16_t niLength = 0;
    for (; ni[niLength]!='\0'; niLength++);

    return niLength;
}

uint16_t
RxNodeId::getParentAddress16() { return *((uint16_t*)&(_cmdData[27+(getNiLength()-1)-CMD_DATA_OFFSET])); }
uint8_t
RxNodeId::getType() { return _cmdData[29+(getNiLength()-1)-CMD_DATA_OFFSET]; }
uint8_t
RxNodeId::getSourceEvent() { return _cmdData[30+(getNiLength()-1)-CMD_DATA_OFFSET]; }
uint16_t
RxNodeId::getDigiProfileId() { return *((uint16_t*)&(_cmdData[31+(getNiLength()-1)-CMD_DATA_OFFSET])); }
uint16_t
RxNodeId::getDigiManufacturerId() { return *((uint16_t*)&(_cmdData[33+(getNiLength()-1)-CMD_DATA_OFFSET])); }

uint32_t
RxNodeId::getDigiDd() {
    if (getCmdDataLength() >= (RX_NODE_ID_HEAD+getNiLength()+4))
        return *((uint32_t*)&(_cmdData[35+(getNiLength()-1)-CMD_DATA_OFFSET]));
    return 0;
}
uint8_t
RxNodeId::getRssi() {
    if (getCmdDataLength() >= (RX_NODE_ID_HEAD+getNiLength()+5))
        return _cmdData[39+(getNiLength()-1)-CMD_DATA_OFFSET];
    return 0;
}


uint16_t
RemoteAtCommandResponse::getCmd() { return *((uint16_t*)&(_cmdData[15-CMD_DATA_OFFSET])); }
uint8_t
RemoteAtCommandResponse::getStatus() { return _cmdData[17-CMD_DATA_OFFSET]; }
uint8_t*
RemoteAtCommandResponse::getData() { return &(_cmdData[18-CMD_DATA_OFFSET]); }
uint16_t
RemoteAtCommandResponse::getDataLength() { return getCmdDataLength() - REMOTE_AT_COMMAND_RESPONSE_HEAD; }


XBee::XBee(): _response(XBeeApiFrame()) {
    // global variables to parse incoming data
    _pos = 0;
    _escape = false;
    _checksumTotal = 0;
    // end

    _response.setCmdData(_responseCmdData);

#if defined(__AVR_ATmega32U4__) || defined(__MK20DX128__) || defined(__MK20DX256__)
    _serial = &Serial1;
#else
    _serial = &Serial;
#endif
}

void
XBee::readPacket() {
    // reset previous complete response
    if (_response.isReady() || _response.hasErrors()) {
        // discard previous packet and start over
        resetStateVariables();

        _response.setMsbLength(0);
        _response.setLsbLength(0);
        _response.setCmdId(0);
        _response.setChecksum(0);

        _response.setReady(false);
        _response.setErrorCode(NO_ERROR);
    }

    while (available()) {
        uint8_t c = read();
        if (_pos > 0 && c == XB_START) {
            _response.setErrorCode(UNEXPECTED_START);
            return;
        }
        if (_pos > MAX_CMD_DATA_SIZE) {
            _response.setErrorCode(PACKET_EXCEEDS_CMD_DATA_LENGTH);
            return;
        }
        if (_pos > 0 && c == XB_ESCAPE) {
            _escape = true;
            continue;
        }
        if (_escape) {
            c ^= 0x20;
            _escape = false;
        }

        if (_pos >= 3) {
            _checksumTotal += c;
        }

        switch (_pos) {
            case 0: {
                if (c == XB_START) {
                    _pos++;
                }
            } break;
            case 1: {
                _response.setMsbLength(c);
                _pos++;
            } break;
            case 2: {
                _response.setLsbLength(c);
                _pos++;
            } break;
            case 3: {
                _response.setCmdId(c);
                _pos++;
            } break;
            default: {
                // check if we're at the end of the packet.
                // Frame Data length does not include
                // start, msb, lsb, and checksum bytes, so add 3
                if (_pos == (_response.getFrameDataLength() + 3)) {
                    // verify checksum
                    if (_checksumTotal == 0xff) {
                        _response.setChecksum(c);
                        _response.setReady(true);

                        _response.setErrorCode(NO_ERROR);
                    } else {
                        _response.setErrorCode(CHECKSUM_FAILURE);
                    }

                    // reset state vars
                    _pos = 0;
                    _checksumTotal = 0;
                    return;
                } else {
                    _response.getCmdData()[_pos - CMD_DATA_OFFSET] = c;
                    _pos++;
                }
            }
        }
    }
}
bool
XBee::readPacket(const uint32_t& timeout) {
    uint32_t start = millis();

    while ((millis() - start) < timeout) {
        readPacket();

        if (getApiFrame().isReady()) {
            return true;
        } else if (getApiFrame().hasErrors()) {
            return false;
        }
    }

    // timed out
    return false;
}
void
XBee::readPacketUntilAvailable() {
    while (!(getApiFrame().isReady() || getApiFrame().hasErrors())) {
        readPacket();
    }
}

void
XBee::getApiFrame(XBeeApiFrame &response) {
    response.setMsbLength(_response.getMsbLength());
    response.setLsbLength(_response.getLsbLength());
    response.setCmdId(_response.getCmdId());
    response.setCmdData(_response.getCmdData());
    response.setChecksum(_response.getChecksum());
    response.setReady(_response.isReady());
    response.setErrorCode(_response.getErrorCode());
}
XBeeApiFrame&
XBee::getApiFrame() { return _response; }

void
XBee::send(XBeeApiFrame &request) {
    // start
    sendByte(XB_START, false);

    // length
    sendByte(request.getMsbLength(), true);
    sendByte(request.getLsbLength(), true);

    // cmdID
    sendByte(request.getCmdId(), true);

    // cmdData
    uint16_t length = request.getCmdDataLength();
    uint8_t* cmdData = request.getCmdData();
    for (uint16_t i = 0; i < length; i++) {
        sendByte(cmdData[i], true);
    }

    // checksum
    sendByte(request.getChecksum(), true);

    // send packet
    flush();
}

void
XBee::begin(Stream &serial) { _serial = &serial; }
void
XBee::setSerial(Stream &serial) { _serial = &serial; }
bool
XBee::available() { return _serial->available(); }
uint8_t
XBee::read() { return _serial->read(); }
void
XBee::flush() { _serial->flush(); }
void
XBee::write(const uint8_t& val) { _serial->write(val); }

void
XBee::sendByte(const uint8_t& b, const bool& escape) {
    if (escape && (b == XB_START || b == XB_ESCAPE || b == XB_ON || b == XB_OFF)) {
        write(XB_ESCAPE);
        write(b ^ 0x20);
    } else {
        write(b);
    }
}

void
XBee::resetStateVariables() {
    _pos = 0;
    _escape = false;
    _checksumTotal = 0;
}
