# define cmake minimum requirement
cmake_minimum_required(VERSION 3.0)

# project information
project(Epiproj)

# Global Libraries
# ##############################################################################

# ITK Libraries
find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif()

# Globals Sets
# ##############################################################################
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(itkVolumeToDepthMapFilter_DIR ${CMAKE_CURRENT_SOURCE_DIR}/itkVolumeToDepthMapFilter/includes)
set(itkMultiscaleVolumeToDepthMapFilter_DIR ${CMAKE_CURRENT_SOURCE_DIR}/itkMultiscaleVolumeToDepthMapFilter/includes)
set(itkDepthMapProjectionFilter_DIR ${CMAKE_CURRENT_SOURCE_DIR}/itkDepthMapProjectionFilter/includes)
set(external_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

# Tests
# ##############################################################################

enable_testing()
include( CTest )

# sub-directories
# ##############################################################################

add_subdirectory(itkVolumeToDepthMapFilter)
add_subdirectory(itkMultiscaleVolumeToDepthMapFilter)
add_subdirectory(itkDepthMapProjectionFilter)
add_subdirectory(epiproj)