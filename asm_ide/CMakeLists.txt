cmake_minimum_required(VERSION 3.5)

project(asm_ide LANGUAGES C)

set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_C_FLAGS "-g -O0") # -fsanitize=address -fsanitize=undefined -fsanitize=leak  -fsanitize=shadow-call-stack


set (CPARSER_INCLUDE
    assembly.h
)

set (CPARSER_SRC
    assembly.c
)

set (SOURCES
    main.c
    )

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)


#include_directories(${GTK3_INCLUDE_DIRS})
#link_directories(${GTK3_LIBRARY_DIRS})

#include_directories(/usr/include/gtk-4.0 /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/cairo /usr/include/gtk-4.0/gdk /usr/include/pango-1.0 /usr/include/harfbuzz /usr/include/gdk-pixbuf-2.0 /usr/include/graphene-1.0 /usr/lib/x86_64-linux-gnu/graphene-1.0/include)

#include_directories(cparser) #  cparser/include cparser/include/parser cparser/include/ast cparser/include/builtin cparser/include/helper
include_directories(. /usr/lib/gcc/x86_64-linux-gnu/11/include)

#include_directories(cparser/ cparser/libfirm cparser/libfirm/include cparser/libfirm/gen/include/libfirm cparser/libfirm/gen/include cparser/include cparser/src)
#add_subdirectory(cparser)

add_executable(asm_ide ${SOURCES} ${CPARSER_INCLUDE} ${CPARSER_SRC})

target_link_libraries(asm_ide PkgConfig::GTK4 m gccjit) # ${GTK3_LIBRARIES} json-c
