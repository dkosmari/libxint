#ifndef XINT_STORAGE_HPP
#define XINT_STORAGE_HPP

#include <memory>
#include <utility>


#ifndef XINT_MAX_LOCAL_BYTES
#define XINT_MAX_LOCAL_BYTES 256
#endif


namespace xint {


    template<typename T>
    struct heap_storage {

        using data_type = T;

        heap_storage() = default;

        heap_storage(const heap_storage& other)
        {
            *ptr = *other.ptr;
        }

        heap_storage&
        operator =(const heap_storage& other)
            noexcept
        {
            *ptr = *other.ptr;
            return *this;
        }


        constexpr const data_type& operator  *() const noexcept { return *ptr; }
        constexpr       data_type& operator  *()       noexcept { return *ptr; }

        // constexpr const data_type* operator ->() const noexcept { return ptr->get(); }
        // constexpr       data_type* operator ->()       noexcept { return ptr->get(); }

        std::unique_ptr<data_type> ptr = std::make_unique<data_type>();
    };


    template<typename T>
    constexpr
    void
    swap(heap_storage<T>& a,
         heap_storage<T>& b)
        noexcept
    {
        using std::swap;
        swap(a.ptr, b.ptr);
    }



    template<typename T>
    struct local_storage {

        using data_type = T;

        constexpr const data_type& operator *() const  noexcept { return data; }
        constexpr       data_type& operator *()        noexcept { return data; }

        // constexpr const data_type* operator ->() const noexcept { return &data; }
        // constexpr       data_type* operator ->()       noexcept { return &data; }

        data_type data;

    };


    template<typename T>
    constexpr
    void
    swap(local_storage<T>& a,
         local_storage<T>& b)
        noexcept
    {
        using std::swap;
        swap(a.data, b.data);
    }

}

#endif
