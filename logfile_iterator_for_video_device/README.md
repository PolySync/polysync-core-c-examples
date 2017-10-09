### logfile_iterator_for_video_device

This example is based on the logfile_iterator example. It differs in that it provides a concrete example of iterating over video device data stored as a `ps_byte_array_msg` in a `plog` file.

Within the logfile iterator callback, this example shows how to use the ps_image_data_msg to convert raw, non-abstracted `plog` data to
individual image files.

### Dependencies

To install on Ubuntu:

```bash
sudo apt-get install libusb-1.0
```

- libuvc
    - If you're not on amd64, Ubuntu 16.04 you'll need to build this library yourself. Follow the instructions [here](https://github.com/ktossell/libuvc) but instead of `sudo make install` as the last step, copy the `libuvc.a` that project builds to this project's `lib` directory.


### Building and running the node

```bash
$ cd logfile_iterator_for_video_device
$ make
$ ./bin/polysync-logfile-iterator-for-video_device-c
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
