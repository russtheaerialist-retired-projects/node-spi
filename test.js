var spi = require('./build/Release/_spi');
console.log(spi);
var x = new spi._spi("/dev/spidev1.1");
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
// x.bitsPerWord(-1); // Error Case: Should throw RangeError
console.log(x.open());
console.log(x.close());
