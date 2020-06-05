# define cmake minimum requirement
cmake_minimum_required(VERSION 3.0)

# project information
project(itkMultiscaleVolumeToDepthMapFilter)

# Include directories
# ##############################################################################

include_directories(${itkMultiscaleVolumeToDepthMapFilter_DIR})
include_directories(${itkVolumeToDepthMapFilter_DIR})

# Set files
# ##############################################################################

set(header
    ./includes/itkMultiscaleVolumeToDepthMapFilter.h
    ./includes/itkMultiscaleVolumeToDepthMapFilter.hxx
    ${itkVolumeToDepthMapFilter_DIR}/itkVolumeToDepthMapFilter.h
    ${itkVolumeToDepthMapFilter_DIR}/itkVolumeToDepthMapFilter.hxx)

# Executable
# ##############################################################################

add_executable(itkMultiscaleVolumeToDepthMapFilterTest
               ./tests/itkMultiscaleVolumeToDepthMapFilterTest.cpp ${header})

target_link_libraries(itkMultiscaleVolumeToDepthMapFilterTest ${ITK_LIBRARIES})

set_target_properties(itkMultiscaleVolumeToDepthMapFilterTest
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Tests
# ##############################################################################

add_test(
  NAME itkMultiscaleVolumeToDepthMapFilterTest1
  COMMAND
    ${BIN_DIR}/itkMultiscaleVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0_Var.tif
    ${DATA_DIR}/C0T0_Map.tif 2 1.5 0 25)
add_test(
  NAME itkMultiscaleVolumeToDepthMapFilterTest2
  COMMAND
    ${BIN_DIR}/itkMultiscaleVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0_Var.tif
    ${DATA_DIR}/C0T0_Map.tif 3 5 1 50)
add_test(
  NAME itkMultiscaleVolumeToDepthMapFilterTest3
  COMMAND
    ${BIN_DIR}/itkMultiscaleVolumeToDepthMapFilterTest ${DATA_DIR}/C0T0_Var.tif
    ${DATA_DIR}/C0T0_Map.tif 3 5 0 25)
