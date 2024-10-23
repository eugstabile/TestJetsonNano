add_executable(${SERVER_NAME}
    "display_image.cpp"
)

target_include_directories(${SERVER_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/websocketpp
)

target_link_libraries(${SERVER_NAME} ${OpenCV_LIBRARIES} ${LIBRARY_NAME} ${Boost_LIBRARIES})