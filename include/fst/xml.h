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
#include "fst/traits.h"
#include "fst/array.h"
#include "fst/memory.h"
#include "fst/status_code.h"
#include "fst/string_view.h"
#include "fst/stream.h"
#include "fst/utility.h"

// On MSVC, disable "conditional expression is constant" warning (level 4).
// This warning is almost impossible to avoid with certain types of templated code
FST_PRAGMA_PUSH()
FST_PRAGMA_DISABLE_WARNING_MSVC(4127)

///////////////////////////////////////////////////////////////////////////
// RAPIDXML_PARSE_ERROR

#define RAPIDXML_PARSE_ERROR(what, where) \
    {                                     \
        parse_error_handler(what, where); \
    }

FST_BEGIN_NAMESPACE

    /// When exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS,
    /// this function is called to notify user about the error.
    /// It must be defined by the user.
    /// <br><br>
    /// This function cannot return. If it does, the results are undefined.
    /// <br><br>
    /// A very simple definition might look like that:
    /// <pre>
    /// void %rapidxml::%parse_error_handler(const char *what, void *where)
    /// {
    ///     std::cout << "Parse error: " << what << "\n";
    ///     std::abort();
    /// }
    /// </pre>
    /// @param what Human readable description of the error.
    /// @param where Pointer to character data where error was detected.
    void parse_error_handler(const char*, const void*) {}
FST_END_NAMESPACE

///////////////////////////////////////////////////////////////////////////
// Pool sizes

#ifndef RAPIDXML_STATIC_POOL_SIZE
// Size of static memory block of xml_memory_pool.
// Define RAPIDXML_STATIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// No dynamic memory allocations are performed by xml_memory_pool until static memory is exhausted.
#define RAPIDXML_STATIC_POOL_SIZE (64 * 1024)
#endif

#ifndef RAPIDXML_DYNAMIC_POOL_SIZE
// Size of dynamic memory block of xml_memory_pool.
// Define RAPIDXML_DYNAMIC_POOL_SIZE before including rapidxml.hpp if you want to override the default value.
// After the static block is exhausted, dynamic blocks with approximately this size are allocated by xml_memory_pool.
#define RAPIDXML_DYNAMIC_POOL_SIZE (64 * 1024)
#endif

#ifndef RAPIDXML_ALIGNMENT
// Memory allocation alignment.
// Define RAPIDXML_ALIGNMENT before including rapidxml.hpp if you want to override the default value, which is the size of pointer.
// All memory allocations for nodes, attributes and strings will be aligned to this value.
// This must be a power of 2 and at least 1, otherwise xml_memory_pool will not work.
#define RAPIDXML_ALIGNMENT sizeof(void*)
#endif

