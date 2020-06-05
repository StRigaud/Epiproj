# define cmake minimum requirement
cmake_minimum_required(VERSION 3.0)

# project information
project(itkVolumeToDepthMapFilter)

# Include directories
# ##############################################################################

include_directories(${itkVolumeToDepthMapFilter_DIR})

# Set files
# ##############################################################################

set(header ./includes/itkVolumeToDepthMapFilter.h
           ./includes/itkVolumeToDepthMapFilter.hxx)

# Executable
# ##############################################################################

add_executable(itkVolumeToDepthMapFilterTest
               ./tests/itkVolumeToDepthMapFilterTest.cpp ${header})

target_link_libraries(itkVolumeToDepthMapFilterTest ${ITK_LIBRARIES})

set_target_properties(itkVolumeToDepthMapFilterTest
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Tests
# ##############################################################################

add_test(
  NAME itkVolumeToDepthMapFilterTest1
  COMMAND
    ${BIN_DIR}/itkVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0.tif
    ${DATA_DIR}/C0T0_Proj.tif 2 0 25 0 2)
add_test(
  NAME itkVolumeToDepthMapFilterTest2
  COMMAND
    ${BIN_DIR}/itkVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0.tif
    ${DATA_DIR}/C0T0_Proj.tif 2 1 25 0)
add_test(
  NAME itkVolumeToDepthMapFilterTest3
  COMMAND
    ${BIN_DIR}/itkVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0.tif
    ${DATA_DIR}/C0T0_Proj.tif 2 0 25 1)
add_test(
  NAME itkVolumeToDepthMapFilterTest4
  COMMAND
    ${BIN_DIR}/itkVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0.tif
    ${DATA_DIR}/C0T0_Proj.tif 1 0 25 0)
