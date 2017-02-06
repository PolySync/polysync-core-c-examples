### logfile_to_pcap_convertor

This example is a tool that converts a `ps_byte_array_msg` from a **Velodyne sensor** and converts the buffer to a pcap formatted file. 

It uses the logfile iterator method to traverse an existing `plog` file.

The PCAP file can be opened with the Velodyne VeloView application.

The tool can be tweaked to convert other data to PCAP format, and to work with other sensors.

### Dependencies

Packages: libglib2.0-dev libpcap-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd logfile_to_pcap_convertor
$ make
$ ./bin/polysync-logfile-to-pcap-convertor-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
