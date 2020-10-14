#pragma once

// todo : move this in config file

#ifdef _MSC_VER // compiling with visual studio
#    define DATA_DIR "../../../../data/"
#else
#    define DATA_DIR "./data/"
#endif // _MSC_VER // compiling with visual studio
