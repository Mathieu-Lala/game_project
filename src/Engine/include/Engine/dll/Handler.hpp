#pragma once

#include <string>
#include <stdexcept>

#if defined(_WIN32)
#    define WIN32_MEAN_AND_LEAN
#    include <windows.h>
#else
#    include <dlfcn.h>
#endif

namespace engine {

namespace dll {

class Handler {
public:
    using Path = std::string;

    Handler() noexcept;
    Handler(Handler &&) noexcept;
    Handler &operator=(Handler &&) noexcept;
    explicit Handler(Path libpath);

    Handler(const Handler &) = delete;
    Handler &operator=(const Handler &) = delete;

    ~Handler();

    bool is_valid() const noexcept;

    void open(Path libpath);
    void close();

    template<typename T>
    T load(const std::string_view symbol_name) const
    {
#if defined(_WIN32)
        auto symbol = ::GetProcAddress(this->m_handler, symbol_name.data());
#else
        auto symbol = ::dlsym(this->m_handler, symbol_name.data());
#endif
        if (!symbol) throw error{};

        return reinterpret_cast<T>(symbol);
    }

    const Path &getPath() const noexcept;

    class error : public std::runtime_error {
    public:
        explicit error(const std::string &msg = getLastError());
        virtual ~error() = default;

        static std::string getLastError();
    };

private:
#if defined(_WIN32)
    using raw_t = HINSTANCE;
    static constexpr raw_t EMPTY = nullptr;
#else
    using raw_t = void *;
    static constexpr raw_t EMPTY = nullptr;
#endif

    raw_t m_handler;
    Path m_libpath;
};

} // namespace dll

} // namespace engine
