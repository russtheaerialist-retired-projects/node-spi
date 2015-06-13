node-spi
========

A NodeJS interface to the SPI bus typically found on embedded linux machines
such as the Raspberry Pi.

There is a native interface and a wrapped JS interface with a slightly
better API.

If you have a project that uses node-spi, please consider adding a link to your project on the wiki:

https://github.com/RussTheAerialist/node-spi/wiki/Projects-that-use-node-spi

The following versions exist

* node-spi 0.1.x - node 0.10.x
* node-spi 0.2 - node 0.12.x


Basic Usage
===========

```javascript
var SPI = require('spi');

var spi = new SPI.Spi('/dev/spidev0.0', {
    'mode': SPI.MODE['MODE_0'],  // always set mode as the first option
    'chipSelect': SPI.CS['none'] // 'none', 'high' - defaults to low
  }, function(s){s.open();});

var txbuf = new Buffer([ 0x23, 0x48, 0xAF, 0x19, 0x19, 0x19 ]);
var rxbuf = new Buffer([ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ]);

spi.transfer(txbuf, rxbuf, function(device, buf) {
    // rxbuf and buf should be the same here
    var s = "";
    for (var i=0; i < buf.length; i++)
        s = s + buf[i] + " ";
        console.log(s + "- " + new Date().getTime());
  });
```

How you should **really** use the library
=========================================

The above basic usage example is not how you should use the library, though.
Ideally, for each SPI device that is being controlled should have it's own
object that implements the protocol necessary to talk to your device so that
the device protocol is defined in one place.

An example project is [node-adafruit-pixel](https://github.com/RussTheAerialist/node-adafruit-pixel)
which is a node module to control the [AdaFruit RGB Pixels](http://www.adafruit.com/products/738).
The interface is defined in terms of color and pixels, and not in messages
being sent via the SPI bus, but it uses node-spi to do it's work.

Native Api Reference
====================

This section documents the native api which is defined in module _spi.node.
This is the interface that the normal Spi interface uses, but having a good
understanding of this part is important, as some people may want to use the
native interface directly.

Creating, Opening, and Closing the device
-----------------------------------------

**\_spi.Spi constructor** - The constructor only requires the path to the spi
dev file in /dev. Options and a callback are not required but can be specified.

Example:
```javascript
var spi = new SPI.Spi('/dev/spidev0.1');
```

Options can include:
* mode
* chipSelect
* size
* bitOrder
* maxSpeed
* halfDuplex
* loopback

Example:
```javascript
var spi = new SPI.Spi('/dev/spidev0.0', {'mode': SPI.MODE['MODE_0']});
```

The callback returns a handle to the newly created SPI object. It might be
handy to .open() it if you set all of your options in one shot.

Example:
```javascript
var spi = new SPI.Spi('/dev/spidev0.0', {}, function(s){s.open();});
```

**open()** - This function takes no arguments and will open the device using
all of the options that were previously set.  Once the device is open, we do not
allow you to change the settings on the device.

Example:
```javascript
var spi = new SPI.Spi('/dev/spidev0.0', {'mode': SPI.MODE['MODE_0']});

// get/set aditional options
spi.maxSpeed(20000); // in Hz
console.log('max speed: ' + spi.maxSpeed());

spi.open(); // once opened, you can't change the options
```

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

**size()** - This allows you to specify the bits per word to send.
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
**transfer(txbuf, rxbuf, callback)** - This takes two buffers, a write and a
read buffer, and optionally a callback. SPI only reads when a byte is written
so communicaton is usually full duplex.

Exmple:
```javascript
var txbuf = new Buffer([ 0x23, 0x48, 0xAF, 0x19, 0x19, 0x19 ]);
var rxbuf = new Buffer([ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ]);

spi.transfer(txbuf, rxbuf, function(device, buf) {
    var s = "";
    for (var i=0; i < buf.length; i++)
        s = s + buf[i] + " ";
        console.log(s);
  });
```

As a convenience feature, read and write functions pad zeros in the opposite
direction to make simple read and writes work.

**read(buffer, callback)** - Reads as much data as the given buffer is big.
The results of the read are available in the callback.

Example:
```javascript
var buf1 = new Buffer(8);
spi.read(buf1, function(device, buf2) {
    var s = "";
    for (var i=0; i < buf.length; i++)
      s = s + buf[i] + " ";
    console.log(s);
  });
```

**write(buffer, callback)** - Writes out the given buffer.

Example:
```javascript
var buf = new Buffer([0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0]);
spi.write(buf, function(device, buf2) {
    var s = "";
    for (var i=0; i < buf.length; i++)
      s = s + buf[i] + " ";
    console.log(s);
  });
```

Remember that these native apis are currently blocking.  I will update, once I
have the hardware to test this properly, to be async instead of blocking.
