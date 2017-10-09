### logfile_iterator_for_video_device

This example is based on the logfile_iterator example. It differs in that it provides a concrete example of iterating over video device data stored as a `ps_byte_array_msg` in a `plog` file.

Within the logfile iterator callback, this example shows how to use the ps_image_data_msg to convert raw, non-abstracted `plog` data to
individual image files.

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu:

```bash
sudo apt-get install libusb-1.0
```

### Building and running the node

```bash
$ cd logfile_iterator_for_video_device
$ make
$ ./bin/polysync-logfile-iterator-for-video_device-c
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
