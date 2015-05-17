/*
    Copyright (c) 2012, Russell Hay <me@russellhay.com>

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

"use strict";

var _spi = require('bindings')('_spi.node');

// Consistance with docs
var MODE = {
    MODE_0: _spi.SPI_MODE_0, 
    MODE_1: _spi.SPI_MODE_1,
    MODE_2: _spi.SPI_MODE_2,
    MODE_3: _spi.SPI_MODE_3
};

var CS = {
    none: _spi.SPI_NO_CS,
    high: _spi.SPI_CS_HIGH,
    low:  _spi.SPI_CS_LOW
};

var ORDER = {
    msb:  _spi.SPI_MSB == 1,
    lsb:  _spi.SPI_LSB == 1
};

function isFunction(object) {
    return object && typeof object == 'function';
}

var Spi = function(device, options, callback) {
    this._spi = new _spi._spi();

    options = options || {}; // Default to an empty object

    for(var attrname in options) {
	var value = options[attrname];
	if (attrname in this._spi) {
	    this._spi[attrname](value);
	}
	else
	    console.log("Unknown option: " + attrname + "=" + value);
    }

    this.device = device;

    isFunction(callback) && callback(this); // TODO: Update once open is async;
}

Spi.prototype.open = function() {
    console.log(this);
    return this._spi.open(this.device);
}

Spi.prototype.close = function() {
    return this._spi.close();
}

Spi.prototype.write = function(buf, callback) {
    this._spi.transfer(buf, new Buffer(buf.length));

    isFunction(callback) && callback(this, buf); // TODO: Update once open is async;
}

Spi.prototype.read = function(buf, callback) {
    this._spi.transfer(new Buffer(buf.length), buf);

    isFunction(callback) && callback(this, buf); // TODO: Update once open is async;
}

Spi.prototype.transfer = function(txbuf, rxbuf, callback) {
    // tx and rx buffers need to be the same size
    this._spi.transfer(txbuf, rxbuf);

    isFunction(callback) && callback(this, rxbuf); // TODO: Update once open is async;
}

Spi.prototype.mode = function(mode) {
    if (typeof(mode) != 'undefined')
	if (mode == MODE['MODE_0'] || mode == MODE['MODE_1'] ||
	    mode == MODE['MODE_2'] || mode == MODE['MODE_3']) {
            this._spi['mode'](mode);
	    return this._spi;
	}
        else {
	    console.log('Illegal mode');
            return -1;
	}
    else
        return this._spi['mode']();
}

Spi.prototype.chipSelect = function(cs) {
    if (typeof(cs) != 'undefined')
	if (cs == CS['none'] || cs == CS['high'] || cs == CS['low']) {
            this._spi['chipSelect'](cs);
	    return this._spi;
	}
        else {
	    console.log('Illegal chip selection');
            return -1;
	}
    else
        return this._spi['chipSelect']();
}

Spi.prototype.bitsPerWord = function(bpw) {
    if (typeof(bpw) != 'undefined')
	if (bpw > 1) {
            this._spi['bitsPerWord'](bpw);
	    return this._spi;
	}
        else {
	    console.log('Illegal bits per word');
            return -1;
	}
    else
        return this._spi['bitsPerWord']();
}

Spi.prototype.bitOrder = function(bo) {
    if (typeof(bo) != 'undefined')
	if (bo == ORDER['msb'] || bo == ORDER['lsb']) {
            this._spi['bitOrder'](bo);
	    return this._spi;
	}
        else {
	    console.log('Illegal bit order');
            return -1;
	}
    else
        return this._spi['bitOrder']();
}

Spi.prototype.maxSpeed = function(speed) {
    if (typeof(speed) != 'undefined')
	if (speed > 0) {
            this._spi['maxSpeed'](speed);
            return this._spi;
	}
        else {
	    console.log('Speed must be positive');
	    return -1;
	}	    
    else
	return this._spi['maxSpeed']();
}

Spi.prototype.halfDuplex = function(duplex) {
    if (typeof(duplex) != 'undefined')
	if (duplex) {
	    this._spi['halfDuplex'](true);
	    return this._spi;
	}
        else {
	    this._spi['halfDuplex'](false);
            return this._spi;
	}
    else
	return this._spi['halfDuplex']();
}

Spi.prototype.loopback = function(loop) {
    if (typeof(loop) != 'undefined')
	if (loop) {
	    this._spi['loopback'](true);
	    return this._spi;
	}
        else {
	    this._spi['loopback'](false);
	    return this._spi;
	}
    else
	return this._spi['loopback']();
}

module.exports.MODE = MODE;
module.exports.CS = CS;
module.exports.ORDER = ORDER;
module.exports.Spi = Spi;
console.log(module.exports);
