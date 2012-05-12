var spi = require('./build/Release/_spi');
console.log(spi);
var x = new spi._spi();
console.log(x.mode());
x.mode(spi.SPI_MODE_3);
console.log(x.mode());
x.mode(spi.SPI_MODE_0);
x.chipSelect(spi.SPI_CS_HIGH);
console.log(x.mode());
x.chipSelect(spi.SPI_NO_CS);
console.log(x.mode());
x.chipSelect(0);
console.log(x.mode());
console.log(x.bitsPerWord());
x.bitsPerWord(10);
console.log(x.bitsPerWord());
console.log(x.maxSpeed());
// x.bitsPerWord(-1); // Error Case: Should throw RangeError
console.log(x.open("/dev/spidev1.1"));
var buff = new Buffer([ 0x56, 0x78, 0x88 ]);
x.transfer(buff, null);

console.log(x.close());
