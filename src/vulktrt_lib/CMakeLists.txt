include(GenerateExportHeader)

find_package(Vulkan REQUIRED)
add_library(vulktrt_lib vulktrt.cpp
        ../../include/Vulktrt/vulktrt.hpp
        ../../include/Vulktrt/DeviceInfo.hpp
        ../../include/Vulktrt/vulkanToString.hpp
        ../../include/Vulktrt/vulkanCheck.hpp
        ../../include/Vulktrt/VulkanLogInfoCallback.hpp
        FPSCounter.cpp
        ../../include/Vulktrt/FPSCounter.hpp
        Window.cpp
        ../../include/Vulktrt/Window.hpp
        FirstApp.cpp
        ../../include/Vulktrt/FirstApp.hpp
        Pipeline.cpp
        ../../include/Vulktrt/Pipeline.hpp
        Device.cpp
        ../../include/Vulktrt/Device.hpp
        SwapChain.cpp
        ../../include/Vulktrt/SwapChain.hpp
        Monitor.cpp
        ../../include/Vulktrt/Monitor.hpp
        Model.cpp
        ../../include/Vulktrt/Model.hpp
        GameObject.cpp
        ../../include/Vulktrt/GameObject.hpp
        Renderer.cpp
        ../../include/Vulktrt/Renderer.hpp
        SimpleRenderSystem.cpp
        ../../include/Vulktrt/SimpleRenderSystem.hpp
        Camera.cpp
        ../../include/Vulktrt/Camera.hpp
        KeyboardMovementController.cpp
        ../../include/Vulktrt/KeyboardMovementController.hpp
        ../../include/Vulktrt/utils.hpp
        Buffer.cpp
        ../../include/Vulktrt/Buffer.hpp
        ../../include/Vulktrt/FrameInfo.hpp
        Descriptors.cpp
        ../../include/Vulktrt/Descriptors.hpp
)

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
        tinyobjloader
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
find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

if (NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found - Required for GLSL compilation")
endif ()
message(STATUS "Using glslangValidator: ${GLSL_VALIDATOR}")

find_program(SPIRV_OPT spirv-opt
        HINTS
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)
if (SPIRV_OPT)
    message(STATUS "SPIR-V optimizer found: ${SPIRV_OPT}")
else ()
    message(WARNING "spirv-opt not found - Skipping SPIR-V optimization")
endif ()

# Optional SPIR-V remapping tool
find_program(SPIRV_REMAP spirv-remap
        HINTS
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)
if (SPIRV_REMAP)
    message(STATUS "SPIR-V remapper found: ${SPIRV_REMAP}")
else ()
    message(WARNING "spirv-remap not found - Skipping SPIR-V remapping")
endif ()


# get all .vert and .frag files in shaders directory
set(SHADER_ROOT_DIR "${PROJECT_SOURCE_DIR}/shaders")
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${SHADER_ROOT_DIR}/*.vert" "${SHADER_ROOT_DIR}/*.frag"
        "${SHADER_ROOT_DIR}/*.comp" "${SHADER_ROOT_DIR}/*.geom"
        "${SHADER_ROOT_DIR}/*.tesc" "${SHADER_ROOT_DIR}/*.tese"
        "${SHADER_ROOT_DIR}/*.rgen" "${SHADER_ROOT_DIR}/*.rahit"
        "${SHADER_ROOT_DIR}/*.rchit" "${SHADER_ROOT_DIR}/*.rmiss"
        "${SHADER_ROOT_DIR}/*.rcall"
)

if (NOT GLSL_SOURCE_FILES)
    message(WARNING "No GLSL shader sources found in ${SHADER_ROOT_DIR}")
else ()
    list(LENGTH GLSL_SOURCE_FILES NUM_SHADERS)
    message(STATUS "Found ${NUM_SHADERS} shader files:")
    foreach (shader ${GLSL_SOURCE_FILES})
        file(RELATIVE_PATH rel_path ${SHADER_ROOT_DIR} ${shader})
        message(STATUS "  ${rel_path}")
    endforeach ()
endif ()


foreach (GLSL ${GLSL_SOURCE_FILES})
    # Get relative path for output messages
    file(RELATIVE_PATH rel_path ${PROJECT_SOURCE_DIR} ${GLSL})

    # Base SPIR-V output
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV_OUT "${SHADER_ROOT_DIR}/${FILE_NAME}.spv")

    # Compile command
    add_custom_command(
            OUTPUT ${SPIRV_OUT}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV_OUT}
            DEPENDS ${GLSL}
            COMMENT "Compiling GLSL: ${rel_path} -> ${FILE_NAME}.spv"
            VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_OUT})

    # Optimization stage
    if (SPIRV_OPT)
        set(OPT_OUT "${SHADER_ROOT_DIR}/${FILE_NAME}.opt.spv")
        add_custom_command(
                OUTPUT ${OPT_OUT}
                COMMAND ${SPIRV_OPT} ${SPIRV_OUT} -O --strip-debug --freeze-spec-const --eliminate-dead-const -o ${OPT_OUT}
                DEPENDS ${SPIRV_OUT}
                COMMENT "Optimizing SPIR-V: ${FILE_NAME}.spv -> ${FILE_NAME}.opt.spv"
                VERBATIM
        )
        list(APPEND SPIRV_BINARY_FILES ${OPT_OUT})
        set(SPIRV_OUT ${OPT_OUT})  # Chain outputs for next stage
    endif ()

    # Remapping stage
    if (SPIRV_REMAP)
        set(REMAP_OUT "${SHADER_ROOT_DIR}/${FILE_NAME}.opt.rmp.spv")
        add_custom_command(
                OUTPUT ${REMAP_OUT}
                COMMAND ${SPIRV_REMAP} --do-everything -i ${SPIRV_OUT} -o ${REMAP_OUT}
                DEPENDS ${SPIRV_OUT}
                COMMENT "Remapping SPIR-V: ${FILE_NAME}${SPIRV_EXT}.spv -> ${FILE_NAME}.opt.rmp.spv"
                VERBATIM
        )
        list(APPEND SPIRV_BINARY_FILES ${REMAP_OUT})
    endif ()
endforeach ()

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
        COMMENT "Building shader resources"
)

add_dependencies(vulktrt_lib Shaders)

generate_export_header(vulktrt_lib EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/Vulktrt/Vulktrt/vulktrt_lib_export.hpp)

if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(vulktrt_lib PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif ()