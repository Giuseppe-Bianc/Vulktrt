macro(Vulktrt_enable_ipo)
  include(CheckIPOSupported)

  # Check compiler support for Interprocedural Optimization
  check_ipo_supported(
          RESULT ipo_supported
          OUTPUT ipo_output
          LANGUAGES CXX
  )

  if(ipo_supported)
    message(STATUS "Interprocedural Optimization (IPO/LTO) Configuration:")
    message(STATUS "-----------------------------------------")
    message(STATUS "Enabling whole-program optimization:")
    message(STATUS "  Benefits:")
    message(STATUS "  * 10-25% improved runtime performance")
    message(STATUS "  * 5-15% reduced binary size")
    message(STATUS "  * Better cross-module optimization")
    message(STATUS "  Requirements:")
    message(STATUS "  * Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "  * Build type: ${CMAKE_BUILD_TYPE} (recommended: Release)")
    message(STATUS "  * Minimum versions:")
    message(STATUS "    - GCC: 4.5+ (10+ recommended)")
    message(STATUS "    - Clang: 3.9+ (15+ recommended)")
    message(STATUS "    - MSVC: 2015 Update 3+ (2022 recommended)")
    message(STATUS "  Trade-offs:")
    message(STATUS "  * 2-4x longer build times")
    message(STATUS "  * 2-3x more memory usage during compilation")
    message(STATUS "  * Harder debugging experience")

    # Enable global IPO with safety checks
    message(STATUS "-----------------------------------------")
  else()
    message(WARNING "Interprocedural Optimization unavailable")
    message(STATUS "Diagnostic output: ${ipo_output}")
    message(STATUS "Troubleshooting steps:")
    message(STATUS "1. Verify compiler version meets minimum requirements")
    message(STATUS "2. Install LTO plugins:")
    message(STATUS "   - Ubuntu: sudo apt-get install gcc-plugin-dev lto-disabled-list")
    message(STATUS "   - Windows: Install Visual Studio 'C++ CMake tools for Windows' component")
    message(STATUS "3. Set AR environment variable for LTO:")
    message(STATUS "   export AR=gcc-ar  # For GCC implementations")
    message(STATUS "4. Configure linker:")
    message(STATUS "   - Use gold linker: -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold")
    message(STATUS "   - For MSVC: Enable Whole Program Optimization in project settings")
  endif()
endmacro()