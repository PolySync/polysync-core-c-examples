### logfile_reader

This example uses the Logfile API to open and read a PolySync logfile `plog` file for replay.

It depends on a file found in the `/tmp/` directory, but can easily be updated to point to any existing `plog` file (the intention of the example).
   /tmp/polysync_logfile.plog
   
Steps to run: 
   1. Update the file `src/logfile_reader.c` on line 74 to point to an existing `plog` file, (point to new rnr_logs location)
   2. Compile
   3. Run

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd logfile_reader
$ make
$ ./bin/polysync-logfile-reader-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
