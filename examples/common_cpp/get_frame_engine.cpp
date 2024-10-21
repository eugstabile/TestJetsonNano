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
void GetFrameEngine::GetFrame(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg)
{  
    // for (auto &pipeline : imagePipelines) {

    auto pipeline = imagePipelines[0];

    cv::UMat image = pipeline->GetImage();

    std::vector<uchar> buf;
    cv::imencode(".jpg", image, buf);

    // Send the encoded image
    s->send(hdl, buf.data(), buf.size(), websocketpp::frame::opcode::binary);

    pipeline->ReturnImage();
    // }

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

    try {
        // Set logging settings
        server_.set_access_channels(websocketpp::log::alevel::all);
        server_.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        server_.init_asio();

        // Register our message handler
        server_.set_message_handler(bind(&WebSocketServer::GetFrame, this, ::_1, ::_2));

        // Listen on port 9002
        server_.listen(9002);

        // Start the server accept loop
        server_.start_accept();

        // Start the ASIO io_service run loop
        server_.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
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

    server_.stop_listening();
    server_.stop();
}
}
