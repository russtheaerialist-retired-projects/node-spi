node-spi
========

A NodeJS interface to the SPI bus on embedded linux machines.

There is a native interface and a wrapped JS interface with a slightly
better API.

**THIS CODE IS NOT FINISHED YET, NOTHING IS FUNCTIONING YET!**

*Note: The first version will be blocking.  I know this is antithetics to
the nodejs philosophy, but I think it's important, when dealing with blocking
interfaces, to get the code working in a blocking manner, and then introduce
the async calls using eio.*

Basic Usage
===========

```javascript
var spi = require("spi");

var MyDevice = new spi.Spi("/dev/spidev1.1", {
  "mode": 0, // Always do mode first if you need something other than Mode 0
  "chip_select": spi.NO_CS
  "max_speed": 1000000, // In Hz
  "size": 8, // How many bits per word
});

var out_buffer = new Buffer([ 0x23, 0x48, 0xAF, 0x19, 0x19, 0x19 ]);

MyDevice.transfer(out_buffer, outbuffer.Length(),
  function(device, recv_buffer) {
  // Do Something with the data in the recv buffer, if anything exists
});
```

How you should **really** use the library
=========================================

The above basic usage example is not how you should use the library, though.
Ideally, for each SPI device that is being controlled should have it's own
object that implements the protocol necessary to talk to your device so that
the device protocol is defined in one place.

An example project is
[node-adafruit-pixel](https://github.com/RussTheAerialist/node-adafruit-pixel)
which is a node module to control the
[AdaFruit RGB Pixels](http://www.adafruit.com/products/738).  The interface is
defined in terms of color and pixels, and not in messages being sent via the
SPI bus, but it uses node-spi to do it's work.

Native Api Reference
====================

This section documents the native api which is defined in module \_spi.node.
This is the interface that the normal Spi interface uses, but having a good
understanding of this part is important, as some people may want to use the
native interface directly.

Creating, Opening, and Closing the device
-----------------------------------------

**\_spi.Spi constructor** - The constructor takes a single argument, the path
to the spi dev file in /dev.  We do not check that the file exists until you
call open.

**open()** - This function takes no arguments and will open the device, setting
all of the options that were previously set.  Once the device is open, we do not
allow you to change the settings to the device.

**close()** - This function should always be called before ending.  Right now
the destructor for the underlying C++ class does not call close(), but that
might change in the future.  You should always call close() when you are done
with the device.

Configuring the Device
----------------------

The following functions all act as getter/setters.  If you do not pass an
argument, it will return the value of the setting.  If you pass in a value,
it will change the value and then return the Spi class to allow function
chaining.

To understand these settings read the
[Linux SPI Summary](http://www.mjmwired.net/kernel/Documentation/spi/spi-summary)

**mode()** - This sets the clock phase and polarity of the clock signal.  This
should always be the first thing you call after open() if you plan to call it.
By default it is set to SPI_MODE_0.  The spi namespace provides constants for
the four SPI_MODE_X values (X being 0-3).

**chipSelect()** - This allows you to specify if the chip select signal should
be used, and if it should go high to select the chip or low.  It defaults to
signal low.  Pass in SPI_NO_CS to turn off Chip Select, and SPI_CS_HIGH to
turn on sending high to select.

**bitsPerWord()** - This allows you to specify the bits per word to send.
This defaults to 8-bits.  Check your device's datasheet for this value.

**bitOrder()** - This allows you to specify the order of the bits.  We default
to MSB, but send True as the argument if you want LSB.  This might not be the
best API.

**maxSpeed()** - This allows you to set the max transfer speed.  Again, check
your device's datasheet.  This is in Hz and defaults to 1Mhz.

**halfDuplex()** - Set this to True if your device does not support full duplex.
This isn't fully supported yet, as I need to add a Read/Write function calls that
is exposed to javascript. *This would be a great workitem for anyone who wants
to contribute*

**loopback()** - This sets the Loopback bit on the SPI controller.  I don't
fully understand what this is used for, but give you the ability to toggle it
if you'd like.

Getting and Sending Data
------------------------

**transfer()** - This takes two buffers, a write buffer and a read buffer.
If you only want to do one way transfer, then pass null to that argument.  For
example, writes would look like this:

```javascript
var buff = new Buffer([0x12, 0x12, 0x12]);
spi.transfer(buff, null);
```

Reads would look like this:

```javascript
var buff = new Buffer(8);
spi.transfer(null, buff);
```

Remember that these native apis are currently blocking.  I will update, once I
have the hardware to test this properly, to be async instead of blocking.
