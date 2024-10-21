# FRAMOS Sensor Module Ecosystem - Software User Guide

---

## Version Info

Version: 2.1.0

Date: 2021-11-05

---

## Getting Started

Getting started with [FRAMOS Sensor Module Ecosystem](https://www.framos.com/en/fsm-startup) and FSM_Jetson-*_UserGuide.

This document contains software installation instructions for FRAMOS Sensor Module Ecosystem on Nvidia Jetson platforms (TX2, Xavier AGX, Nano, Xavier NX and TX2 NX) and Dragon Board 410c platform.

---

## Installations on Target system
- **Note**: Test Target system (Nvidia Jetson Xavier AGX) IP address: 10.32.66.6, user name: nvidia, password: nvidia
- **Note**: If the user you created differs from user: nvidia, changes to commands when copying files from host to target will be necessary.

Prerequisites:

- `framos-libsv*.deb` package copied to the target system
  - Copy `framos-libsv.deb` package to home folder on the target system (using SSH or your preferred method):
        ```
    scp ~/Downloads/framos-libsv_*_arm64.deb nvidia@10.32.66.6:.
        ```
  
- target system with Internet access, required for automatic installation of dependencies

**Install the software package**:

```
sudo apt update && sudo apt install ./framos-libsv_*_arm64.deb
```

---

## Software usage

**Using and building LibSV examples**:

Using prebuilt examples is supported on all Jetson platforms mentioned at the beginning of this document and also on Dragon Board 410c platform. Building examples from source is supported only on Jetson platforms.

Prerequisites:
- Installed OpenCV, LibSV, Jetson Multimedia API, libgtkglext1 and libcanberra-gtk0 library

Using example binaries:

```
cd ~
/opt/framos/libsv/bin/acquire_image
/opt/framos/libsv/bin/save_image
/opt/framos/libsv/bin/display_image
/opt/framos/libsv/bin/acquire_image_c
/opt/framos/libsv/bin/save_image_c
```

Building examples:

1. Install Jetson Multimeda API:

   ```
   sudo apt update && sudo apt install nvidia-l4t-jetson-multimedia-api
   ```
2. Copy the source files and build script to a location with user permissions, for example home directory:

   ```
   cp -r /opt/framos/libsv/examples ~
   ```

3. Launch the build script

   ```
   cd ~/examples/bin
   ./build.sh
   ```

   This will build all examples and place the resulting binaries in the `build` folder.

4. Launch one of the examples

    ```
    cd ../build/
    ./display_image
    ./acquire_image
    ./save_image
    ./acquire_image_c
    ./save_image_c
    ```

**Using and building LibArgus examples**

- **Note**: LibArgus is only supported on Nvidia Jetson platforms (TX2, Xavier AGX, Nano, Xavier NX and TX2 NX)
- The LibArgus library is automatically installed with L4T OS. LibArgus is using the hardware Image Signal Processing available on Nvidia Jetson platforms.
- The sources of LibArgus examples are provided by Nvidia as part of the Jetson Multimedia API.

Using example binaries:

1. To check the L4T version on a platform:

   ```
   cat /etc/nv_tegra_release
   ```

   
2. Launch the example. Replace `{L4T_VERSION}` with the version of L4T being used:
   ```
   cd ~
   ```

   ```
   /opt/framos/libsv/argus/L4T-{L4T_VERSION}/argus_camera
   ```

   To select the device and resolution in `argus_camera` example application run help and look under `--device` and `--sensormode` property where all probed sensors and supported resolutions are listed:

   ```
   /opt/framos/libsv/argus/L4T-{L4T_VERSION}/argus_camera -h
   ```

   Example for sensor on connector J6 and first supported resolution:

   ```
   /opt/framos/libsv/argus/L4T-{L4T_VERSION}/argus_camera --device=1 --sensormode=0
   ```

Building argus_camera example:

1. Install Jetson Multimeda API:

   ```
   sudo apt update && sudo apt install nvidia-l4t-jetson-multimedia-api
   ```

2. Install required dependencies for argus_camera example:
   ```
   sudo apt install cmake build-essential pkg-config libx11-dev libgtk-3-dev libexpat1-dev libjpeg-dev libgstreamer1.0-dev    
   ```

3. Make a build folder in location with write permissions, for example home directory:

   ```
   cd 

   mkdir argus_camera_build && cd argus_camera_build
   ```
4. Create makefiles:

   ```
   cmake /usr/src/jetson_multimedia_api/argus
   ```
5. Build argus_camera example:
   ```
   make argus_camera
   ```

   Executable file is located in `~/argus_camera_build/apps/camera/ui/camera`.
   These instructions were adapted from the `README.txt` provided by Nvidia in `/usr/src/jetson_multimedia_api/argus/`.

**Timestamps in LibSV**

Each acquired frame in LibSV has a timestamp provided by the V4L2 driver on the specific platform.

**Using other V4L2 software (optional)**

- This is optional step as those packages are mostly for debug/validation usage when developing the drivers. 

1. add `universe` repository:
    ```
    sudo add-apt-repository universe
    sudo apt-get update
    ```
2. install `qv4l2` application:
    ```
    sudo apt-get install qv4l2
    ```
3. install `v4l2` tool:
    ```
    sudo apt-get install v4l-utils
    ```
---

## V4L2 NV Extensions API (Only for Jetson platforms)
This package also contains examples of using libv4l2_nvargus interface for streaming. There are three examples and they can be run as other examples:

```
cd ~
/opt/framos/libsv/bin/acquire_image_isp
/opt/framos/libsv/bin/display_image_isp
/opt/framos/libsv/bin/save_image_isp
```

More information can be found inside Readme file in `/opt/framos/libsv` folder.

---
## Known issues

In this chapter issues that are known will be listed. If the specific issue has a workaround  this also will be described.

### Known issues on Nvidia Jetson platforms

1. Error when launching `display_image` or `display_image_isp`: 

    ```
    error while loading shared libraries: libgtkglext-x11-1.0.so.0: cannot open shared object file: No such file or directory
    ```
    Make sure that the board is connected to the Internet and install the dependency manually:
    
    ````
    sudo apt update && sudo apt install libgtkglext1
    ````
    
2. Error when launching `display_image` or `display_image_isp`: 

    ```
    Failed to load module "canberra-gtk-module"
    ```

    Make sure that the board is connected to the Internet and install the dependency manually:

    ````
    sudo apt update && sudo apt install libcanberra-gtk0
    ````

3. Streaming with slave/master configuration, libArgus stops stream starting from sensor on connector J5 to J8. If the master is placed on connector before slave, it will be set to software standby and slave sensor will lost trigger signals and won't output frames. That will cause argus to crash and reboot will be necessary.

    **Place slave sensors before master on FPA connectors.**

4. Frame rate and Exposure control ranges are updated after start stream. Driver is calculating one-line time reading current Image sensor registers settings. Those registers are modified by driver depending on various Image sensor configuration and set on start stream. After modifying those registers, one-line time is recalculated, and ranges of Frame rate and Exposure controls are updated.

5. If the driver is supporting different pixel formats on certain resolution, control ranges will be taken from the first mode (first supported pixel resolution) and it could be that before first stream those are not the maximum supported ranges by sensor for that resolution and pixel format. Control ranges will be correctly updated after the start stream.


### Known issues on Dragon Board 410c
Example display_image is a resource intensive application. 
It may experience issues running on a platform with limited resources like DragonBoard 410c.
These issues occur on image sensors that have large frame sizes and when software resizing is disabled.

1. Application crashes due to limited RAM space

    The amount of RAM required for displaying a stream in real time may not be available on DragonBoard 410c. As a result, system will terminate the application to free up space.

    Workaround: Resize displayed image ("software image resizing resolution" option in application).

2. Displayed image is corrupted under high load

    High processor load required for displaying the stream in real time can cause the DragonBoard's image acquisition system to generate corrupt images.

    Workaround: Resize displayed image ("software image resizing resolution" option in application). Select a different frame size if software resizing does not help. 

These issues occur only in the display_image example. Other applications are not affected by them.

---
## Revision History

| Version | Date       | Description                                                  |
| ------- | ---------- | ------------------------------------------------------------ |
| 2.1.0   | 2021-11-05 | Added chapter "V4L2 NV Extensions API". Supported platform TX2 NX. Updated chapters "Using and building LibSV examples", "Using and building LibArgus examples" and "Known issues". |
| 2.0.0   | 2021-04-16 | Added instructions for .deb package                          |
| 1.0.2   | 2020-12-01 | Minor bug fixes and improvements                             |
| 1.0.1   | 2020-03-19 | Updated chapter: "Using LibArgus examples"                   |
| 1.0.0   | 2020-03-05 | Initial release                                              |

---
