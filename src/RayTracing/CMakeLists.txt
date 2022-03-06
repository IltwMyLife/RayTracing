set(INCROOT ${CMAKE_SOURCE_DIR}/include/${PROJECT_NAME})
set(SRCROOT ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME})


set(SRC
    ${SRCROOT}/Main.cpp

    ${INCROOT}/Application.hpp
    ${SRCROOT}/Application.cpp
    ${INCROOT}/Camera.hpp
    ${SRCROOT}/Camera.cpp
    ${INCROOT}/Cursor.hpp
    ${SRCROOT}/Cursor.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/extlibs/imgui ${CMAKE_SOURCE_DIR}/extlibs/imgui-sfml)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-system sfml-window sfml-graphics)
target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/assets ${CMAKE_SOURCE_DIR}/build/src/${PROJECT_NAME}/assets)