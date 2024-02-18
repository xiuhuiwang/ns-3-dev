check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/experiment-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME experiment
    SOURCE_FILES model/experiment.cc
                 helper/experiment-helper.cc
                 model/exp-data-header.cc
                 model/seq-header.cc
                 model/compression-app-layer.cc
                 model/compression-packet-gen.cc
                 model/compression-sender.cc
                 model/compression-receiver.cc
                 helper/compression-sender-receiver-helper.cc
    HEADER_FILES model/experiment.h
                 helper/experiment-helper.h
                 model/exp-data-header.h
                 model/seq-header.h
                 model/compression-app-layer.h
                 model/compression-packet-gen.h
                 model/compression-sender.h
                 model/compression-receiver.h
                 helper/compression-sender-receiver-helper.h
    LIBRARIES_TO_LINK ${libcore}
                      ${libinternet}
                      ${libapplications}
                      ${libpoint-to-point}
                      ${libnetwork}
    TEST_SOURCES test/experiment-test-suite.cc
                 ${examples_as_tests_sources}
)

