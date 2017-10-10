### logfile_iterator_for_video_device

This example is based on the logfile_iterator example. It differs in that it provides a concrete example of iterating over video device data stored as a `ps_image_data_msg` in a `plog` file.

Within the logfile iterator callback, this example shows how to use the ps_image_data_msg to convert raw, non-abstracted `plog` data to
individual image files.

### Requirements

This example requries that the image data was logged with YUYV pixel formatting. This is the default format for video-device nodes defined in the SDF. To verify the logged pixel format, check the SDF entry for the device.

### Dependencies

To install on Ubuntu:

```bash
sudo apt-get install libusb-1.0
sudo apt install libpopt-dev
```

- libuvc
    - If you're not on amd64, Ubuntu 16.04 you'll need to build this library yourself. Follow the instructions [here](https://github.com/ktossell/libuvc). **NOTE: Instead of `sudo make install` as the last step in the above process, copy the `libuvc.a` that project builds to this project's `lib` directory (if you already installed it, copy the library and you'll be fine).


### Building

```bash
$ cd logfile_iterator_for_video_device
$ make
```

### Running
```
$ ./bin/polysync-logfile-iterator-for-video_device-c -p <PATH> -o <PATH> -f <FORMAT>
```

### Usage
```
Usage: polysync-logfile-iterator-for-video-device-c [OPTION...]
  -p, --path=PATH         required, path to video-device plog
  -o, --outdir=PATH       specifiy image file output directory path, defaults
                          to /tmp
  -f, --format=FORMAT     specifiy file format 'bmp' or 'ppm', defaults to
                          'ppm'

Help options:
  -?, --help              Show this help message
      --usage             Display brief usage message

```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](http://docs.polysync.io/articles/).