FST_BEGIN_NAMESPACE
    //
    // Forward declarations
    //

    //
    template <class Ch>
    class basic_xml_node;

    //
    template <class Ch>
    class basic_xml_attribute;

    //
    template <class Ch, class _MemoryCategory, class _MemoryZone>
    class basic_xml_document;

    //
    using xml_document = basic_xml_document<char, __fst::default_memory_category, __fst::default_memory_zone>;

    //
    using xml_attribute = basic_xml_attribute<char>;

    //
    using xml_node = basic_xml_node<char>;

    /// Enumeration listing all node types produced by the parser.
    /// Use basic_xml_node::type() function to query node type.
    enum class xml_node_type {
        document, /// A document node. Name and value are empty.
        element, /// An element node. Name contains element name. Value contains text of first data node.
        data, /// A data node. Name is empty. Value contains data text.
        cdata, /// A CDATA node. Name is empty. Value contains data text.
        comment, /// A comment node. Name is empty. Value contains comment text.
        declaration, /// A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
        doctype, /// A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
        pi /// A PI node. Name contains target. Value contains instructions.
    };

    ///////////////////////////////////////////////////////////////////////
    // Internals

    /// \cond internal
    namespace internal
    {

        // Struct that contains lookup tables for the parser
        // It must be a template to allow correct linking (because it has static data members, which are defined in a header file).
        template <int Dummy>
        struct lookup_tables
        {
            static const unsigned char lookup_whitespace[256]; // Whitespace table
            static const unsigned char lookup_node_name[256]; // Node name table
            static const unsigned char lookup_text[256]; // Text table
            static const unsigned char lookup_text_pure_no_ws[256]; // Text table
            static const unsigned char lookup_text_pure_with_ws[256]; // Text table
            static const unsigned char lookup_attribute_name[256]; // Attribute name table
            static const unsigned char lookup_attribute_data_1[256]; // Attribute data table with single quote
            static const unsigned char lookup_attribute_data_1_pure[256]; // Attribute data table with single quote
            static const unsigned char lookup_attribute_data_2[256]; // Attribute data table with double quotes
            static const unsigned char lookup_attribute_data_2_pure[256]; // Attribute data table with double quotes
            //static const unsigned char lookup_digits[256]; // Digits
            //static const unsigned char lookup_upcase[256]; // To uppercase conversion table for ASCII characters
        };

        // Compare strings for equality
        template <class Ch>
        inline bool compare(const Ch* p1, size_t size1, const Ch* p2, size_t size2)
        {
            return size1 == size2 ? __fst::char_traits<Ch>::compare(p1, p2, size1) == 0 : false;
        }
    } // namespace internal
    /// \endcond

    ///////////////////////////////////////////////////////////////////////
    // Memory pool

    /// This class is used by the parser to create new nodes and attributes, without overheads of dynamic memory allocation.
    /// In most cases, you will not need to use this class directly.
    /// However, if you need to create nodes manually or modify names/values of nodes,
    /// you are encouraged to use xml_memory_pool of relevant basic_xml_document to allocate the memory.
    /// Not only is this faster than allocating them by using <code>new</code> operator,
    /// but also their lifetime will be tied to the lifetime of document,
    /// possibly simplyfing memory management.
    /// <br><br>
    /// Call allocate_node() or allocate_attribute() functions to obtain new nodes or attributes from the pool.
    /// You can also call allocate_string() function to allocate strings.
    /// Such strings can then be used as names or values of nodes without worrying about their lifetime.
    /// Note that there is no <code>free()</code> function -- all allocations are freed at once when clear() function is called,
    /// or when the pool is destroyed.
    /// <br><br>
    /// It is also possible to create a standalone xml_memory_pool, and use it
    /// to allocate nodes, whose lifetime will not be tied to any document.
    /// <br><br>
    /// Pool maintains <code>RAPIDXML_STATIC_POOL_SIZE</code> bytes of statically allocated memory.
    /// Until static memory is exhausted, no dynamic memory allocations are done.
    /// When static memory is exhausted, pool allocates additional blocks of memory of size <code>RAPIDXML_DYNAMIC_POOL_SIZE</code> each,
    /// by using global <code>new[]</code> and <code>delete[]</code> operators.
    /// This behaviour can be changed by setting custom allocation routines.
    /// Use set_allocator() function to set them.
    /// <br><br>
    /// Allocations for nodes, attributes and strings are aligned at <code>RAPIDXML_ALIGNMENT</code> bytes.
    /// This value defaults to the size of pointer on target architecture.
    /// <br><br>
    /// To obtain absolutely top performance from the parser,
    /// it is important that all nodes are allocated from a single, contiguous block of memory.
    /// Otherwise, cache misses when jumping between two (or more) disjoint blocks of memory can slow down parsing quite considerably.
    /// If required, you can tweak <code>RAPIDXML_STATIC_POOL_SIZE</code>, <code>RAPIDXML_DYNAMIC_POOL_SIZE</code> and <code>RAPIDXML_ALIGNMENT</code>
    /// to obtain best wasted memory to performance compromise.
    /// To do it, define their values before rapidxml.hpp file is included.
    /// @param Ch Character type of created nodes.
    template <class Ch, class _MemoryCategory, class _MemoryZone>
    class xml_memory_pool
    {
      public:
        /// Constructs empty pool with default allocator functions.
        xml_memory_pool() { init(); }

        /// Destroys pool and frees all the memory.
        /// This causes memory occupied by nodes allocated by the pool to be freed.
        /// Nodes allocated from the pool are no longer valid.
        ~xml_memory_pool() { clear(); }

        /// Allocates a new node from the pool, and optionally assigns name and value to it.
        /// If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
        /// If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
        /// will call rapidxml::parse_error_handler() function.
        /// @param type Type of node to create.
        /// @param name Name to assign to the node, or 0 to assign no name.
        /// @param value Value to assign to the node, or 0 to assign no value.
        /// @param name_size Size of name to assign, or 0 to automatically calculate size from name string.
        /// @param value_size Size of value to assign, or 0 to automatically calculate size from value string.
        /// @return Pointer to allocated node. This pointer will never be NULL.
        basic_xml_node<Ch>* allocate_node(xml_node_type type, const Ch* name = 0, const Ch* value = 0, size_t name_size = 0, size_t value_size = 0)
        {
            void* memory = allocate_aligned(sizeof(basic_xml_node<Ch>));

            basic_xml_node<Ch>* node = fst_placement_new(memory) basic_xml_node<Ch>(type);
            if (name)
            {
                if (name_size > 0) { node->name(name, name_size); }
                else { node->name(name); }
            }

            if (value)
            {
                if (value_size > 0) { node->value(value, value_size); }
                else { node->value(value); }
            }

            return node;
        }

        /// Allocates a new attribute from the pool, and optionally assigns name and value to it.
        /// If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
        /// If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
        /// will call rapidxml::parse_error_handler() function.
        /// @param name Name to assign to the attribute, or 0 to assign no name.
        /// @param value Value to assign to the attribute, or 0 to assign no value.
        /// @param name_size Size of name to assign, or 0 to automatically calculate size from name string.
        /// @param value_size Size of value to assign, or 0 to automatically calculate size from value string.
        /// @return Pointer to allocated attribute. This pointer will never be NULL.
        basic_xml_attribute<Ch>* allocate_attribute(const Ch* name = 0, const Ch* value = 0, size_t name_size = 0, size_t value_size = 0)
        {
            void* memory = allocate_aligned(sizeof(basic_xml_attribute<Ch>));
            basic_xml_attribute<Ch>* attribute = fst_placement_new(memory) basic_xml_attribute<Ch>;

            if (name)
            {
                if (name_size > 0) { attribute->name(name, name_size); }
                else { attribute->name(name); }
            }

            if (value)
            {
                if (value_size > 0) { attribute->value(value, value_size); }
                else { attribute->value(value); }
            }
            return attribute;
        }

        /// Allocates a char array of given size from the pool, and optionally copies a given string to it.
        /// If the allocation request cannot be accomodated, this function will throw <code>std::bad_alloc</code>.
        /// If exceptions are disabled by defining RAPIDXML_NO_EXCEPTIONS, this function
        /// will call rapidxml::parse_error_handler() function.
        /// @param source String to initialize the allocated memory with, or 0 to not initialize it.
        /// @param size Number of characters to allocate, or zero to calculate it automatically from source string length; if size is 0, source string must be specified and null terminated.
        /// @return Pointer to allocated char array. This pointer will never be NULL.
        Ch* allocate_string(const Ch* source = 0, size_t size = 0)
        {
            fst_assert(source || size); // Either source or size (or both) must be specified

            if (size == 0) { size = __fst::char_traits<Ch>::length(source) + 1; }

            Ch* result = static_cast<Ch*>(allocate_aligned(size * sizeof(Ch)));

            if (source)
            {
                for (size_t i = 0; i < size; ++i)
                {
                    result[i] = source[i];
                }
            }

            return result;
        }

        /// Clones an basic_xml_node and its hierarchy of child nodes and attributes.
        /// Nodes and attributes are allocated from this memory pool.
        /// Names and values are not cloned, they are shared between the clone and the source.
        /// Result node can be optionally specified as a second parameter,
        /// in which case its contents will be replaced with cloned source node.
        /// This is useful when you want to clone entire document.
        /// @param source Node to clone.
        /// @param result Node to put results in, or 0 to automatically allocate result node
        /// @return Pointer to cloned node. This pointer will never be NULL.
        basic_xml_node<Ch>* clone_node(const basic_xml_node<Ch>* source, basic_xml_node<Ch>* result = 0)
        {
            // Prepare result node
            if (result)
            {
                result->remove_all_attributes();
                result->remove_all_nodes();
                result->type(source->type());
            }
            else { result = allocate_node(source->type()); }

            // Clone name and value
            result->name(source->name().data(), source->name_size());
            result->value(source->value(), source->value_size());

            // Clone child nodes and attributes
            for (basic_xml_node<Ch>* child = source->first_node(); child; child = child->next_sibling())
            {
                result->append_node(clone_node(child));
            }

            for (basic_xml_attribute<Ch>* attr = source->first_attribute(); attr; attr = attr->next_attribute())
            {
                result->append_attribute(allocate_attribute(attr->name().data(), attr->value(), attr->name_size(), attr->value_size()));
            }

            return result;
        }

        /// Clears the pool.
        /// This causes memory occupied by nodes allocated by the pool to be freed.
        /// Any nodes or strings allocated from the pool will no longer be valid.
        void clear()
        {
            while (m_begin != m_static_memory.data())
            {
                char* previous_begin = reinterpret_cast<header*>(align(m_begin))->previous_begin;

                _MemoryZone::aligned_deallocate(m_begin, _MemoryCategory::id());
                //delete[] m_begin;

                m_begin = previous_begin;
            }

            init();
        }

      private:
        struct header
        {
            char* previous_begin;
        };

        void init()
        {
            m_begin = m_static_memory.data();
            m_ptr = align(m_begin);
            m_end = m_static_memory.data() + m_static_memory.size();
        }

        static char* align(char* ptr) noexcept
        {
            size_t alignment = ((RAPIDXML_ALIGNMENT - (size_t(ptr) & (RAPIDXML_ALIGNMENT - 1))) & (RAPIDXML_ALIGNMENT - 1));
            return ptr + alignment;
        }

        char* allocate_raw(size_t size)
        {
            // TODO:
            void* memory = _MemoryZone::aligned_allocate(size, RAPIDXML_ALIGNMENT, _MemoryCategory::id());
            return static_cast<char*>(memory);
        }

        void* allocate_aligned(size_t size)
        {
            // Calculate aligned pointer
            char* result = align(m_ptr);

            // If not enough memory left in current pool, allocate a new pool
            if (result + size > m_end)
            {
                // Calculate required pool size (may be bigger than RAPIDXML_DYNAMIC_POOL_SIZE)
                size_t pool_size = RAPIDXML_DYNAMIC_POOL_SIZE;
                if (pool_size < size) { pool_size = size; }

                // Allocate
                size_t alloc_size
                    = sizeof(header) + (2 * RAPIDXML_ALIGNMENT - 2) + pool_size; // 2 alignments required in worst case: one for header, one for actual allocation
                char* raw_memory = allocate_raw(alloc_size);

                // Setup new pool in allocated memory
                char* pool = align(raw_memory);
                header* new_header = reinterpret_cast<header*>(pool);
                new_header->previous_begin = m_begin;
                m_begin = raw_memory;
                m_ptr = pool + sizeof(header);
                m_end = raw_memory + alloc_size;

                // Calculate aligned pointer again using new pool
                result = align(m_ptr);
            }

            // Update pool and return aligned pointer
            m_ptr = result + size;
            return result;
        }

        char* m_begin; // Start of raw memory making up current pool
        char* m_ptr; // First free byte in current pool
        char* m_end; // One past last available byte in current pool
        //char m_static_memory[RAPIDXML_STATIC_POOL_SIZE]; // Static raw memory
        __fst::array<char, RAPIDXML_STATIC_POOL_SIZE, RAPIDXML_ALIGNMENT> m_static_memory;
    };

    ///////////////////////////////////////////////////////////////////////////
    // XML base

    /// Base class for basic_xml_node and basic_xml_attribute implementing common functions:
    /// name(), name_size(), value(), value_size() and parent().
    ///
    /// @tparam Ch Character type to use
    template <class Ch = char>
    class basic_xml_base
    {
      public:
        using char_type = Ch;
        using char_traits = __fst::char_traits<char_type>;
        using view_type = __fst::basic_string_view<char_type>;

        // Construct a base with empty name, value and parent
        basic_xml_base() noexcept = default;

        /// Gets name of the node.
        /// Interpretation of name depends on type of node.
        /// @return Name of node, or empty string if node has no name.
        FST_NODISCARD FST_ALWAYS_INLINE view_type name() const noexcept { return m_name ? view_type(m_name, m_name_size) : view_type(nullstr(), 0); }

        /// Gets size of node name, not including terminator character.
        /// This function works correctly irrespective of whether name is or is not zero terminated.
        /// @return Size of node name, in characters.
        FST_NODISCARD FST_ALWAYS_INLINE size_t name_size() const noexcept { return m_name ? m_name_size : 0; }

        /// Gets value of node.
        /// Interpretation of value depends on type of node.
        /// <br><br>
        /// @return Value of node, or empty string if node has no value.
        FST_NODISCARD FST_ALWAYS_INLINE view_type value() const noexcept { return m_value ? view_type(m_value, m_value_size) : view_type(nullstr(), 0); }

        /// Gets size of node value, not including terminator character.
        /// This function works correctly irrespective of whether value is or is not zero terminated.
        /// @return Size of node value, in characters.
        FST_NODISCARD FST_ALWAYS_INLINE size_t value_size() const noexcept { return m_value ? m_value_size : 0; }

        /// Sets name of node to a non zero-terminated string.
        /// See @ref ownership_of_strings.
        /// <br><br>
        /// Note that node does not own its name or value, it only stores a pointer to it.
        /// It will not delete or otherwise free the pointer on destruction.
        /// It is reponsibility of the user to properly manage lifetime of the string.
        /// The easiest way to achieve it is to use xml_memory_pool of the document to allocate the string -
        /// on destruction of the document the string will be automatically freed.
        /// <br><br>
        /// Size of name must be specified separately, because name does not have to be zero terminated.
        /// Use name(const Ch *) function to have the length automatically calculated (string must be zero terminated).
        ///
        /// @param name Name of node to set. Does not have to be zero terminated.
        /// @param size Size of name, in characters. This does not include zero terminator, if one is present.
        FST_ALWAYS_INLINE void name(const char_type* name, size_t size) noexcept
        {
            m_name = name;
            m_name_size = size;
        }

        FST_ALWAYS_INLINE void name(view_type str) noexcept
        {
            m_name = str.data();
            m_name_size = str.size();
        }

        /// Sets name of node to a zero-terminated string.
        /// See also @ref ownership_of_strings and basic_xml_node::name(const Ch *, size_t).
        ///
        /// @param name Name of node to set. Must be zero terminated.
        FST_ALWAYS_INLINE void name(const char_type* name) noexcept { this->name(name, char_traits::length(name)); }

        /// Sets value of node to a non zero-terminated string.
        /// See @ref ownership_of_strings.
        /// <br><br>
        /// Note that node does not own its name or value, it only stores a pointer to it.
        /// It will not delete or otherwise free the pointer on destruction.
        /// It is reponsibility of the user to properly manage lifetime of the string.
        /// The easiest way to achieve it is to use xml_memory_pool of the document to allocate the string -
        /// on destruction of the document the string will be automatically freed.
        /// <br><br>
        /// Size of value must be specified separately, because it does not have to be zero terminated.
        /// Use value(const Ch *) function to have the length automatically calculated (string must be zero terminated).
        /// <br><br>
        /// If an element has a child node of type data, it will take precedence over element value when printing.
        /// If you want to manipulate data of elements using values, use parser flag rapidxml::parse_no_data_nodes to prevent creation of data nodes by the parser.
        ///
        /// @param value value of node to set. Does not have to be zero terminated.
        /// @param size Size of value, in characters. This does not include zero terminator, if one is present.
        FST_ALWAYS_INLINE void value(const char_type* value, size_t size) noexcept
        {
            m_value = value;
            m_value_size = size;
        }

        FST_ALWAYS_INLINE void value(view_type str) noexcept
        {
            m_value = str.data();
            m_value_size = str.size();
        }

        /// Sets value of node to a zero-terminated string.
        /// See also @ref ownership_of_strings and basic_xml_node::value(const Ch *, size_t).
        ///
        /// @param value Vame of node to set. Must be zero terminated.
        FST_ALWAYS_INLINE void value(const char_type* value) noexcept { this->value(value, char_traits::length(value)); }

        /// Gets node parent.
        /// @return Pointer to parent node, or 0 if there is no parent.
        FST_NODISCARD FST_ALWAYS_INLINE basic_xml_node<char_type>* parent() const noexcept { return m_parent; }

      protected:
        // Return empty string
        static char_type* nullstr() noexcept
        {
            static char_type __zero = char_type('\0');
            return &__zero;
        }

        const char_type* m_name = nullptr; // Name of node, or nullptr if no name
        const char_type* m_value = nullptr; // Value of node, or nullptr if no value
        size_t m_name_size = 0; // Length of node name, or undefined of no name
        size_t m_value_size = 0; // Length of node value, or undefined if no value
        basic_xml_node<char_type>* m_parent = nullptr; // Pointer to parent node, or nullptr if none
    };

    /// Class representing attribute node of XML document.
    /// Each attribute has name and value strings, which are available through name() and value() functions (inherited from basic_xml_base).
    /// Note that after parse, both name and value of attribute will point to interior of source text used for parsing.
    /// Thus, this text must persist in memory for the lifetime of attribute.
    /// @param Ch Character type to use.
    template <class Ch = char>
    class basic_xml_attribute : public basic_xml_base<Ch>
    {
        friend class basic_xml_node<Ch>;

      public:
        ///////////////////////////////////////////////////////////////////////////
        // Construction & destruction

        /// Constructs an empty attribute with the specified type.
        /// Consider using xml_memory_pool of appropriate basic_xml_document if allocating attributes manually.
        basic_xml_attribute() noexcept = default;

        ///////////////////////////////////////////////////////////////////////////
        // Related nodes access

        /// Gets document of which attribute is a child.
        /// @return Pointer to document that contains this attribute, or 0 if there is no parent document.
        /*basic_xml_document<Ch>* document() const noexcept
        {
            if (basic_xml_node<Ch>* node = this->parent())
            {
                while (node->parent())
                {
                    node = node->parent();
                }
                return node->type() == document ? static_cast<basic_xml_document<Ch>*>(node) : nullptr;
            }
            return nullptr;
        }*/

        /// Gets previous attribute, optionally matching attribute name.
        /// @param name Name of attribute to find, or 0 to return previous attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found attribute, or 0 if not found.
        basic_xml_attribute<Ch>* previous_attribute(const Ch* name = 0, size_t name_size = 0) const noexcept
        {
            if (name)
            {
                if (!name_size) { name_size = __fst::char_traits<Ch>::length(name); }

                for (basic_xml_attribute<Ch>* attribute = m_prev_attribute; attribute; attribute = attribute->m_prev_attribute)
                {
                    if (internal::compare(attribute->name().data(), attribute->name_size(), name, name_size)) { return attribute; }
                }

                return nullptr;
            }

            return this->m_parent ? m_prev_attribute : nullptr;
        }

        /// Gets next attribute, optionally matching attribute name.
        /// @param name Name of attribute to find, or 0 to return next attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found attribute, or 0 if not found.
        basic_xml_attribute<Ch>* next_attribute(const Ch* name, size_t name_size = 0) const noexcept
        {
            if (name)
            {
                if (name_size == 0) { name_size = __fst::char_traits<Ch>::length(name); }

                for (basic_xml_attribute<Ch>* attribute = m_next_attribute; attribute; attribute = attribute->m_next_attribute)
                {
                    if (internal::compare(attribute->name().data(), attribute->name_size(), name, name_size)) { return attribute; }
                }

                return nullptr;
            }

            return this->m_parent ? m_next_attribute : nullptr;
        }

        basic_xml_attribute<Ch>* next_attribute() const noexcept { return this->m_parent ? m_next_attribute : nullptr; }

      private:
        basic_xml_attribute<Ch>* m_prev_attribute; // Pointer to previous sibling of attribute, or 0 if none; only valid if parent is non-zero
        basic_xml_attribute<Ch>* m_next_attribute; // Pointer to next sibling of attribute, or 0 if none; only valid if parent is non-zero
    };

    ///////////////////////////////////////////////////////////////////////////
    // XML node

    /// Class representing a node of XML document.
    /// Each node may have associated name and value strings, which are available through name() and value() functions.
    /// Interpretation of name and value depends on type of the node.
    /// Type of node can be determined by using type() function.
    /// <br><br>
    /// Note that after parse, both name and value of node, if any, will point interior of source text used for parsing.
    /// Thus, this text must persist in the memory for the lifetime of node.
    /// @param Ch Character type to use.
    template <class Ch = char>
    class basic_xml_node : public basic_xml_base<Ch>
    {
      public:
        using char_type = Ch;
        using char_traits = __fst::char_traits<char_type>;
        using node_pointer = basic_xml_node<char_type>*;
        using attribute_pointer = basic_xml_attribute<char_type>*;

        ///////////////////////////////////////////////////////////////////////////
        // Construction & destruction

        /// Constructs an empty node with the specified type.
        /// Consider using xml_memory_pool of appropriate document to allocate nodes manually.
        /// @param type Type of node to construct.
        basic_xml_node(xml_node_type type) noexcept
            : m_type(type)
            , m_first_node(nullptr)
            , m_first_attribute(nullptr)
        {}

        // No copying
        basic_xml_node(const basic_xml_node&) = delete;
        basic_xml_node& operator=(const basic_xml_node&) = delete;

        ///////////////////////////////////////////////////////////////////////////
        // Node data access

        /// Gets type of node.
        /// @return Type of node.
        FST_NODISCARD FST_ALWAYS_INLINE xml_node_type type() const noexcept { return m_type; }

        ///////////////////////////////////////////////////////////////////////////
        // Related nodes access

        /// Gets document of which node is a child.
        /// @return Pointer to document that contains this node, or 0 if there is no parent document.
        /*FST_NODISCARD FST_ALWAYS_INLINE basic_xml_document<Ch>* document() const noexcept
        {
            basic_xml_node<Ch>* node = const_cast<basic_xml_node<Ch>*>(this);
            while (node->parent())
            {
                node = node->parent();
            }

            return node->type() == document ? static_cast<basic_xml_document<Ch>*>(node) : nullptr;
        }*/

        /// Gets first child node, optionally matching node name.
        /// @param name Name of child to find, or 0 to return first child regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found child, or 0 if not found.
        node_pointer first_node(const Ch* name, size_t name_size = 0) const noexcept
        {
            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (node_pointer child = m_first_node; child; child = child->next_sibling())
                {
                    if (internal::compare(child->name().data(), child->name_size(), name, name_size)) { return child; }
                }
                return nullptr;
            }

            return m_first_node;
        }

        FST_NODISCARD FST_ALWAYS_INLINE node_pointer first_node() const noexcept { return m_first_node; }

        /// Gets last child node, optionally matching node name.
        /// Behaviour is undefined if node has no children.
        /// Use first_node() to test if node has children.
        /// @param name Name of child to find, or 0 to return last child regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found child, or 0 if not found.
        node_pointer last_node(const Ch* name, size_t name_size = 0) const noexcept
        {
            fst_assert(m_first_node); // Cannot query for last child if node has no children
            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (node_pointer child = m_last_node; child; child = child->previous_sibling())
                {
                    if (internal::compare(child->name().data(), child->name_size(), name, name_size)) { return child; }
                }
                return nullptr;
            }

            return m_last_node;
        }

        FST_NODISCARD FST_ALWAYS_INLINE node_pointer last_node() const noexcept
        {
            fst_assert(m_first_node); // Cannot query for last child if node has no children
            return m_last_node;
        }

        /// Gets previous sibling node, optionally matching node name.
        /// Behaviour is undefined if node has no parent.
        /// Use parent() to test if node has a parent.
        /// @param name Name of sibling to find, or 0 to return previous sibling regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found sibling, or 0 if not found.
        node_pointer previous_sibling(const Ch* name, size_t name_size = 0) const noexcept
        {
            fst_assert(this->m_parent); // Cannot query for siblings if node has no parent

            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (node_pointer sibling = m_prev_sibling; sibling; sibling = sibling->m_prev_sibling)
                {
                    if (internal::compare(sibling->name().data(), sibling->name_size(), name, name_size)) { return sibling; }
                }
                return nullptr;
            }

            return m_prev_sibling;
        }

        FST_NODISCARD FST_ALWAYS_INLINE node_pointer previous_sibling() const noexcept
        {
            fst_assert(this->m_parent); // Cannot query for siblings if node has no parent
            return m_prev_sibling;
        }

        /// Gets next sibling node, optionally matching node name.
        /// Behaviour is undefined if node has no parent.
        /// Use parent() to test if node has a parent.
        /// @param name Name of sibling to find, or 0 to return next sibling regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found sibling, or 0 if not found.
        node_pointer next_sibling(const Ch* name, size_t name_size = 0) const noexcept
        {
            fst_assert(this->m_parent); // Cannot query for siblings if node has no parent

            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (node_pointer sibling = m_next_sibling; sibling; sibling = sibling->m_next_sibling)
                {
                    if (internal::compare(sibling->name().data(), sibling->name_size(), name, name_size)) { return sibling; }
                }
                return nullptr;
            }

            return m_next_sibling;
        }

        FST_NODISCARD FST_ALWAYS_INLINE node_pointer next_sibling() const noexcept
        {
            fst_assert(this->m_parent); // Cannot query for siblings if node has no parent
            return m_next_sibling;
        }

        /// Gets first attribute of node, optionally matching attribute name.
        /// @param name Name of attribute to find, or 0 to return first attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found attribute, or 0 if not found.
        attribute_pointer first_attribute(const Ch* name, size_t name_size = 0) const noexcept
        {
            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (attribute_pointer attribute = m_first_attribute; attribute; attribute = attribute->m_next_attribute)
                {
                    if (internal::compare(attribute->name().data(), attribute->name_size(), name, name_size)) { return attribute; }
                }
                return nullptr;
            }

            return m_first_attribute;
        }

        FST_NODISCARD FST_ALWAYS_INLINE attribute_pointer first_attribute() const noexcept { return m_first_attribute; }

        /// Gets last attribute of node, optionally matching attribute name.
        /// @param name Name of attribute to find, or 0 to return last attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
        /// @param name_size Size of name, in characters, or 0 to have size calculated automatically from string
        /// @param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
        /// @return Pointer to found attribute, or 0 if not found.
        attribute_pointer last_attribute(const Ch* name, size_t name_size = 0) const noexcept
        {
            if (name)
            {
                if (name_size == 0) { name_size = char_traits::length(name); }

                for (attribute_pointer attribute = m_last_attribute; attribute; attribute = attribute->m_prev_attribute)
                {
                    if (internal::compare(attribute->name().data(), attribute->name_size(), name, name_size)) { return attribute; }
                }

                return nullptr;
            }

            return m_first_attribute ? m_last_attribute : nullptr;
        }

        FST_NODISCARD FST_ALWAYS_INLINE attribute_pointer last_attribute() const noexcept { return m_first_attribute ? m_last_attribute : nullptr; }

        ///////////////////////////////////////////////////////////////////////////
        // Node modification

        /// Sets type of node.
        /// @param type Type of node to set.
        FST_ALWAYS_INLINE void type(xml_node_type type) noexcept { m_type = type; }

        ///////////////////////////////////////////////////////////////////////////
        // Node manipulation

        /// Prepends a new child node.
        /// The prepended child becomes the first child, and all existing children are moved one position back.
        /// @param child Node to prepend.
        void prepend_node(node_pointer child) noexcept
        {
            fst_assert(child && !child->parent() && child->type() != xml_node_type::document);

            if (first_node())
            {
                child->m_next_sibling = m_first_node;
                m_first_node->m_prev_sibling = child;
            }
            else
            {
                child->m_next_sibling = nullptr;
                m_last_node = child;
            }

            m_first_node = child;
            child->m_parent = this;
            child->m_prev_sibling = nullptr;
        }

        /// Appends a new child node.
        /// The appended child becomes the last child.
        /// @param child Node to append.
        void append_node(node_pointer child) noexcept
        {
            fst_assert(child && !child->parent() && child->type() != xml_node_type::document);

            if (m_first_node)
            {
                child->m_prev_sibling = m_last_node;
                m_last_node->m_next_sibling = child;
            }
            else
            {
                child->m_prev_sibling = nullptr;
                m_first_node = child;
            }

            m_last_node = child;
            child->m_parent = this;
            child->m_next_sibling = nullptr;
        }

        /// Inserts a new child node at specified place inside the node.
        /// All children after and including the specified node are moved one position back.
        /// @param where Place where to insert the child, or 0 to insert at the back.
        /// @param child Node to insert.
        void insert_node(node_pointer where, node_pointer child) noexcept
        {
            fst_assert(!where || where->parent() == this);
            fst_assert(child && !child->parent() && child->type() != xml_node_type::document);

            if (where == m_first_node) { prepend_node(child); }
            else if (where == nullptr) { append_node(child); }
            else
            {
                child->m_prev_sibling = where->m_prev_sibling;
                child->m_next_sibling = where;
                where->m_prev_sibling->m_next_sibling = child;
                where->m_prev_sibling = child;
                child->m_parent = this;
            }
        }

        /// Removes first child node.
        /// If node has no children, behaviour is undefined.
        /// Use first_node() to test if node has children.
        void remove_first_node() noexcept
        {
            fst_assert(first_node());

            node_pointer child = m_first_node;
            m_first_node = child->m_next_sibling;

            if (child->m_next_sibling) { child->m_next_sibling->m_prev_sibling = nullptr; }
            else { m_last_node = nullptr; }

            child->m_parent = nullptr;
        }

        /// Removes last child of the node.
        /// If node has no children, behaviour is undefined.
        /// Use first_node() to test if node has children.
        void remove_last_node() noexcept
        {
            fst_assert(first_node());

            node_pointer child = m_last_node;
            if (child->m_prev_sibling)
            {
                m_last_node = child->m_prev_sibling;
                child->m_prev_sibling->m_next_sibling = nullptr;
            }
            else { m_first_node = nullptr; }

            child->m_parent = nullptr;
        }

        /// Removes specified child from the node
        // @param where Pointer to child to be removed.
        void remove_node(node_pointer where) noexcept
        {
            fst_assert(where && where->parent() == this);
            fst_assert(first_node());

            if (where == m_first_node) { remove_first_node(); }
            else if (where == m_last_node) { remove_last_node(); }
            else
            {
                where->m_prev_sibling->m_next_sibling = where->m_next_sibling;
                where->m_next_sibling->m_prev_sibling = where->m_prev_sibling;
                where->m_parent = nullptr;
            }
        }

        /// Removes all child nodes (but not attributes).
        void remove_all_nodes() noexcept
        {
            for (node_pointer node = first_node(); node; node = node->m_next_sibling)
            {
                node->m_parent = nullptr;
            }

            m_first_node = nullptr;
        }

        /// Prepends a new attribute to the node.
        /// @param attribute Attribute to prepend.
        void prepend_attribute(attribute_pointer attribute) noexcept
        {
            fst_assert(attribute && !attribute->parent());

            if (first_attribute())
            {
                attribute->m_next_attribute = m_first_attribute;
                m_first_attribute->m_prev_attribute = attribute;
            }
            else
            {
                attribute->m_next_attribute = nullptr;
                m_last_attribute = attribute;
            }

            m_first_attribute = attribute;
            attribute->m_parent = this;
            attribute->m_prev_attribute = nullptr;
        }

        /// Appends a new attribute to the node.
        /// @param attribute Attribute to append.
        void append_attribute(attribute_pointer attribute) noexcept
        {
            fst_assert(attribute && !attribute->parent());

            if (m_first_attribute)
            {
                attribute->m_prev_attribute = m_last_attribute;
                m_last_attribute->m_next_attribute = attribute;
            }
            else
            {
                attribute->m_prev_attribute = nullptr;
                m_first_attribute = attribute;
            }

            m_last_attribute = attribute;
            attribute->m_parent = this;
            attribute->m_next_attribute = nullptr;
        }

        /// Inserts a new attribute at specified place inside the node.
        /// All attributes after and including the specified attribute are moved one position back.
        /// @param where Place where to insert the attribute, or 0 to insert at the back.
        /// @param attribute Attribute to insert.
        void insert_attribute(attribute_pointer where, attribute_pointer attribute) noexcept
        {
            fst_assert(!where || where->parent() == this);
            fst_assert(attribute && !attribute->parent());

            if (where == m_first_attribute) { prepend_attribute(attribute); }
            else if (where == nullptr) { append_attribute(attribute); }
            else
            {
                attribute->m_prev_attribute = where->m_prev_attribute;
                attribute->m_next_attribute = where;
                where->m_prev_attribute->m_next_attribute = attribute;
                where->m_prev_attribute = attribute;
                attribute->m_parent = this;
            }
        }

        /// Removes first attribute of the node.
        /// If node has no attributes, behaviour is undefined.
        /// Use first_attribute() to test if node has attributes.
        void remove_first_attribute() noexcept
        {
            fst_assert(first_attribute());

            attribute_pointer attribute = m_first_attribute;
            if (attribute->m_next_attribute) { attribute->m_next_attribute->m_prev_attribute = nullptr; }
            else { m_last_attribute = nullptr; }

            attribute->m_parent = nullptr;
            m_first_attribute = attribute->m_next_attribute;
        }

        /// Removes last attribute of the node.
        /// If node has no attributes, behaviour is undefined.
        /// Use first_attribute() to test if node has attributes.
        void remove_last_attribute() noexcept
        {
            fst_assert(first_attribute());

            attribute_pointer attribute = m_last_attribute;
            if (attribute->m_prev_attribute)
            {
                attribute->m_prev_attribute->m_next_attribute = nullptr;
                m_last_attribute = attribute->m_prev_attribute;
            }
            else { m_first_attribute = nullptr; }

            attribute->m_parent = nullptr;
        }

        /// Removes specified attribute from node.
        /// @param where Pointer to attribute to be removed.
        void remove_attribute(attribute_pointer where) noexcept
        {
            fst_assert(first_attribute() && where->parent() == this);

            if (where == m_first_attribute) { remove_first_attribute(); }
            else if (where == m_last_attribute) { remove_last_attribute(); }
            else
            {
                where->m_prev_attribute->m_next_attribute = where->m_next_attribute;
                where->m_next_attribute->m_prev_attribute = where->m_prev_attribute;
                where->m_parent = nullptr;
            }
        }

        /// Removes all attributes of node.
        void remove_all_attributes() noexcept
        {
            for (attribute_pointer attribute = first_attribute(); attribute; attribute = attribute->m_next_attribute)
            {
                attribute->m_parent = nullptr;
            }

            m_first_attribute = nullptr;
        }

      private:
        // Note that some of the pointers below have UNDEFINED values if certain other pointers are 0.
        // This is required for maximum performance, as it allows the parser to omit initialization of
        // unneded/redundant values.
        //
        // The rules are as follows:
        // 1. first_node and first_attribute contain valid pointers, or 0 if node has no children/attributes respectively
        // 2. last_node and last_attribute are valid only if node has at least one child/attribute respectively, otherwise they contain garbage
        // 3. prev_sibling and next_sibling are valid only if node has a parent, otherwise they contain garbage

        xml_node_type m_type; // Type of node; always valid
        node_pointer m_first_node; // Pointer to first child node, or 0 if none; always valid
        node_pointer m_last_node; // Pointer to last child node, or 0 if none; this value is only valid if m_first_node is non-zero
        attribute_pointer m_first_attribute; // Pointer to first attribute of node, or 0 if none; always valid
        attribute_pointer m_last_attribute; // Pointer to last attribute of node, or 0 if none; this value is only valid if m_first_attribute is non-zero
        node_pointer m_prev_sibling; // Pointer to previous sibling of node, or 0 if none; this value is only valid if m_parent is non-zero
        node_pointer m_next_sibling; // Pointer to next sibling of node, or 0 if none; this value is only valid if m_parent is non-zero
    };

    ///////////////////////////////////////////////////////////////////////////
    // XML document

    /// This class represents root of the DOM hierarchy.
    /// It is also an basic_xml_node and a xml_memory_pool through public inheritance.
    /// Use parse() function to build a DOM tree from a zero-terminated XML text string.
    /// parse() function allocates memory for nodes and attributes by using functions of basic_xml_document,
    /// which are inherited from xml_memory_pool.
    /// To access root node of the document, use the document itself, as if it was an basic_xml_node.
    /// @param Ch Character type to use.
    template <class Ch, class _MemoryCategory, class _MemoryZone>
    class basic_xml_document
        : public basic_xml_node<Ch>
        , public xml_memory_pool<Ch, _MemoryCategory, _MemoryZone>
    {

      public:
        /// Constructs empty XML document
        basic_xml_document()
            : basic_xml_node<Ch>(xml_node_type::document)
        {}

        /// Parses zero-terminated XML string according to given flags.
        /// Passed string will be modified by the parser, unless rapidxml::parse_non_destructive flag is used.
        /// The string must persist for the lifetime of the document.
        /// In case of error, rapidxml::parse_error exception will be thrown.
        /// <br><br>
        /// If you want to parse contents of a file, you must first load the file into the memory, and pass pointer to its beginning.
        /// Make sure that data is zero-terminated.
        /// <br><br>
        /// Document can be parsed into multiple times.
        /// Each new call to parse removes previous nodes and attributes (if any), but does not clear memory pool.
        /// @param text XML data to parse; pointer is non-const to denote fact that this data may be modified by the parser.
        __fst::error_result parse(const Ch* text)
        {
            fst_assert(text);
            __fst::error_result er;

            // Remove current contents
            this->remove_all_nodes();
            this->remove_all_attributes();

            // Parse BOM, if any
            parse_bom(text);

            // Parse children
            while (1)
            {
                // Skip whitespace before node
                skip<whitespace_pred>(text);
                if (*text == 0) break;

                // Parse and append new child
                if (*text == Ch('<'))
                {
                    ++text; // Skip '<'
                    if (basic_xml_node<Ch>* node = parse_node(text, er); node && !er) { this->append_node(node); }
                    if (er) { return er; }
                }
                else
                {
                    //RAPIDXML_PARSE_ERROR("expected <", text);
                    fst::print("expected <", text);
                    return __fst::status_code::unknown;
                }
            }

            return __fst::status_code::success;
        }

        /// Clears the document by deleting all nodes and clearing the memory pool.
        /// All nodes owned by document pool are destroyed.
        void clear()
        {
            this->remove_all_nodes();
            this->remove_all_attributes();
            xml_memory_pool<Ch>::clear();
        }

        ///
        FST_NODISCARD inline basic_xml_node<Ch>* create_declaration_node() noexcept
        {
            fst::xml_node* decl_node = this->allocate_node(fst::xml_node_type::declaration);
            decl_node->append_attribute(this->allocate_attribute("version", "1.0", 7, 3));
            decl_node->append_attribute(this->allocate_attribute("encoding", "UTF-8", 8, 5));
            return decl_node;
        }

        ///
        FST_NODISCARD inline basic_xml_node<Ch>* create_node(__fst::basic_string_view<Ch> name) noexcept
        {
            Ch* name_str = this->allocate_string(name.data(), name.size());
            return this->allocate_node(xml_node_type::element, name_str, nullptr, name.size(), 0);
        }

        ///
        template <class _T, __fst::enable_if_t<__fst::is_integral_v<_T>, int> = 0>
        FST_NODISCARD inline basic_xml_node<Ch>* create_node(__fst::basic_string_view<Ch> name, _T value) noexcept
        {
            Ch* name_str = this->allocate_string(name.data(), name.size());

            if constexpr (__fst::is_unsigned_v<_T>)
            {
                Ch buffer[21];
                const Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, value);
                Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                return this->allocate_node(xml_node_type::element, name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
            }
            else
            {
                if (value < 0)
                {
                    Ch buffer[21];
                    Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, static_cast<__fst::make_unsigned_t<_T>>(0 - static_cast<__fst::make_unsigned_t<_T>>(value)));
                    *--begin = (Ch) '-';
                    Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                    return this->allocate_node(xml_node_type::element, name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
                }
                else
                {
                    Ch buffer[21];
                    Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, static_cast<__fst::make_unsigned_t<_T>>(value));
                    Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                    return this->allocate_node(xml_node_type::element, name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
                }
            }
        }

        template <class _T, __fst::enable_if_t<__fst::is_floating_point_v<_T>, int> = 0>
        FST_NODISCARD inline basic_xml_node<Ch>* create_node(__fst::basic_string_view<Ch> name, _T value) noexcept
        {
            Ch buffer[33];
            int len = __fst::snprintf(&buffer[0], 32, "%g", (double) value);
            if (len <= 0) { return nullptr; }

            Ch* name_str = this->allocate_string(name.data(), name.size());
            Ch* value_str = this->allocate_string(&buffer[0], (size_t) len);
            return this->allocate_node(xml_node_type::element, name_str, value_str, name.size(), (size_t) len);
        }

        FST_NODISCARD inline basic_xml_node<Ch>* create_node(__fst::basic_string_view<Ch> name, __fst::basic_string_view<Ch> value) noexcept
        {
            Ch* name_str = this->allocate_string(name.data(), name.size());
            Ch* value_str = this->allocate_string(value.data(), value.size());
            return this->allocate_node(xml_node_type::element, name_str, value_str, name.size(), value.size());
        }

        ///
        template <class _T, __fst::enable_if_t<__fst::is_integral_v<_T>, int> = 0>
        FST_NODISCARD inline basic_xml_attribute<Ch>* create_attribute(__fst::basic_string_view<Ch> name, _T value) noexcept
        {
            Ch* name_str = this->allocate_string(name.data(), name.size());

            if constexpr (__fst::is_unsigned_v<_T>)
            {
                Ch buffer[21];
                const Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, value);
                Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                return this->allocate_attribute(name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
            }
            else
            {
                if (value < 0)
                {
                    Ch buffer[21];
                    Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, static_cast<__fst::make_unsigned_t<_T>>(0 - static_cast<__fst::make_unsigned_t<_T>>(value)));
                    *--begin = (Ch) '-';
                    Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                    return this->allocate_attribute(name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
                }
                else
                {
                    Ch buffer[21];
                    Ch* begin = stream_detail::uint_to_buff(&buffer[0] + 21, static_cast<__fst::make_unsigned_t<_T>>(value));
                    Ch* value_str = this->allocate_string(begin, size_t(&buffer[0] + 21 - begin));
                    return this->allocate_attribute(name_str, value_str, name.size(), size_t(&buffer[0] + 21 - begin));
                }
            }
        }

        template <class _T, __fst::enable_if_t<__fst::is_floating_point_v<_T>, int> = 0>
        FST_NODISCARD inline basic_xml_attribute<Ch>* create_attribute(__fst::basic_string_view<Ch> name, _T value) noexcept
        {
            Ch buffer[33];
            int len = __fst::snprintf(&buffer[0], 32, "%g", (double) value);
            if (len <= 0) { return nullptr; }

            Ch* name_str = this->allocate_string(name.data(), name.size());
            Ch* value_str = this->allocate_string(&buffer[0], (size_t) len);
            return this->allocate_attribute(name_str, value_str, name.size(), (size_t) len);
        }

        FST_NODISCARD inline basic_xml_attribute<Ch>* create_attribute(__fst::basic_string_view<Ch> name, __fst::basic_string_view<Ch> value) noexcept
        {
            Ch* name_str = this->allocate_string(name.data(), name.size());
            Ch* value_str = this->allocate_string(value.data(), value.size());
            return this->allocate_attribute(name_str, value_str, name.size(), value.size());
        }

      private:
        ///////////////////////////////////////////////////////////////////////
        // Internal character utility functions

        // Detect whitespace character
        struct whitespace_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_whitespace[static_cast<unsigned char>(ch)]; }
        };

        // Detect node name character
        struct node_name_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_node_name[static_cast<unsigned char>(ch)]; }
        };

        // Detect attribute name character
        struct attribute_name_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_attribute_name[static_cast<unsigned char>(ch)]; }
        };

        // Detect text character (PCDATA)
        struct text_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_text[static_cast<unsigned char>(ch)]; }
        };

        // Detect text character (PCDATA) that does not require processing
        struct text_pure_no_ws_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_text_pure_no_ws[static_cast<unsigned char>(ch)]; }
        };

        // Detect text character (PCDATA) that does not require processing
        struct text_pure_with_ws_pred
        {
            static unsigned char test(Ch ch) { return internal::lookup_tables<0>::lookup_text_pure_with_ws[static_cast<unsigned char>(ch)]; }
        };

        // Detect attribute value character
        template <Ch Quote>
        struct attribute_value_pred
        {
            static unsigned char test(Ch ch)
            {
                if (Quote == Ch('\'')) return internal::lookup_tables<0>::lookup_attribute_data_1[static_cast<unsigned char>(ch)];
                if (Quote == Ch('\"')) return internal::lookup_tables<0>::lookup_attribute_data_2[static_cast<unsigned char>(ch)];
                return 0; // Should never be executed, to avoid warnings on Comeau
            }
        };

        // Detect attribute value character
        template <Ch Quote>
        struct attribute_value_pure_pred
        {
            static unsigned char test(Ch ch)
            {
                if (Quote == Ch('\'')) return internal::lookup_tables<0>::lookup_attribute_data_1_pure[static_cast<unsigned char>(ch)];
                if (Quote == Ch('\"')) return internal::lookup_tables<0>::lookup_attribute_data_2_pure[static_cast<unsigned char>(ch)];
                return 0; // Should never be executed, to avoid warnings on Comeau
            }
        };

        // Insert coded character, using UTF8 or 8-bit ASCII
        // template <int Flags>
        static __fst::error_result insert_coded_character(const Ch*& text, unsigned long code)
        {
            //if constexpr (Flags & parse_no_utf8)
            //{
            //    // Insert 8-bit ASCII character
            //    // Todo: possibly verify that code is less than 256 and use replacement char otherwise?
            //    text[0] = (Ch) static_cast<unsigned char>(code);
            //    text += 1;
            //}
            //else
            {
                // Insert UTF8 sequence
                if (code < 0x80) // 1 byte sequence
                {
                    text[0] = (Ch) static_cast<unsigned char>(code);
                    text += 1;
                }
                else if (code < 0x800) // 2 byte sequence
                {
                    text[1] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[0] = (Ch) static_cast<unsigned char>(code | 0xC0);
                    text += 2;
                }
                else if (code < 0x10000) // 3 byte sequence
                {
                    text[2] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[1] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[0] = (Ch) static_cast<unsigned char>(code | 0xE0);
                    text += 3;
                }
                else if (code < 0x110000) // 4 byte sequence
                {
                    text[3] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[2] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[1] = (Ch) static_cast<unsigned char>((code | 0x80) & 0xBF);
                    code >>= 6;
                    text[0] = (Ch) static_cast<unsigned char>(code | 0xF0);
                    text += 4;
                }
                else // Invalid, only codes up to 0x10FFFF are allowed in Unicode
                {
                    RAPIDXML_PARSE_ERROR("invalid numeric character entity", text);
                    return __fst::status_code::unknown;
                }
            }

            return __fst::status_code::success;
        }

        // Skip characters until predicate evaluates to true
        template <class StopPred>
        static void skip(const Ch*& text)
        {
            const Ch* tmp = text;
            while (StopPred::test(*tmp))
                ++tmp;
            text = tmp;
        }

        // Skip characters until predicate evaluates to true while doing the following:
        // - replacing XML character entity references with proper characters (&apos; &amp; &quot; &lt; &gt; &#...;)
        // - condensing whitespace sequences to single space character
        template <class StopPred, class StopPredPure>
        static const Ch* skip_and_expand_character_refs(const Ch*& text)
        {
            // If entity translation, whitespace condense and whitespace trimming is disabled, use plain skip
            /*if constexpr (Flags & parse_no_entity_translation && !(Flags & parse_normalize_whitespace) && !(Flags & parse_trim_whitespace))
            {
                skip<StopPred>(text);
                return text;
            }*/
            //if constexpr (!(Flags & parse_normalize_whitespace) && !(Flags & parse_trim_whitespace))
            {
                skip<StopPred>(text);
                return text;
            }

            //// Use simple skip until first modification is detected
            //skip<StopPredPure>(text);

            //// Use translation skip
            //const Ch* src = text;
            //const Ch* dest = src;
            //while (StopPred::test(*src))
            //{
            //    //// If entity translation is enabled
            //    //if (!(Flags & parse_no_entity_translation))
            //    //{
            //    //    // Test if replacement is needed
            //    //    if (src[0] == Ch('&'))
            //    //    {
            //    //        switch (src[1])
            //    //        {

            //    //        // &amp; &apos;
            //    //        case Ch('a'):
            //    //            if (src[2] == Ch('m') && src[3] == Ch('p') && src[4] == Ch(';'))
            //    //            {
            //    //                *dest = Ch('&');
            //    //                ++dest;
            //    //                src += 5;
            //    //                continue;
            //    //            }
            //    //            if (src[2] == Ch('p') && src[3] == Ch('o') && src[4] == Ch('s') && src[5] == Ch(';'))
            //    //            {
            //    //                *dest = Ch('\'');
            //    //                ++dest;
            //    //                src += 6;
            //    //                continue;
            //    //            }
            //    //            break;

            //    //        // &quot;
            //    //        case Ch('q'):
            //    //            if (src[2] == Ch('u') && src[3] == Ch('o') && src[4] == Ch('t') && src[5] == Ch(';'))
            //    //            {
            //    //                *dest = Ch('"');
            //    //                ++dest;
            //    //                src += 6;
            //    //                continue;
            //    //            }
            //    //            break;

            //    //        // &gt;
            //    //        case Ch('g'):
            //    //            if (src[2] == Ch('t') && src[3] == Ch(';'))
            //    //            {
            //    //                *dest = Ch('>');
            //    //                ++dest;
            //    //                src += 4;
            //    //                continue;
            //    //            }
            //    //            break;

            //    //        // &lt;
            //    //        case Ch('l'):
            //    //            if (src[2] == Ch('t') && src[3] == Ch(';'))
            //    //            {
            //    //                *dest = Ch('<');
            //    //                ++dest;
            //    //                src += 4;
            //    //                continue;
            //    //            }
            //    //            break;

            //    //        // &#...; - assumes ASCII
            //    //        case Ch('#'):
            //    //            if (src[2] == Ch('x'))
            //    //            {
            //    //                unsigned long code = 0;
            //    //                src += 3; // Skip &#x
            //    //                while (1)
            //    //                {
            //    //                    unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
            //    //                    if (digit == 0xFF) break;
            //    //                    code = code * 16 + digit;
            //    //                    ++src;
            //    //                }
            //    //                er = insert_coded_character(dest, code); // Put character in output
            //    //                if (er) { return 0; }
            //    //            }
            //    //            else
            //    //            {
            //    //                unsigned long code = 0;
            //    //                src += 2; // Skip &#
            //    //                while (1)
            //    //                {
            //    //                    unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
            //    //                    if (digit == 0xFF) break;
            //    //                    code = code * 10 + digit;
            //    //                    ++src;
            //    //                }
            //    //                er = insert_coded_character(dest, code); // Put character in output
            //    //                if (er) { return 0; }
            //    //            }
            //    //            if (*src == Ch(';'))
            //    //                ++src;
            //    //            else
            //    //            {
            //    //                RAPIDXML_PARSE_ERROR("expected ;", src);
            //    //                er = __fst::status_code::unknown;
            //    //                return 0;
            //    //            }
            //    //            continue;

            //    //        // Something else
            //    //        default:
            //    //            // Ignore, just copy '&' verbatim
            //    //            break;
            //    //        }
            //    //    }
            //    //}

            //    // If whitespace condensing is enabled
            //    if constexpr (Flags & parse_normalize_whitespace)
            //    {
            //        // Test if condensing is needed
            //        if (whitespace_pred::test(*src))
            //        {
            //            *dest = Ch(' ');
            //            ++dest; // Put single space in dest
            //            ++src; // Skip first whitespace char
            //            // Skip remaining whitespace chars
            //            while (whitespace_pred::test(*src))
            //                ++src;
            //            continue;
            //        }
            //    }

            //    // No replacement, only copy character
            //    *dest++ = *src++;
            //}

            //// Return new end
            //text = src;
            //return dest;
        }

        ///////////////////////////////////////////////////////////////////////
        // Internal parsing functions

        // Parse BOM, if any

        void parse_bom(const Ch*& text)
        {
            // UTF-8?
            if (static_cast<unsigned char>(text[0]) == 0xEF && static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF)
            {
                text += 3; // Skup utf-8 bom
            }
        }

        // Parse XML declaration (<?xml...)
        basic_xml_node<Ch>* parse_xml_declaration(const Ch*& text, __fst::error_result& er)
        {
            // If parsing of declaration is disabled
            //if constexpr (!(Flags & parse_declaration_node))
            {
                // Skip until end of declaration
                while (text[0] != Ch('?') || text[1] != Ch('>'))
                {
                    if (!text[0])
                    {
                        RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                        er = __fst::status_code::unknown;
                        return nullptr;
                    }
                    ++text;
                }
                text += 2; // Skip '?>'
                return nullptr;
            }

            //// Create declaration
            //basic_xml_node<Ch>* declaration = this->allocate_node(declaration);

            //// Skip whitespace before attributes or ?>
            //skip<whitespace_pred>(text);

            //// Parse declaration attributes
            //er = parse_node_attributes(text, declaration);
            //if (er) { return nullptr; }

            //// Skip ?>
            //if (text[0] != Ch('?') || text[1] != Ch('>'))
            //{
            //    RAPIDXML_PARSE_ERROR("expected ?>", text);
            //    er = __fst::status_code::unknown;
            //    return nullptr;
            //}
            //text += 2;

            //return declaration;
        }

        // Parse XML comment (<!--...)
        basic_xml_node<Ch>* parse_comment(const Ch*& text, __fst::error_result& er)
        {
            // We're not parsing comment.

            // Skip until end of comment
            while (text[0] != Ch('-') || text[1] != Ch('-') || text[2] != Ch('>'))
            {
                if (!text[0])
                {
                    RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                    er = __fst::status_code::unknown;
                    return nullptr;
                }
                ++text;
            }
            text += 3; // Skip '-->'
            return nullptr; // Do not produce comment node
        }

        // Parse DOCTYPE
        basic_xml_node<Ch>* parse_doctype(const Ch*& text, __fst::error_result& er)
        {
            // Remember value start
            //const Ch* value = text;

            // Skip to >
            while (*text != Ch('>'))
            {
                // Determine character type
                switch (*text)
                {

                // If '[' encountered, scan for matching ending ']' using naive algorithm with depth
                // This works for all W3C test files except for 2 most wicked
                case Ch('['): {
                    ++text; // Skip '['
                    int depth = 1;
                    while (depth > 0)
                    {
                        switch (*text)
                        {
                        case Ch('['): ++depth; break;
                        case Ch(']'): --depth; break;
                        case 0: {
                            RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                            er = __fst::status_code::unknown;
                            return nullptr;
                        }
                        }
                        ++text;
                    }
                    break;
                }

                // Error on end of text
                case Ch('\0'): {
                    RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                    er = __fst::status_code::unknown;
                    return nullptr;
                }

                // Other character, skip it
                default: ++text;
                }
            }

            // If DOCTYPE nodes enabled
            //if constexpr (Flags & parse_doctype_node)
            //{
            //    // Create a new doctype node
            //    basic_xml_node<Ch>* doctype = this->allocate_node(doctype);
            //    doctype->value(value, text - value);

            //    text += 1; // skip '>'
            //    return doctype;
            //}
            //else
            {
                text += 1; // skip '>'
                return nullptr;
            }
        }

        // Parse PI
        basic_xml_node<Ch>* parse_pi(const Ch*& text, __fst::error_result& er)
        {
            // If creation of PI nodes is enabled
            //if constexpr (Flags & parse_pi_nodes)
            {
                // Create pi node
                basic_xml_node<Ch>* ppi = this->allocate_node(xml_node_type::pi);

                // Extract PI target name
                const Ch* name = text;
                skip<node_name_pred>(text);
                if (text == name)
                {
                    RAPIDXML_PARSE_ERROR("expected PI target", text);
                    er = __fst::status_code::unknown;
                    return nullptr;
                }
                ppi->name(name, (size_t) (text - name));

                // Skip whitespace between pi target and pi
                skip<whitespace_pred>(text);

                // Remember start of pi
                const Ch* value = text;

                // Skip to '?>'
                while (text[0] != Ch('?') || text[1] != Ch('>'))
                {
                    if (*text == Ch('\0'))
                    {
                        RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                        er = __fst::status_code::unknown;
                        return nullptr;
                    }
                    ++text;
                }

                // Set pi value (verbatim, no entity expansion or whitespace normalization)
                ppi->value(value, (size_t) (text - value));

                text += 2; // Skip '?>'
                return ppi;
            }
            //else
            //{
            //    // Skip to '?>'
            //    while (text[0] != Ch('?') || text[1] != Ch('>'))
            //    {
            //        if (*text == Ch('\0'))
            //        {
            //            RAPIDXML_PARSE_ERROR("unexpected end of data", text);
            //            er = __fst::status_code::unknown;
            //            return nullptr;
            //        }
            //        ++text;
            //    }
            //    text += 2; // Skip '?>'
            //    return nullptr;
            //}
        }

        // Parse and append data
        // Return character that ends data.
        // This is necessary because this character might have been overwritten by a terminating 0
        Ch parse_and_append_data(basic_xml_node<Ch>* node, const Ch*& text, const Ch* contents_start)
        {
            // Backup to contents start if whitespace trimming is disabled
            //if constexpr (!(Flags & parse_trim_whitespace))
            {
                text = contents_start;
            }

            // Skip until end of data
            const Ch *value = text, *end;
            //if constexpr (Flags & parse_normalize_whitespace) { end = skip_and_expand_character_refs<text_pred, text_pure_with_ws_pred>(text, er); }
            {
                end = skip_and_expand_character_refs<text_pred, text_pure_no_ws_pred>(text);
            }

            //if (er) { return 0; }

            // Trim trailing whitespace if flag is set; leading was already trimmed by whitespace skip after >
            //if constexpr (Flags & parse_trim_whitespace)
            //{
            //    //if constexpr (Flags & parse_normalize_whitespace)
            //    //{
            //    //    // Whitespace is already condensed to single space characters by skipping function, so just trim 1 char off the end
            //    //    if (*(end - 1) == Ch(' ')) --end;
            //    //}
            //    //else
            //    //{
            //    //    // Backup until non-whitespace character is found
            //    //    while (whitespace_pred::test(*(end - 1)))
            //    //        --end;
            //    //}
            //}

            // If characters are still left between end and value (this test is only necessary if normalization is enabled)
            // Create new data node
            //if constexpr (!(Flags & parse_no_data_nodes))
            {
                basic_xml_node<Ch>* data = this->allocate_node(xml_node_type::data);
                data->value(value, (size_t) (end - value));
                node->append_node(data);
            }

            // Add data to parent node if no data exists yet
            //if constexpr (!(Flags & parse_no_element_values))
            {
                if (*node->value().data() == Ch('\0')) { node->value(value, (size_t) (end - value)); }
            }

            // Return character that ends data
            return *text;
        }

        // Parse CDATA
        basic_xml_node<Ch>* parse_cdata(const Ch*& text, __fst::error_result& er)
        {
            // If CDATA is disabled
            //if constexpr (Flags & parse_no_data_nodes)
            //{
            //    // Skip until end of cdata
            //    while (text[0] != Ch(']') || text[1] != Ch(']') || text[2] != Ch('>'))
            //    {
            //        if (!text[0])
            //        {
            //            RAPIDXML_PARSE_ERROR("unexpected end of data", text);
            //            er = __fst::status_code::unknown;
            //            return nullptr;
            //        }
            //        ++text;
            //    }
            //    text += 3; // Skip ]]>
            //    return 0; // Do not produce CDATA node
            //}

            // Skip until end of cdata
            const Ch* value = text;
            while (text[0] != Ch(']') || text[1] != Ch(']') || text[2] != Ch('>'))
            {
                if (!text[0])
                {
                    RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                    er = __fst::status_code::unknown;
                    return nullptr;
                }
                ++text;
            }

            // Create new cdata node
            basic_xml_node<Ch>* cdata = this->allocate_node(xml_node_type::cdata);
            cdata->value(value, (size_t) (text - value));

            text += 3; // Skip ]]>
            return cdata;
        }

        // Parse element node
        basic_xml_node<Ch>* parse_element(const Ch*& text, __fst::error_result& er)
        {
            // Create element node
            basic_xml_node<Ch>* element = this->allocate_node(xml_node_type::element);

            // Extract element name
            const Ch* name = text;
            skip<node_name_pred>(text);
            if (text == name)
            {
                RAPIDXML_PARSE_ERROR("expected element name", text);
                er = __fst::status_code::unknown;
                return nullptr;
            }
            element->name(name, (size_t) (text - name));

            // Skip whitespace between element name and attributes or >
            skip<whitespace_pred>(text);

            // Parse attributes, if any
            if (er = parse_node_attributes(text, element)) { return nullptr; }

            // Determine ending type
            if (*text == Ch('>'))
            {
                ++text;
                if (er = parse_node_contents(text, element)) { return nullptr; }
            }
            else if (*text == Ch('/'))
            {
                ++text;
                if (*text != Ch('>'))
                {
                    RAPIDXML_PARSE_ERROR("expected >", text);
                    er = __fst::status_code::unknown;
                    return nullptr;
                }
                ++text;
            }
            else
            {
                RAPIDXML_PARSE_ERROR("expected >", text);
                er = __fst::status_code::unknown;
                return nullptr;
            }

            // Return parsed element
            return element;
        }

        // Determine node type, and parse it
        basic_xml_node<Ch>* parse_node(const Ch*& text, __fst::error_result& er)
        {
            // Parse proper node type
            switch (text[0])
            {

            // <...
            default:
                // Parse and append element node
                return parse_element(text, er);

            // <?...
            case Ch('?'):
                ++text; // Skip ?

                if ((text[0] == Ch('x') || text[0] == Ch('X')) && //
                    (text[1] == Ch('m') || text[1] == Ch('M')) && //
                    (text[2] == Ch('l') || text[2] == Ch('L')) && //
                    whitespace_pred::test(text[3]))
                {
                    // '<?xml ' - xml declaration
                    text += 4; // Skip 'xml '
                    return parse_xml_declaration(text, er);
                }
                else
                {
                    // Parse PI
                    return parse_pi(text, er);
                }

            // <!...
            case Ch('!'):

                // Parse proper subset of <! node
                switch (text[1])
                {

                // <!-
                case Ch('-'):
                    if (text[2] == Ch('-'))
                    {
                        // '<!--' - xml comment
                        text += 3; // Skip '!--'
                        return parse_comment(text, er);
                    }
                    break;

                // <![
                case Ch('['):
                    if (text[2] == Ch('C') && text[3] == Ch('D') && text[4] == Ch('A') && text[5] == Ch('T') && text[6] == Ch('A') && text[7] == Ch('['))
                    {
                        // '<![CDATA[' - cdata
                        text += 8; // Skip '![CDATA['
                        return parse_cdata(text, er);
                    }
                    break;

                // <!D
                case Ch('D'):
                    if (text[2] == Ch('O') && text[3] == Ch('C') && text[4] == Ch('T') && text[5] == Ch('Y') && text[6] == Ch('P') && text[7] == Ch('E')
                        && whitespace_pred::test(text[8]))
                    {
                        // '<!DOCTYPE ' - doctype
                        text += 9; // skip '!DOCTYPE '
                        return parse_doctype(text, er);
                    }

                } // switch

                // Attempt to skip other, unrecognized node types starting with <!
                ++text; // Skip !
                while (*text != Ch('>'))
                {
                    if (*text == 0)
                    {
                        RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                        er = __fst::status_code::unknown;
                        return nullptr;
                    }
                    ++text;
                }
                ++text; // Skip '>'
                return nullptr; // No node recognized
            }
        }

        // Parse contents of the node - children, data etc.
        __fst::error_result parse_node_contents(const Ch*& text, basic_xml_node<Ch>* node)
        {
            __fst::error_result er;

            // For all children and text
            while (1)
            {
                // Skip whitespace between > and node contents
                const Ch* contents_start = text; // Store start of node contents before whitespace is skipped
                skip<whitespace_pred>(text);
                Ch next_char = *text;

            // After data nodes, instead of continuing the loop, control jumps here.
            // This is because zero termination inside parse_and_append_data() function
            // would wreak havoc with the above code.
            // Also, skipping whitespace after data nodes is unnecessary.
            after_data_node:

                // Determine what comes next: node closing, child node, data node, or 0?
                switch (next_char)
                {

                // Node closing or child node
                case Ch('<'):
                    if (text[1] == Ch('/'))
                    {
                        // Node closing
                        text += 2; // Skip '</'
                        //if constexpr (Flags & parse_validate_closing_tags)
                        //{
                        //    // Skip and validate closing tag name
                        //    const Ch* closing_name = text;
                        //    skip<node_name_pred>(text);

                        //    if (!internal::compare(node->name().data(), node->name_size(), closing_name, text - closing_name, true))
                        //    {
                        //        RAPIDXML_PARSE_ERROR("invalid closing tag name", text);
                        //        return __fst::status_code::unknown;
                        //    }
                        //}
                        //else
                        {
                            // No validation, just skip name
                            skip<node_name_pred>(text);
                        }
                        // Skip remaining whitespace after node name
                        skip<whitespace_pred>(text);
                        if (*text != Ch('>'))
                        {
                            RAPIDXML_PARSE_ERROR("expected >", text);
                            return __fst::status_code::unknown;
                        }
                        ++text; // Skip '>'
                        return __fst::status_code::success; // Node closed, finished parsing contents
                    }
                    else
                    {
                        // Child node
                        ++text; // Skip '<'
                        if (basic_xml_node<Ch>* child = parse_node(text, er); child && !er) { node->append_node(child); }
                        if (er) { return er; }
                    }
                    break;

                // End of data - error
                case Ch('\0'): {
                    RAPIDXML_PARSE_ERROR("unexpected end of data", text);
                    return __fst::status_code::unknown;
                }

                // Data node
                default: {
                    next_char = parse_and_append_data(node, text, contents_start);
                    //if (er) { return er; }
                    goto after_data_node; // Bypass regular processing after data nodes
                }
                }
            }
        }

        // Parse XML attributes of the node
        __fst::error_result parse_node_attributes(const Ch*& text, basic_xml_node<Ch>* node)
        {
            //__fst::error_result er;

            // For all attributes
            while (attribute_name_pred::test(*text))
            {
                // Extract attribute name
                const Ch* name = text;
                ++text; // Skip first character of attribute name
                skip<attribute_name_pred>(text);
                if (text == name)
                {
                    RAPIDXML_PARSE_ERROR("expected attribute name", name);
                    return __fst::status_code::unknown;
                }

                // Create new attribute
                basic_xml_attribute<Ch>* attribute = this->allocate_attribute();
                attribute->name(name, (size_t) (text - name));
                node->append_attribute(attribute);

                // Skip whitespace after attribute name
                skip<whitespace_pred>(text);

                // Skip =
                if (*text != Ch('='))
                {
                    RAPIDXML_PARSE_ERROR("expected =", text);
                    return __fst::status_code::unknown;
                }
                ++text;

                // Skip whitespace after =
                skip<whitespace_pred>(text);

                // Skip quote and remember if it was ' or "
                Ch quote = *text;
                if (quote != Ch('\'') && quote != Ch('"'))
                {
                    RAPIDXML_PARSE_ERROR("expected ' or \"", text);
                    return __fst::status_code::unknown;
                }
                ++text;

                // Extract attribute value and expand char refs in it
                const Ch *value = text, *end;
                //const int AttFlags = Flags & ~parse_normalize_whitespace; // No whitespace normalization in attributes
                if (quote == Ch('\'')) { end = skip_and_expand_character_refs<attribute_value_pred<Ch('\'')>, attribute_value_pure_pred<Ch('\'')>>(text); }
                else { end = skip_and_expand_character_refs<attribute_value_pred<Ch('"')>, attribute_value_pure_pred<Ch('"')>>(text); }

                //if (er) { return er; }
                // Set attribute value
                attribute->value(value, (size_t) (end - value));

                // Make sure that end quote is present
                if (*text != quote)
                {
                    RAPIDXML_PARSE_ERROR("expected ' or \"", text);
                    return __fst::status_code::unknown;
                }
                ++text; // Skip quote

                // Skip whitespace after attribute value
                skip<whitespace_pred>(text);
            }
            return __fst::status_code::success;
        }
    };

    /// \cond internal
    namespace internal
    {

        // Whitespace (space \n \r \t)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_whitespace[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, // 0
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // F
        };

        // Node name (anything but space \n \r \t / > ? \0)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_node_name[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Text (i.e. PCDATA) (anything but < \0)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_text[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Text (i.e. PCDATA) that does not require processing when ws normalization is disabled
        // (anything but < \0 &)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_text_pure_no_ws[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Text (i.e. PCDATA) that does not require processing when ws normalizationis is enabled
        // (anything but < \0 & space \n \r \t)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_text_pure_with_ws[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Attribute name (anything but space \n \r \t / < > = ? ! \0)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_attribute_name[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Attribute data with single quote (anything but ' \0)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Attribute data with single quote that does not require processing (anything but ' \0 &)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1_pure[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Attribute data with double quote (anything but " \0)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        // Attribute data with double quote that does not require processing (anything but " \0 &)
        template <int Dummy>
        const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2_pure[256] = {
            // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
            1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // F
        };

        //// Digits (dec and hex, 255 denotes end of numeric character reference)
        //template <int Dummy>
        //const unsigned char lookup_tables<Dummy>::lookup_digits[256] = {
        //    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 1
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 2
        //    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 255, 255, 255, 255, 255, 255, // 3
        //    255, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 4
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 5
        //    255, 10, 11, 12, 13, 14, 15, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 6
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 7
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 8
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 9
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // A
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // B
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // C
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // D
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // E
        //    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 // F
        //};

        // Upper case conversion
        //template <int Dummy>
        //const unsigned char lookup_tables<Dummy>::lookup_upcase[256] = {
        //    // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A   B   C   D   E   F
        //    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // 0
        //    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, // 1
        //    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, // 2
        //    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, // 3
        //    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, // 4
        //    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, // 5
        //    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, // 6
        //    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127, // 7
        //    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, // 8
        //    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, // 9
        //    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, // A
        //    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, // B
        //    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, // C
        //    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, // D
        //    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, // E
        //    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 // F
        //};
    } // namespace internal
    /// \endcond

    ///////////////////////////////////////////////////////////////////////
    // Printing flags

    const int print_no_indenting = 0x1; //!< Printer flag instructing the printer to suppress indenting of XML. See print() function.

    ///////////////////////////////////////////////////////////////////////
    // Internal

    //! \cond internal
    namespace internal
    {

        ///////////////////////////////////////////////////////////////////////////
        // Internal character operations

        // Copy characters from given range to given output iterator
        template <class OutIt, class Ch>
        inline OutIt copy_chars(const Ch* begin, const Ch* end, OutIt out)
        {
            while (begin != end)
            {
                *out++ = *begin++;
            }
            return out;
        }

        // Copy characters from given range to given output iterator and expand
        // characters into references (&lt; &gt; &apos; &quot; &amp;)
        template <class OutIt, class Ch>
        inline OutIt copy_and_expand_chars(const Ch* begin, const Ch* end, Ch noexpand, OutIt out)
        {
            while (begin != end)
            {
                if (*begin == noexpand)
                {
                    *out++ = *begin; // No expansion, copy character
                }
                else
                {
                    switch (*begin)
                    {
                    case Ch('<'):
                        *out++ = Ch('&');
                        *out++ = Ch('l');
                        *out++ = Ch('t');
                        *out++ = Ch(';');
                        break;
                    case Ch('>'):
                        *out++ = Ch('&');
                        *out++ = Ch('g');
                        *out++ = Ch('t');
                        *out++ = Ch(';');
                        break;
                    case Ch('\''):
                        *out++ = Ch('&');
                        *out++ = Ch('a');
                        *out++ = Ch('p');
                        *out++ = Ch('o');
                        *out++ = Ch('s');
                        *out++ = Ch(';');
                        break;
                    case Ch('"'):
                        *out++ = Ch('&');
                        *out++ = Ch('q');
                        *out++ = Ch('u');
                        *out++ = Ch('o');
                        *out++ = Ch('t');
                        *out++ = Ch(';');
                        break;
                    case Ch('&'):
                        *out++ = Ch('&');
                        *out++ = Ch('a');
                        *out++ = Ch('m');
                        *out++ = Ch('p');
                        *out++ = Ch(';');
                        break;
                    default: *out++ = *begin; // No expansion, copy character
                    }
                }
                ++begin; // Step to next character
            }
            return out;
        }

        // Fill given output iterator with repetitions of the same character
        template <class OutIt, class Ch>
        inline OutIt fill_chars(OutIt out, int n, Ch ch)
        {
            for (int i = 0; i < n; ++i)
            {
                *out++ = ch;
            }

            return out;
        }

        // Find character
        template <class Ch, Ch ch>
        inline bool find_char(const Ch* begin, const Ch* end)
        {
            while (begin != end)
            {
                if (*begin++ == ch) { return true; }
            }

            return false;
        }

        ///////////////////////////////////////////////////////////////////////////
        // Internal printing operations
        template <class OutIt, class Ch>
        inline OutIt print_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent);

        // Print children of the node
        template <class OutIt, class Ch>
        inline OutIt print_children(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            for (basic_xml_node<Ch>* child = node->first_node(); child; child = child->next_sibling())
            {
                out = print_node(out, child, flags, indent);
            }

            return out;
        }

        // Print attributes of the node
        template <class OutIt, class Ch>
        inline OutIt print_attributes(OutIt out, const basic_xml_node<Ch>* node)
        {
            for (basic_xml_attribute<Ch>* attribute = node->first_attribute(); attribute; attribute = attribute->next_attribute())
            {
                if (!attribute->name().empty() && !attribute->value().empty())
                {
                    // Print attribute name
                    *out = Ch(' '), ++out;
                    out = copy_chars(attribute->name().begin(), attribute->name().end(), out);
                    *out = Ch('='), ++out;
                    // Print attribute value using appropriate quote type
                    if (find_char<Ch, Ch('"')>(attribute->value().begin(), attribute->value().end()))
                    {
                        *out = Ch('\''), ++out;
                        out = copy_and_expand_chars(attribute->value().begin(), attribute->value().end(), Ch('"'), out);
                        *out = Ch('\''), ++out;
                    }
                    else
                    {
                        *out = Ch('"'), ++out;
                        out = copy_and_expand_chars(attribute->value().begin(), attribute->value().end(), Ch('\''), out);
                        *out = Ch('"'), ++out;
                    }
                }
            }
            return out;
        }

        // Print data node
        template <class OutIt, class Ch>
        inline OutIt print_data_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::data);

            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            out = copy_and_expand_chars(node->value().begin(), node->value().end(), Ch(0), out);
            return out;
        }

        // Print data node
        template <class OutIt, class Ch>
        inline OutIt print_cdata_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::cdata);

            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<');
            ++out;
            *out = Ch('!');
            ++out;
            *out = Ch('[');
            ++out;
            *out = Ch('C');
            ++out;
            *out = Ch('D');
            ++out;
            *out = Ch('A');
            ++out;
            *out = Ch('T');
            ++out;
            *out = Ch('A');
            ++out;
            *out = Ch('[');
            ++out;
            out = copy_chars(node->value().begin(), node->value().end(), out);
            *out = Ch(']');
            ++out;
            *out = Ch(']');
            ++out;
            *out = Ch('>');
            ++out;
            return out;
        }

        // Print element node
        template <class OutIt, class Ch>
        inline OutIt print_element_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::element);

            // Print element name and attributes, if any
            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<'), ++out;
            out = copy_chars(node->name().begin(), node->name().end(), out);
            out = print_attributes(out, node);

            // If node is childless
            if (node->value_size() == 0 && !node->first_node())
            {
                // Print childless node tag ending
                *out = Ch('/'), ++out;
                *out = Ch('>'), ++out;
            }
            else
            {
                // Print normal node tag ending
                *out = Ch('>'), ++out;

                // Test if node contains a single data node only (and no other nodes)
                basic_xml_node<Ch>* child = node->first_node();
                if (!child)
                {
                    // If node has no children, only print its value without indenting
                    out = copy_and_expand_chars(node->value().begin(), node->value().end(), Ch(0), out);
                }
                else if (child->next_sibling() == 0 && child->type() == xml_node_type::data)
                {
                    // If node has a sole data child, only print its value without indenting
                    out = copy_and_expand_chars(child->value().begin(), child->value().end(), Ch(0), out);
                }
                else
                {
                    // Print all children with full indenting
                    if (!(flags & print_no_indenting)) *out = Ch('\n'), ++out;

                    out = print_children(out, node, flags, indent + 1);

                    if (!(flags & print_no_indenting)) out = fill_chars(out, indent, Ch('\t'));
                }

                // Print node end
                *out = Ch('<'), ++out;
                *out = Ch('/'), ++out;
                out = copy_chars(node->name().begin(), node->name().end(), out);
                *out = Ch('>'), ++out;
            }
            return out;
        }

        // Print declaration node
        template <class OutIt, class Ch>
        inline OutIt print_declaration_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            // Print declaration start
            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<'), ++out;
            *out = Ch('?'), ++out;
            *out = Ch('x'), ++out;
            *out = Ch('m'), ++out;
            *out = Ch('l'), ++out;

            // Print attributes
            out = print_attributes(out, node);

            // Print declaration end
            *out = Ch('?'), ++out;
            *out = Ch('>'), ++out;

            return out;
        }

        // Print comment node
        template <class OutIt, class Ch>
        inline OutIt print_comment_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::comment);

            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<'), ++out;
            *out = Ch('!'), ++out;
            *out = Ch('-'), ++out;
            *out = Ch('-'), ++out;
            out = copy_chars(node->value().begin(), node->value().end(), out);
            *out = Ch('-'), ++out;
            *out = Ch('-'), ++out;
            *out = Ch('>'), ++out;
            return out;
        }

        // Print doctype node
        template <class OutIt, class Ch>
        inline OutIt print_doctype_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::doctype);

            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<'), ++out;
            *out = Ch('!'), ++out;
            *out = Ch('D'), ++out;
            *out = Ch('O'), ++out;
            *out = Ch('C'), ++out;
            *out = Ch('T'), ++out;
            *out = Ch('Y'), ++out;
            *out = Ch('P'), ++out;
            *out = Ch('E'), ++out;
            *out = Ch(' '), ++out;
            out = copy_chars(node->value().begin(), node->value().end(), out);
            *out = Ch('>'), ++out;
            return out;
        }

        // Print pi node
        template <class OutIt, class Ch>
        inline OutIt print_pi_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            fst_assert(node->type() == xml_node_type::pi);

            if (!(flags & print_no_indenting)) { out = fill_chars(out, indent, Ch('\t')); }

            *out = Ch('<'), ++out;
            *out = Ch('?'), ++out;
            out = copy_chars(node->name().begin(), node->name().end(), out);
            *out = Ch(' '), ++out;
            out = copy_chars(node->value().begin(), node->value().end(), out);
            *out = Ch('?'), ++out;
            *out = Ch('>'), ++out;
            return out;
        }

        // Print node
        template <class OutIt, class Ch>
        inline OutIt print_node(OutIt out, const basic_xml_node<Ch>* node, int flags, int indent)
        {
            // Print proper node type
            switch (node->type())
            {

            // Document
            case xml_node_type::document: out = print_children(out, node, flags, indent); break;

            // Element
            case xml_node_type::element: out = print_element_node(out, node, flags, indent); break;

            // Data
            case xml_node_type::data: out = print_data_node(out, node, flags, indent); break;

            // CDATA
            case xml_node_type::cdata: out = print_cdata_node(out, node, flags, indent); break;

            // Declaration
            case xml_node_type::declaration: out = print_declaration_node(out, node, flags, indent); break;

            // Comment
            case xml_node_type::comment: out = print_comment_node(out, node, flags, indent); break;

            // Doctype
            case xml_node_type::doctype: out = print_doctype_node(out, node, flags, indent); break;

            // Pi
            case xml_node_type::pi:
                out = print_pi_node(out, node, flags, indent);
                break;

                // Unknown
            default: fst_error("error"); break;
            }

            // If indenting not disabled, add line break after node
            if (!(flags & print_no_indenting))
            {
                *out = Ch('\n');
                ++out;
            }

            // Return modified iterator
            return out;
        }
    } // namespace internal
    //! \endcond

    //********************************************************************************************************
    /// TODO: Slow as fuck
    //********************************************************************************************************

    ///////////////////////////////////////////////////////////////////////////
    // Printing

    //
    //! Prints XML to given output iterator.
    //! \param out Output iterator to print to.
    //! \param node Node to be printed. Pass xml_document to print entire document.
    //! \param flags Flags controlling how XML is printed.
    //! \return Output iterator pointing to position immediately after last character of printed text.
    template <class OutIt, class Ch>
    inline OutIt xml_print_output(OutIt out, const basic_xml_node<Ch>& node, int flags = 0)
    {
        return internal::print_node(out, &node, flags, 0);
    }

    //! Prints XML to given output stream.
    //! \param out Output stream to print to.
    //! \param node Node to be printed. Pass xml_document to print entire document.
    //! \param flags Flags controlling how XML is printed.
    //! \return Output stream.
    template <class Ch>
    inline __fst::output_stream<Ch>& xml_print(__fst::output_stream<Ch> & out, const basic_xml_node<Ch>& node, int flags = 0)
    {
        xml_print_output(__fst::output_stream_iterator<Ch>(out), node, flags);
        return out;
    }

    //! Prints formatted XML to given output stream. Uses default printing flags. Use print() function to customize printing process.
    //! \param out Output stream to print to.
    //! \param node Node to be printed.
    //! \return Output stream.
    template <class Ch>
    inline __fst::output_stream<Ch>& operator<<(__fst::output_stream<Ch>& out, const basic_xml_node<Ch>& node)
    {
        return xml_print(out, node);
    }

    ///
    template <class _CharT, class _MemoryCategory, class _MemoryZone>
    class basic_xml_stream
    {
      public:
        using document_type = __fst::basic_xml_document<_CharT, _MemoryCategory, _MemoryZone>;
        using node_type = __fst::basic_xml_node<_CharT>;

        inline basic_xml_stream(document_type& doc, node_type* node) noexcept
            : _doc(doc)
            , _node(node)
        {}

        basic_xml_stream(const basic_xml_stream&) = delete;
        basic_xml_stream(basic_xml_stream&&) = delete;

        ~basic_xml_stream() noexcept = default;

        basic_xml_stream& operator=(const basic_xml_stream&) = delete;
        basic_xml_stream& operator=(basic_xml_stream&&) = delete;

        inline document_type& document() noexcept { return _doc; }
        inline node_type* node() noexcept { return _node; }

      private:
        document_type& _doc;
        node_type* _node;
    };

    using xml_stream = __fst::basic_xml_stream<char, __fst::default_memory_category, __fst::default_memory_zone>;

FST_END_NAMESPACE

// Undefine internal macros
#undef RAPIDXML_PARSE_ERROR

FST_PRAGMA_POP()
