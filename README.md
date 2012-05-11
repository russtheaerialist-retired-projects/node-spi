node-spi
========

A NodeJS interface to the SPI bus on embedded linux machines.

There is a native interface and a wrapped JS interface with a slightly
better API.

**THIS CODE IS NOT FINISHED YET, NOTHING IS FUNCTIONING YET!**

Usage
=====

```javascript
var spi = require("spi");

var MyDevice = new spi.Spi("/dev/spidev1.1", {
  "mode": 0,
  "chip_select": spi.NO_CS
  "max_speed": 1000000, // In Hz
  "size": 8, // How many bits per word
});

var out_buffer = [ 0x23, 0x48, 0xAF, 0x19, 0x19, 0x19 ];

MyDevice.transfer(out_buffer, outbuffer.Length(),
  function(device, recv_buffer) {
  // Do Something with the data in the recv buffer, if anything exists
});
```
