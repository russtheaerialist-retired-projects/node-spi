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
        Spi() : m_fd(-1), m_mode(0) { }
        ~Spi() { } // Probably close fd if it's open

        SPI_FUNC(New);
        SPI_FUNC(Open);
        SPI_FUNC(Close);
        SPI_FUNC(Transfer);
        SPI_FUNC(GetSetMode);
        SPI_FUNC(SetChipSelect);
        SPI_FUNC(SetMaxSpeed);
        SPI_FUNC(Set3Wire);
        SPI_FUNC(SetLoop);
        SPI_FUNC(SetLSB);
        SPI_FUNC(SetWordSize);

        int m_fd;
        int m_mode;
};

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
