#define BUILDING_NODE_EXTENSION

#include "spi_binding.h"

#include <stdio.h>
#include <linux/spi/spidev.h>
#include <v8.h>
#include <node.h>

using namespace v8;
using namespace node;

extern "C" {
  void init (Handle<Object> target) {
    Spi::Initialize(target);
  }

  NODE_MODULE(spi, init);
}

Persistent<Function> Spi::constructor;

void Spi::Initialize(Handle<Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);

  t->SetClassName(String::NewSymbol("_spi"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  t->PrototypeTemplate()->Set(String::NewSymbol("open"),
                              FunctionTemplate::New(Open)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("close"),
                              FunctionTemplate::New(Close)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("transfer"),
                              FunctionTemplate::New(Transfer)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("mode"),
                              FunctionTemplate::New(GetSetMode)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("chip_select"),
                              FunctionTemplate::New(SetChipSelect)->GetFunction());

  constructor = Persistent<Function>::New(t->GetFunction());
  target->Set(String::NewSymbol("_spi"), constructor);

  // SPI modes
  NODE_DEFINE_CONSTANT(target, SPI_MODE_0);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_1);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_2);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_3);

  // Logic Level High for Chip Select
  NODE_DEFINE_CONSTANT(target, SPI_CS_HIGH);

  // LSB First
  NODE_DEFINE_CONSTANT(target, SPI_LSB_FIRST);

  // Half-Duplex
  NODE_DEFINE_CONSTANT(target, SPI_3WIRE);

  // Loop? Not sure what this one does
  NODE_DEFINE_CONSTANT(target, SPI_LOOP);

  // No Chip Select
  NODE_DEFINE_CONSTANT(target, SPI_NO_CS);

}

// new Spi(string device)
//Handle<Value> Spi::New(const Arguments& args) {
SPI_FUNC_IMPL(New) {
  FUNCTION_PREAMBLE;
  REQ_ARGS(1);
  // TODO: Add argument handling

  Spi* spi = new Spi();
  spi->Wrap(args.This());

  return args.This();
}

Handle<Value> Spi::Open(const Arguments& args) {
  FUNCTION_PREAMBLE;

  FUNCTION_CHAIN;
}

Handle<Value> Spi::Close(const Arguments &args) {
  FUNCTION_PREAMBLE;
  ONLY_IF_OPEN;

  FUNCTION_CHAIN;
}

Handle<Value> Spi::Transfer(const Arguments &args) {
  FUNCTION_PREAMBLE;
  ASSERT_OPEN;

  FUNCTION_CHAIN;
}

// This overrides any of the OTHER set functions since modes are predefined
// sets of options.
SPI_FUNC_IMPL(GetSetMode) {
  FUNCTION_PREAMBLE;
  GETTER(1, Number::New(self->m_mode));
  REQ_INT_ARG(0, in_mode);
  ASSERT_NOT_OPEN;

  if (in_mode == SPI_MODE_0 || in_mode == SPI_MODE_1 ||
      in_mode == SPI_MODE_2 || in_mode == SPI_MODE_3) {
    self->m_mode = in_mode;
  } else {
    ThrowException(Exception::RangeError(String::New(
      "Argument 1 must be one of the SPI_MODE_X constants"
    )));
  }
  FUNCTION_CHAIN;
}
SPI_FUNC_IMPL(SetChipSelect) {
  FUNCTION_PREAMBLE;
  REQ_INT_ARG(0, in_value);
  ASSERT_NOT_OPEN;

  switch(in_value) {
    case SPI_CS_HIGH:
      self->m_mode |= SPI_CS_HIGH;
      self->m_mode &= ~SPI_NO_CS;
      break;
    case SPI_NO_CS:
      self->m_mode |= SPI_NO_CS;
      self->m_mode &= ~SPI_CS_HIGH;
      break;
    default:
      self->m_mode &= ~(SPI_NO_CS|SPI_CS_HIGH);
      break;
  }

  FUNCTION_CHAIN;
}
SPI_FUNC_EMPTY(SetMaxSpeed);
SPI_FUNC_EMPTY(Set3Wire);
SPI_FUNC_EMPTY(SetLoop);
SPI_FUNC_EMPTY(SetLSB);
SPI_FUNC_EMPTY(SetWordSize);
