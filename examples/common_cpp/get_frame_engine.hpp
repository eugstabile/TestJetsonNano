#pragma once

#include "sv/sv.h"
#include "resize_options.hpp"
#include "configured_camera.hpp"
#include "image_writer.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace common
{
    typedef websocketpp::server<websocketpp::config::asio> server;
    class ImagePipeline;

    class GetFrameEngine
    {        
        public:
            GetFrameEngine(const ConfiguredCameras &cameras);
            GetFrameEngine(const ConfiguredCamera &camera);

            ~GetFrameEngine();
            
            ResizeOptions GetResizeOptions();
            void SetResizeOptions(ResizeOptions options);
            
            void Start();
            void Stop();
        
        private:
            server server_;

            void GetFrame(websocketpp::connection_hdl hdl, server::message_ptr msg);

            ResizeOptions resizeOptions;
            
            std::vector<std::unique_ptr<ImagePipeline>> imagePipelines;

            void Construct(const ConfiguredCameras &cameras);
    };
}