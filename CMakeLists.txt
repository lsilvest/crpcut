#  Copyright 2009-2013 Bjorn Fahller <bjorn@fahller.se>
#  All rights reserved
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.

#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.



cmake_minimum_required(VERSION 2.6)
if (COMMAND cmake_policy)
  cmake_policy(SET CMP0002 NEW)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)
project(crpcut)
include(CheckFunctionExists)
include(CheckLibraryExists)

if (CMAKE_VERSION)
  if ("${CMAKE_VERSION}" VERSION_GREATER "2.8.7")
    set(HAS_OBJECT_LIBS "yep")
  endif ("${CMAKE_VERSION}" VERSION_GREATER "2.8.7")
endif (CMAKE_VERSION)
set(CRPCUT_LIB_MAJOR_VERSION "1")
set(CRPCUT_LIB_MINOR_VERSION "9")
set(CRPCUT_LIB_PATCH_VERSION "5")
#set(CRPCUT_LIB_ALIAS_VERSION "")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O")
set(CMAKE_CXX_FLAGS_MAINT "-ggdb -Wall -Wextra -Wcast-qual -Wconversion -Wshadow -Wunused-parameter -Wwrite-strings -pedantic -Wno-variadic-macros")
set(CRPCUT_LIB_VERSION "${CRPCUT_LIB_MAJOR_VERSION}.${CRPCUT_LIB_MINOR_VERSION}.${CRPCUT_LIB_PATCH_VERSION}")
add_definitions(-DCRPCUT_VERSION_STRING="${CRPCUT_LIB_VERSION}")
set(CRPCUT_LIB_SOVERSION ${CRPCUT_LIB_MAJOR_VERSION})
if (CRPCUT_LIB_ALIAS_VERSION)
  set(CRPCUT_PRINT_VERSION "${CRPCUT_LIB_VERSION} \\(a.k.a ${CRPCUT_LIB_ALIAS_VERSION}\\)")
else (CRPCUT_LIB_ALIAS_VERSION)
  set(CRPCUT_PRINT_VERSION "${CRPCUT_LIB_VERSION}")
endif (CRPCUT_LIB_ALIAS_VERSION)
set(SHAREDIR "share/crpcut-${CRPCUT_LIB_VERSION}")
if ("${BUILD_SHARED_LIBS}" STREQUAL "")
  set(BUILD_SHARED_LIBS ON)
endif ("${BUILD_SHARED_LIBS}" STREQUAL "")
file(GLOB LIBHEAP_SRCS
     src/heap.cpp
)
file(GLOB HEAP_FAKE_SRCS
     src/heap_fake.cpp
)
file(GLOB LIB_SRCS
     src/check_name.cpp
     src/cli/activation_param.cpp
     src/cli/boolean_flip.cpp
     src/cli/interpreter.cpp
     src/cli/named_param.cpp
     src/cli/param.cpp
     src/clocks/clocks.cpp
     src/collate_result.cpp
     src/comm/data_reader.cpp
     src/comm/data_writer.cpp
     src/comm/file_descriptor.cpp
     src/comm/reporter.cpp
     src/convert_traits.cpp
     src/crpcut.cpp
     src/deadline_monitor.cpp
     src/event.cpp
     src/failed_check_reporter.cpp
     src/fdreader.cpp
     src/filesystem_operations.cpp
     src/fsfuncs.cpp
     src/istream_wrapper.cpp
     src/namespace_info.cpp
     src/output/formatter.cpp
     src/output/heap_buffer.cpp
     src/output/nil_formatter.cpp
     src/output/text_formatter.cpp
     src/output/text_modifier.cpp
     src/output/writer.cpp
     src/output/xml_formatter.cpp
     src/parameter_stream_traits.cpp
     src/pipe_pair.cpp
     src/policies/core_dumps/default_handler.cpp
     src/policies/core_dumps/ignore.cpp
     src/policies/deaths/no_action.cpp
     src/policies/deaths/none.cpp
     src/policies/deaths/wipe_working_dir.cpp
     src/policies/dependencies/base.cpp
     src/policies/exception_translator.cpp
     src/policies/no_core_file.cpp
     src/policies/policies.cpp
     src/policies/timeout/cputime_enforcer.cpp
     src/policies/timeout/monotonic_enforcer.cpp
     src/posix_error.cpp
     src/posix_write.cpp
     src/presentation.cpp
     src/presentation_output.cpp
     src/presentation_reader.cpp
     src/printer.cpp
     src/process_control.cpp
     src/registrator_list.cpp
     src/regex.cpp
     src/report_reader.cpp
     src/scope/time_base.cpp
     src/tag.cpp
     src/tag_filter.cpp
     src/tag_info.cpp
     src/tag_list_root.cpp
     src/test_case_base.cpp
     src/test_case_registrator.cpp
     src/test_case_result.cpp
     src/test_environment.cpp
     src/test_monitor.cpp
     src/test_runner.cpp
     src/test_suite_base.cpp
     src/test_wrapper.cpp
     src/text_writer.cpp
     src/timeboxed.cpp
     src/working_dir_allocator.cpp
     src/wrapped/posix_encapsulation.cpp
)
include_directories(include)
file(GLOB DOGFOOD_SRCS
     src/dogfood/binary_tester_test.cpp
     src/dogfood/bool_tester_test.cpp
     src/dogfood/buffer_vector_test.cpp
     src/dogfood/check_name_test.cpp
     src/dogfood/cli/activation_param_test.cpp
     src/dogfood/cli/boolean_flip_test.cpp
     src/dogfood/cli/interpreter_test.cpp
     src/dogfood/cli/named_param_test.cpp
     src/dogfood/cli/param_list_test.cpp
     src/dogfood/cli/param_test.cpp
     src/dogfood/cli/value_param_test.cpp
     src/dogfood/collate_test.cpp
     src/dogfood/comm/data_reader_test.cpp
     src/dogfood/comm/data_writer_test.cpp
     src/dogfood/comm/direct_reporter_test.cpp
     src/dogfood/comm/reporter_test.cpp
     src/dogfood/datatypes/string_traits_test.cpp
     src/dogfood/deadline_monitor_test.cpp
     src/dogfood/dogfood.cpp
     src/dogfood/expr_test.cpp
     src/dogfood/failed_check_reporter_test.cpp
     src/dogfood/fixed_string_test.cpp
     src/dogfood/fsfuncs_test.cpp
     src/dogfood/list_elem_test.cpp
     src/dogfood/namespace_info_test.cpp
     src/dogfood/output/heap_buffer_test.cpp
     src/dogfood/output/formatter_test.cpp
     src/dogfood/output/nil_formatter_test.cpp
     src/dogfood/output/text_formatter_test.cpp
     src/dogfood/output/text_modifier_test.cpp
     src/dogfood/output/writer_test.cpp
     src/dogfood/output/xml_formatter_test.cpp
     src/dogfood/policies/core_dumps_test.cpp
     src/dogfood/policies/deaths_test.cpp
     src/dogfood/policies/exception_translator_test.cpp
     src/dogfood/posix_error_test.cpp
     src/dogfood/pred_test.cpp
     src/dogfood/presentation_output_test.cpp
     src/dogfood/presentation_reader_test.cpp
     src/dogfood/printer_test.cpp
     src/dogfood/regex_test.cpp
     src/dogfood/registrator_list_test.cpp
     src/dogfood/report_reader_test.cpp
     src/dogfood/scope/time_test.cpp
     src/dogfood/show_value_test.cpp
     src/dogfood/tag_filter_test.cpp
     src/dogfood/tag_list_test.cpp
     src/dogfood/test_case_registrator_test.cpp
     src/dogfood/test_runner_test.cpp
     src/dogfood/test_wrapper_test.cpp
     src/dogfood/working_dir_allocator_test.cpp
)
if (NOT CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX /usr)
endif (NOT CMAKE_INSTALL_PREFIX)

