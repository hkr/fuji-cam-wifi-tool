# fuji-cam-wifi-tool
Trying to reverse-engineer the Fuji X-T10 wifi remote control protocol

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
Supported commands are `connect`, `shutter`, `stream`, `info`, `set_iso`, `aperture`, `white_balance`, `shutter_speed`.
I suggest to look at the code.

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
