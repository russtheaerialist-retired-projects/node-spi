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

#pragma once

#include <v8.h>
#include <node.h>
#include <node_object_wrap.h>

using namespace v8;
using namespace node;

#define SPI_FUNC(NAME) static void NAME (const FunctionCallbackInfo<Value>& args)
#define SPI_FUNC_IMPL(NAME) void Spi::NAME (const FunctionCallbackInfo<Value>& args)
#define SPI_FUNC_EMPTY(NAME) void Spi::NAME (const FunctionCallbackInfo<Value>& args) { \
    args.GetReturnValue().Set(false);  \
}

class Spi : public ObjectWrap {
    public:
        static Persistent<Function> constructor;
        static void Initialize(Handle<Object> target);

    private:
        Spi() : m_fd(-1),
	        m_mode(0),
	        m_max_speed(1000000),  // default speed in Hz () 1MHz
	        m_delay(0),            // expose delay to options
	        m_bits_per_word(8) { } // default bits per word


          ~Spi() { } // Probably close fd if it's open

        SPI_FUNC(New);
        SPI_FUNC(Open);
        SPI_FUNC(Close);
        SPI_FUNC(Transfer);
        SPI_FUNC(GetSetMode);
        SPI_FUNC(GetSetChipSelect);
        SPI_FUNC(GetSetMaxSpeed);
        SPI_FUNC(GetSet3Wire);
        SPI_FUNC(GetSetDelay);
        SPI_FUNC(GetSetLoop);
        SPI_FUNC(GetSetBitOrder);
        SPI_FUNC(GetSetBitsPerWord);

        void full_duplex_transfer(Isolate* isolate, const FunctionCallbackInfo<Value> &args, char *write, char *read, size_t length, uint32_t speed, uint16_t delay, uint8_t bits);
        bool require_arguments(Isolate* isolate, const FunctionCallbackInfo<Value>& args, int count);
        bool get_argument(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int offset, int& value);
        bool get_argument(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int offset, bool& value);
        bool get_argument_greater_than(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int offset, int target, int& value);
        bool get_if_no_args(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int offset, unsigned int value);
        bool get_if_no_args(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int offset, bool value);

        void get_set_mode_toggle(Isolate *isolate, const FunctionCallbackInfo<Value>& args, int mask);

        int m_fd;
        uint32_t m_mode;
        uint32_t m_max_speed;
        uint16_t m_delay;
        uint8_t m_bits_per_word;
};

#define EXCEPTION(X) isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, X)))

#define FUNCTION_PREAMBLE                          \
             Isolate* isolate = args.GetIsolate(); \
             HandleScope scope(isolate);           \
             Spi* self = ObjectWrap::Unwrap<Spi>(args.This())

#define FUNCTION_CHAIN args.GetReturnValue().Set(args.This())

#define ASSERT_OPEN if (self->m_fd == -1) { EXCEPTION("Device not opened"); return; } 
#define ASSERT_NOT_OPEN if (self->m_fd != -1) { EXCEPTION("Cannot be called once device is opened"); return; }
#define ONLY_IF_OPEN if (self->m_fd == -1) { FUNCTION_CHAIN; return; }

#define REQ_INT_ARG_GT(I, NAME, VAR, VAL)                                      \
  REQ_INT_ARG(I, VAR);                                                         \
  if (VAR <= VAL)                                                              \
    return ThrowException(Exception::TypeError(                                \
       String::NewFromUtf8(isolate, #NAME " must be greater than " #VAL )));

#define SPI_FUNC_BOOLEAN_TOGGLE_IMPL(NAME, ARGUMENT)                           \
SPI_FUNC_IMPL(NAME) {                                                          \
  FUNCTION_PREAMBLE;                                                           \
  self->get_set_mode_toggle(isolate, args, ARGUMENT);                          \
}

#define SET_IOCTL_VALUE(FD, CTRL, VALUE)                                       \
  retval = ioctl(FD, CTRL, &(VALUE));                                          \
  if (retval == -1) {                                                          \
    EXCEPTION("Unable to set " #CTRL);                                         \
    return;                                                                    \
  }

#define MAX(a,b) (a>b ? a:b)
