include("${CMAKE_CURRENT_LIST_DIR}/Utilities.cmake")

macro(detect_architecture)
    # Convert CMAKE_SYSTEM_PROCESSOR to lowercase for uniformity
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" CMAKE_SYSTEM_PROCESSOR_LOWER)

    if (CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "^(x86|i[3456]86)$")
        set(VCVARSALL_ARCH "x86")
    elseif (CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "^(x64|x86_64|amd64)$")
        set(VCVARSALL_ARCH "x64")
    elseif (CMAKE_SYSTEM_PROCESSOR_LOWER STREQUAL "arm")
        set(VCVARSALL_ARCH "arm")
    elseif (CMAKE_SYSTEM_PROCESSOR_LOWER MATCHES "^(arm64|aarch64)$")
        set(VCVARSALL_ARCH "arm64")
    else ()
        set(VCVARSALL_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
        message(WARNING "Unknown architecture detected: ${CMAKE_SYSTEM_PROCESSOR_LOWER}. Using system processor architecture: ${VCVARSALL_ARCH}.")
    endif ()

    # Determine pointer size (4 for 32-bit, 8 for 64-bit)
    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCHITECTURE_BITNESS "64-bit")
    elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCHITECTURE_BITNESS "32-bit")
    else ()
        set(ARCHITECTURE_BITNESS "Unknown bitness")
    endif ()

    # Output detailed architecture information
    message(STATUS "Detected system processor: ${CMAKE_SYSTEM_PROCESSOR}")
    message(STATUS "Normalized processor: ${CMAKE_SYSTEM_PROCESSOR_LOWER}")
    message(STATUS "Architecture bitness: ${ARCHITECTURE_BITNESS}")
    message(STATUS "VCVARSALL_ARCH set to: ${VCVARSALL_ARCH}")
endmacro()

# Run vcvarsall.bat and set CMake environment variables
function(run_vcvarsall)
    # Check if using MSVC and vcvarsall has not been run yet
    if (MSVC AND NOT "$ENV{VSCMD_VER}")
        message(STATUS "MSVC detected without VSCMD_VER - setting up environment...")
        set(VCVARSALL_SUFFIXES
                "VC/Auxiliary/Build"
                "Common7/Tools"
                "Tools"
                "Enterprise/VC/Auxiliary/Build"  # VS Enterprise editions
                "Professional/VC/Auxiliary/Build" # VS Professional editions
                "Community/VC/Auxiliary/Build"    # VS Community editions
        )
        set(BASE_PATHS
                "${MSVC_DIR}"
                "${MSVC_DIR}/.."
                "${MSVC_DIR}/../.."
                "${MSVC_DIR}/../../../../../../.."
                "${MSVC_DIR}/../../../../../../../.."
        )
        # Find vcvarsall.bat
        get_filename_component(MSVC_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        find_file(VCVARSALL_FILE
                NAMES vcvarsall.bat
                PATHS ${BASE_PATHS}
                PATH_SUFFIXES ${VCVARSALL_SUFFIXES}
                DOC "Path to Visual Studio environment configuration script"
                NO_DEFAULT_PATH
                NO_PACKAGE_ROOT_PATH
                NO_CMAKE_ENVIRONMENT_PATH
        )

    if (EXISTS "${VCVARSALL_FILE}")
      # Detect the architecture
      detect_architecture()

      # Run vcvarsall.bat and print informative messages
      message(STATUS "Searching for vcvarsall.bat in the Visual Studio directory...")
      message(STATUS "Found vcvarsall.bat at: ${VCVARSALL_FILE}")

      # Run vcvarsall.bat to set up the MSVC environment
      message(STATUS "Running `${VCVARSALL_FILE} ${VCVARSALL_ARCH}` to set up the MSVC environment")
      execute_process(
              COMMAND cmd /c "${VCVARSALL_FILE}" "${VCVARSALL_ARCH}" "&&" "call" "echo" "VCVARSALL_ENV_START" "&" "set"
              RESULT_VARIABLE VCVARSALL_RESULT
              OUTPUT_VARIABLE VCVARSALL_OUTPUT
              ERROR_VARIABLE VCVARSALL_ERROR
              OUTPUT_STRIP_TRAILING_WHITESPACE)
      # Report results and additional information
      if (NOT VCVARSALL_RESULT EQUAL 0)
          message(WARNING "VCVARSALL execution failed with error code ${VCVARSALL_RESULT}")
          message(WARNING "Error output: ${VCVARSALL_ERROR}")
      else()
          message(STATUS "VCVARSALL execution succeeded.")
          # Parse the output and set the environment variables
          find_substring_by_prefix(VCVARSALL_ENV "VCVARSALL_ENV_START" "${VCVARSALL_OUTPUT}")
          if (NOT VCVARSALL_ENV)
              message(WARNING "No environment variables found in the vcvarsall.bat output. Check for errors.")
          else ()
              set_env_from_string("${VCVARSALL_ENV}")
              message(STATUS "MSVC environment is set up successfully.")
          endif ()
      endif()
    else ()
      message(WARNING "Could not find `vcvarsall.bat` for automatic MSVC environment preparation. Please manually open the MSVC command prompt and rebuild the project.")
    endif ()
  endif ()
endfunction()
