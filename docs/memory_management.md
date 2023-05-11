# Memory management

</br>
</br>

# Memory category
A memory category is basically a tag, used by the profiler, passed to every allocations / deallocations to track down which sections of the code are allocating.

Using this macro to declare a memory category will ensure that all memory categories refers to the same type when building in release. This will prevent a lot of unnecessary different template implementations. In other words, when building in release, all memory categories are `fst::default_memory_category`.

```cpp
FST_DECLARE_MEMORY_CATEGORY(my_memory_category_struct_name, "label");
```

</br>

<!-- Memory zone -->
# Memory zone
A memory zone is in charge of allocating and deallocating memory. This can somehow be seen as an equivalent to `std::allocator`.
The memory zone is responsible of calling the profiler when doing any supported operations. A memory zone can either be static or not. A static memory zone is highly preferred since it won't need to be contained and passed around by any container.

Use `fst::is_static_memory_zone<my_memory_zone>::value` to make sure your memory zone is a valid static memory zone.

All allocations / deallocations gets a `fst::memory_category_id` that is forwarded to the profiler. Deallocating the same memory with a different memory category id will trigger an error. When necessary, use the memory_zone `move_allocation` to notify the profiler of a category change.

```cpp
void move_allocation(void* ptr, fst::memory_category_id from_mid, fst::memory_category_id to_mid);
``` 

## Anatomy of a memory zone.

A memory zone is a class derived from `fst::memory_zone<>` that has a `name`, an `allocate` / `deallocate` method and an `aligned_allocate` / `aligned_deallocate` method. The memory zone base class takes care of generating a unique id.

### `name`
Name of the memory zone (*for profiling purposes*).
```cpp
static constexpr const char* name = "my_memory_zone";
``` 

### `allocate`
Conceptually the same as `malloc(size)`.
```cpp
static void* allocate(size_t size, fst::memory_category_id mid) noexcept;
```

### `deallocate`
Conceptually the same as `free(ptr)`.
Anything allocated with `allocate` needs to be deallocated with `deallocate`.

```cpp
static void deallocate(void* ptr, fst::memory_category_id mid) noexcept;
```

### `aligned_allocate`
Conceptually the same as `std::aligned_alloc(alignment, size)`.
Notice the inverted order of parameters with `std::aligned_alloc`.
```cpp
static void* aligned_allocate(size_t size, size_t alignment, fst::memory_category_id mid) noexcept;
```

### `aligned_deallocate`
Anything allocated with `aligned_allocate` needs to be deallocated with `aligned_deallocate`.
```cpp
static void aligned_deallocate(void* ptr, fst::memory_category_id mid) noexcept;
```
</br>

### Full example
```cpp
struct my_memory_zone : fst::memory_zone<my_memory_zone>
{
    static constexpr const char* name = "my_memory_zone";
    static void* allocate(size_t size, fst::memory_category_id mid) noexcept;
    static void deallocate(void* ptr, fst::memory_category_id mid) noexcept;
    static void* aligned_allocate(size_t size, size_t alignment, fs
    fst::memory_category_id mid) noexcept;
    static void aligned_deallocate(void* ptr, fst::memory_category_id mid) noexcept;
};
```

<!-- </br> -->
</br>

# Container

```cpp
template <class T, class MemoryCategory, class MemoryZone>
class unique_ptr;
```

```cpp
template <class T, class MemoryCategory, class MemoryZone>
class optional_ptr;
```

```cpp
template <typename T, size_t Alignment, class MemoryCategory, class MemoryZone>
class vector;
```

```cpp
template <class T, class MemoryZone, class MemoryCategory>
class memory_zone_allocator;
```
    
    

</br>

# Profiler