# Check for variadic macros


file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/variadic_macro.cpp
  "#define APA(...) main(__VA_ARGS__)\nint APA(int argc, char *argv[]) {}\n")
try_compile(HAVE_VARIADIC_MACROS ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/CMakeTmp/variadic_macro.cpp)
if (NOT HAVE_VARIADIC_MACROS)
  message(FATAL_ERROR "${CMAKE_CXX_COMPILER} doesn't support variadic macros")
endif(NOT HAVE_VARIADIC_MACROS)

file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/exit.cpp
  "#include <cstdlib>\nint main() { std::_Exit(0); }\n")
try_compile(HAVE__EXIT ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/CMakeTmp/exit.cpp)
if (NOT HAVE__EXIT)
  message(FATAL_ERROR "${CMAKE_CXX_COMPILER} doesn't support std::_Exit()")
endif(NOT HAVE__EXIT)

# Check for decltype and typeof


include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-pthread HAVE_PTHREAD_SWITCH)
if (DEFINED HAVE_PTHREAD_SWITCH)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(DEFINED HAVE_PTHREAD_SWITCH)
check_cxx_compiler_flag(-std=c++0x HAVE_CXX0X)
check_cxx_compiler_flag(-std=c++11 HAVE_CXX11)
check_cxx_compiler_flag(-std=c++14 HAVE_CXX14)
file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/typeof.cpp
  "template <typename T> T* func(T);\n"
  "typedef typeof(func(3.14)) apa;\n"
  "int main() {}\n")
try_compile(HAVE_TYPEOF ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/CMakeTmp/typeof.cpp)
if (NOT HAVE_TYPEOF)
  if (NOT HAVE_CXX0X)
    mesage(FATAL_ERROR
      "${CMAKE_CXX_COMPILER} supports neither decltype nor typeof")
  endif (NOT HAVE_CXX0X)
endif (NOT HAVE_TYPEOF)

# check for TR1

file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/tr1.cpp
  "#include <tr1/type_traits>\n"
  "const bool v = std::tr1::is_reference<int>::type::value;\n"
  "int main() { return v; }\n")
try_compile(HAVE_TR1 ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/CMakeTmp/tr1.cpp)
if(NOT HAVE_TR1)
  file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/boost_tr1.cpp
    "#include <boost/tr1/type_traits.hpp>\n"
    "const bool v = std::tr1::is_reference<int>::type::value;\n"
    "int main() { return v; }\n")
  try_compile(HAVE_BOOST_TR1 ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/boost_tr1.cpp)
  if (NOT HAVE_BOOST_TR1)
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER} doesn't have std/tr1 nor boost/tr1")
  endif (NOT HAVE_BOOST_TR1)
endif(NOT HAVE_TR1)

# check for valgrind

file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/valgrind.cpp
  "#include <valgrind/valgrind.h>\n"
  "int main(int argc, char *argv[])\n"
  "{\n"
  "  VALGRIND_MALLOCLIKE_BLOCK(argv, 4, 0, 0);\n"
  "  return 0;\n"
  "}\n")
try_compile(HAVE_VALGRIND ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/CMakeTmp/valgrind.cpp)

# check for backtrace

check_function_exists("backtrace" USE_BACKTRACE)
if (USE_BACKTRACE)
  add_definitions(-DUSE_BACKTRACE)
  message("*** using backtrace() for information on heap objects")
endif (USE_BACKTRACE)


if (DEFINED EXTRA_LIBS)
  set(EXTRA_LIBS "${EXTRA_LIBS} -l${CMAKE_DL_LIBS}${PTHREAD}")
else(DEFINED EXTRA_LIBS)
  set(EXTRA_LIBS "-l${CMAKE_DL_LIBS}${PTHREAD}")
endif(DEFINED EXTRA_LIBS)

# check for clock_gettime()


check_function_exists("clock_gettime()" HAVE_CLOCK_GETTIME)
if(HAVE_CLOCK_GETTIME)
  add_definitions(-DHAVE_CLOCK_GETTIME=libc)
else(HAVE_CLOCK_GETTIME)
  check_library_exists("rt" "clock_gettime" "" RT_HAS_CLOCK_GETTIME)
  if(RT_HAS_CLOCK_GETTIME)
#    set(EXTRA_LIBS "-lrt;${EXTRA_LIBS}")
    set(HAVE_CLOCK_GETTIME 1)
    add_definitions(-DHAVE_CLOCK_GETTIME=librt)
  endif(RT_HAS_CLOCK_GETTIME)
