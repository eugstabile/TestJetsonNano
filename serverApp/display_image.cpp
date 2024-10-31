#include "sv/sv.h"

#include "common.hpp"
#include "get_frame_engine.hpp"
#include "camera_configurator.hpp"

#include <memory>

int main() 
{
    ICameraList cameras = sv::GetAllCameras();
    if (cameras.size() == 0) {
        std::cout << "No cameras detected! Exiting..." << std::endl;
        return 0;
    }

    auto selected = common::SelectMultipleCameras(cameras);

    IControl *control = selected[0]->GetControl(SV_V4L2_IMAGEFORMAT);
    
    if (control) {
        common::SelectPixelFormat(control);
    }
    common::ConfiguredCameras configuredCameras = common::CameraConfigurator(selected).Configure();

    
    
    common::GetFrameEngine frameEngine(configuredCameras);
    


    frameEngine.Start();

    return 0;
}

