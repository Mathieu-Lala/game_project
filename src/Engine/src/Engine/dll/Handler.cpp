#include <utility>
#include "Engine/dll/Handler.hpp"

engine::dll::Handler::Handler() noexcept :
    m_handler   (EMPTY)
{ }

engine::dll::Handler::Handler(Handler &&o) noexcept :
    m_handler   (std::exchange(o.m_handler, EMPTY)),
    m_libpath   (std::move(o.m_libpath))
{ }

engine::dll::Handler &engine::dll::Handler::operator=(Handler &&o) noexcept
{
    this->m_handler = std::exchange(o.m_handler, EMPTY);
    this->m_libpath = std::move(o.m_libpath);
    return *this;
}

engine::dll::Handler::Handler(Path libpath) :
    m_handler   (EMPTY)
{
    this->open(std::move(libpath));
}

engine::dll::Handler::~Handler()
{
    this->close();
}

bool engine::dll::Handler::is_valid() const noexcept
{
    return this->m_handler != EMPTY;
}

void engine::dll::Handler::open(Path libpath)
{
    this->close();

    this->m_libpath = std::move(libpath);
#if defined(_WIN32)
    this->m_handler = ::LoadLibrary(this->m_libpath.c_str());
#else
    this->m_handler = ::dlopen(this->m_libpath.c_str(), RTLD_LAZY);
#endif

    if (!this->is_valid())
        throw error{ };
}

void engine::dll::Handler::close()
{
    if (!this->is_valid())
        return;

#if defined(_WIN32)
    auto ok = ::FreeLibrary(this->m_handler);
#else
    auto ok = !::dlclose(this->m_handler);
# endif

    this->m_handler = EMPTY;
    this->m_libpath = "";

    if (!ok)
        throw error{ };
}

const engine::dll::Handler::Path &engine::dll::Handler::getPath() const noexcept
{
    return this->m_libpath;
}

engine::dll::Handler::error::error(const std::string &msg) :
    std::runtime_error  (msg)
{ }

std::string engine::dll::Handler::error::getLastError()
{
#if defined(_WIN32)
    const auto id = ::GetLastError();
    if (!id)
        return "";
    LPSTR buffer = nullptr;
    const auto size = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, id,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&buffer), 0, NULL
    );
    std::string message(buffer, size);
    ::LocalFree(buffer);
    return message;
#else
    return ::dlerror();
#endif
}
