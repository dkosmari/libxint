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

        constexpr const data_type* operator ->() const noexcept { return ptr->get(); }
        constexpr       data_type& operator ->()       noexcept { return ptr->get(); }

        std::unique_ptr<data_type> ptr = std::make_unique<data_type>();
    };


    template<typename T>
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

        constexpr const data_type& operator ->() const noexcept { return &data; }
        constexpr       data_type& operator ->()       noexcept { return &data; }

        data_type data;

    };


    template<typename T>
    void
    swap(local_storage<T>& a,
         local_storage<T>& b)
        noexcept
    {
        using std::swap;
        swap(a.data, b.data);
    }


    template<typename T>
    struct auto_storage :
        std::conditional_t<sizeof(T) <= XINT_MAX_LOCAL_BYTES,
                           local_storage<T>,
                           heap_storage<T>> {

        static inline constexpr bool is_local = sizeof(T) <= XINT_MAX_LOCAL_BYTES;
        using base_type = std::conditional_t<is_local,
                                             local_storage<T>,
                                             heap_storage<T>>;

    };


    template<typename T>
    void
    swap(auto_storage<T>& a,
         auto_storage<T>& b)
        noexcept
    {
        using base_t = auto_storage<T>::base_type;
        return swap(static_cast<base_t&>(a),
                    static_cast<base_t&>(b));
    }

}

#endif