endif(HAVE_CLOCK_GETTIME)
if(HAVE_CLOCK_GETTIME)
  set(MAY_HAVE_MONO 1)
  set(MAY_HAVE_CPU 1)
  set(HAVE_CLOCK 1)
endif(HAVE_CLOCK_GETTIME)

# check for mach_absolute_time()

check_function_exists("mach_absolute_time" HAVE_MACH_ABSOLUTE_TIME)
if(HAVE_MACH_ABSOLUTE_TIME)
  set(MAY_HAVE_MONO 1)
  set(HAVE_CLOCK 1)
  add_definitions(-DHAVE_MACH_ABSOLUTE_TIME)
endif(HAVE_MACH_ABSOLUTE_TIME)


# check for getitimer()

check_function_exists("getitimer" HAVE_ITIMER)
if(HAVE_ITIMER)
  add_definitions(-DHAVE_ITIMER)
  set(MAY_HAVE_CPU 1)
  set(HAVE_CLOCK 1)
endif(HAVE_ITIMER)

# check for gettimeofday()

check_function_exists("gettimeofday" HAVE_GETTIMEOFDAY)
if(HAVE_GETTIMEOFDAY)
  add_definitions(-DHAVE_GETTIMEOFDAY)
  set(HAVE_CLOCK 1)
endif(HAVE_GETTIMEOFDAY)


if (NOT HAVE_CLOCK)
  message(FATAL_ERROR "Velly velly solly, but you have no clocks!")
endif (NOT HAVE_CLOCK)
if (NOT MAY_HAVE_CPU)
  message(FATAL_ERROR "Velly velly solly, but I can't measure CPU-time")
endif (NOT MAY_HAVE_CPU)
if (NOT MAY_HAVE_MONO)
  message("Monotonic time source unavailable - using realtime clocks!")
endif (NOT MAY_HAVE_MONO)


# check google-mock
if (WITH_GOOGLE_MOCK)
  if (NOT GOOGLE_MOCK_DIR)
    find_library(GOOGLE_MOCK_LIB "gmock")
    if (NOT GOOGLE_MOCK_LIB)
      message(FATAL_ERROR
        "Can't find google mock, try to add -DGOOGLE_MOCK_DIR=<path> to the command line")
    endif (NOT GOOGLE_MOCK_LIB)
    set(GMOCK_LIBS "-lgmock -lgtest")
  else (NOT GOOGLE_MOCK_DIR)
    include_directories(AFTER ${GOOGLE_MOCK_DIR}/include)
    set(GMOCK_LIBS "-Wl,-rpath,${CMAKE_BINARY_DIR}/lib:${GOOGLE_MOCK_DIR}/lib -L${GOOGLE_MOCK_DIR}/lib -lgmock -lgtest")
  endif (NOT GOOGLE_MOCK_DIR)
  file(GLOB GMOCK_LIB_SRCS src/gmock.cpp)
  set(SELFTEST_PARAM ${SELFTEST_PARAM} gmock)

  file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/gmock.cpp
    "#include <gmock/gmock.h>\nclass C : public ::testing::EmptyTestEventListener {};\nint main() {\n  C obj;\n  ::testing::UnitTest::GetInstance()->listeners().Append(&obj);\n}\n")
  try_compile(HAVE_GMOCK ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/gmock.cpp
    CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${GOOGLE_MOCK_DIR}/include
    -DLINK_DIRECTORIES=${GOOGLE_MOCK_DIR}/lib
    -DLINK_LIBRARIES=gtest)
  if (NOT HAVE_GMOCK)
    set(GMOCK_PTHREAD_DIR "gtest -lpthread")
    try_compile(HAVE_GMOCK ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/gmock.cpp
    CMAKE_FLAGS -DINCLUDE_DIRECTORIES=${GOOGLE_MOCK_DIR}/include
    -DLINK_DIRECTORIES=${GOOGLE_MOCK_DIR}/lib
    -DLINK_LIBRARIES=${GMOCK_PTHREAD_DIR})
    if (NOT HAVE_GMOCK)
      message(FATAL_ERROR "Unknown version of Google Mock")
    endif (NOT HAVE_GMOCK)
    if (DEFINED GMOCK_LIBS)
      set(GMOCK_LIBS "${GMOCK_LIBS} -lpthread")
    else(DEFINED GMOCK_LIBS)
      set(GMOCK_LIBS "-lpthread")
    endif(DEFINED GMOCK_LIBS)
  endif (NOT HAVE_GMOCK)
endif (WITH_GOOGLE_MOCK)

include_directories(include)

if ("${LIBRARY_OUTPUT_PATH}" STREQUAL "")
  if (NOT DEFINED LIB_SUFFIX)
    if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
      set(LIB_SUFFIX 64)
    endif ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
  endif (NOT DEFINED LIB_SUFFIX)
  set(LIBRARY_OUTPUT_PATH "lib${LIB_SUFFIX}")
endif ("${LIBRARY_OUTPUT_PATH}" STREQUAL "")

if (WITHOUT_HEAP)
  set(LIBHEAP "")
  set(CRPCUT_LIB "crpcut_basic")
else (WITHOUT_HEAP)
  add_library(crpcut_heap ${LIBHEAP_SRCS})
  set_target_properties(crpcut_heap PROPERTIES
    VERSION ${CRPCUT_LIB_VERSION}
    SOVERSION ${CRPCUT_LIB_SOVERSION})
  set(LIBHEAP "crpcut_heap")
  file(GLOB HEAP_TEST_SRCS test-src/heap.cpp)
  install(TARGETS crpcut_heap DESTINATION ${LIBRARY_OUTPUT_PATH})
  set(SELFTEST_PARAM ${SELFTEST_PARAM} heap)
  set(CRPCUT_LIB "crpcut")
endif (WITHOUT_HEAP)
if (HAS_OBJECT_LIBS)
  add_library(common_lib OBJECT ${LIB_SRCS})
  set_target_properties(common_lib PROPERTIES COMPILE_FLAGS "-fPIC")
  set(COMMON $<TARGET_OBJECTS:common_lib>)
