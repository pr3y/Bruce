#include <cstdlib>
#include <limits>
#include <new>
#include <vector>

template <class T> struct Mallocator {
    typedef T value_type;

    Mallocator() = default;

    template <class U> constexpr Mallocator(const Mallocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) throw std::bad_array_new_length();

        if (auto p = static_cast<T *>(std::malloc(n * sizeof(T)))) {
            report(p, n);
            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T *p, std::size_t n) noexcept {
        report(p, n, 0);
        std::free(p);
    }

private:
    void report(T *p, std::size_t n, bool alloc = true) const {
        std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n << " bytes at " << std::hex
                  << std::showbase << reinterpret_cast<void *>(p) << std::dec << '\n';
    }
};
