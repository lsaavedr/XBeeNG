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

#ifndef XBeeNG_h
#define XBeeNG_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <inttypes.h>

#define XBEENG_WITH_EXTRAS

#ifdef XBEENG_WITH_EXTRAS
#include <initializer_list>
#endif

// set to ATAP value of XBee. AP=2 is recommended
#define ATAP 2

#define XB_START    0x7e
#define XB_ESCAPE   0x7d
#define XB_ON       0x11
#define XB_OFF      0x13

/*
 * This value determines the size of the byte array for
 * receiving RX packets.
 * Most users won't be dealing with packets this large so
 * you can adjust this value to reduce memory consumption.
 * But, remember that if a RX packet exceeds this size, it
 * cannot be parsed!
 */

/*
 * This value is determined by the largest packet size
 */
#define MAX_CMD_DATA_SIZE 110

// API Frame Names
#define TX_64_REQUEST 0x00
#define TX_16_REQUEST 0x01
#define AT_COMMAND 0x08
#define AT_QUEUE_COMMAND 0x09
#define TX_REQUEST 0x10
#define EXPLICIT_TX_REQUEST 0x11
#define REMOTE_AT_COMMAND 0x17
#define RX_64_RESPONSE 0x80
#define RX_16_RESPONSE 0x81
#define RX_64_DATA_SAMPLE 0x82
#define RX_16_DATA_SAMPLE 0x83
#define AT_COMMAND_RESPONSE 0x88
#define TX_STATUS_RESPONSE 0x89
#define MODEM_STATUS 0x8A
#define TX_STATUS 0x8B
#define ROUTE_INFORMATION 0x8D
#define AGGREGATE_ADDRESSING 0x8E
#define RX_RESPONSE 0x90
#define EXPLICIT_RX_RESPONSE 0x91
#define RX_DATA_SAMPLE 0x92
#define RX_NODE_ID 0x95
#define REMOTE_AT_COMMAND_RESPONSE 0x97
// API Frame Names and Values Received from the Module:
// Others:
#define CREATE_SOURCE_ROUTE 0x21
#define SENSOR_READ 0x94
#define OTA_FIRMWARE_UPDATE_STATUS 0xa0
#define ROUTE_RECORD 0xa1
#define MTO_ROUTE_REQUEST 0xa3

#define RESERVED_LENGTH 2
#define RADIUS 0x00
#define RESERVED_1 0xFF
#define RESERVED_2 0xFE
#define TX_API_LENGTH 12
#define DM_RSSI_OFFSET 8
//no actual RSSI byte in the packet, 2 bytes before data starts

/**
 * TX STATUS constants
 */
#define    SUCCESS 0x0
#define CCA_FAILURE 0x2
#define INVALID_DESTINATION_ENDPOINT_SUCCESS 0x15
#define    NETWORK_ACK_FAILURE 0x21
#define NOT_JOINED_TO_NETWORK 0x22
#define    SELF_ADDRESSED 0x23
#define ADDRESS_NOT_FOUND 0x24
#define ROUTE_NOT_FOUND 0x25
#define PAYLOAD_TOO_LARGE 0x74

// modem status page 57
#define HARDWARE_RESET 0
#define WATCHDOG_TIMER_RESET 1
#define ASSOCIATED 2
#define DISASSOCIATED 3
#define SYNCHRONIZATION_LOST 4
#define COORDINATOR_REALIGNMENT 5
#define COORDINATOR_STARTED 6

#define ZB_BROADCAST_RADIUS_MAX_HOPS 0

#define ZB_TX_UNICAST 0
#define ZB_TX_BROADCAST 8

//page 57 DM datasheet
#define AT_OK 0
#define AT_ERROR  1
#define AT_INVALID_COMMAND 2
#define AT_INVALID_PARAMETER 3
#define AT_NO_RESPONSE 4

// Error codes (an implementation artifact):
#define NO_ERROR 0
#define CHECKSUM_FAILURE 1
#define PACKET_EXCEEDS_CMD_DATA_LENGTH 2
#define UNEXPECTED_START 3

#define CMD_DATA_OFFSET 4

/**
 * The super class of all XBee communication packet.
 * Users should never attempt to create an instance of this
 * class; instead create an instance of a subclass.
 * It is recommend to reuse subclasses to conserve memory.
 * This class allocates a buffer to.
 */
class XBeeApiFrame {
protected:
public:
    /**
     * Default constructor
     */
    XBeeApiFrame();
    ~XBeeApiFrame();
    /**
     * Returns the MSB length of the packet
     */
    uint8_t getMsbLength();
    void setMsbLength(const uint8_t& msbLength);
    /**
     * Returns the LSB length of the packet
     */
    uint8_t getLsbLength();
    void setLsbLength(const uint8_t& lsbLength);
    /**
     * Returns the length of the frame data
     */
    uint16_t getFrameDataLength();
    void setFrameDataLength(const uint16_t& frameDataLength);
    /**
     * Returns the cmdID
     */
    uint8_t getCmdId();
    void setCmdId(const uint8_t& cmdId);
    /**
     * Returns the length of the cmdData:
     * all bytes after the cmdID, and prior to the checksum
     */
    uint16_t getCmdDataLength();
    void setCmdDataLength(const uint16_t& cmdDataLength);
    /**
     * Returns the buffer that contains the cmdData.
     * Starts with byte that follows cmdID and includes all bytes
     * prior to the checksum.
     * Length is specified by getCmdDataLength()
     */
    uint8_t* getCmdData();
    void setCmdData(uint8_t* cmdDataPtr);
    /**
     * Returns the packet checksum
     */
    uint8_t getChecksum();
    void setChecksum(const uint8_t& checksum);
    void setChecksum();
    /**
     * Returns true if the response has been successfully
     * parsed and is ready for use.
     */
    bool isReady();
    void setReady(const bool& ready);
    /**
     * Returns true if the frame contains errors
     */
    bool hasErrors();
    /**
     * Returns an error code, or zero, if successful.
     * Error codes include:
     * - CHECKSUM_FAILURE
     * - PACKET_EXCEEDS_CMD_DATA_LENGTH
     * - UNEXPECTED_START
     */
    uint8_t getErrorCode();
    void setErrorCode(const uint8_t& errorCode);
#ifdef XBEENG_WITH_EXTRAS
    /*
     */
    void printSummary(Stream& strm);
#endif
protected:
    uint8_t _cmdId;
    uint8_t* _cmdData;

private:
    uint8_t _msbLength;
    uint8_t _lsbLength;
    uint8_t _checksum;

    bool _ready;
    uint8_t _errorCode;

    void printHex(Stream& strm, const uint8_t& hex);
};

class TxRxXBeeApiFrame : public XBeeApiFrame {
public:
    uint32_t getAddress64Msb();
    uint32_t getAddress64Lsb();
    uint16_t getAddress16();
};

/**
 * This class is extended by all frame descriptions
 * that include a frame id
 */
#define FRAME_ID_HEAD 1
class FrameIdDescription : public XBeeApiFrame {
public:
    uint8_t getFrameId();
    void setFrameId(const uint8_t& frameId);
};

#define TXRX_FRAME_ID_HEAD 11
class TxRxFrameIdDescription : public FrameIdDescription {
public:
    uint32_t getAddress64Msb();
    uint32_t getAddress64Lsb();
    void setAddress64(const uint32_t& address64Msb, const uint32_t& address64Lsb);

    uint16_t getAddress16();
    void setAddress16(const uint16_t& address16);
protected:
    void setAddress64(const uint32_t& address64Msb, const uint32_t& address64Lsb, bool performChecksum);
    void setAddress16(const uint16_t& address16, bool performChecksum);
};

#define TX_64_REQUEST_HEAD 10
class Tx64Request : public TxRxFrameIdDescription {
private:
    uint16_t getAddress16();
    void setAddress16(const uint16_t& address16);
public:
    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const uint8_t* data, const uint16_t& dataLength);
    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t* data, const uint16_t& dataLength);

    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const char* data);
    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const char* data);

#ifdef XBEENG_WITH_EXTRAS
    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const std::initializer_list<uint8_t>& data);
    Tx64Request(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const std::initializer_list<uint8_t>& data);
#endif

    uint8_t getOptions();
    void setOptions(const uint8_t& options);

    uint8_t* getData();
    void setData(const uint8_t* data, const uint16_t& dataLength);
    void setData(const char* data);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data);
#endif
    uint16_t getDataLength();
private:
    void setOptions(const uint8_t& options, const bool& performChecksum);

    void setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum);
    void setData(const char* data, const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum);
#endif
};

#define TX_16_REQUEST_HEAD 4
class Tx16Request : public FrameIdDescription {
public:
    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const uint8_t* data, const uint16_t& dataLength);

    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const uint8_t& options, const char* data);
    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const char* data);

#ifdef XBEENG_WITH_EXTRAS
    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const uint8_t& options,
        const std::initializer_list<uint8_t>& data);
    Tx16Request(const uint8_t& frameId,
        const uint16_t& address16, const std::initializer_list<uint8_t>& data);
#endif

    uint16_t getAddress16();
    void setAddress16(const uint16_t& address16);

    uint8_t getOptions();
    void setOptions(const uint8_t& options);

    uint8_t* getData();
    void setData(const uint8_t* data, const uint16_t& dataLength);
    void setData(const char* data);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data);
#endif
    uint16_t getDataLength();
private:
    void setAddress16(const uint16_t& address16, const bool& performChecksum);

    void setOptions(const uint8_t& options, const bool& performChecksum);

    void setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum);
    void setData(const char* data, const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum);
#endif
};

#define AT_COMMAND_HEAD 3
class AtCommand : public FrameIdDescription {
public:
    AtCommand(const uint8_t& frameId, const uint8_t* data, const uint16_t& dataLength);
    AtCommand(const uint8_t& frameId, const char* data);

    AtCommand(const uint8_t& frameId, const uint16_t& cmd);
    AtCommand(const uint8_t& frameId, const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength);

#ifdef XBEENG_WITH_EXTRAS
    AtCommand(const uint8_t& frameId, const std::initializer_list<uint8_t>& data);
    AtCommand(const uint8_t& frameId, const char (&cmd)[3], const std::initializer_list<uint8_t>& param);
#endif

    uint16_t getCmd();
    void setCmd(const uint16_t& cmd);
    void setCmd(const char (&cmd)[3]);
#ifdef XBEENG_WITH_EXTRAS
    void setCmd(const char (&cmd)[3], const std::initializer_list<uint8_t>& param);
#endif

    uint8_t* getParam();
    void setParam(const uint8_t* param, const uint16_t& paramLengt);
    void setParam(const char* param);
#ifdef XBEENG_WITH_EXTRAS
    void setParam(const std::initializer_list<uint8_t>& param);
#endif
    uint16_t getParamLength();
private:
    void setCmd(const uint16_t& cmd, const bool& performChecksum);
    void setCmd(const char (&cmd)[3], const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setCmd(const char (&cmd)[3], const std::initializer_list<uint8_t>& param, const bool& performChecksum);
#endif

    void setParam(const uint8_t* param, const uint16_t& paramLength, const bool& performChecksum);
    void setParam(const char* param, const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setParam(const std::initializer_list<uint8_t>& param, const bool& performChecksum);
#endif
};

#define AT_QUEUE_COMMAND_HEAD 3
class AtQueueCommand : public AtCommand {
public:
    AtQueueCommand(const uint8_t& frameId, const uint8_t* data, const uint16_t& dataLength);
    AtQueueCommand(const uint8_t& frameId, const char* data);

    AtQueueCommand(const uint8_t& frameId, const uint16_t& cmd);
    AtQueueCommand(const uint8_t& frameId, const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength);
};

#define TX_REQUEST_HEAD 13
#define BROADCAST_ADDRESS16 0xfffe
#define BROADCAST_ADDRESS64 0xffff
class TxRequest : public TxRxFrameIdDescription {
public:
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& broadcast, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t* data, const uint16_t& dataLength);
    TxRequest(const uint8_t& frameId, const uint8_t* data, const uint16_t& dataLength);

    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
        const char* data);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& broadcast, const uint8_t& options,
        const char* data);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const char* data);
    TxRequest(const uint8_t& frameId, const char* data);

#ifdef XBEENG_WITH_EXTRAS
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& broadcast, const uint8_t& options,
        const std::initializer_list<uint8_t>& data);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& broadcast, const uint8_t& options,
        const std::initializer_list<uint8_t>& data);
    TxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const std::initializer_list<uint8_t>& data);
    TxRequest(const uint8_t& frameId,
        const std::initializer_list<uint8_t>& data);
#endif

    uint8_t getBroadcast();
    void setBroadcast(const uint8_t& broadcast);

    uint8_t getOptions();
    void setOptions(const uint8_t& options);

    uint8_t* getData();
    void setData(const uint8_t* data, const uint16_t& dataLength);
    void setData(const char* data);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data);
#endif
    uint16_t getDataLength();
private:
    void setBroadcast(const uint8_t& broadcast, const bool& performChecksum);
    void setOptions(const uint8_t& options, const bool& performChecksum);
    void setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum);
    void setData(const char* data, const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum);
#endif
};

#define EXPLICIT_TX_REQUEST_HEAD 19
class ExplicitTxRequest : public TxRxFrameIdDescription {
public:
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t* data, const uint16_t& dataLength);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t* data, const uint16_t& dataLength);

    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const char* data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const char* data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const char* data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const char* data);

#ifdef XBEENG_WITH_EXTRAS
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const std::initializer_list<uint8_t>& data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const uint8_t& broadcast, const uint8_t& options,
        const std::initializer_list<uint8_t>& data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const std::initializer_list<uint8_t>& data);
    ExplicitTxRequest(const uint8_t& frameId,
        const uint8_t& sourceEndpoint, const uint8_t& destinationEndpoint,
        const uint16_t& clusterId, const uint16_t& profileId,
        const std::initializer_list<uint8_t>& data);
#endif

    uint8_t getSourceEndpoint();
    void setSourceEndpoint(const uint8_t& sourceEndpoint);

    uint8_t getDestinationEndpoint();
    void setDestinationEndpoint(const uint8_t& destinationEndpoint);

    uint16_t getClusterId();
    void setClusterId(const uint16_t& clusterId);

    uint16_t getProfileId();
    void setProfileId(const uint16_t& profileId);

    uint8_t getBroadcast();
    void setBroadcast(const uint8_t& broadcast);

    uint8_t getOptions();
    void setOptions(const uint8_t& options);

    uint8_t* getData();
    void setData(const uint8_t* data, const uint16_t& dataLength);
    void setData(const char* data);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data);
#endif
    uint16_t getDataLength();

private:
    void setSourceEndpoint(const uint8_t& sourceEndpoint, const bool& performChecksum);
    void setDestinationEndpoint(const uint8_t& destinationEndpoint, const bool& performChecksum);
    void setClusterId(const uint16_t& clusterId, const bool& performChecksum);
    void setProfileId(const uint16_t& profileId, const bool& performChecksum);
    void setBroadcast(const uint8_t& broadcast, const bool& performChecksum);
    void setOptions(const uint8_t& options, const bool& performChecksum);
    void setData(const uint8_t* data, const uint16_t& dataLength, const bool& performChecksum);
    void setData(const char* data, const bool& performChecksum);
#ifdef XBEENG_WITH_EXTRAS
    void setData(const std::initializer_list<uint8_t>& data, const bool& performChecksum);
#endif
};

#define REMOTE_AT_COMMAND_HEAD 14
class RemoteAtCommand : public TxRxFrameIdDescription {
public:
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& options,
        const char* data);
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const char* data);

    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& options,
        const uint8_t* data, const uint16_t& dataLength);
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const uint8_t* data, const uint16_t& dataLength);

    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& options,
        const uint16_t& cmd);
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options, const uint16_t& cmd);

    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& options,
        const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength);
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options,
        const uint16_t& cmd, const uint8_t* param, const uint16_t& paramLength);

    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint16_t& address16, const uint8_t& options,
        const char (&cmd)[3], const uint8_t* param, const uint16_t& paramLength);
    RemoteAtCommand(const uint8_t& frameId,
        const uint32_t& address64Msb, const uint32_t& address64Lsb,
        const uint8_t& options,
        const char (&cmd)[3], const uint8_t* param, const uint16_t& paramLength);

    uint8_t getOptions();
    void setOptions(const uint8_t& options);

    uint16_t getCmd();
    void setCmd(const uint16_t& cmd);
    void setCmd(const char* cmd);

    uint8_t* getParam();
    void setParam(const uint8_t* param, const uint16_t& paramLength);
    uint16_t getParamLength();
