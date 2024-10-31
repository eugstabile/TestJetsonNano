#include "get_frame_engine.hpp"

#include "hotkey_action.hpp"
#include "sequential_image_pipeline.hpp"
#include <linux/videodev2.h>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include <opencv2/highgui.hpp>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

namespace common
{


GetFrameEngine::GetFrameEngine(const ConfiguredCameras &cameras) {
    Construct(cameras);
}

GetFrameEngine::GetFrameEngine(const ConfiguredCamera &camera) {
    ConfiguredCameras cameras;
    cameras.push_back(camera);
    Construct(cameras);
}

GetFrameEngine::~GetFrameEngine() {
    Stop();    
}

void GetFrameEngine::Construct(const ConfiguredCameras &cameras) {
    if (cameras.empty()) {
        throw std::invalid_argument("Camera list empty"); 
    }

    resizeOptions = {};

    for (auto camera : cameras) {
        imagePipelines.push_back(std::unique_ptr<SequentialImagePipeline>(new SequentialImagePipeline(camera.camera)));
        imagePipelines.back()->SetDebayer(camera.debayering);
        imagePipelines.back()->SetResizeOptions(camera.resizeOptions);
    }
}


/**
 * Streams from multiple sensors are displayed in separate windows. Windows are refreshed sequentially 
 * because OpenCV functions manipulating GUI (imshow, waitKey) must be called from the main thread.
 * This may limit display fps when streaming from multiple sensors.
 */
    void GetFrameEngine::GetFrame(websocketpp::connection_hdl hdl, server::message_ptr msg) {  
        std::cout << msg->get_payload() << std::endl;

        auto&& pipeline = imagePipelines.front();
        IImage image = pipeline->GetRawImage();
            
        std::cout << "pasando pipe" << std::endl;

        switch (image.pixelFormat) {
            case V4L2_PIX_FMT_SBGGR8:
            case V4L2_PIX_FMT_SGBRG8:
            case V4L2_PIX_FMT_SGRBG8:
            case V4L2_PIX_FMT_SRGGB8:
                std::cout << "8" << std::endl;
                break;
            case V4L2_PIX_FMT_SBGGR10:
            case V4L2_PIX_FMT_SBGGR10P:
            case V4L2_PIX_FMT_SGBRG10:
            case V4L2_PIX_FMT_SGBRG10P:
            case V4L2_PIX_FMT_SGRBG10:
            case V4L2_PIX_FMT_SGRBG10P:
            case V4L2_PIX_FMT_SRGGB10:
            case V4L2_PIX_FMT_SRGGB10P:
                std::cout << "10" << std::endl;
                break;
            case V4L2_PIX_FMT_SBGGR12:
            case V4L2_PIX_FMT_SBGGR12P:
            case V4L2_PIX_FMT_SGBRG12:
            case V4L2_PIX_FMT_SGBRG12P:
            case V4L2_PIX_FMT_SGRBG12:
            case V4L2_PIX_FMT_SGRBG12P:
            case V4L2_PIX_FMT_SRGGB12:
            case V4L2_PIX_FMT_SRGGB12P:
                std::cout << "12" << std::endl;
                break;
        }

        std::cout << "pixelFormat: " << image.pixelFormat << std::endl;
        std::cout << "Length: " << image.length << std::endl;
        std::cout << "width: " << image.width << std::endl;
        std::cout << "height: " << image.height << std::endl;
        std::cout << "stride: " << image.stride << std::endl;

        // Send the encoded image
        websocketpp::lib::error_code ec;
        server_.send(hdl, image.data, image.length, websocketpp::frame::opcode::binary, ec);

        if (ec) {
            std::cout << "SERVER: Echo failed because: " << ec.message() << std:: endl;
        } else {
            std::cout << "SERVER: sendddd: " << ec.message() << std:: endl;
        }
        
        pipeline->ReturnImage();

    }

/**
 * Sensors in master mode have to be started before sensors in slave mode that they control.
 * Application may hang otherwise.
 */
void GetFrameEngine::Start() {

        std::for_each(imagePipelines.begin(), imagePipelines.end(), 
            [](std::unique_ptr<SequentialImagePipeline> &pipeline) { 
                if (pipeline->IsMaster()) pipeline->Start(); 
            }
        );

        std::for_each(imagePipelines.begin(), imagePipelines.end(),
            [](std::unique_ptr<SequentialImagePipeline> &pipeline) {
                if (!pipeline->IsMaster()) pipeline->Start();
            }
        );

        try {

            server_.set_access_channels(websocketpp::log::alevel::all);
            server_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            server_.init_asio();

            server_.set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {
                this->GetFrame(hdl, msg);
            });
                
            server_.listen(9002);
            server_.start_accept();
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
    void GetFrameEngine::Stop() {
        std::for_each(imagePipelines.begin(), imagePipelines.end(),
            [](std::unique_ptr<SequentialImagePipeline> &pipeline) {
                if (!pipeline->IsMaster()) pipeline->Stop();
            }
        );

        std::for_each(imagePipelines.begin(), imagePipelines.end(), 
            [](std::unique_ptr<SequentialImagePipeline> &pipeline) { 
                if (pipeline->IsMaster()) pipeline->Stop(); 
            }
        );

        server_.stop_listening();
        server_.stop();
    }

}
