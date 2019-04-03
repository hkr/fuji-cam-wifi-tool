# fuji-cam-wifi-tool
Trying to reverse-engineer the wifi remote control protocol used by Fuji X series cameras

## Build instructions

### Mac OS X, Linux, Windows
```
mkdir fuji-tool
cd fuji-tool
git clone https://github.com/hkr/fuji-cam-wifi-tool
git -C fuji-cam-wifi-tool submodule update --init --recursive
mkdir build
cd build
cmake ../fuji-cam-wifi-tool
cmake --build .
```

## Run the tool

The tool fuji_cam_wifi_tool is an interactive shell (based on [linenoise](https://github.com/arangodb/linenoise-ng)) that can be used to send commands to the camera.
At this time it is very limited and mostly undocumented.
Supported commands are:
```
fcwt> help
Usage:
    help|connect|info|shutter|focus-unlock
    increment|decrement TARGET
    set TARGET VALUE1 [VALUE2]
    stream [cv]
TARGET:
    iso:           takes an integer value (e.g. 200)
    f-number:      takes a float value (e.g. 3.5)
    focus-point:   takes a pair of values denoting the focus point coordinates X and Y (e.g. 5 5)
    shutter-speed: takes a value of the form N/M or N (e.g. 1/20, 3)
    exposure-compensation:
                   takes a float value (e.g. 1.3)
    white-balance|film-simulation|self-timer|flash:
                   takes an integer value. Value is one of the values in camera properties
```
Mac OS X:
```
./tool/fuji_cam_wifi_tool
```
Steps to connect the tool to the camera:
- Press the "Wireless Communication" button on the X-T10
- Connect your computer to the camera's Wifi Access Point (in ad-hoc mode)
- Send the `connect` command.
- Press the `OK (Change)` button (only needed the first time, `fuji_cam_wifi_tool` connects as "HackedClient", see main.cpp)
- On success the tool should print a lot of debug data and the current camera settings
- Try taking a picture by sending "shutter"
