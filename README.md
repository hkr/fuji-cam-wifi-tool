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

To build with OpenCV:
```
cmake ../fuji-cam-wifi-tool -DWITH_OPENCV=yes
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

## Using live preview as a linux webcam

    modprobe v4l2loopback

See what device this created eg /dev/video2

Then running the command (having built with opencv):

    ./tool/fuji_cam_wifi_tool
    fcwt> connect
    fcwt> stream_cv /dev/video2

Opening up the v4l device in chrome or vlc should now show the same output, but without any focus overlays:

    vlc v4l2:///dev/video2

## Wireshark debugging

To dump control messages (excluding `info`) set the following wireshark filter:

    ip.dst == 192.168.0.1 && len(data.data) > 0 && !(data.data[6:2] == 15:10)

To capture the jpeg start of all streamed images use:

    ip.src == 192.168.0.1 && tcp.port == 55742 && data.data[18:2] == ff:d8

protocol is to look under data section of wireshark, first 4 bytes are the
size, next 2 bytes are 01 00, then comes 2 byte command (reversed, network
format ) eg 15 10 = 0x1015 = `single_part` message.
