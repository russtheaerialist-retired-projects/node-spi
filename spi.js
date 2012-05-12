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

var MODE = [
  _spi.SPI_MODE_0,
  _spi.SPI_MODE_1,
  _spi.SPI_MODE_2,
  _spi.SPI_MODE_3
];

var CS = {
  'high': _spi.SPI_CS_HIGH,
  'low': 0,
  'none': _spi.SPI_NO_CS
};

function isFunction(object) {
   return object && getClass.call(object) == '[object Function]';
}

var Spi = function(device, options, callback) {
  this._spi = new _spi._spi();

  if (callback == undefined) {
    callback = options;
    options = {};
  }

  options = options || { }; // Default to an empty object

  for(var attrname in options) {
    var value = options[attrname];
    if (attrname in this._spi) {
      console.trace("Setting " + attrname + "=" + value);
      this._spi[attrname](value);
    }
  }

  this._spi.open(device);

  callback(this); // TODO: Update once open is async;
}

Spi.prototype.write = function(buf, callback) {
  this._spi.transfer(buf, null);

  if (callback !== undefined) {
    callback(this, buf); // TODO: Update once transfer is async;
  }
}

Spi.prototype.read = function(buf, callback) {
  this._spi.transfer(null, buf);

  if (callback !== undefined) {
    callback(this, buf); // TODO: Update once transfer is async;
  }
}
Spi.prototype.transfer = function(wrbuf, rdbuf, callback) {
  this._spi.transfer(wrbuf, rdbuf);

  if (callback !== undefined) {
    callback(this, rdbuf); // TODO: Update once transfer is async;
  }
}

module.exports.MODE = MODE;
module.exports.CS = CS;
module.exports.Spi = Spi;
