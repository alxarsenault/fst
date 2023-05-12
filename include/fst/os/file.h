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

#include "fst/os/common.h"
#include "fst/os/path.h"
#include "fst/status_code.h"

FST_BEGIN_SUB_NAMESPACE(os)

    class file
    {
      public:
        struct native;

        enum class flags {
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

        file() noexcept = default;
        ~file() noexcept;

        __fst::status open(const __fst::os::path& fpath, flags flgs) noexcept;
        __fst::status close() noexcept;

      private:
        native* _native = nullptr;
    };

    FST_DECLARE_ENUM_CLASS_OPERATORS(file::flags);
FST_END_SUB_NAMESPACE