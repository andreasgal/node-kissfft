var fft = require('./index').fft;

fft(Float32Array([1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0]), Float32Array(16), function (err, result) {
  console.log(Array.prototype.join.call(result, ','));
});

fft(Float32Array([1, 2, 3, 4, 5, 6, 7, 8]), Float32Array(16), function (err, result) {
  console.log(Array.prototype.join.call(result, ','));
});
