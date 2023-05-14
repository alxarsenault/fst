///
///
///

FST_BEGIN_NAMESPACE

    ///
    FST_INLINE_VAR constexpr size_t platform_alignment = sizeof(void*);
    using platform_alignment_t = __fst::size_constant<platform_alignment>;

    ///
    FST_INLINE_VAR constexpr size_t default_alignment = 16;
    FST_INLINE_VAR constexpr size_t default_alignment_bit_index = 4;
    using default_alignment_t = __fst::size_constant<default_alignment>;

    ///
    FST_INLINE_VAR constexpr size_t default_vectorized_alignment = 32;
    FST_INLINE_VAR constexpr size_t default_vectorized_bit_index = 5;
    FST_INLINE_VAR constexpr size_t default_vectorized_size_threshold = 4096;
    using default_vectorized_alignment_t = __fst::size_constant<default_vectorized_alignment>;

    ///
    template <class _T>
    FST_INLINE_VAR constexpr size_t default_alignof = alignof(_T) < __fst::default_alignment ? __fst::default_alignment : alignof(_T);

    template <class _T>
    using default_alignof_t = __fst::size_constant<default_alignof<_T>>;

    ///
    template <class _T>
    FST_INLINE_VAR constexpr size_t default_vectorized_alignof = alignof(_T) < __fst::default_vectorized_alignment ? __fst::default_vectorized_alignment : alignof(_T);

    template <class _T>
    using default_vectorized_alignof_t = __fst::size_constant<default_vectorized_alignof<_T>>;

FST_END_NAMESPACE
