#include "get_frame_engine.hpp"

#include "hotkey_action.hpp"
#include "sequential_image_pipeline.hpp"
#include "parallel_image_pipeline.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

#include <opencv2/highgui.hpp>

namespace common
{

GetFrameEngine::GetFrameEngine(const ConfiguredCameras &cameras)
{
    Construct(cameras);
}

GetFrameEngine::GetFrameEngine(const ConfiguredCamera &camera)
{
    ConfiguredCameras cameras;
    cameras.push_back(camera);
    Construct(cameras);
}

GetFrameEngine::~GetFrameEngine()
{
    Stop();    
}

void GetFrameEngine::Construct(const ConfiguredCameras &cameras)
{
    if (cameras.empty()) {
        throw std::invalid_argument("Camera list empty"); 
    }

    resizeOptions = {};

    for (auto camera : cameras) {
        if (sv::GetPlatform() == SV_PLATFORM_DRAGONBOARD_410C)
            imagePipelines.push_back(std::unique_ptr<ImagePipeline>(new SequentialImagePipeline(camera.camera)));
        else
            imagePipelines.push_back(std::unique_ptr<ImagePipeline>(new ParallelImagePipeline(camera.camera)));
        
        imagePipelines.back()->SetDebayer(camera.debayering);
        imagePipelines.back()->SetResizeOptions(camera.resizeOptions);
    }
}


/**
 * Streams from multiple sensors are displayed in separate windows. Windows are refreshed sequentially 
 * because OpenCV functions manipulating GUI (imshow, waitKey) must be called from the main thread.
 * This may limit display fps when streaming from multiple sensors.
 */
void GetFrameEngine::GetFrame()
{  
    for (auto &pipeline : imagePipelines) {
    
        cv::UMat image = pipeline->GetImage();
        cv::imshow(pipeline->GetName(), image);

        pipeline->ReturnImage();
    }

}

void DisplayEngine::ConstructWindows()
{
    for (auto &pipeline : imagePipelines) {
        cv::namedWindow(pipeline->GetName(), cv::WINDOW_OPENGL | cv::WINDOW_AUTOSIZE);
    }
}

/**
 * Sensors in master mode have to be started before sensors in slave mode that they control.
 * Application may hang otherwise.
 */
void GetFrameEngine::Start()
{
    std::for_each(imagePipelines.begin(), imagePipelines.end(), 
        [](std::unique_ptr<ImagePipeline> &pipeline) { 
            if (pipeline->IsMaster()) pipeline->Start(); 
        }
    );

    std::for_each(imagePipelines.begin(), imagePipelines.end(),
        [](std::unique_ptr<ImagePipeline> &pipeline) {
            if (!pipeline->IsMaster()) pipeline->Start();
        }
    );

    ConstructWindows();
}

/**
 * Sensors in master mode have to be stopped after sensors in slave mode that they control.
 * Application may hang otherwise.
 */
void GetFrameEngine::Stop()
{
    std::for_each(imagePipelines.begin(), imagePipelines.end(),
        [](std::unique_ptr<ImagePipeline> &pipeline) {
            if (!pipeline->IsMaster()) pipeline->Stop();
        }
    );

    std::for_each(imagePipelines.begin(), imagePipelines.end(), 
        [](std::unique_ptr<ImagePipeline> &pipeline) { 
            if (pipeline->IsMaster()) pipeline->Stop(); 
        }
    );
}
}
