namespace detail
{
    template <class _T, size_t _Size, size_t _Alignment, class _DataBase, class _Derived>
    class container_base : public _DataBase
    {
      public:
        using data_base_type = _DataBase;
        using derived_type = _Derived;

        using value_type = typename data_base_type::value_type;
        using size_type = typename data_base_type::size_type;
        using difference_type = typename data_base_type::difference_type;
        using pointer = typename data_base_type::pointer;
        using const_pointer = typename data_base_type::const_pointer;
        using reference = typename data_base_type::reference;
        using const_reference = typename data_base_type::const_reference;
        using iterator = typename data_base_type::iterator;
        using const_iterator = typename data_base_type::const_iterator;

        using vector_type = __fst::simd::vector_type_t<value_type>;
        using vector_op = __fst::simd::vector_op<vector_type, value_type>;

        static constexpr size_t vector_alignment = alignof(vector_type);
        static constexpr size_t vector_size = sizeof(vector_type) / sizeof(value_type);

        static_assert(_Alignment >= vector_alignment, "Wrong alignment");

      private:
        using op = vector_op;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        static constexpr bool is_valid_range = __fst::is_convertible_v<_TT*, _T*> && !__fst::is_constructible_v<_SBase, _TBase>
                                               && !__fst::is_same_v<container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>, container_base>
                                               && __fst::is_same_v<_SBase, data_base_type> && __fst::is_same_v<_SDerived, _Derived>;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        using enable_if_array_t
            = __fst::enable_if_t<is_valid_range<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> && __fst::simd::is_array<_SDerived>::value, int>;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        using enable_if_fixed_range_t
            = __fst::enable_if_t<is_valid_range<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> && __fst::simd::is_fixed_range<_SDerived>::value, int>;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        using enable_if_range_t
            = __fst::enable_if_t<is_valid_range<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> && __fst::simd::is_range<_SDerived>::value, int>;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        using enable_if_buffer_t
            = __fst::enable_if_t<is_valid_range<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> && __fst::simd::is_buffer<_SDerived>::value, int>;

        template <class _TT, size_t _TSize, size_t _TAlign, class _TBase, class _TDerived, class _SBase, class _SDerived>
        using enable_if_fixed_buffer_t
            = __fst::enable_if_t<is_valid_range<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> && __fst::simd::is_fixed_buffer<_SDerived>::value, int>;

      public:
        using data_base_type::alignment;
        using data_base_type::data;
        using data_base_type::data_base_type;
        using data_base_type::size;

        constexpr container_base() noexcept = default;

        //
        // Copy constructors.
        //

