//var spi = require('./build/Release/_spi');
//console.log(spi);
//var x = new spi._spi();
//console.log(x.mode());
//x.mode(spi.SPI_MODE_3);
//console.log(x.mode());
//x.mode(spi.SPI_MODE_0);
//x.chipSelect(spi.SPI_CS_HIGH);
//console.log(x.mode());
//x.chipSelect(spi.SPI_NO_CS);
//console.log(x.mode());
//x.chipSelect(0);
//console.log(x.mode());
//console.log(x.bitsPerWord());
//x.bitsPerWord(10);
//console.log(x.bitsPerWord());
//console.log(x.maxSpeed());
// x.bitsPerWord(-1); // Error Case: Should throw RangeError
//console.log(x.open("/dev/spidev1.1"));
//var buff = new Buffer([ 0x56, 0x78, 0x88 ]);
//x.transfer(buff, null);
//
//console.log(x.close());

var spi = require('./spi');

var rotate = function(spi, buf) {
    var tmp = buf[0];
    for(var i=0; i<buf.length; i++) {
        buf[i] = (i != buf.length-1) ? buf[i+1] : tmp;
    }
    spi.write(buf);
}

var pixelCtrl = new spi.Spi('/dev/spidev1.1', {
    'mode': spi.MODE[0], // Make sure that mode is the first option!
    'chipSelect': spi.CS['none']
}, function(spi) {
    console.log("Callback called");
    var buf = new Buffer([0xff, 0x00, 0x00]); // Turn on Red;
    spi.write(buf, function(spi, buf) {
        setInterval(function() {
            rotate(spi, buf);
        }, 500)
    });
});
