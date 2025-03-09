add_executable(vulktrt main.cpp)

get_target_property(target_name vulktrt NAME)
include("${CMAKE_SOURCE_DIR}/cmake/Simd.cmake")
set_simd_instructions(${target_name})

target_link_libraries(
  vulktrt
  PRIVATE Vulktrt::Vulktrt_options
          Vulktrt::Vulktrt_warnings)

target_link_system_libraries(
  vulktrt
  PRIVATE
          CLI11::CLI11
          Vulktrt::vulktrt_lib)

target_include_directories(vulktrt PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
