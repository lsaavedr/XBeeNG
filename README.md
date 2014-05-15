## XBeeNG

Arduino library for communicating with XBees in API=2 mode, with sopport for both [XBee Proprietary RF Modules](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/zigbee-mesh-module/xbee-proprietary-rf-modules/) and [XBee ZigBee/802.15.4 RF Modules](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/zigbee-mesh-module/xbee-zigbee-802154-rf-modules/), with the classic 20 pin footprint.

### XBee Proprietary RF Modules Supported:
- [XBee-PRO® 900HP](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-pro-900hp) / [XBee-PRO XSC](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-pro-xsc) (aka XB900HP).
- [XBee-PRO 868](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-pro-868) (aka XB868).
- [XBee® DigiMesh® 2.4](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/zigbee-mesh-module/xbee-digimesh-2-4) (aka XBDM).

### XBee ZigBee/802.15.4 RF Modules Supported:
- [XBee® ZB](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/zigbee-mesh-module/xbee-zb-module) (aka XBZB).
- [XBee® 802.15.4](http://www.digi.com/products/wireless-wired-embedded-solutions/zigbee-rf-modules/point-multipoint-rfmodules/xbee-series1-module) (aka XB802).

## Examples

### Basic Setup:
```c++
// Create an XBee object at the top of your sketch
XBee xbng = XBee();

void setup() {
	// Start the serial port
	Serial3.begin(9600);

	// Tell XBee to use this Hardware Serial. It's also possible to use SoftwareSerial
	xbng.begin(Serial3);

	// Others Settings
	...
}
```

### Basic XB900HP Network Settings
To understand the network setting in XB900HP module see the [user manual](http://ftp1.digi.com/support/documentation/90002173_M.pdf) (pag. 33).
```c++
...
	// Set the Preamble ID [HP] (range: 0-7)
	AtCommand atCmd = AtCommand(1, "HP", { 0x05 });// FrameID=1, cmd="HP", param={ 0x05 }
    xbng.send(atCmd);

	// Set the Network ID [ID] (range: 0-0x7FFF)
    atCmd.setCmd("ID", { 0x33, 0x31 });
    xbng.send(atCmd);

	// Set the Channel Mask [CM] (range: 0x1FFFFFF-0xFFFFFFFFFFFFFFFF)
    atCmd.setCmd("CM", {0x0F, 0xF5, 0xFF, 0xFF});
    xbng.send(atCmd);
...
```

### Basic XBDM Network Settings
To understand the network setting in XBDM module see the [user manual](http://ftp1.digi.com/support/documentation/90000991_K.pdf) (pag. 41).
```c++
...
	// Set the Network ID [ID] (range: 0-0x7FFF)
    AtCommand atCmd = AtCommand(1, "ID", { 0x33, 0x31 });// FrameID=1, cmd="ID", param={ 0x33, 0x31 }
    xbng.send(atCmd);

	// Set the Operating Channel [CH] (range: 0x0B-0x1A for XBee, and 0x0c-0x17 for XBee-PRO)
    atCmd.setCmd("CH", { 0x0F });
    xbng.send(atCmd);
...
```

### Basic XB802 Network Settings (P2P)
To understand the network setting in XB802 module see the [user manual](http://ftp1.digi.com/support/documentation/90000982_N.pdf) (pag. 18).
```c++
...
	// Set the module to operate as an End Device (CE = 0)
    AtCommand atCmd = AtCommand(1, "CE", { 0x00 });

...
```

### Basic Sending Data (with XB900HP, XB868, XBDM, XBZB)
```c++
...
	// Sending { 0xDA, 0x7A } to address64: 0x0013A200 0x403E0F30
	TxRequest txReq = TxRequest(2, 0x0013A200, 0x403E0F30, { 0xDA, 0x7A } );
    xbng.send(txReq);
...
```

### Basic Consumption Response
```c++
...
	// Consumption response
    xbng.readPacket(); // Removing old data for consuming new data
    xbng.readPacketUntilAvailable(); // Force read a new valid data
    
    // Getting data to print a summary
    XBeeApiFrame* xbApiFrame = &xbng.getApiFrame();
    xbApiFrame->printSummary(Serial);
...
```