add_library(${LIBRARY_NAME} STATIC 
    "camera_configurator.cpp"
    "debayering_control.cpp"
    "display_engine.cpp"
    "get_frame_engine.cpp"
    "hotkey_action.cpp"
    "image_pipeline.cpp"
    "image_processor.cpp"
    "image_util.cpp"
    "image_writer.cpp"
    "resize_control.cpp"
    "string_util.cpp"
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/websocketpp
)


target_link_libraries(${LIBRARY_NAME} PUBLIC ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})