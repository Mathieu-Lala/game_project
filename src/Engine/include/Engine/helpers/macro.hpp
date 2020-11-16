#pragma once

#if defined(__has_cpp_attribute)
#    if __has_cpp_attribute(likely) && __has_cpp_attribute(unlikely)
#        define LIKELY   [[likely]]
#        define UNLIKELY [[unlikely]]
#    else
#        define LIKELY
#        define UNLIKELY
#    endif
#else
#    define LIKELY
#    define UNLIKELY
#endif