else (HAS_OBJECT_LIBS)
  set(COMMON ${LIB_SRCS})
endif (HAS_OBJECT_LIBS)

find_package(Threads REQUIRED)

add_library(crpcut_basic ${COMMON} ${HEAP_FAKE_SRCS})
set_target_properties(crpcut_basic PROPERTIES
  VERSION ${CRPCUT_LIB_VERSION}
  SOVERSION ${CRPCUT_LIB_SOVERSION})
add_library(crpcut ${COMMON} ${LIBHEAP_SRCS})
set_target_properties(crpcut PROPERTIES
  VERSION ${CRPCUT_LIB_VERSION}
  SOVERSION ${CRPCUT_LIB_SOVERSION})
target_link_libraries(crpcut ${CMAKE_DL_LIBS})
target_link_libraries(crpcut ${CMAKE_THREAD_LIBS_INIT})
if(GMOCK_LIBS)
  file(GLOB GMOCK_TEST_SRCS   test-src/gmock.cpp)
endif(GMOCK_LIBS)


file(GLOB TEST_SRCS
     test-src/unitt.cpp
     test-src/asserts_and_depends.cpp
     test-src/verify.cpp
     test-src/output.cpp
     test-src/timeouts.cpp
     test-src/death_by_exit.cpp
     test-src/death_by_signal.cpp
     test-src/death_by_exception.cpp
     test-src/parametrized.cpp
     test-src/wrapped.cpp
     test-src/ext_parameters.cpp
     test-src/predicates.cpp
     test-src/regex.cpp
     test-src/fp.cpp
     test-src/collate.cpp
     test-src/suite_deps1.cpp
     test-src/suite_deps2.cpp
     test-src/bad_forks.cpp
     ${GMOCK_TEST_SRCS}
     ${HEAP_TEST_SRCS}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FEATURE_SELECTION}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
  add_executable(testprog ${TEST_SRCS})
else ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
  add_executable(testprog EXCLUDE_FROM_ALL ${TEST_SRCS})
endif ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
if (GMOCK_LIBS)
  target_link_libraries(testprog ${CRPCUT_LIB}  ${GMOCK_LIBS} ${EXTRA_LIBS})
else (GMOCK_LIBS)
  target_link_libraries(testprog ${CRPCUT_LIB} ${EXTRA_LIBS})
endif (GMOCK_LIBS)

if (HAVE_CXX14)
  include(ProcessorCount)
  ProcessorCount(NUM_CPUS)
  if (NUM_CPUS EQUAL 0)
    set(NUM_CPUS 1)
  endif (NUM_CPUS EQUAL 0)
  if (NOT WITHOUT_HEAP)
    add_custom_target(dogfood_test dogfood -c ${NUM_CPUS} DEPENDS dogfood VERBATIM)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
      add_executable(dogfood ${DOGFOOD_SRCS})
    else ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
      add_executable(dogfood EXCLUDE_FROM_ALL ${DOGFOOD_SRCS})
    endif ("${CMAKE_BUILD_TYPE}" STREQUAL "maint")
    target_link_libraries(dogfood ${CRPCUT_LIB} ${EXTRA_LIBS} )
  endif(NOT WITHOUT_HEAP)

endif (HAVE_CXX14)
find_program(AWK "awk")
find_program(BASH "bash")
find_program(XMLLINT "xmllint")

