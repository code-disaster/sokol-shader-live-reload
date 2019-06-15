cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(FIPS_DYNAMIC_CRT 1)

get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

fips_setup(PROJECT sample)

add_definitions(-DSOKOL_NO_DEPRECATED)
add_definitions(-DSOKOL_D3D11)
set(slang "hlsl5")

#
# sokol_app/sokol_gfx as shared DLL
#
fips_begin_sharedlib(sokol-dll)
  fips_vs_warning_level(3)
  fips_files(sokol-dll.c)
fips_end_sharedlib()

#
# reloadable guest DLL
#
fips_begin_sharedlib(guest)
  fips_files(guest.c)
  sokol_shader(cube.glsl ${slang})
  fips_deps(sokol-dll)
fips_end_sharedlib()

#
# host application
#
fips_begin_app(host cmdline)
  add_definitions(-DCR_DEPLOY_PATH="${FIPS_PROJECT_DEPLOY_DIR}")
  fips_files(host.cc cube.c cube.h cube.glsl)
  fips_deps(sokol-dll)
fips_end_app()

set_target_properties(guest PROPERTIES COMPILE_FLAGS "/std:c++latest")
set_target_properties(host PROPERTIES COMPILE_FLAGS "/std:c++latest")

fips_finish()
