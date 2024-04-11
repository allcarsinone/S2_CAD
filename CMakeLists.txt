cmake_minimum_required(VERSION 3.5)

project(jobshop LANGUAGES C)

find_package(OpenMP COMPONENTS C REQUIRED)

add_executable(jobshop main.c)
#add_executable(jobshop mainSimple.c)

target_link_libraries(jobshop PRIVATE OpenMP::OpenMP_C)

include(GNUInstallDirs)
install(TARGETS jobshop
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
