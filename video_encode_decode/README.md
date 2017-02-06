### video_encode_decode
This is a video API example
You would use this example if you needed a node to talk directly to a video device, and didn’t want to use the `generic-video-device` driver, which supports all V4L USB video devices.
It connects to a USB webcam (that must be Video4Linux (V4L) compatible), encodes the sensor data, and then decodes the same image stream to show the image processing pipeline.
This DOES NOT connect to a node defined in the SDF, it connects directly to the hardware device using the Video API.
It expects XXXXX pixel format

### Hardware requirements
Video Device:  USB webcam

### Dependencies
Packages: libglib2.0-dev libgstreamer1.0-0
To install on Ubuntu
`sudo apt-get install <package>`

### Building and Running the Node
```bash
$ cd video_encode_decode
$ make
$ ./bin/polysync-video-encode-decode-c 
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)