if (WITH_HTML_DOCS)
  add_custom_target(can_dump_core
    ulimit -c | egrep -q "\\(Unlimited\\)\\|\\[1-9\\]" && touch can_dump_core)
  find_program(XSLTPROC "xsltproc")
  find_program(SED "sed")
  file(GLOB HTML_CHUNK_XSL
    /usr/share/xml/docbook/stylesheet/docbook-xsl-ns/html/chunk.xsl)
  if (NOT HTML_CHUNK_XSL)
    file(GLOB HTML_CHUNK_XSL
      /usr/share/sgml/docbook/xsl-ns-stylesheets/html/chunk.xsl)
  endif(NOT HTML_CHUNK_XSL)
  find_program(GDB "gdb")
  if (NOT XSLTPROC)
    message("xsltproc is not found - Can't build documentation")
  else (NOT XSLTPROC)
    if (NOT HTML_CHUNK_XSL)
      message("docbook-xsl-ns not found, can't build docs")
    else (NOT HTML_CHUNK_XSL)
      if (NOT GDB)
        message("gdb not found, can't build docs")
      else(NOT GDB)
        if (NOT SED)
          message("sed not found, can't build docs")
        else (NOT SED)
          if (NOT HAVE_GMOCK)
            message("google-mock not found, can't build docs")
          endif(NOT HAVE_GMOCK)
        endif(NOT SED)
      endif (NOT GDB)
    endif(NOT HTML_CHUNK_XSL)
  endif(NOT XSLTPROC)

  if(XSLTPROC AND HTML_CHUNK_XSL AND GDB AND SED AND HAVE_GMOCK)
    file(GLOB DOC_SRC_LIB "doc-src")
    file(GLOB DOC_SOURCES "${DOC_SRC_LIB}/*.xml")
    file(GLOB TOP_DOC_SOURCE "${DOC_SRC_LIB}/docs.xml")
    add_custom_target(
      stripped-src
      test -d stripped-src || mkdir stripped-src
      )
    add_custom_target(
      doc-bin
      test -d doc-bin || mkdir doc-bin
      )
    add_custom_target(
      sample-output
      test -d sample-output || mkdir sample-output
      )
    add_custom_target(
      html
      test -d html || mkdir html
      )

    add_custom_target(
      refman
      test -d html/refman || mkdir html/refman
      DEPENDS html
      )

    set(STRIP_SOURCES sed "0,/^$/D\\;s/\\(.*\\)/     \\1/g")

    function(sample_code src_name)
      set(COMPILE_PROGRAM 1)
      set (OPTION 1)
      foreach(f ${ARGN})
        if (DEFINED OPTION)
          unset(OPTION)
          if ("${f}" STREQUAL "NOCOMPILE")
            unset(COMPILE_PROGRAM)
          elseif ("${f}" STREQUAL "COMPILE_OPT")
            set(X_COMPILE_OPT 1)
          elseif("${f}" STREQUAL "LIBS")
            set(X_LIBS 1)
          else("${f}" STREQUAL "LIBS")
            message(FATAL_ERROR
              "Got \"${f}\": Expected COMPILE_OPT")
          endif("${f}" STREQUAL "NOCOMPILE")
        else(DEFINED OPTION)
          if (DEFINED X_COMPILE_OPT)
            SET(COMPILE_OPT ${f})
            unset(X_COMPILE_OPT)
          elseif(DEFINED X_LIBS)
            set(LINK_LIBS ${f})
            unset(X_LIBS)
          else(DEFINED X_LIBS)
            message(FATAL_ERROR "What now?")
          endif(DEFINED X_COMPILE_OPT)
          set(OPTION 1)
        endif(DEFINED OPTION)
      endforeach()
      GET_FILENAME_COMPONENT(basename ${src_name} NAME_WE)
      if (${basename} STREQUAL ${src_name})
        set(filename "${src_name}.cpp")
      else(${basename} STREQUAL ${src_name})
        set(filename "${src_name}")
      endif(${basename} STREQUAL ${src_name})
      file(GLOB SRC "${DOC_SRC_LIB}/samples/${filename}")
      add_custom_target(
        "${filename}"
        ${STRIP_SOURCES} < ${SRC} > ${filename}
        DEPENDS stripped-src ${SRC}
        WORKING_DIRECTORY stripped-src
        VERBATIM)
      if (DEFINED COMPILE_PROGRAM)
        add_executable(${basename} EXCLUDE_FROM_ALL ${SRC})
        if (DEFINED COMPILE_OPT)
          set_target_properties(
            "${basename}"
            PROPERTIES
            COMPILE_FLAGS ${COMPILE_OPT})
        endif(DEFINED COMPILE_OPT)
        target_link_libraries(${basename} crpcut ${LINK_LIBS} ${EXTRA_LIBS})
      endif(DEFINED COMPILE_PROGRAM)
    endfunction(sample_code)

    function(sample_output program)
      foreach(f ${ARGN})
        if ("${f}" STREQUAL "OUTNAME")
          set(X_OUTNAME 1)
          unset(X_PARAMS)
        elseif("${f}" STREQUAL "PARAMS")
          set(X_PARAMS 1)
        elseif("${f}" STREQUAL "FILTER")
          unset(X_PARAMS)
          set(X_FILTER 1)
        elseif (DEFINED X_OUTNAME)
          set(OUTNAME ${f})
        elseif(DEFINED X_PARAMS)
          if (DEFINED PARAMS)
            set(PARAMS "${PARAMS};${f}")
          else(DEFINED PARAMS)
            set(PARAMS ${f})
          endif(DEFINED PARAMS)
        elseif(DEFINED X_FILTER)
          set(FILTER ${f})
        else(DEFINED X_FILTER)
          message(FATAL_ERROR
            "Got \"${f}\": Expected one of OUTNAME|PARAMS|FILTER")
        endif("${f}" STREQUAL "OUTNAME")
      endforeach()
      if (NOT DEFINED OUTNAME)
        set (OUTNAME "${program}.out")
      endif(NOT DEFINED OUTNAME)
      if (NOT DEFINED FILTER)
        set (FILTER "s/\\(.*\\)/     \\1/g\;s:[-./_[:alnum:]]*/crpcut/doc-src/::g")
      endif(NOT DEFINED FILTER)
      add_custom_target(
        "${OUTNAME}"
        ${DOC_SRC_LIB}/nondecorated ../test/${program} ${PARAMS} | c++filt | sed ${FILTER} > "${OUTNAME}"
        DEPENDS sample-output ${program}
        WORKING_DIRECTORY sample-output
        VERBATIM)
    endfunction(sample_output)

    function(sample_result name)
      sample_code(${name})
      sample_output(${name} ${ARGN})
    endfunction(sample_result)

    sample_result(all_tests)
    sample_result(exit_any_code)
    sample_code(assert_eq_fails COMPILE_OPT "-ggdb")
    sample_output(assert_eq_fails)

    add_custom_target(
      coloured-test-report.out
      ${DOC_SRC_LIB}/colourize-output ../test/report_example --verbose --tags=/another_tag  > coloured-test-report.out
      DEPENDS ${DOC_SRC_LIB}/colourize-output sample-output report_example
      WORKING_DIRECTORY sample-output
      VERBATIM)

    add_custom_target(
      COPYING
      sed "s/^/    /1" < ${CMAKE_SOURCE_DIR}/COPYING > COPYING
      DEPENDS sample-output ${CMAKE_SOURCE_DIR}/COPYING
      WORKING_DIRECTORY sample-output
      VERBATIM)

    file(GLOB DEBUG_CMD "${DOC_SRC_LIB}/samples/debug_cmd")
    add_custom_target(
      debug_assert_eq_fails.out
      ${GDB} ../test/assert_eq_fails  < ${DEBUG_CMD} | sed "1,1s/^.*\\(GNU.*\\)/bash > gdb .\\/test\\/assert_eq_fails\\n\\1/g;s/\\((gdb).*\\)(gdb) \\(.*\\)$/\\1\\n\\2/g;s/^\\(..\\)\\((gdb).*\\)/\\1\\n\\2/g;s/\\[answered.*//g"  > debug_assert_eq_fails.out
      DEPENDS sample-output assert_eq_fails
      WORKING_DIRECTORY sample-output
      VERBATIM)

    sample_result(verify_eq_fails)
    sample_result(assert_ge_fails)

    sample_result(verify_ge_fails)
    sample_result(assert_le_fails)
    sample_result(verify_le_fails)
    sample_result(assert_lt_fails)
    sample_result(verify_lt_fails)
    sample_result(assert_gt_fails)
    sample_result(verify_gt_fails)
    sample_result(assert_ne_fails)
    sample_result(verify_ne_fails)
    sample_result(assert_false_fails)
    sample_result(verify_false_fails)
    sample_result(assert_true_fails)
    sample_result(describe_exception)
    sample_result(verify_true_fails)
    sample_result(assert_no_throw_fails)
    sample_result(verify_no_throw_fails)
    sample_result(assert_throw_fails)
    sample_result(verify_throw_fails)
    sample_result(assert_pred_fails)
    sample_result(verify_pred_fails)
    sample_result(assert_scope_max_cpu)
    sample_result(verify_scope_max_cpu)
    sample_result(assert_scope_max_real)
    sample_result(verify_scope_max_real)
    sample_result(assert_scope_min_real)
    sample_result(verify_scope_min_real)
    sample_result(assert_scope_leak_free PARAMS "-b")
    sample_result(verify_scope_leak_free PARAMS "-b")
    sample_result(usleep)
    sample_result(wrap_func)
    sample_result(wrap_v_func)
    sample_result(wrapped)
    sample_result(collate)
    sample_result(collate_case)
    sample_result(deadline_cpu)
    sample_result(deadline_realtime)
    sample_result(depends_on)
    sample_result(expect_exception)
    sample_result(expect_exit)
    sample_result(expect_signal_death)
    sample_result(expect_realtime_timeout)
    sample_result(fail)
    sample_result(fixture_construction_timeout)
    sample_result(fixture_destruction_timeout)
    sample_result(info)
    sample_result(no_core)
    sample_result(infinite_loop)
    sample_result(file_fail)
    sample_result(ambiguous_fix)
    sample_result(simple_fix)
    sample_result(file_fix)
    sample_result(abs_diff)
    sample_result(relative_diff)
    sample_result(ulps_diff)
    sample_result(regex)
    sample_result(bare-minimum)
    sample_output(bare-minimum PARAMS "--list"
                               OUTNAME bare-minimum-list.out)
    sample_output(bare-minimum PARAMS "--verbose"
                               OUTNAME bare-minimum-verbose.out)
    sample_result(iso8859_1_code PARAMS "--verbose" "--output-charset=UTF8")
    sample_result(utf8_code PARAMS "--verbose" "--output-charset=UTF8")
    sample_result(simple_testsuite)
    sample_result(test_tag PARAMS "--tags=-known_bug/uncertain")
    sample_code(report_example)
    sample_output(report_example PARAMS "--verbose" "--tags=/another_tag"
                                 OUTNAME "report_example_human.out")
    sample_output(report_example PARAMS "--xml=yes" "--verbose" "--tags=/another_tag"
                                 OUTNAME "report_example_xml.out")
    sample_code(ilist_element.hpp NOCOMPILE)

    sample_result(itest)
    sample_code(symtable.hpp NOCOMPILE)
    sample_result(symtable-test PARAMS "-v")
    sample_result(symtable-test2)
    sample_result(tag-example PARAMS "--tags=/insert")
    sample_output(tag-example PARAMS "--list-tags"
                              OUTNAME "tag-example-list.out")
    sample_output(tag-example PARAMS "-v" "--tags=-lookup/insert" "construct_and_destroy" "abnormal"
                              OUTNAME "tag-example-select.out")
    sample_result(suite-example PARAMS "-v" "--nodeps" "normal_access")
    sample_result(string-template-fix PARAMS "-v")
    sample_result(regex-simple PARAMS "-v")
    sample_code(sorted-names.hpp NOCOMPILE)
    sample_result(collate_example PARAMS "-v")

    sample_code(stream-cast.hpp NOCOMPILE)
    sample_result(heap-check)

    sample_code(process-example.hpp NOCOMPILE)
    sample_result(process-test)

    sample_result(simple-pred)
    sample_result(template-pred)
    sample_code(gmock-example LIBS ${GMOCK_LIBS})
    sample_output(gmock-example)

    sample_result(simple-depend)
    sample_code(suite_depend.cpp NOCOMPILE)
    sample_code(suite_define.cpp NOCOMPILE)
    
    sample_result(show-value-example PARAMS "-v")
    sample_result(match-operator-example)
    #
    # suite_depend
    #
    file(GLOB SUITE_DEPEND "${DOC_SRC_LIB}/samples/suite_depend.cpp")
    file(GLOB SUITE_DEFINE "${DOC_SRC_LIB}/samples/suite_define.cpp")
    add_executable(suite_depend EXCLUDE_FROM_ALL
      ${SUITE_DEPEND} ${SUITE_DEFINE})
    target_link_libraries(suite_depend ${LIBHEAP} crpcut ${EXTRA_LIBS})
    add_custom_target(
      suite_depend.out
      ${DOC_SRC_LIB}/nondecorated ../test/suite_depend | sed "s/\\(.*\\)/     \\1/g" > suite_depend.out
      DEPENDS suite_depend sample-output
      WORKING_DIRECTORY sample-output
      VERBATIM)

    add_custom_target(
      doc-src
      test -L doc-src || ln -s ${DOC_SRC_LIB} doc-src
      )

    add_custom_target(
      crpcut.xsl
      cat ${DOC_SRC_LIB}/crpcut-template.xsl | sed "s?CHUNK?${HTML_CHUNK_XSL}?g\;s?PVERSION?${CRPCUT_PRINT_VERSION}?g\;s?VERSION?${CRPCUT_LIB_VERSION}?g" > crpcut.xsl
      )

    set(SS "stripped-src")
    set(SO "sample-output")
    #
    # html docs
    #
    add_custom_target(
      html_docs ALL
      ${XSLTPROC}
      --xinclude
      ${CMAKE_BINARY_DIR}/crpcut.xsl - < ${TOP_DOC_SOURCE}
      DEPENDS html
      doc-src
      ${SS}/all_tests.cpp                 ${SO}/all_tests.out
      ${SS}/exit_any_code.cpp             ${SO}/exit_any_code.out
      ${SS}/assert_ne_fails.cpp           ${SO}/assert_ne_fails.out
      ${SS}/verify_ne_fails.cpp           ${SO}/verify_ne_fails.out
      ${SS}/assert_le_fails.cpp           ${SO}/assert_le_fails.out
      ${SS}/verify_le_fails.cpp           ${SO}/verify_le_fails.out
      ${SS}/assert_lt_fails.cpp           ${SO}/assert_lt_fails.out
      ${SS}/verify_lt_fails.cpp           ${SO}/verify_lt_fails.out
      ${SS}/assert_eq_fails.cpp           ${SO}/assert_eq_fails.out
      ${SS}/verify_eq_fails.cpp           ${SO}/verify_eq_fails.out
      ${SS}/assert_ge_fails.cpp           ${SO}/assert_ge_fails.out
      ${SS}/verify_ge_fails.cpp           ${SO}/verify_ge_fails.out
                                          ${SO}/debug_assert_eq_fails.out
      ${SS}/assert_gt_fails.cpp           ${SO}/assert_gt_fails.out
      ${SS}/verify_gt_fails.cpp           ${SO}/verify_gt_fails.out
      ${SS}/assert_false_fails.cpp        ${SO}/assert_false_fails.out
      ${SS}/verify_false_fails.cpp        ${SO}/verify_false_fails.out
      ${SS}/assert_true_fails.cpp         ${SO}/assert_true_fails.out
      ${SS}/describe_exception.cpp        ${SO}/describe_exception.out
      ${SS}/verify_true_fails.cpp         ${SO}/verify_true_fails.out
      ${SS}/assert_no_throw_fails.cpp     ${SO}/assert_no_throw_fails.out
      ${SS}/verify_no_throw_fails.cpp     ${SO}/verify_no_throw_fails.out
      ${SS}/assert_throw_fails.cpp        ${SO}/assert_throw_fails.out
      ${SS}/verify_throw_fails.cpp        ${SO}/verify_throw_fails.out
      ${SS}/assert_pred_fails.cpp         ${SO}/assert_pred_fails.out
      ${SS}/verify_pred_fails.cpp         ${SO}/verify_pred_fails.out
      ${SS}/assert_scope_max_cpu.cpp      ${SO}/assert_scope_max_cpu.out
      ${SS}/verify_scope_max_cpu.cpp      ${SO}/verify_scope_max_cpu.out
      ${SS}/assert_scope_max_real.cpp     ${SO}/assert_scope_max_real.out
      ${SS}/verify_scope_max_real.cpp     ${SO}/verify_scope_max_real.out
      ${SS}/assert_scope_min_real.cpp     ${SO}/assert_scope_min_real.out
      ${SS}/verify_scope_min_real.cpp     ${SO}/verify_scope_min_real.out
      ${SS}/assert_scope_leak_free.cpp    ${SO}/assert_scope_leak_free.out
      ${SS}/verify_scope_leak_free.cpp    ${SO}/verify_scope_leak_free.out
      ${SS}/usleep.cpp                    ${SO}/usleep.out
      ${SS}/wrap_func.cpp                 ${SO}/wrap_func.out
      ${SS}/wrap_v_func.cpp               ${SO}/wrap_v_func.out
      ${SS}/wrapped.cpp                   ${SO}/wrapped.out
      ${SS}/collate.cpp                   ${SO}/collate.out
      ${SS}/collate_case.cpp              ${SO}/collate_case.out
      ${SS}/deadline_cpu.cpp              ${SO}/deadline_cpu.out
      ${SS}/deadline_realtime.cpp         ${SO}/deadline_realtime.out
      ${SS}/depends_on.cpp                ${SO}/depends_on.out
      ${SS}/expect_exception.cpp          ${SO}/expect_exception.out
      ${SS}/expect_exit.cpp               ${SO}/expect_exit.out
      ${SS}/expect_signal_death.cpp       ${SO}/expect_signal_death.out
      ${SS}/expect_realtime_timeout.cpp   ${SO}/expect_realtime_timeout.out
      ${SS}/fixture_construction_timeout.cpp
                                          ${SO}/fixture_construction_timeout.out
      ${SS}/fixture_destruction_timeout.cpp
                                          ${SO}/fixture_destruction_timeout.out
      ${SS}/fail.cpp                      ${SO}/fail.out
      ${SS}/info.cpp                      ${SO}/info.out
      ${SS}/no_core.cpp                   ${SO}/no_core.out
      ${SS}/abs_diff.cpp                  ${SO}/abs_diff.out
      ${SS}/relative_diff.cpp             ${SO}/relative_diff.out
      ${SS}/ulps_diff.cpp                 ${SO}/ulps_diff.out
      ${SS}/regex.cpp                     ${SO}/regex.out
      ${SS}/bare-minimum.cpp              ${SO}/bare-minimum.out
                                          ${SO}/bare-minimum-list.out
                                          ${SO}/bare-minimum-verbose.out
      ${SS}/utf8_code.cpp                 ${SO}/utf8_code.out
      ${SS}/iso8859_1_code.cpp            ${SO}/iso8859_1_code.out
      ${SS}/simple_testsuite.cpp          ${SO}/simple_testsuite.out
      ${SS}/test_tag.cpp                  ${SO}/test_tag.out
      ${SS}/report_example.cpp            ${SO}/report_example_human.out
                                          ${SO}/report_example_xml.out
                                          ${SO}/coloured-test-report.out
      ${SS}/ilist_element.hpp
      ${SS}/itest.cpp                     ${SO}/itest.out
      ${SS}/simple-depend.cpp             ${SO}/simple-depend.out
      ${SS}/simple_fix.cpp                ${SO}/simple_fix.out
      ${SS}/file_fix.cpp                  ${SO}/file_fix.out
      ${SS}/ambiguous_fix.cpp             ${SO}/ambiguous_fix.out
      ${SS}/file_fail.cpp                 ${SO}/file_fail.out
      ${SS}/infinite_loop.cpp             ${SO}/infinite_loop.out
      ${SS}/suite_depend.cpp              ${SO}/suite_depend.out
      ${SS}/suite_define.cpp
      ${SS}/symtable.hpp
      ${SS}/symtable-test.cpp             ${SO}/symtable-test.out
      ${SS}/symtable-test2.cpp            ${SO}/symtable-test2.out
      ${SS}/tag-example.cpp               ${SO}/tag-example-list.out
                                          ${SO}/tag-example.out
                                          ${SO}/tag-example-select.out
      ${SS}/suite-example.cpp             ${SO}/suite-example.out
      ${SS}/string-template-fix.cpp       ${SO}/string-template-fix.out
      ${SS}/regex-simple.cpp              ${SO}/regex-simple.out
      ${SS}/collate_example.cpp           ${SO}/collate_example.out
      ${SS}/sorted-names.hpp
      ${SS}/heap-check.cpp                ${SO}/heap-check.out
      ${SS}/stream-cast.hpp
      ${SS}/process-example.hpp
      ${SS}/process-test.cpp              ${SO}/process-test.out
      ${SS}/simple-pred.cpp               ${SO}/simple-pred.out
      ${SS}/template-pred.cpp             ${SO}/template-pred.out
      ${SS}/gmock-example.cpp             ${SO}/gmock-example.out
      ${SS}/show-value-example.cpp        ${SO}/show-value-example.out
      ${SS}/match-operator-example.cpp    ${SO}/match-operator-example.out
      can_dump_core
                                          ${SO}/COPYING
      ${DOC_SOURCES} ${DOC_SRC_LIB}/crpcut-doc.css crpcut.xsl
      WORKING_DIRECTORY html)

   add_custom_target(
     homepage-${CRPCUT_LIB_VERSION}.html
     sed "s:PVERSION:${CRPCUT_PRINT_VERSION}:g\;s:VERSION:${CRPCUT_LIB_VERSION}:g" < ${DOC_SRC_LIB}/homepage.html > homepage-${CRPCUT_LIB_VERSION}.html
     DEPENDS ${DOC_SRC_LIB}/homepage.html
     )
   add_custom_target(
     homepage-${CRPCUT_LIB_VERSION}.css
     test -L homepage-${CRPCUT_LIB_VERSION}.css || ln -s ${DOC_SRC_LIB}/homepage.css ./homepage-${CRPCUT_LIB_VERSION}.css
     )
   add_custom_target(
     changelog-${CRPCUT_LIB_VERSION}.html
     ${SED} 's/&/\\&amp\;/g\;s/</\\&lt\;/g\;s/>/\\&gt\;/g' < ${CMAKE_SOURCE_DIR}/ChangeLog.txt | ${AWK} "-f" ${DOC_SRC_LIB}/mkchangelog.awk "-v" version=${CRPCUT_LIB_VERSION} > changelog-${CRPCUT_LIB_VERSION}.html
     DEPENDS ${CMAKE_SOURCE_DIR}/ChangeLog.txt ${DOC_SRC_LIB}/mkchangelog.awk
     )
   add_custom_target(
     sourceforge-release
     ${DOC_SRC_LIB}/make-release-archives "${CMAKE_SOURCE_DIR}" "${CRPCUT_LIB_VERSION}" "${CMAKE_INSTALL_PREFIX}"
     DEPENDS
     changelog-${CRPCUT_LIB_VERSION}.html
     homepage-${CRPCUT_LIB_VERSION}.html
     homepage-${CRPCUT_LIB_VERSION}.css

     )
  endif(XSLTPROC AND HTML_CHUNK_XSL AND GDB AND SED AND HAVE_GMOCK)
