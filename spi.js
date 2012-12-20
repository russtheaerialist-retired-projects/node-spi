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
    MODE_0: _spi.MODE_0, 
    MODE_1: _spi.MODE_1,
    MODE_2: _spi.MODE_2,
    MODE_3: _spi.MODE_3
};

var CS = {
    none: _spi.NO_CS,
    high: _spi.CS_HIGH,
    low:  _spi.CS_LOW
};

function isFunction(object) {
   return object && typeof object == 'function';
}

var Spi = function(device, options, callback) {
  this._spi = new _spi._spi();

  if (callback == undefined) {
    callback = options;
    options = {};
  }

  options = options || {}; // Default to an empty object

  for(var attrname in options) {
    var value = options[attrname];
    if (attrname in this._spi) {
      // console.trace("Setting " + attrname + "=" + value);
      this._spi[attrname](value);
    }
  }

  this.device = device;

  this._spi.open(device);

  isFunction(callback) && callback(this); // TODO: Update once open is async;
}

Spi.prototype.open = function() {
	return this._spi.open(this.device);
}

Spi.prototype.close = function() {
	return this._spi.close();
}

Spi.prototype.write = function(buf, callback) {
  this._spi.transfer(buf, null);

  isFunction(callback) && callback(this, buf); // TODO: Update once open is async;
}

Spi.prototype.read = function(buf, callback) {
  this._spi.transfer(null, buf);

  isFunction(callback) && callback(this, buf); // TODO: Update once open is async;
}
Spi.prototype.transfer = function(txbuf, rxbuf, callback) {
  rxbuf = new Buffer(txbuf.length);  // tx and rx buffers need to be the same size
  this._spi.transfer(txbuf, rxbuf);

  isFunction(callback) && callback(this, rxbuf); // TODO: Update once open is async;
}

module.exports.MODE = MODE;
module.exports.CS = CS;
module.exports.Spi = Spi;
