/* -*- Mode: Java; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set shiftwidth=2 tabstop=2 autoindent cindent expandtab: */

var fft = require('./index').fft;
var ref = require('fft').complex;

function join(array, separator) {
  return Array.prototype.join.call(array, separator);
}

function test(name, input) {
  exports[name] = function(test) {
    var complex = [];
    input.forEach(function (r) {
      complex.push(r, 0);
    });
    var output = [];
    (new ref(input.length, false)).simple(output, complex, 'complex');
    test.expect(4);
    fft(Float32Array(complex), Float32Array(output.length), function (err, result) {
      test.ok(!err);
      test.equal(join(result, ','), output.join(','));
      fft(Float32Array(input), Float32Array(input.length+2), function (err, result_r) {
        test.ok(!err);
        test.equal(join(result, ',').indexOf(join(result_r, ',')), 0);
        test.done();
      });
    });
  };
}

test('fft1', [1,2,3,4]);
