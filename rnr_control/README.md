### rnr_control

`rnr_control` is more of a tool than an example. It exposes a lot of the functionality of the Record and Replay (RnR) API as a command line tool.

Use this example to command active nodes to open and replay the specified logfile session (a collection of `plog` files), using the `-t <session-id>`. 

```
$ polysync-rnr-control-c -t 1000 
```

It's recommended to allow nodes a few seconds to load and synchronize the first logfile chunk. Use the `-s <time-in-us>` flag to specify a relative start time. For example, to allow nodes up to 5 seconds, use the flag `-s 5000000`.

```
$ polysync-rnr-control-c -t 1000 -s 5000000
```

To stop an active replay session, pass the `-q` flag. 

```
$ polysync-rnr-control-c -q
```

You can see all supported commands by passing the `-h` help flag.  

### Dependencies

Packages: libglib2.0-dev

To install on Ubuntu: 

```bash
sudo apt-get install <package>
```

### Building and running the node

```bash
$ cd rnr_control
$ make
$ ./bin/polysync-rnr-control-c 
```

For more API examples, visit the "Tutorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/).
