#include <nan.h>

namespace fft {
#include "kiss_fft.c"
};
namespace fftr {
using namespace fft;
#include "kiss_fftr.c"
};

using namespace v8;

typedef fft::kiss_fft_cpx kiss_fft_cpx;

template <typename cfg_type, typename in_type, typename out_type>
class Worker : public NanAsyncWorker {
public:
  void HandleOKCallback();
  void Execute();

  Worker(NanCallback* callback, Local<Object> input, Local<Object> output);
  ~Worker();

private:
  cfg_type cfg;
  const in_type* in;
  out_type* out;
};


static void Alloc(fft::kiss_fft_cfg& cfg, int nfft) {
  cfg = fft::kiss_fft_alloc(nfft, false, 0, 0);
}

static void Alloc(fftr::kiss_fftr_cfg& cfg, int nfft) {
  cfg = fftr::kiss_fftr_alloc(nfft, false, 0, 0);
}

static void Free(fft::kiss_fft_cfg cfg) {
  fft::kiss_fft_free(cfg);
}

static void Free(fftr::kiss_fftr_cfg cfg) {
  fftr::kiss_fftr_free(cfg);
}

static void FFT(fft::kiss_fft_cfg cfg, const kiss_fft_cpx* in, kiss_fft_cpx* out) {
  kiss_fft(cfg, in, out);
}

static void FFT(fftr::kiss_fftr_cfg cfg, const kiss_fft_scalar* in, kiss_fft_cpx* out) {
  kiss_fftr(cfg, in, out);
}

template <typename cfg_type, typename in_type, typename out_type>
Worker<cfg_type, in_type, out_type>::Worker(NanCallback *callback, Local<Object> input, Local<Object> output)
  : NanAsyncWorker(callback) {
  int nfft = input->GetIndexedPropertiesExternalArrayDataLength();
  if (nfft == output->GetIndexedPropertiesExternalArrayDataLength())
    nfft /= 2;
  Alloc(cfg, nfft);
  SaveToPersistent("input", input);
  SaveToPersistent("output", output);
  in = (const in_type*) input->GetIndexedPropertiesExternalArrayData();
  out = (out_type*) output->GetIndexedPropertiesExternalArrayData();
}

template <typename cfg_type, typename in_type, typename out_type>
Worker<cfg_type, in_type, out_type>::~Worker() {
  Free(cfg);
}

template <typename cfg_type, typename in_type, typename out_type>
void Worker<cfg_type, in_type, out_type>::HandleOKCallback() {
  NanScope();

  Local<Object> output = GetFromPersistent("output");
  Handle<Value> argv[2] = { Null(), output };
  callback->Call(2, argv);
}

template <typename cfg_type, typename in_type, typename out_type>
void Worker<cfg_type, in_type, out_type>::Execute() {
  FFT(cfg, in, out);
}

NAN_METHOD(_fft) {
  NanScope();

  Local<Object> input = args[0].As<Object>();
  Local<Object> output = args[1].As<Object>();
  Local<Function> callback = args[2].As<Function>();

  NanCallback* nanCallback = new NanCallback(callback);

  if (!input->HasIndexedPropertiesInExternalArrayData() ||
      input->GetIndexedPropertiesExternalArrayDataType() != kExternalFloatArray ||
      !output->HasIndexedPropertiesInExternalArrayData() ||
      output->GetIndexedPropertiesExternalArrayDataType() != kExternalFloatArray) {
    NanThrowTypeError("Float32Array expected");
    NanReturnUndefined();
  }
  int input_len = input->GetIndexedPropertiesExternalArrayDataLength();
  int output_len = output->GetIndexedPropertiesExternalArrayDataLength();
  if (input_len != output_len && input_len + 2 != output_len) {
    NanThrowTypeError("Mismatch of array length for input and output");
    NanReturnUndefined();
  }
  if (output_len & 1) {
    NanThrowTypeError("Output array must have an even number of elements");
    NanReturnUndefined();
  }
  if (input_len == output_len) {
    NanAsyncQueueWorker(new Worker<fft::kiss_fft_cfg, kiss_fft_cpx, kiss_fft_cpx>(nanCallback, input, output));
  } else {
    NanAsyncQueueWorker(new Worker<fftr::kiss_fftr_cfg, kiss_fft_scalar, kiss_fft_cpx>(nanCallback, input, output));
  }
  NanReturnUndefined();
}

void Init(Handle<Object> exports) {
  exports->Set(NanNew("fft"), NanNew<FunctionTemplate>(_fft)->GetFunction());
}

NODE_MODULE(kissfft, Init)
