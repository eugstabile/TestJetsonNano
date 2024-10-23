add_executable(${SERVER_NAME}
    "display_image.cpp"
)

target_link_libraries(${SERVER_NAME} ${OpenCV_LIBRARIES} ${LIBRARY_NAME} ${Boost_LIBRARIES} -lsv)