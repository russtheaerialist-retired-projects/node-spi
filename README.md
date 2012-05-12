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

**chipSelect()** - This
