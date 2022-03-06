add_subdirectory(SFML)

set(IMGUI_SFML_FIND_SFML FALSE)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/extlibs/imgui)
set(SFML_DIR ${CMAKE_SOURCE_DIR}/extlibs/SFML)
add_subdirectory(imgui-sfml)