# define cmake minimum requirement
cmake_minimum_required(VERSION 3.0)

# project information
project(itkDepthMapProjectionFilter)

# Include directories
# ##############################################################################

include_directories(${itkDepthMapProjectionFilter_DIR})

# Set files
# ##############################################################################

set(header ./includes/itkDepthMapProjectionFilter.h
           ./includes/itkDepthMapProjectionFilter.hxx)

# Executable
# ##############################################################################

add_executable(itkDepthMapProjectionFilterTest
               ./tests/itkDepthMapProjectionFilterTest.cpp ${header})

target_link_libraries(itkDepthMapProjectionFilterTest ${ITK_LIBRARIES})

set_target_properties(itkDepthMapProjectionFilterTest
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Tests
# ##############################################################################

add_test(
  NAME itkDepthMapProjectionFilterTest1
  COMMAND ${BIN_DIR}/itkDepthMapProjectionFilterTest ${DATA_DIR}/C0T0.tif
          ${DATA_DIR}/C0T0_Map.tif ${DATA_DIR}/C0T0_Proj.tif)