private:
    void setOptions(const uint8_t& options, const bool& performChecksum);
    void setCmd(const uint16_t& cmd, const bool& performChecksum);
    void setCmd(const char* cmd, const bool& performChecksum);
    void setParam(const uint8_t* param, const uint16_t& paramLength, const bool& performChecksum);
};

#define RX_64_RESPONSE_HEAD 10
class Rx64Response : public TxRxXBeeApiFrame {
private:
    uint16_t getAddress16();
public:
    uint8_t getRssi();
    uint8_t getOptions();

    uint8_t* getData();
    uint16_t getDataLength();
};

#define RX_16_RESPONSE_HEAD 4
class Rx16Response : public XBeeApiFrame {
public:
    uint16_t getAddress16();

    uint8_t getRssi();
    uint8_t getOptions();

    uint8_t* getData();
    uint16_t getDataLength();
};

#define RX_64_DATA_SAMPLE_HEAD 15
class Rx64DataSample : public Rx64Response {
private:
    uint8_t* getData();
    uint16_t getDataLength();
public:
    uint8_t getNSamples();

    uint16_t getDigitalMask();
    uint8_t getAnalogMask();

    uint16_t getDigitalSamples();

    uint16_t* getAnalogSamples();
    uint16_t getAnalogSamplesLength();
};

#define RX_16_DATA_SAMPLE_HEAD 9
class Rx16DataSample : public Rx16Response {
private:
    uint8_t* getData();
    uint16_t getDataLength();
public:
    uint8_t getNSamples();

    uint16_t getDigitalMask();
    uint8_t getAnalogMask();

    uint16_t getDigitalSamples();

    uint16_t* getAnalogSamples();
    uint16_t getAnalogSamplesLength();
};

#define AT_COMMAND_RESPONSE_HEAD 4
class AtCommandResponse : public FrameIdDescription {
public:
    uint16_t getCmd();
    uint8_t getStatus();

    uint8_t* getData();
    uint16_t getDataLength();
};

#define TX_STATUS_RESPONSE_HEAD 2
class TxStatusResponse : public FrameIdDescription {
public:
    uint8_t getStatus();
};

#define MODEM_STATUS_HEAD 1
class ModemStatus : public XBeeApiFrame {
public:
    uint8_t getStatus();
};

#define TX_STATUS_HEAD 6
class TxStatus : public FrameIdDescription {
public:
    uint16_t getAddress16();
    uint8_t getRetryCount();
    uint8_t getDeliveryStatus();
    uint8_t getDiscoveryStatus();
};

#define ROUTE_INFORMATION_HEAD 41
class RouteInformation : public XBeeApiFrame {
public:
    uint8_t getSourceEvent();
    uint8_t getLength();
    uint32_t getTimestamp();
    uint8_t getAckTimoutCount();

    uint32_t getDestinationAddress64Msb();
    uint32_t getDestinationAddress64Lsb();

    uint32_t getSourceAddress64Msb();
    uint32_t getSourceAddress64Lsb();

    uint32_t getResponderAddress64Msb();
    uint32_t getResponderAddress64Lsb();

    uint32_t getReceiverAddress64Msb();
    uint32_t getReceiverAddress64Lsb();
};

#define AGGREGATE_ADDRESSING_HEAD 17
class AggregateAddressing : public XBeeApiFrame {
public:
    uint8_t getFormatId();

    uint32_t getNewAddress64Msb();
    uint32_t getNewAddress64Lsb();

    uint32_t getOldAddress64Msb();
    uint32_t getOldAddress64Lsb();
};

#define RX_RESPONSE_HEAD 11
class RxResponse : public TxRxXBeeApiFrame {
public:
    uint8_t getOptions();

    uint8_t* getData();
    uint16_t getDataLength();
};

#define EXPLICIT_RX_RESPONSE_HEAD 17
class ExplicitRxResponse : public TxRxXBeeApiFrame {
public:
    uint8_t getSourceEndpoint();
    uint8_t getDestinationEndpoint();

    uint16_t getClusterId();
    uint16_t getProfileId();

    uint8_t getOptions();

    uint8_t* getData();
    uint16_t getDataLength();
};

#define RX_DATA_SAMPLE_HEAD 17
class RxDataSample : public RxResponse {
private:
    uint8_t* getData();
    uint16_t getDataLength();
public:
    uint8_t getNSamples();

    uint16_t getDigitalMask();
    uint8_t getAnalogMask();

