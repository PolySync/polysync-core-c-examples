### Enumerate dynamic drivers
This example publishes a `ps_command_msg` to enumerate any dynamic drivers connected to the
PolySync bus.  All dynamic drivers that include their node ID's and guids will receive a `ps_response_msg`.

### Hardware requirements

### Dependencies

### Building and Running the Node
```bash
$ cd data_generator
$ make
$ ./bin/polysync-data-generator-c
```

For more API examples visit the "Turorials" and "Development" sections in the PolySync Help Center [here](https://help.polysync.io/articles/)