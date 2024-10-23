add_executable(${CLIENT_NAME}
    "recive_image.cpp"
)

target_include_directories(${CLIENT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/websocketpp
)

target_link_libraries(${CLIENT_NAME} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})