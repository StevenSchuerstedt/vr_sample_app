#####################################
#
# VR_Sample_App
#
#####################################

add_executable (VR_Sample_App "VR_Sample_App.cpp" "VR_Sample_App" "shader.h" "shader.cpp")

target_link_libraries (VR_Sample_App LINK_PUBLIC VR_Library libglew_static glfw)


target_include_directories (VR_Sample_App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glew/include)
target_include_directories (VR_Sample_App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
target_include_directories (VR_Sample_App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/glm/glm)
target_include_directories (VR_Sample_App PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vr_library/VR_Library)



add_subdirectory ("glew")
add_subdirectory ("glfw")
add_subdirectory ("glm")
add_subdirectory ("vr_library")