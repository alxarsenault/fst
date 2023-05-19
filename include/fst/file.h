//
// MIT License
//
// Copyright (c) 2023 Alexandre Arsenault
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

#include "fst/common.h"
#include "fst/status_code.h"
#include "fst/stream.h"

FST_BEGIN_NAMESPACE
    enum class open_mode {
        //
        read = 1,

        //
        write = 2,

        // Creates a new file, always.
        // If the specified file exists and is writable, the function overwrites the file, the function succeeds,
        // and last-error code is set to ERROR_ALREADY_EXISTS (183).
        // If the specified file does not exist and is a valid path, a new file is created, the function succeeds,
        // and the last-error code is set to zero.
        create_always = 4,

        // Creates a new file, only if it does not already exist.
        // If the specified file exists, the function fails and the last-error code is set to ERROR_FILE_EXISTS (80).
        // If the specified file does not exist and is a valid path to a writable location, a new file is created.
        create_new = 8,

        // Opens a file, always.
        // If the specified file exists, the function succeeds and the last-error code is set to ERROR_ALREADY_EXISTS (183).
        // If the specified file does not exist and is a valid path to a writable location, the function creates a
        // file and the last-error code is set to zero.
        open_always = 16,

        // Opens a file or device, only if it exists.
        // If the specified file or device does not exist, the function fails and the last-error code is set to
        // ERROR_FILE_NOT_FOUND (2).
        open_existing = 32,

        // Opens a file and truncates it so that its size is zero bytes, only if it exists.
        // If the specified file does not exist, the function fails and the last-error code is set to ERROR_FILE_NOT_FOUND (2).
        // The calling process must open the file with the GENERIC_WRITE bit set as part of the dwDesiredAccess parameter
        truncate_existing = 64,
    };

    FST_DECLARE_ENUM_CLASS_OPERATORS(open_mode);

    ///
    class file
    {
      public:
        file() noexcept = default;
        ~file() noexcept;

        FST_NODISCARD __fst::status open(const char* filepath, open_mode flags) noexcept;
        __fst::status close() noexcept;

        FST_NODISCARD bool is_open() const noexcept;

        FST_NODISCARD size_t write(const void* buffer, size_t buffer_size) const noexcept;
        FST_NODISCARD size_t read(void* buffer, size_t buffer_size) const noexcept;

        FST_NODISCARD size_t size() const noexcept;

      private:
        struct native;
        native* _native = nullptr;
    };

    inline __fst::status write_to_file(const char* filepath, open_mode flags, const void* buffer, size_t buffer_size) noexcept
    {

        __fst::file fout;
        __fst::status st = fout.open(filepath, flags);
        if (!st) { return st; }

        if (const size_t sz = fout.write(buffer, buffer_size); sz != buffer_size)
        {
            fout.close();
            return __fst::status_code::bad_file_descriptor;
        }

        return fout.close();
    }
    ///
    inline __fst::output_stream<char> file_stream(__fst::file & file_ref) noexcept
    {
        return __fst::output_stream<char>{ &file_ref,
            [](void* data, const char* str, size_t size, stream_modifier) noexcept -> size_t
            {
                __fst::file* file_ptr = (__fst::file*) data;
                size_t sz = file_ptr->write((const void*) str, size);
                fst_assert(sz == size, "error");
                //__fst::unused(sz);
                return sz;
            } };
    }

FST_END_NAMESPACE
