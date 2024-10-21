#include "sv/sv.h"

#include "common_cpp/common.hpp"
#include "common_cpp/get_frame_engine.hpp"
#include "common_cpp/camera_configurator.hpp"

#include <memory>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

int main() 
{
    ICameraList cameras = sv::GetAllCameras();
    if (cameras.size() == 0) {
        std::cout << "No cameras detected! Exiting..." << std::endl;
        return 0;
    }

    auto selected = common::SelectMultipleCameras(cameras);

    common::ConfiguredCameras configuredCameras = common::CameraConfigurator(selected).Configure();

    common::GetFrameEngine frameEngine(configuredCameras);
    
    frameEngine.Start();

    while (true) {
        frameEngine.GetFrame();  
    }
    
    frameEngine.Stop();  

    return 0;
}

