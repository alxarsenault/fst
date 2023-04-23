/////
/////
/////
//#pragma once
//
//#include "fst/common.h"
//
//#include "fst/math.h"
//#include "fst/memory.h"
//#include "fst/traits.h"
//#include "fst/utility.h"
//
//FST_BEGIN_NAMESPACE
//
//    ///
//    template <typename _T, size_t _Size, class _MemoryZone, class _MemoryCategory>
//    class buffer_base : private _MemoryZone
//    {
//      public:
//        static_assert(_FST::is_power_of_two(_Size), "_Size must be a power of two");
//
//        buffer_base() noexcept { _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), alignment(), _MemoryCategory::id()); }
//
//        inline buffer_base(const buffer_base& buf) noexcept
//            : _MemoryZone((const _MemoryZone&) buf)
//        {
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), alignment(), _MemoryCategory::id());
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory>
//        inline buffer_base(const buffer_base<_T, _OtherSize, _OtherMemoryZone, _OtherMemoryCategory>& buf) noexcept
//        {
//            fst_assert(buf.size() == size(), "wrong size");
//
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), alignment(), _MemoryCategory::id());
//
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//        }
//
//        inline buffer_base(_MemoryZone _zone) noexcept
//            : _MemoryZone(_zone)
//        {
//            // fst_assert(__buffer_size % /*_Size*/ == 0, "buffer_size must be a
//            // multiple of _Size");
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), alignment(), _MemoryCategory::id());
//        }
//
//        inline buffer_base(buffer_base&& d) noexcept
//            : _data(d._data)
//        {
//            d._data = nullptr;
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory>
//        inline buffer_base(buffer_base<_T, _OtherSize, _OtherMemoryZone, _OtherMemoryCategory>&& buf) noexcept
//        {
//            if (!buf._data)
//            {
//                _data = nullptr;
//                return;
//            }
//
//            if constexpr (_FST::is_same_v<_MemoryZone, _OtherMemoryZone> && !_FST::is_same_v<_MemoryCategory, _OtherMemoryCategory>)
//            {
//                _MemoryZone::template move_allocation<_OtherMemoryCategory, _MemoryCategory>(buf._data);
//                _data = buf._data;
//                buf._data = nullptr;
//            }
//            else
//            {
//                _data = (_T*) _MemoryZone::template aligned_allocate<_MemoryCategory>(buf.size() * sizeof(_T), alignment());
//                if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//
//                buf.reset();
//            }
//        }
//
//        inline ~buffer_base() noexcept { reset(); }
//
//        inline buffer_base& operator=(const buffer_base& buf) noexcept
//        {
//            reset();
//
//            if (!buf._data) { return *this; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), alignment(), _MemoryCategory::id());
//
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//
//            return *this;
//        }
//
//        inline buffer_base& operator=(buffer_base&& d) noexcept
//        {
//            reset();
//            _data = d._data;
//            d._data = nullptr;
//            return *this;
//        }
//
//        inline void reset() noexcept
//        {
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//            }
//        }
//
//        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t size() noexcept { return _Size; }
//
//        FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return alignof(_T); }
//
//        _T* _data = nullptr;
//    };
//
//    ///
//    template <typename _T, class _MemoryZone, class _MemoryCategory>
//    class buffer_base<_T, _FST::dynamic_size, _MemoryZone, _MemoryCategory> : public _MemoryZone
//    {
//      public:
//        buffer_base() noexcept = default;
//
//        inline buffer_base(const buffer_base& buf) noexcept
//            : _MemoryZone((const _MemoryZone&) buf)
//        {
//            _data = nullptr;
//            _size = 0;
//            if (!buf._data) { return; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), alignment(), _MemoryCategory::id());
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory>
//        inline buffer_base(const buffer_base<_T, _OtherSize, _OtherMemoryZone, _OtherMemoryCategory>& buf) noexcept
//            : buffer_base()
//        {
//            if (!buf._data) { return; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), alignment(), _MemoryCategory::id());
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//        }
//
//        inline buffer_base(size_t __size, _MemoryZone _zone) noexcept
//            : _MemoryZone(_zone)
//        {
//            // fst_assert(__buffer_size % /*_Size*/ == 0, "buffer_size must be a
//            // multiple of _Size");
//            _data = (_T*) _MemoryZone::aligned_allocate(__size * sizeof(_T), alignment(), _MemoryCategory::id());
//
//            if (_data) { _size = __size; }
//        }
//
//        inline buffer_base(size_t __size) noexcept
//            : buffer_base(__size, _MemoryZone{})
//        {}
//
//        inline buffer_base(buffer_base&& d) noexcept
//            : _data(d._data)
//            , _size(d._size)
//        {
//            d._data = nullptr;
//            d._size = 0;
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory>
//        inline buffer_base(buffer_base<_T, _OtherSize, _OtherMemoryZone, _OtherMemoryCategory>&& buf) noexcept
//            : buffer_base()
//        {
//            if (!buf._data) { return; }
//
//            if constexpr (_FST::is_same_v<_MemoryZone, _OtherMemoryZone>)
//            {
//                _MemoryZone::template move_allocation<_OtherMemoryCategory, _MemoryCategory>(buf._data);
//                _data = buf._data;
//                _size = buf._size;
//                buf._data = nullptr;
//                buf._size = 0;
//            }
//            else
//            {
//                _data = (_T*) _MemoryZone::template aligned_allocate<_MemoryCategory>(buf.size() * sizeof(_T), alignment());
//                if (_data)
//                {
//                    _size = buf.size();
//                    _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//                }
//
//                buf.reset();
//            }
//        }
//
//        inline ~buffer_base() noexcept { reset(); }
//
//        inline buffer_base& operator=(const buffer_base& buf) noexcept
//        {
//            reset();
//
//            if (!buf._data) { return *this; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), alignment(), _MemoryCategory::id());
//
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//
//            return *this;
//        }
//
//        inline buffer_base& operator=(buffer_base&& d) noexcept
//        {
//            reset();
//            _data = d._data;
//            _size = d._size;
//            d._data = nullptr;
//            d._size = 0;
//            return *this;
//        }
//
//        inline void reset() noexcept
//        {
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//            }
//
//            _size = 0;
//        }
//
//        inline void reset(size_t new_size) noexcept
//        {
//            if (_size == new_size) return;
//
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//                _size = 0;
//            }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(new_size * sizeof(_T), alignment(), _MemoryCategory::id());
//
//            if (_data) { _size = new_size; }
//        }
//
//        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
//
//        FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return alignof(_T); }
//
//        _T* _data = nullptr;
//        size_t _size = 0;
//    };
//
//    ///
//    template <typename _T, size_t _Size, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
//    class aligned_buffer_base : public _MemoryZone
//    {
//      public:
//        static_assert(_FST::is_power_of_two(_Size), "_Size must be a power of two");
//        static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");
//
//        aligned_buffer_base() noexcept { _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), _Alignment, _MemoryCategory::id()); }
//
//        inline aligned_buffer_base(const aligned_buffer_base& buf) noexcept
//            : _MemoryZone((const _MemoryZone&) buf)
//        {
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
//        inline aligned_buffer_base(const aligned_buffer_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>& buf) noexcept
//        {
//            fst_assert(buf.size() == size(), "wrong size");
//
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//        }
//
//        inline aligned_buffer_base(_MemoryZone _zone) noexcept
//            : _MemoryZone(_zone)
//        {
//            // fst_assert(__buffer_size % /*_Size*/ == 0, "buffer_size must be a
//            // multiple of _Size");
//            _data = (_T*) _MemoryZone::aligned_allocate(size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//        }
//
//        inline aligned_buffer_base(aligned_buffer_base&& d) noexcept
//            : _data(d._data)
//        {
//            d._data = nullptr;
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
//        inline aligned_buffer_base(aligned_buffer_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>&& buf) noexcept
//        {
//            if (!buf._data)
//            {
//                _data = nullptr;
//                return;
//            }
//
//            if constexpr (_FST::is_same_v<_MemoryZone, _OtherMemoryZone> && !_FST::is_same_v<_MemoryCategory, _OtherMemoryCategory>)
//            {
//                _MemoryZone::template move_allocation<_OtherMemoryCategory, _MemoryCategory>(buf._data);
//                _data = buf._data;
//                buf._data = nullptr;
//            }
//            else
//            {
//                _data = (_T*) _MemoryZone::template aligned_allocate<_MemoryCategory>(buf.size() * sizeof(_T), _Alignment);
//                if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//
//                buf.reset();
//            }
//        }
//
//        inline ~aligned_buffer_base() noexcept { reset(); }
//
//        inline aligned_buffer_base& operator=(const aligned_buffer_base& buf) noexcept
//        {
//            reset();
//
//            if (!buf._data) { return *this; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//
//            if (_data) { _FST::memcpy(_data, buf._data, size() * sizeof(_T)); }
//
//            return *this;
//        }
//
//        inline aligned_buffer_base& operator=(aligned_buffer_base&& d) noexcept
//        {
//            reset();
//            _data = d._data;
//            d._data = nullptr;
//            return *this;
//        }
//
//        inline void reset() noexcept
//        {
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//            }
//        }
//
//        FST_NODISCARD FST_ALWAYS_INLINE static constexpr size_t size() noexcept { return _Size; }
//
//        FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
//
//        _T* _data = nullptr;
//    };
//
//    ///
//    template <typename _T, size_t _Alignment, class _MemoryZone, class _MemoryCategory>
//    class aligned_buffer_base<_T, _FST::dynamic_size, _Alignment, _MemoryZone, _MemoryCategory> : private _MemoryZone
//    {
//      public:
//        static_assert(_FST::is_power_of_two(_Alignment), "_Alignment must be a power of two");
//
//        aligned_buffer_base() noexcept = default;
//
//        inline aligned_buffer_base(const aligned_buffer_base& buf) noexcept
//            : _MemoryZone((const _MemoryZone&) buf)
//        {
//            _data = nullptr;
//            _size = 0;
//            if (!buf._data) { return; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
//        inline aligned_buffer_base(const aligned_buffer_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>& buf) noexcept
//            : aligned_buffer_base()
//        {
//            if (!buf._data) { return; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//        }
//
//        inline aligned_buffer_base(size_t __size, _MemoryZone _zone) noexcept
//            : _MemoryZone(_zone)
//        {
//            // fst_assert(__buffer_size % /*_Size*/ == 0, "buffer_size must be a
//            // multiple of _Size");
//            _data = (_T*) _MemoryZone::aligned_allocate(__size * sizeof(_T), _Alignment, _MemoryCategory::id());
//
//            if (_data) { _size = __size; }
//        }
//
//        inline aligned_buffer_base(size_t __size) noexcept
//            : aligned_buffer_base(__size, _MemoryZone{})
//        {}
//
//        inline aligned_buffer_base(aligned_buffer_base&& d) noexcept
//            : _data(d._data)
//            , _size(d._size)
//        {
//            d._data = nullptr;
//            d._size = 0;
//        }
//
//        template <size_t _OtherSize, class _OtherMemoryZone, class _OtherMemoryCategory, size_t _OtherAlignment>
//        inline aligned_buffer_base(aligned_buffer_base<_T, _OtherSize, _OtherAlignment, _OtherMemoryZone, _OtherMemoryCategory>&& buf) noexcept
//            : aligned_buffer_base()
//        {
//            if (!buf._data) { return; }
//
//            if constexpr (_FST::is_same_v<_MemoryZone, _OtherMemoryZone>)
//            {
//                _MemoryZone::template move_allocation<_OtherMemoryCategory, _MemoryCategory>(buf._data);
//                _data = buf._data;
//                _size = buf._size;
//                buf._data = nullptr;
//                buf._size = 0;
//            }
//            else
//            {
//                _data = (_T*) _MemoryZone::template aligned_allocate<_MemoryCategory>(buf.size() * sizeof(_T), _Alignment);
//                if (_data)
//                {
//                    _size = buf.size();
//                    _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//                }
//
//                buf.reset();
//            }
//        }
//
//        inline ~aligned_buffer_base() noexcept { reset(); }
//
//        inline aligned_buffer_base& operator=(const aligned_buffer_base& buf) noexcept
//        {
//            reset();
//
//            if (!buf._data) { return *this; }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(buf.size() * sizeof(_T), _Alignment, _MemoryCategory::id());
//
//            if (_data)
//            {
//                _size = buf.size();
//                _FST::memcpy(_data, buf._data, _size * sizeof(_T));
//            }
//
//            return *this;
//        }
//
//        inline aligned_buffer_base& operator=(aligned_buffer_base&& d) noexcept
//        {
//            reset();
//            _data = d._data;
//            _size = d._size;
//            d._data = nullptr;
//            d._size = 0;
//            return *this;
//        }
//
//        inline void reset() noexcept
//        {
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//            }
//
//            _size = 0;
//        }
//
//        inline void reset(size_t new_size) noexcept
//        {
//            if (_size == new_size) return;
//
//            if (_data)
//            {
//                _MemoryZone::aligned_deallocate((void*) _data, _MemoryCategory::id());
//                _data = nullptr;
//                _size = 0;
//            }
//
//            _data = (_T*) _MemoryZone::aligned_allocate(new_size * sizeof(_T), _Alignment, _MemoryCategory::id());
//
//            if (_data) { _size = new_size; }
//        }
//
//        FST_NODISCARD FST_ALWAYS_INLINE constexpr size_t size() const noexcept { return _size; }
//
//        FST_NODISCARD static FST_ALWAYS_INLINE constexpr size_t alignment() noexcept { return _Alignment; }
//
//        _T* _data = nullptr;
//        size_t _size = 0;
//    };
//
//    ///
//    template <typename _T, size_t _Size, class _MemoryZone = _FST::default_memory_zone, class _MemoryCategory = _FST::default_memory_category>
//    class buffer : public contiguous_container_base<_T, buffer_base<_T, _Size, _MemoryZone, _MemoryCategory>>
//    {
//      public:
//        using base = contiguous_container_base<_T, buffer_base<_T, _Size, _MemoryZone, _MemoryCategory>>;
//        using base::base;
//        using value_type = typename base::value_type;
//        using size_type = typename base::size_type;
//        using difference_type = typename base::difference_type;
//        using pointer = typename base::pointer;
//        using const_pointer = typename base::const_pointer;
//        using reference = typename base::reference;
//        using const_reference = typename base::const_reference;
//        using iterator = typename base::iterator;
//        using const_iterator = typename base::const_iterator;
//
//        buffer() noexcept = default;
//        buffer(const buffer&) noexcept = default;
//        buffer(buffer&&) noexcept = default;
//
//        buffer& operator=(const buffer&) noexcept = default;
//        buffer& operator=(buffer&&) noexcept = default;
//    };
//
//    ///
//    template <typename _T, size_t _Size, size_t _Alignment, class _MemoryZone = _FST::simd_memory_zone, class _MemoryCategory = _FST::dsp_memory_category>
//    class aligned_buffer : public contiguous_container_base<_T, aligned_buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>>
//    {
//      public:
//        using base = contiguous_container_base<_T, aligned_buffer_base<_T, _Size, _Alignment, _MemoryZone, _MemoryCategory>>;
//
//        using base::base;
//        using value_type = typename base::value_type;
//        using size_type = typename base::size_type;
//        using difference_type = typename base::difference_type;
//        using pointer = typename base::pointer;
//        using const_pointer = typename base::const_pointer;
//        using reference = typename base::reference;
//        using const_reference = typename base::const_reference;
//        using iterator = typename base::iterator;
//        using const_iterator = typename base::const_iterator;
//
//        aligned_buffer() noexcept = default;
//        aligned_buffer(const aligned_buffer&) noexcept = default;
//        aligned_buffer(aligned_buffer&&) noexcept = default;
//
//        aligned_buffer& operator=(const aligned_buffer&) noexcept = default;
//        aligned_buffer& operator=(aligned_buffer&&) noexcept = default;
//    };
//
//FST_END_NAMESPACE
