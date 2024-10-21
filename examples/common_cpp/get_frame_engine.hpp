#pragma once

#include "sv/sv.h"
#include "resize_options.hpp"
#include "configured_camera.hpp"
#include "image_writer.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace common
{
    enum class SaveImageOptions { PNG, JPEG, TIFF, DISABLED };

    class HotkeyAction;
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
            void GetFrame();
            void Stop();
        
        private:

            ResizeOptions resizeOptions;
            
            void ConstructWindows();
            
            std::vector<std::unique_ptr<ImagePipeline>> imagePipelines;

            void Construct(const ConfiguredCameras &cameras);
    };
}