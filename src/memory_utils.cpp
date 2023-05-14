#include "fst/memory_utils.h"

FST_BEGIN_NAMESPACE

    FST_NODISCARD size_t mem_cache_size() noexcept
    {
        return 64;
    }

    FST_NODISCARD size_t mem_page_size() noexcept
    {
        return 4096;
    }

FST_END_NAMESPACE

//
//#pragma once
//#include "fst/common.h"
//
//// clang-format off
//#if __FST_UNISTD__
//  #include <unistd.h>
//#endif // __FST_UNISTD__
//
//#if __FST_MACOS__
//  #include <sys/sysctl.h>
//#elif __FST_WINDOWS__
//  #include <stdlib.h>
//  #include <windows.h>
//  #include <sysinfoapi.h>
//#elif __FST_LINUX__
//  #include <stdio.h>
//#endif
//// clang-format on
//
//namespace fst::memory {
//namespace detail {
////
//// get_page_size.
////
//#if __FST_UNISTD__
//inline std::size_t get_page_size() {
//  long pagesize = sysconf(_SC_PAGE_SIZE);
//  return pagesize >= 0 ? (std::size_t)pagesize : 0;
//}
//
//#elif __FST_WINDOWS__
//  inline std::size_t get_page_size() {
//    SYSTEM_INFO sys_info;
//    GetSystemInfo(&sys_info);
//    return sys_info.dwPageSize >= 0 ? (std::size_t)sys_info.dwPageSize : 0;
//  }
//
//#else
//inline std::size_t get_page_size() {
//  return 0;
//}
//#endif
//
////
//// get_cache_size.
////
//// https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size
//
//#if __FST_MACOS__
//inline std::size_t get_cache_size() {
//  size_t line_size = 0;
//  size_t sizeof_line_size = sizeof(line_size);
//  sysctlbyname("hw.cachelinesize", &line_size, &sizeof_line_size, 0, 0);
//  return line_size;
//}
//
//#elif __FST_WINDOWS__
//  inline std::size_t get_cache_size() {
//    size_t line_size = 0;
//    DWORD buffer_size = 0;
//    DWORD i = 0;
//    SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;
//
//    GetLogicalProcessorInformation(0, &buffer_size);
//    buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
//    GetLogicalProcessorInformation(&buffer[0], &buffer_size);
//
//    for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
//        if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
//            line_size = buffer[i].Cache.LineSize;
//            break;
//        }
//    }
//
//    free(buffer);
//    return line_size;
//  }
//
//#elif __FST_LINUX__
//inline std::size_t get_cache_size() {
//  FILE* p = 0;
//  p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r");
//  unsigned int i = 0;
//  if (p) {
//    fscanf(p, "%d", &i);
//    fclose(p);
//  }
//  return i;
//}
//
//#else
//#warning "Unsupported platform."
//  inline std::size_t get_cache_size() { return 0; }
//#endif
//} // namespace detail.
//
//inline std::size_t get_page_size() {
//  static std::size_t size = detail::get_page_size();
//  return size;
//}
//
//inline std::size_t get_cache_size() {
//  static std::size_t size = detail::get_cache_size();
//  return size;
//}
//} // namespace fst::memory
