include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(Vulktrt_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(Vulktrt_setup_options)
  option(Vulktrt_ENABLE_HARDENING "Enable hardening" ON)
  option(Vulktrt_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    Vulktrt_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    Vulktrt_ENABLE_HARDENING
    OFF)

  Vulktrt_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR Vulktrt_PACKAGING_MAINTAINER_MODE)
    option(Vulktrt_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(Vulktrt_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(Vulktrt_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Vulktrt_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Vulktrt_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Vulktrt_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(Vulktrt_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(Vulktrt_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Vulktrt_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(Vulktrt_ENABLE_IPO "Enable IPO/LTO" ON)
    option(Vulktrt_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(Vulktrt_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Vulktrt_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(Vulktrt_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(Vulktrt_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Vulktrt_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Vulktrt_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Vulktrt_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(Vulktrt_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(Vulktrt_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Vulktrt_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      Vulktrt_ENABLE_IPO
      Vulktrt_WARNINGS_AS_ERRORS
      Vulktrt_ENABLE_USER_LINKER
      Vulktrt_ENABLE_SANITIZER_ADDRESS
      Vulktrt_ENABLE_SANITIZER_LEAK
      Vulktrt_ENABLE_SANITIZER_UNDEFINED
      Vulktrt_ENABLE_SANITIZER_THREAD
      Vulktrt_ENABLE_SANITIZER_MEMORY
      Vulktrt_ENABLE_UNITY_BUILD
      Vulktrt_ENABLE_CLANG_TIDY
      Vulktrt_ENABLE_CPPCHECK
      Vulktrt_ENABLE_COVERAGE
      Vulktrt_ENABLE_PCH
      Vulktrt_ENABLE_CACHE)
  endif()

  Vulktrt_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (Vulktrt_ENABLE_SANITIZER_ADDRESS OR Vulktrt_ENABLE_SANITIZER_THREAD OR Vulktrt_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(Vulktrt_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(Vulktrt_global_options)
  include(cmake/Simd.cmake)
  check_all_simd_features()
  print_simd_support()
  if(Vulktrt_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    Vulktrt_enable_ipo()
  endif()

  Vulktrt_supports_sanitizers()

  if(Vulktrt_ENABLE_HARDENING AND Vulktrt_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR Vulktrt_ENABLE_SANITIZER_UNDEFINED
       OR Vulktrt_ENABLE_SANITIZER_ADDRESS
       OR Vulktrt_ENABLE_SANITIZER_THREAD
       OR Vulktrt_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${Vulktrt_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${Vulktrt_ENABLE_SANITIZER_UNDEFINED}")
    Vulktrt_enable_hardening(Vulktrt_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(Vulktrt_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(Vulktrt_warnings INTERFACE)
  add_library(Vulktrt_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  Vulktrt_set_project_warnings(
    Vulktrt_warnings
    ${Vulktrt_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(Vulktrt_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    Vulktrt_configure_linker(Vulktrt_options)
  endif()

  include(cmake/Sanitizers.cmake)
  Vulktrt_enable_sanitizers(
    Vulktrt_options
    ${Vulktrt_ENABLE_SANITIZER_ADDRESS}
    ${Vulktrt_ENABLE_SANITIZER_LEAK}
    ${Vulktrt_ENABLE_SANITIZER_UNDEFINED}
    ${Vulktrt_ENABLE_SANITIZER_THREAD}
    ${Vulktrt_ENABLE_SANITIZER_MEMORY})

  set_target_properties(Vulktrt_options PROPERTIES UNITY_BUILD ${Vulktrt_ENABLE_UNITY_BUILD})

  if(Vulktrt_ENABLE_PCH)
    target_precompile_headers(
      Vulktrt_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(Vulktrt_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    Vulktrt_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(Vulktrt_ENABLE_CLANG_TIDY)
    Vulktrt_enable_clang_tidy(Vulktrt_options ${Vulktrt_WARNINGS_AS_ERRORS})
  endif()

  if(Vulktrt_ENABLE_CPPCHECK)
    Vulktrt_enable_cppcheck(${Vulktrt_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(Vulktrt_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    Vulktrt_enable_coverage(Vulktrt_options)
  endif()

  if(Vulktrt_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(Vulktrt_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(Vulktrt_ENABLE_HARDENING AND NOT Vulktrt_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR Vulktrt_ENABLE_SANITIZER_UNDEFINED
       OR Vulktrt_ENABLE_SANITIZER_ADDRESS
       OR Vulktrt_ENABLE_SANITIZER_THREAD
       OR Vulktrt_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    Vulktrt_enable_hardening(Vulktrt_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