endif(WITH_HTML_DOCS)

find_program(RUBY "ruby")
if(RUBY)
  add_custom_target(selftest ${RUBY} ${CMAKE_SOURCE_DIR}/selftest.rb ${SELFTEST_PARAM} DEPENDS testprog VERBATIM)
else(RUBY)
  message("ruby is not found - selftest target not available")
endif(RUBY)

if (AWK AND BASH AND XMLLINT)
  add_custom_target(sanitycheck ${BASH} ${CMAKE_SOURCE_DIR}/sanitycheck.sh ${AWK} ${CMAKE_SOURCE_DIR} ${SELFTEST_PARAM} DEPENDS testprog VERBATIM)
endif (AWK AND BASH AND XMLLINT)


set(EXECUTABLE_OUTPUT_PATH test)

if(NOT HAVE_CXX14 AND NOT HAVE_CXX11 AND HAVE_CXX0X AND NOT HAVE_TYPEOF)
  message("*** IMPORTANT *** You must compile your tests using -std=c++0x")
  add_definitions(-std=c++0x)
endif(NOT HAVE_CXX14 AND NOT HAVE_CXX11 AND HAVE_CXX0X AND NOT HAVE_TYPEOF)
if (HAVE_VALGRIND)
  message("*** compiling with support for valgrind")
  add_definitions(-DHAVE_VALGRIND)
  set(CMAKE_CXX_FLAGS_MAINT "${CMAKE_CXX_FLAGS_MAINT} -Wno-long-long")