        inline constexpr container_base(const container_base&) noexcept = default;

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase,
            __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*> && __fst::is_constructible_v<_SBase, _TBase>, int> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
            : data_base_type((const _TBase&) rb)
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_array_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
            : data_base_type((value_type*) &rb._data[0])
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
            : data_base_type((value_type*) &rb._data[0], rb.size())
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);
        }
        /*template<class _TT, class _TBase, class _TDerived, size_t _TSize, size_t
    _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
    enable_if_range_t<_TT, _TBase, _TDerived, _SBase, _SDerived> = 0>
    FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize,
    _TAlign, _TBase, _TDerived>& rb) noexcept :
    data_base_type((value_type*)&rb._data[0], rb.size())
    {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);
    }*/

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
            : data_base_type(rb.size())
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            assign(rb);
        }
        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            assign(rb);
        }

        //
        // Move constructors.
        //

        inline constexpr container_base(container_base&&) noexcept = default;

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase,
            __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*> && __fst::is_constructible_v<_SBase, _TBase>, int> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
            : data_base_type(__fst::move((_TBase&) rb))
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_array_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&&) noexcept
        {
            static_assert(__fst::always_false<_Derived>, "Can't assign an rvalue into a range");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&&) noexcept
        {
            static_assert(__fst::always_false<_Derived>, "Can't assign an rvalue into a range");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
            : data_base_type(rb.size())
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            assign(rb);
        }

        //
        // Copy assignments.
        //

        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base&) noexcept = default;

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase,
            __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*> && __fst::is_constructible_v<_SBase, _TBase>, int> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            (data_base_type&) (*this) = (const _TBase&) rb;
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_array_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            this->_data = (value_type*) &rb._data[0];
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            this->_data = (value_type*) &rb._data[0];
            this->_size = rb.size();
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            this->reset(rb.size());
            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            // this->reset(rb.size());
            assign(rb);
        }

        //
        // Move assignments.
        //

        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base&&) noexcept = default;

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase,
            __fst::enable_if_t<__fst::is_convertible_v<_TT*, _T*> && __fst::is_constructible_v<_SBase, _TBase>, int> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            (data_base_type&) (*this) = __fst::move((_TBase&) rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_array_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");
            validate_size(rb);

            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&&) noexcept
        {
            static_assert(__fst::always_false<_Derived>, "Can't assign an rvalue into a range");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_range_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&&) noexcept
        {
            static_assert(__fst::always_false<_Derived>, "Can't assign an rvalue into a range");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            this->reset(rb.size());
            assign(rb);
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SBase = _DataBase, class _SDerived = _Derived,
            enable_if_fixed_buffer_t<_TT, _TSize, _TAlign, _TBase, _TDerived, _SBase, _SDerived> = 0>
        FST_ALWAYS_INLINE constexpr container_base& operator=(container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>&& rb) noexcept
        {
            static_assert(_TAlign >= vector_alignment, "wrong alignment");

            // this->reset(rb.size());
            assign(rb);
        }
        //
        //
        //

        FST_NODISCARD FST_ALWAYS_INLINE vector_type vdata(size_type index) const noexcept { return op::load(data(index * vector_size)); }

        template <size_t I>
        FST_NODISCARD FST_ALWAYS_INLINE vector_type vdata() const noexcept
        {
            constexpr size_type index = I * vector_size;
            return op::load(data(index));
        }

//
//
//
#define FST_SIMD_LOOP(...)                                                      \
    if constexpr (_Size == __fst::dynamic_size)                                  \
    {                                                                           \
        for (size_type I = 0; I < size(); I += vector_size)                     \
        {                                                                       \
            __VA_ARGS__;                                                        \
        }                                                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        static constexpr size_t loop_size = size() / vector_size;               \
        __fst::unroll<loop_size, vector_size>([&]<size_t I>() { __VA_ARGS__; }); \
    }                                                                           \
    (void) 0

#define FST_SIMD_SEQ_LOOP(__InSize, __InAlign, __InDerived, __InName, OP)                                             \
    if constexpr (_Size == __InSize) { OP(__InName); }                                                                \
    else if constexpr (__fst::simd::has_fixed_size<_Derived>::value && __fst::simd::has_fixed_size<__InDerived>::value) \
    {                                                                                                                 \
        static_assert(__InName.size() <= size() && (size() & (__InName.size() - 1)) == 0, "Wrong size");              \
        constexpr size_type count = size() / __InName.size();                                                         \
        constexpr size_type salign = __fst::minimum(_Alignment, __InAlign);                                            \
        for (size_type i = 0, k = 0; i < count; i++, k += __InName.size())                                            \
        {                                                                                                             \
            __fst::simd::detail::range_base<_T, __InSize, salign>(data(k)).OP(__InName);                               \
        }                                                                                                             \
    }                                                                                                                 \
    else                                                                                                              \
    {                                                                                                                 \
        fst_assert(__InName.size() <= size() && (size() & (__InName.size() - 1)) == 0, "Wrong size");                 \
        const size_type count = size() / __InName.size();                                                             \
        constexpr size_type salign = __fst::minimum(_Alignment, __InAlign);                                            \
        for (size_type i = 0, k = 0; i < count; i++, k += __InName.size())                                            \
        {                                                                                                             \
            __fst::simd::detail::range_base<_T, __fst::dynamic_size, salign>(data(k), __InName.size()).OP(__InName);    \
        }                                                                                                             \
    }                                                                                                                 \
    (void) 0

        // data[i] = 0
        FST_ALWAYS_INLINE derived_type& clear() noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load_zero();
            FST_SIMD_LOOP(op::store(data(I), v));
            return (derived_type&) *this;
        }

        // data[i] = value
        FST_ALWAYS_INLINE derived_type& fill(value_type value) noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load(value);
            FST_SIMD_LOOP(op::store(data(I), v));
            return (derived_type&) *this;
        }

        // data[i] = (start_value + i) * delta
        FST_ALWAYS_INLINE derived_type& fill_increment(value_type delta, value_type start_value = 0) noexcept FST_UNIT_TESTED
        {
            for (size_t i = 0; i < size(); i++)
            {
                this->operator[](i) = (start_value + (value_type) i) * delta;
            }

            return (derived_type&) *this;
        }

        //
        //
        //

        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& assign(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::load(a.data(I))));
            return (derived_type&) *this;
        }

        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_assign(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, assign);
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += value
        FST_ALWAYS_INLINE derived_type& operator+=(value_type value) noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load(value);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), v)));
            return (derived_type&) *this;
        }

        // data[i] -= value
        FST_ALWAYS_INLINE derived_type& operator-=(value_type value) noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load(value);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), v)));
            return (derived_type&) *this;
        }

        // data[i] *= value
        FST_ALWAYS_INLINE derived_type& operator*=(value_type value) noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load(value);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), v)));
            return (derived_type&) *this;
        }

        // data[i] /= value
        FST_ALWAYS_INLINE derived_type& operator/=(value_type value) noexcept FST_UNIT_TESTED
        {
            const vector_type v = op::load(value);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), v)));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& operator+=(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& operator-=(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& operator*=(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& operator/=(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            return *this += a;
        }

        // data[i] = a
        FST_ALWAYS_INLINE derived_type& add(value_type a) noexcept FST_UNIT_TESTED { return *this += a; }

        // data[i] -= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            return *this -= a;
        }

        // data[i] -= a
        FST_ALWAYS_INLINE derived_type& sub(value_type a) noexcept FST_UNIT_TESTED { return *this -= a; }

        // data[i] = a[i] - data[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& rsub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(a.data(I)), op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a - data[i]
        FST_ALWAYS_INLINE derived_type& rsub(value_type a) noexcept FST_UNIT_TESTED
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::sub(avec, op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            return *this *= a;
        }

        // data[i] *= a
        FST_ALWAYS_INLINE derived_type& mul(value_type a) noexcept FST_UNIT_TESTED { return *this *= a; }

        // data[i] /= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            return *this /= a;
        }

        // data[i] /= a
        FST_ALWAYS_INLINE derived_type& div(value_type a) noexcept FST_UNIT_TESTED { return *this /= a; }

        // data[i] = a[i] / data[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& rdiv(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(a.data(I)), op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a / data[i]
        FST_ALWAYS_INLINE derived_type& rdiv(value_type a) noexcept FST_UNIT_TESTED
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::div(avec, op::load(data(I)))));
            return (derived_type&) *this;
        }

        //
        //
        //
        // data[i] += a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_add(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, add);
            return (derived_type&) *this;
        }

        // data[i] -= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, sub);
            return (derived_type&) *this;
        }

        // data[i] *= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, mul);
            return (derived_type&) *this;
        }

        // data[i] /= a[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, div);
            return (derived_type&) *this;
        }

        // data[i] = a[i] - data[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& seq_rsub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept FST_UNIT_TESTED
        {
            FST_SIMD_SEQ_LOOP(_ASize, _AAlign, _ADerived, a, rsub);
            return (derived_type&) *this;
        }
        //
        //
        //

        // data[i] += a[i] + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::add(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] += a[i] + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::add(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] += a + b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_sum(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::add(avec, op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::add(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::add(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] = (a[i] + b[i]) - data[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& rsub_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::sub(op::add(op::load(a.data(I)), op::load(b.data(I))), op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (a[i] + b) - data[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& rsub_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::add(op::load(a.data(I)), bvec), op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (a + b[i]) - data[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& rsub_sum(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::add(avec, op::load(b.data(I))), op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::add(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::add(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& div_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::add(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& div_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::add(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += a[i] - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::sub(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] += a[i] - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::sub(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            static_assert(_BAlign >= vector_alignment, "wrong alignment");
            validate_size(b);

            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::sub(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::sub(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::sub(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            static_assert(_AAlign >= vector_alignment, "wrong alignment");
            validate_size(a);

            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::sub(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& div_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::sub(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& div_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::sub(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += a[i] * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::mul(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] += a[i] * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::mul(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::mul(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::mul(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::mul(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::mul(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& div_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::mul(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& div_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::mul(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] += a[i] / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::div(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] += a[i] / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(data(I)), op::div(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::div(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] -= a[i] / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(data(I)), op::div(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::div(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] *= a[i] / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(data(I)), op::div(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& div_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::div(op::load(a.data(I)), op::load(b.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] /= a[i] / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& div_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(data(I)), op::div(op::load(a.data(I)), bvec))));
            return (derived_type&) *this;
        }

        //
        //
        //

        // data[i] = a[i] + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& assign_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(a.data(I)), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a[i] + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& assign_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(a.data(I)), bvec)));
            return (derived_type&) *this;
        }

        // data[i] = a[i] - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& assign_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(a.data(I)), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a[i] - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& assign_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::load(a.data(I)), bvec)));
            return (derived_type&) *this;
        }

        // data[i] = a[i] * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& assign_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(a.data(I)), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a[i] * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& assign_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(a.data(I)), bvec)));
            return (derived_type&) *this;
        }

        // data[i] = a[i] / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& assign_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(a.data(I)), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = a[i] / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& assign_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::load(a.data(I)), bvec)));
            return (derived_type&) *this;
        }

        //
        // Trigo
        //

        // data[i] = sin(data[i])
        FST_ALWAYS_INLINE derived_type& sin() noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sin(op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = sin(a[i])
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sin(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sin(op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = cos(data[i])
        FST_ALWAYS_INLINE derived_type& cos() noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::cos(op::load(data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = tan(data[i])
        FST_ALWAYS_INLINE derived_type& tan() noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::tan(op::load(data(I)))));
            return (derived_type&) *this;
        }

        //
        // add_then_mul
        //

        // data[i] = (data[i] + a[i]) * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_then_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::add(op::load(a.data(I)), op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a[i]) * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_then_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::add(op::load(a.data(I)), op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a) * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_then_mul(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::add(avec, op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a) * b
        FST_ALWAYS_INLINE derived_type& add_then_mul(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::add(avec, op::load(data(I))))));
            return (derived_type&) *this;
        }

        //
        // sub_then_mul
        //

        // data[i] = (data[i] - a[i]) * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_then_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::sub(op::load(data(I)), op::load(a.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] - a[i]) * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sub_then_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::sub(op::load(data(I)), op::load(a.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] - a) * b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& sub_then_mul(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::sub(op::load(data(I)), avec))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] - a) * b
        FST_ALWAYS_INLINE derived_type& sub_then_mul(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::sub(op::load(data(I)), avec))));
            return (derived_type&) *this;
        }

        //
        // sub_then_mul
        //

        // data[i] = (a[i] - data[i]) * b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& rsub_then_mul(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::sub(op::load(a.data(I)), op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (a[i] - data[i]) * b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& rsub_then_mul(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::sub(op::load(a.data(I)), op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (a - data[i]) * b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& rsub_then_mul(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::mul(op::load(b.data(I)), op::sub(avec, op::load(data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (a - data[i]) * b
        FST_ALWAYS_INLINE derived_type& rsub_then_mul(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::mul(bvec, op::sub(avec, op::load(data(I))))));
            return (derived_type&) *this;
        }

        //
        // mul_then_sum
        //

        // data[i] = (data[i] * a[i]) + b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sum(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(b.data(I)), op::mul(op::load(data(I)), op::load(a.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a[i]) + b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sum(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(bvec, op::mul(op::load(data(I)), op::load(a.data(I))))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a) + b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sum(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::add(op::load(b.data(I)), op::mul(op::load(data(I)), avec))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a) + b
        FST_ALWAYS_INLINE derived_type& mul_then_sum(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::add(bvec, op::mul(op::load(data(I)), avec))));
            return (derived_type&) *this;
        }

        //
        // mul_then_sub
        //

        // data[i] = (data[i] * a[i]) - b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sub(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::mul(op::load(data(I)), op::load(a.data(I))), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a[i]) - b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sub(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::mul(op::load(data(I)), op::load(a.data(I))), bvec)));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a) - b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& mul_then_sub(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::mul(op::load(data(I)), avec), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] * a) - b
        FST_ALWAYS_INLINE derived_type& mul_then_sub(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sub(op::mul(op::load(data(I)), avec), bvec)));
            return (derived_type&) *this;
        }

        //
        // add_then_div
        //

        // data[i] = (data[i] + a[i]) / b[i]
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived, class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_then_div(
            const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::div(op::add(op::load(data(I)), op::load(a.data(I))), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a[i]) / b
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& add_then_div(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::add(op::load(a.data(I)), op::load(data(I)), bvec))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a) / b[i]
        template <class _BT, size_t _BSize, size_t _BAlign, class _BBase, class _BDerived>
        FST_ALWAYS_INLINE derived_type& add_then_div(value_type a, const container_base<_BT, _BSize, _BAlign, _BBase, _BDerived>& b) noexcept
        {
            const vector_type avec = op::load(a);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::add(op::load(data(I)), avec), op::load(b.data(I)))));
            return (derived_type&) *this;
        }

        // data[i] = (data[i] + a) / b
        FST_ALWAYS_INLINE derived_type& add_then_div(value_type a, value_type b) noexcept
        {
            const vector_type avec = op::load(a);
            const vector_type bvec = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::div(op::add(avec, op::load(data(I)), bvec))));
            return (derived_type&) *this;
        }

        //
        //
        //

        FST_ALWAYS_INLINE derived_type& min(value_type v) noexcept
        {
            const vector_type value = op::load(v);
            FST_SIMD_LOOP(op::store(data(I), op::min(op::load(data(I)), value)));
            return (derived_type&) *this;
        }

        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& min(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::min(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        FST_ALWAYS_INLINE derived_type& max(value_type v) noexcept
        {
            const vector_type value = op::load(v);
            FST_SIMD_LOOP(op::store(data(I), op::max(op::load(data(I)), value)));
            return (derived_type&) *this;
        }

        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& max(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a) noexcept
        {
            FST_SIMD_LOOP(op::store(data(I), op::max(op::load(data(I)), op::load(a.data(I)))));
            return (derived_type&) *this;
        }

        //
        //
        //

        FST_NODISCARD FST_ALWAYS_INLINE value_type horizontal_sum() const noexcept
        {
            // hadd [a0 + a1, a2 + a3, b0 + b1, b2 + b3]
            vector_type value = op::load_zero();
            if constexpr (__fst::simd::has_fixed_size<container_base>::value)
            {
                static constexpr size_t loop_size = data_base_type::size() / vector_size;
                __fst::unroll<loop_size / 2, vector_size * 2>([&]<size_t I>() { value = op::add(value, op::hadd(op::load(data(I)), op::load(data(I + vector_size)))); });
            }
            else
            {
                constexpr size_type delta = 2 * vector_size;
                for (size_type I = 0; I < size() - vector_size; I += delta)
                {
                    value = op::add(value, op::hadd(op::load(data(I)), op::load(data(I + vector_size))));
                }
            }

            return op::hsum(value);
        }

        //
        //
        //

        // data[i] = sin(a[i] * b)
        template <class _AT, size_t _ASize, size_t _AAlign, class _ABase, class _ADerived>
        FST_ALWAYS_INLINE derived_type& sin_of_mul_ab(const container_base<_AT, _ASize, _AAlign, _ABase, _ADerived>& a, value_type b) noexcept
        {
            const vector_type b_value = op::load(b);
            FST_SIMD_LOOP(op::store(data(I), op::sin(op::mul(op::load(a.data(I)), b_value))));
            return (derived_type&) *this;
        }

      private:
        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SDerived = _Derived,
            __fst::enable_if_t<__fst::simd::has_fixed_size<_SDerived>::value && __fst::simd::has_fixed_size<_TDerived>::value, int> = 0>
        FST_ALWAYS_INLINE constexpr void validate_size(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
            static_assert(size() == rb.size(), "range size mismatch");
        }

        template <class _TT, class _TBase, class _TDerived, size_t _TSize, size_t _TAlign, class _SDerived = _Derived,
            __fst::enable_if_t<__fst::simd::has_dynamic_size<_SDerived>::value || __fst::simd::has_dynamic_size<_TDerived>::value, int> = 0>
        FST_ALWAYS_INLINE constexpr void validate_size(const container_base<_TT, _TSize, _TAlign, _TBase, _TDerived>& rb) noexcept
        {
#if FST_HAS_ASSERT
            fst_assert(size() == rb.size(), "range size mismatch");
#else
            __fst::unused(rb);
#endif // FST_HAS_ASSERT
        }
    };
} // namespace detail