    uint16_t getDigitalSamples();

    uint16_t* getAnalogSamples();
    uint16_t getAnalogSamplesLength();
};

#define RX_NODE_ID_HEAD 29
class RxNodeId : public RxResponse {
private:
    uint8_t* getData();
    uint16_t getDataLength();
public:
    uint16_t getSourceAddress16();
    uint32_t getNetworkAddress64Msb();
    uint32_t getNetworkAddress64Lsb();

    uint8_t* getNi();
    uint16_t getNiLength();

    uint16_t getParentAddress16();

    uint8_t getType();

    uint8_t getSourceEvent();

    uint16_t getDigiProfileId();
    uint16_t getDigiManufacturerId();

    uint32_t getDigiDd();
    uint8_t getRssi();
};

#define REMOTE_AT_COMMAND_RESPONSE_HEAD 14
class RemoteAtCommandResponse : public TxRxFrameIdDescription {
public:
    uint16_t getCmd();
    uint8_t getStatus();

    uint8_t* getData();
    uint16_t getDataLength();
};

/**
 * Primary interface for communicating with an XBee Radio.
 * This class provides methods for sending and receiving
 * packets with an XBee radio via the serial port.
 * The XBee radio must be configured in API (packet) mode (AP=2)
 * in order to use this software.
 * <p/>
 * Since this code is designed to run on a microcontroller,
 * with only one thread, you are responsible for reading the
 * data of the serial buffer in a timely manner.  This involves
 * a call to a variant of readPacket(...).
 * If your serial port is receiving data faster than you are
 * reading, you can expect to lose packets.
 * <p/>
 * In order to conserve resources, this class only supports
 * storing one response packet in memory at a time.
 * This means that you must fully consume the packet prior to
 * calling readPacket(...), because calling readPacket(...)
 * overwrites the previous response.
 * <p/>
 * This class creates an array of size MAX_CMD_DATA_SIZE for
 * storing the response packet.  You may want to adjust this
 * value to conserve memory.
 *
 * \author Andrew Rapp
 */
class XBee {
public:
    XBee();
    /**
     * Reads all available serial bytes until a packet is parsed,
     * an error occurs, or the buffer is empty.
     * You may call <i>xbee</i>.getResponse().isReady() after
     * calling this method to determine if a packet is ready, or
     * <i>xbee</i>.getResponse().isError() to determine if a error
     * occurred.
     * <p/>
     * This method should always return quickly since it does not
     * wait for serial data to arrive.
     * You will want to use this method if you are doing other
     * timely stuff in your loop, where a delay would cause problems.
     * NOTE: calling this method resets the current response, so
     * make sure you first consume the current response
     */
    void readPacket();
    /**
     * Waits a maximum of <i>timeout</i> milliseconds for a response
     * packet before timing out; returns true if packet is read.
     * Returns false if timeout or error occurs.
     */
    bool readPacket(const uint32_t& timeout);
    /**
     * Reads until a packet is received or an error occurs.
     * Caution: use this carefully since if you don't get a response,
     * your Arduino code will hang on this call forever!! often it's
     * better to use a timeout: readPacket(int)
     */
    void readPacketUntilAvailable();
    /**
     * Set response with the internal response values.
     */
    void getApiFrame(XBeeApiFrame &response);
    /**
     * Returns a reference to the current response
     * Note: once readPacket is called again this response will be
     * overwritten!
     */
    XBeeApiFrame& getApiFrame();
    /**
     * Sends a XBeeRequest (TX packet) out the serial port
     */
    void send(XBeeApiFrame &request);
    /**
     * Starts the serial connection on the specified serial port
     */
    void begin(Stream &serial);
    /**
     * Specify the serial port.
     */
    void setSerial(Stream &serial);

private:
    bool available();
    uint8_t read();
    void flush();
    void write(const uint8_t& val);
    void sendByte(const uint8_t& b, const bool& escape);

    // just a state variables for packet parsing
    // and has no relevance for the response otherwise
    uint16_t _pos;
    bool _escape;
    uint8_t _checksumTotal;
    void resetStateVariables();

    // buffer for incoming RX packets.
    XBeeApiFrame _response;
    uint8_t _responseCmdData[MAX_CMD_DATA_SIZE];

    Stream* _serial;
};

#endif //XBeeNG_h