endif (HAVE_VALGRIND)

if (WITH_HTML_DOCS)
  install(
    DIRECTORY ${CMAKE_BINARY_DIR}/html
    DESTINATION "${SHAREDIR}/doc"
    )
  install(
    DIRECTORY ${DOC_SRC_LIB}/icons/
    DESTINATION "${SHAREDIR}/doc/html/images"
    FILES_MATCHING PATTERN "*.png"
    )
  install(
    FILES ${DOC_SRC_LIB}/crpcut-doc.css
    DESTINATION "${SHAREDIR}/doc/html"
    )
endif (WITH_HTML_DOCS)

install(
  FILES include/crpcut.hpp
  DESTINATION include
)

install(
  FILES ${TEST_SRCS}
  DESTINATION "${SHAREDIR}/example_source"
)
install(
  FILES crpcut.xsd TODO
  DESTINATION "${SHAREDIR}"
  )

install(TARGETS crpcut crpcut_basic crpcut_heap EXPORT crpcutTargets
    LIBRARY DESTINATION ${LIBRARY_OUTPUT_PATH}
    ARCHIVE DESTINATION ${LIBRARY_OUTPUT_PATH}
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/crpcut-${CRPCUT_LIB_VERSION}/crpcutConfigVersion.cmake"
  VERSION ${CRPCUT_LIB_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT crpcutTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/crpcut-${CRPCUT_LIB_VERSION}/crpcutTargets.cmake"
  NAMESPACE crpcut::
)
configure_file(crpcutConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/crpcut-${CRPCUT_LIB_VERSION}/crpcutConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/crpcut)
install(EXPORT crpcutTargets
  FILE
    crpcutTargets.cmake
  NAMESPACE
    crpcut::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    crpcutConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/crpcut-${CRPCUT_LIB_VERSION}/crpcutConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)

