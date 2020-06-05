# define cmake minimum requirement
cmake_minimum_required(VERSION 3.0)

# project information
project(epiproj)

# Include directories
# ##############################################################################

include_directories(${itkVolumeToDepthMapFilter_DIR})
include_directories(${itkMultiscaleVolumeToDepthMapFilter_DIR})
include_directories(${itkDepthMapProjectionFilter_DIR})
include_directories(${external_DIR})

# Executable
# ##############################################################################

add_executable(epiprojDepthMapGenerator ./epiprojDepthMapGenerator.cpp)
add_executable(epiprojDepthMapProjector ./epiprojDepthMapProjector.cpp)

target_link_libraries(epiprojDepthMapGenerator ${ITK_LIBRARIES})
target_link_libraries(epiprojDepthMapProjector ${ITK_LIBRARIES})

set_target_properties(epiprojDepthMapGenerator
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set_target_properties(epiprojDepthMapProjector
                      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Tests
# ##############################################################################

add_test(NAME compute_depthmap
         COMMAND ${BIN_DIR}/epiprojDepthMapGenerator ${DATA_DIR}/C0T0_Var.tif
                 ${DATA_DIR}/C0T0_Map.tif 6.0)

add_test(NAME compute_projection
         COMMAND ${BIN_DIR}/epiprojDepthMapProjector ${DATA_DIR}/C0T0.tif
                 ${DATA_DIR}/C0T0_Map.tif ${DATA_DIR}/C0T0_Proj.tif 1)
