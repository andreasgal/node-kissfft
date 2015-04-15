var fft = require('./index').fft;

fft(Float32Array([3, 0, 2, 0, 1, 0, 1, 0]), Float32Array(8), function (err, result) {
  console.log(result);
});

fft(Float32Array([3, 2, 1, 1]), Float32Array(8), function (err, result) {
  console.log(result);
});
