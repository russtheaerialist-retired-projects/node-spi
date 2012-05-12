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

using namespace v8;
using namespace node;

#define SPI_FUNC(NAME) static Handle<Value> NAME (const Arguments& args)
#define SPI_FUNC_IMPL(NAME) Handle<Value> Spi::NAME (const Arguments& args)
#define SPI_FUNC_EMPTY(NAME) Handle<Value> Spi::NAME (const Arguments& args) { \
    HandleScope scope;                                                         \
    return scope.Close(Boolean::New(0));                                       \
}

class Spi : ObjectWrap {
    public:
        static Persistent<Function> constructor;
        static void Initialize(Handle<Object> target);

    private:
        Spi() : m_fd(-1), m_mode(0), m_bits_per_word(8),
                m_max_speed(1000000) { }
        ~Spi() { } // Probably close fd if it's open

        SPI_FUNC(New);
        SPI_FUNC(Open);
        SPI_FUNC(Close);
        SPI_FUNC(Transfer);
        SPI_FUNC(GetSetMode);
        SPI_FUNC(GetSetChipSelect);
        SPI_FUNC(GetSetMaxSpeed);
        SPI_FUNC(GetSet3Wire);
        SPI_FUNC(GetSetLoop);
        SPI_FUNC(GetSetBitOrder);
        SPI_FUNC(GetSetBitsPerWord);

        Handle<Value> full_duplex_transfer(char *write, char *read, size_t length);

        int m_fd;
        int m_mode;
        int m_bits_per_word;
        int m_max_speed;
};

#define ERROR(STR) ThrowException(Exception::Error(String::New(STR)))

#define FUNCTION_PREAMBLE HandleScope scope;                \
             Spi* self = ObjectWrap::Unwrap<Spi>(args.This())

#define FUNCTION_CHAIN return scope.Close(args.This())

#define ASSERT_OPEN if (self->m_fd == -1) return ThrowException( \
              Exception::Error(String::New(                 \
                  "Device not opened"                       \
              )))
#define ASSERT_NOT_OPEN if (self->m_fd != -1) return ThrowException( \
              Exception::Error(String::New(                          \
                  "Cannot be called once device is opened"           \
              )))

#define ONLY_IF_OPEN if (self->m_fd == -1) return scope.Close(args.This())

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return ThrowException(Exception::TypeError(                         \
                             String::New("Expected " #N "arguments")));

#define GETTER(N, VAR) if (args.Length() < (N))                         \
    return scope.Close(VAR)

#define REQ_INT_ARG(I, VAR)                                                    \
  int VAR;                                                                     \
  if (args.Length() <= I || !args[I]->IsInt32())                               \
    return ThrowException(Exception::TypeError(                                \
                          String::New("Argument " #I " must be an integer" )));\
  VAR = args[I]->Int32Value();

#define REQ_BOOL_ARG(I, VAR)                                                   \
  bool VAR;                                                                     \
  if (args.Length() <= I || !args[I]->IsBoolean())                             \
    return ThrowException(Exception::TypeError(                                \
                          String::New("Argument " #I " must be a boolean" ))); \
  VAR = args[I]->BooleanValue();

#define REQ_INT_ARG_GT(I, NAME, VAR, VAL)                                      \
  REQ_INT_ARG(I, VAR);                                                         \
  if (VAR <= VAL)                                                              \
    return ThrowException(Exception::TypeError(                                \
       String::New(#NAME " must be greater than " #VAL )));

#define SPI_FUNC_BOOLEAN_TOGGLE_IMPL(NAME, ARGUMENT)                           \
SPI_FUNC_IMPL(NAME) {                                                          \
  FUNCTION_PREAMBLE;                                                           \
  GETTER(1, Boolean::New((self->m_mode&ARGUMENT) > 0));                        \
  REQ_BOOL_ARG(0, in_value);                                                   \
  if (in_value) {                                                              \
    self->m_mode |= ARGUMENT;                                                  \
  } else {                                                                     \
    self->m_mode &= ~ARGUMENT;                                                 \
  }                                                                            \
  FUNCTION_CHAIN;                                                              \
}

#define SET_IOCTL_VALUE(FD, CTRL, VALUE)                                       \
  retval = ioctl(FD, CTRL, &(VALUE));                                          \
  if (retval == -1) return ThrowException(Exception::Error(String::New(        \
       "Unable to set " #CTRL)));

#define MAX(a,b) (a>b ? a:b)
