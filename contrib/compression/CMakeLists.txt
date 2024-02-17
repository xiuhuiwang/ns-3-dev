check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/compression-examples-test-suite.cc
        )
endif()

option(NS3_PYTHON_BINDINGS "Build ns-3 python bindings" ON)

build_lib(
    LIBNAME compression
    SOURCE_FILES model/compression.cc
                 model/compression-net-device.cc
                 model/compression-header.cc
                 model/compression-channel.cc
                 model/compression-remote-channel.cc
                 helper/compression-helper.cc
    HEADER_FILES model/compression.h
                 model/compression-net-device.h
                 model/compression-header.h
                 model/compression-channel.h
                 model/compression-remote-channel.h
                 helper/compression-helper.h
    LIBRARIES_TO_LINK ${libcore}
                      ${libinternet}
                      ${libapplications}
                      ${libpoint-to-point}
                      ${libnetwork}
    TEST_SOURCES test/compression-test-suite.cc
                 ${examples_as_tests_sources}
)

find_package(ZLIB REQUIRED)

