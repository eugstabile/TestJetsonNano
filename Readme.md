# LibSV Software Package

## Disclaimer

Software provided in this package is in the prototype stage of development and is not intended for use in production.

## Supported platforms

- Jetson TX2 
  - L4T 32.6.1, JetPack 4.6
- Jetson Xavier AGX
  - L4T 32.6.1, JetPack 4.6
- Jetson Nano
  - L4T 32.6.1, JetPack 4.6
- Jetson Xavier NX
  - L4T 32.6.1, JetPack 4.6
- Jetson TX2 NX
  - L4T 32.6.1, JetPack 4.6
- DragonBoard 410c 
  - Linaro Debian Buster, kernel 4.14.15

## LibSV software library

### About 

LibSV (Streamlined V4L2 Library) is a software library that provides simplified and consistent control of sensors exposed using V4L2 API.

Main features of LibSV are:

- ease of use - simple interface that wraps the complex and low-level V4L2 API 
- consistency - consistent interface across all supported platforms

### Interface

Interfaces for following languages are provided:

- C++
- C

LibSV interface is documented in the file `/usr/include/sv/sv.h`.

### Examples

#### Available examples

- acquire_image - basic C++ example that configures sensors and acquires images
- save_image - C++ example that saves acquired images as raw files
- display_image - C++ example that displays acquired images using OpenCV
- acquire_image_c - basic C example that configures sensor and acquires images
- save_image_c - C example that saves acquired images as raw files

#### Executable examples

Executable examples are located in `/opt/framos/libsv/bin` folder. Prebuilt examples are supported on Jetson platforms and on the Dragon Board 410c platform.

#### Example sources

Source code for examples is located in `/opt/framos/libsv/examples` folder.

Examples can be built using the `/opt/framos/libsv/examples/bin/build.sh` script.

Prerequisites for building:
- supported only on Jetson platforms
- g++ compiler
- OpenCV 4.5.2
- libgtkglext1
- libcanberra-gtk0
- jetson_multimedia_api package
  - can be installed with `sudo apt install nvidia-l4t-jetson-multimedia-api`

### Platform-specific processing

Most user-space applications cannot handle raw data provided by image sensors. Moreover, different image pipelines on different platforms produce different raw outputs. Because of that, LibSV can reformat the raw data it acquires from image sensor into a raw format that most applications can handle and that is consistent across platforms. Note that using this feature may decrease performance. See examples for reference.

## Package contents

### `/usr/include/sv`

Contains LibSV library headers.

### `/usr/lib/libsv.so`

LibSV software library.

### `/opt/framos/libsv/argus`

For Jetson platforms.

Contains following folders:
- `L4T-28.2.1` - LibArgus files compatible with L4T 28.2.1
- `L4T-32.2.1` - LibArgus files compatible with L4T 32.2.1
- `L4T-32.3.1` - LibArgus files compatible with L4T 32.3.1
- `L4T-32.4.2` - LibArgus files compatible with L4T 32.4.2
- `L4T-32.4.3` - LibArgus files compatible with L4T 32.4.3
- `L4T-32.4.4` - LibArgus files compatible with L4T 32.4.4
- `L4T-32.5.1` - LibArgus files compatible with L4T 32.5.1
- `L4T-32.6.1` - LibArgus files compatible with L4T 32.6.1

Each folder contains following files:
- `argus_camera` - example application that uses LibArgus software library to capture image from sensor, part of the Tegra Multimedia API
- `LICENSE.txt` - LibArgus license

### `/opt/framos/libsv/bin`

Contains executable LibSV examples and examples of using V4L2 NV Extensions API.

### `/opt/framos/libsv/examples`

Contains source code and build script for LibSV examples and examples of using V4L2 NV Extensions API.

### `/opt/framos/libsv/opencv`

Contains binaries, readme and license for OpenCV v4.5.2. It is used by LibSV example to process and display image.

### `/opt/framos/libsv/Changelog.txt`

Changelog for this package.

### `/opt/framos/libsv/Readme.md`

This readme file.


## V4L2 NV Extensions API (Only for Jetson platforms)
This package also contains examples of using libv4l2_nvargus interface for streaming.

### Available examples
- acquire_image_isp - basic C++ example that configures sensors and acquires images
- save_image_isp - C++ example that saves acquired images as raw files
- display_image_isp - C++ example that displays acquired images using OpenCV

### Executable examples

Executable examples are located in `/opt/framos/libsv/bin` folder. For using the display_image_isp example, libgtkglext1 package is required.

### Examples source

Source code for examples is located in `/opt/framos/libsv/examples` folder.

Examples can be built using the `/opt/framos/libsv/examples/bin/build.sh` script.

Prerequisites for building:

- g++ compiler
- OpenCV 4.5.2
- libgtkglext1
- libcanberra-gtk0
- jetson_multimedia_api package
  - can be installed with `sudo apt install nvidia-l4t-jetson-multimedia-api`

### Known issues
- V4L2 NV Extensions API supports only one camera per process. Controlling two or more cameras from the same application is not possible. In case of multiple cameras, each camera can be controlled through its on instance of the application.
- Modes with 8bpp are not supported.
- On some sensors, the max frame rate for 12bpp modes is limited to the max frame rate of the same frame size in 10bpp mode.
