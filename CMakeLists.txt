cmake_minimum_required(VERSION 3.5)

project(jobshop LANGUAGES C)

add_executable(jobshop main.c)

include(GNUInstallDirs)
install(TARGETS jobshop
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
