include(GenerateExportHeader)

find_package(Vulkan REQUIRED)
add_library(vulktrt_lib vulktrt.cpp)

get_target_property(target_name vulktrt_lib NAME)
include("${CMAKE_SOURCE_DIR}/cmake/Simd.cmake")
set_simd_instructions(${target_name})

add_library(Vulktrt::vulktrt_lib ALIAS vulktrt_lib)

target_link_libraries(vulktrt_lib
        PRIVATE
        Vulktrt_options
        Vulktrt_warnings
        PUBLIC
        Vulkan::Vulkan
        glfw
        Vulktrt::vulktrt_lib_core
)

target_include_directories(vulktrt_lib
        ${WARNING_GUARD} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        "${CMAKE_BINARY_DIR}/configured_files/include"
)
if (Vandior_BUILD_FUZZ_TESTS)
  target_link_libraries(vulktrt_lib PRIVATE -fsanitize=fuzzer-no-link)
  target_compile_options(vulktrt_lib PRIVATE -fsanitize=fuzzer-no-link)
endif ()

target_compile_features(vulktrt_lib PUBLIC cxx_std_${CMAKE_CXX_STANDARD})


set_target_properties(
        vulktrt_lib
        PROPERTIES VERSION ${PROJECT_VERSION}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES)

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)


find_program(SPIRV_OPT spirv-opt HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

find_program(SPIRV_REMAP spirv-remap HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)


# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach (GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
          OUTPUT ${SPIRV}
          COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
          DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

foreach (BSPIRV ${SPIRV_BINARY_FILES})
  get_filename_component(FILE_NAME ${BSPIRV} NAME_WLE)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.opt.spv")
  add_custom_command(
          OUTPUT ${SPIRV}
          COMMAND ${SPIRV_OPT} ${BSPIRV} -o ${SPIRV} -O
          DEPENDS ${BSPIRV})
  list(APPEND SPIRV_BINARY_FILES_OPT ${SPIRV})
endforeach (BSPIRV)

foreach (RSPIRV ${SPIRV_BINARY_FILES_OPT})
  get_filename_component(FILE_NAME ${RSPIRV} NAME_WLE)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.rmp.spv")
  add_custom_command(
          OUTPUT ${SPIRV}
          COMMAND ${SPIRV_REMAP} -v --do-everything -i ${RSPIRV} -o ${SPIRV}
          DEPENDS ${RSPIRV})
  list(APPEND SPIRV_BINARY_FILES_REMAP ${SPIRV})
endforeach (RSPIRV)

list(APPEND SPIRV_BINARY_FILES ${SPIRV_BINARY_FILES_OPT})
list(APPEND SPIRV_BINARY_FILES ${SPIRV_BINARY_FILES_REMAP})

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(vulktrt_lib Shaders)

generate_export_header(vulktrt_lib EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/Vulktrt/Vulktrt/vulktrt_lib_export.hpp)

if (NOT BUILD_SHARED_LIBS)
  target_compile_definitions(vulktrt_lib PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif ()