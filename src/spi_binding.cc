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

#define BUILDING_NODE_EXTENSION

#include "spi_binding.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <v8.h>
#include <node.h>
#include <node_buffer.h>

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
  t->PrototypeTemplate()->Set(String::NewSymbol("chipSelect"),
                              FunctionTemplate::New(GetSetChipSelect)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("bitsPerWord"),
                              FunctionTemplate::New(GetSetBitsPerWord)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("bitOrder"),
                              FunctionTemplate::New(GetSetBitOrder)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("maxSpeed"),
                              FunctionTemplate::New(GetSetMaxSpeed)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("halfDuplex"),
                              FunctionTemplate::New(GetSet3Wire)->GetFunction());
  t->PrototypeTemplate()->Set(String::NewSymbol("loopback"),
                              FunctionTemplate::New(GetSetLoop)->GetFunction());

  constructor = Persistent<Function>::New(t->GetFunction());
  target->Set(String::NewSymbol("_spi"), constructor);

  // SPI modes
  NODE_DEFINE_CONSTANT(target, SPI_MODE_0);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_1);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_2);
  NODE_DEFINE_CONSTANT(target, SPI_MODE_3);

  // Logic Level High for Chip Select
  NODE_DEFINE_CONSTANT(target, SPI_CS_HIGH);

  // No Chip Select
  NODE_DEFINE_CONSTANT(target, SPI_NO_CS);

}

// new Spi(string device)
//Handle<Value> Spi::New(const Arguments& args) {
SPI_FUNC_IMPL(New) {


  Spi* spi = new Spi();
  spi->Wrap(args.This());

  return args.This();
}

// TODO: Make Non-blocking once basic functionality is proven
Handle<Value> Spi::Open(const Arguments& args) {
  FUNCTION_PREAMBLE;
  REQ_ARGS(1);
  ASSERT_NOT_OPEN;

  String::Utf8Value device(args[0]->ToString());
  int retval = 0;

  self->m_fd = open(*device, O_RDWR); // Blocking!
  if (self->m_fd < 0) {
    return ThrowException(Exception::Error(String::New("Unable to open device")));
  }

  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_WR_MODE, self->m_mode);
  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_RD_MODE, self->m_mode);
  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_WR_BITS_PER_WORD, self->m_bits_per_word);
  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_RD_BITS_PER_WORD, self->m_bits_per_word);
  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_WR_MAX_SPEED_HZ, self->m_max_speed);
  SET_IOCTL_VALUE(self->m_fd, SPI_IOC_RD_MAX_SPEED_HZ, self->m_max_speed);

  FUNCTION_CHAIN;
}

Handle<Value> Spi::Close(const Arguments &args) {
  FUNCTION_PREAMBLE;
  ONLY_IF_OPEN;

  close(self->m_fd);
  self->m_fd = -1;

  FUNCTION_CHAIN;
}

// tranfer(write_buffer, read_buffer);
Handle<Value> Spi::Transfer(const Arguments &args) {
  FUNCTION_PREAMBLE;
  ASSERT_OPEN;
  REQ_ARGS(2);

  if ((args[0]->IsNull()) && (args[1]->IsNull())) {
    return ERROR("Both buffers cannot be null");
  }

  char *write_buffer = NULL;
  char *read_buffer = NULL;
  size_t write_length = -1;
  size_t read_length = -1;
  Local<Object> write_buffer_obj;
  Local<Object> read_buffer_obj;

  if (Buffer::HasInstance(args[0])) {
    write_buffer_obj = args[0]->ToObject();
    write_buffer = Buffer::Data(write_buffer_obj);
    write_length = Buffer::Length(write_buffer_obj);
  }

  if (Buffer::HasInstance(args[1])) {
    read_buffer_obj = args[1]->ToObject();
    read_buffer = Buffer::Data(read_buffer_obj);
    read_length = Buffer::Length(read_buffer_obj);
  }

  if (write_length > 0 && read_length > 0 && write_length != read_length) {
    return ERROR("Read and write buffers MUST be the same length");
  }

  Handle<Value> retval = self->full_duplex_transfer(write_buffer, read_buffer,
                                MAX(write_length, read_length));
  if (!retval->IsUndefined()) {
    return retval;
  }

  FUNCTION_CHAIN;
}

Handle<Value> Spi::full_duplex_transfer(char *write, char *read, size_t length) {
  struct spi_ioc_transfer data = { 0 };

  //  .tx_buf = (unsigned long)write,
  //  .rx_buf = (unsigned long)read,
  //  .len = length,
  //  .delay_usecs = 0, // Unsure exactly what delay is for..
  //  .speed_hz = 0, // Use the max speed set when opened
  //  .bits_per_word = 0, // Use the bits per word set when opened
  //};

  int ret = ioctl(this->m_fd, SPI_IOC_MESSAGE(1), &data);
  if (ret == 1) {
    return ERROR("Unable to send SPI message");
  }

  return Undefined();
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
SPI_FUNC_IMPL(GetSetChipSelect) {
  FUNCTION_PREAMBLE;
  GETTER(1, Number::New(self->m_mode&(SPI_CS_HIGH|SPI_NO_CS)));
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

SPI_FUNC_IMPL(GetSetBitsPerWord) {
  FUNCTION_PREAMBLE;
  GETTER(1, Number::New(self->m_bits_per_word));
  REQ_INT_ARG_GT(0, BitsPerWord, in_value, 0);
  ASSERT_NOT_OPEN;

  // TODO: Bounds checking?  Need to look up what the max value is
  self->m_bits_per_word = in_value;

  FUNCTION_CHAIN;
}

SPI_FUNC_IMPL(GetSetMaxSpeed) {
  FUNCTION_PREAMBLE;
  GETTER(1, Number::New(self->m_max_speed));
  REQ_INT_ARG_GT(0, MaxSpeed, in_value, 0);
  ASSERT_NOT_OPEN;

  // TODO: Bounds Checking? Need to look up what the max value is
  self->m_max_speed = in_value;

  FUNCTION_CHAIN;
}
SPI_FUNC_IMPL(GetSet3Wire) {
  FUNCTION_PREAMBLE;
  GETTER(1, Boolean::New((self->m_mode&SPI_3WIRE) > 0));
  REQ_BOOL_ARG(0, in_value);

  if (in_value) {
    self->m_mode |= SPI_3WIRE;
  } else {
    self->m_mode &= ~SPI_3WIRE;
  }
  FUNCTION_CHAIN;
}

SPI_FUNC_BOOLEAN_TOGGLE_IMPL(GetSetLoop, SPI_LOOP);
SPI_FUNC_BOOLEAN_TOGGLE_IMPL(GetSetBitOrder, SPI_LSB_FIRST);
