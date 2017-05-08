### logfile_iterator

This example is powerful and popular. It uses the logfile API to iterate over a specified logfile (`plog` file) and read the contents. It's important to note that the `plog` data contains the raw, non-abstracted sensor data as it was recieved on the wire. 

In this example a logfile iterator callback is set up, which is called every time a new logfile “record” is seen from the recorded `plog` file. Within the callback, the application has access to the raw data. This allows the application to access, convert, or otherwise process that data. 

This example then prints some details from the header of each of the records.

The logfile iteration happens outside the PolySync time domain, and is execuated as fast as computationally possible. 

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install libglib2.0-dev
```

### Building and running the node

```bash
$ cd logfile_iterator
$ make
$ ./bin/polysync-logfile-iterator-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
