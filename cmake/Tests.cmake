function(Vulktrt_enable_coverage project_name)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Add coverage flags with compiler-specific fallbacks
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(COVERAGE_FLAG "--coverage")
        elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # Use both Clang coverage formats for wider tool compatibility
            set(COVERAGE_FLAG "--coverage -fprofile-instr-generate -fcoverage-mapping")
        endif ()

        # Add compilation flags
        target_compile_options(${target_name} PRIVATE
                ${COVERAGE_FLAG}
                -O0        # Disable optimizations for accurate coverage
                -g         # Generate debug information
                -fno-inline # Disable function inlining
                -fno-elide-constructors # Disable constructor elision
        )

        # Add linking flags
        target_link_options(${target_name} PRIVATE
                ${COVERAGE_FLAG}
        )
    endif ()
endfunction()
