<h1>Ping Utility</h1>

## About The Project

This is my university project Ping Utility.

Short instructions for use.

First, move to the ping2020/Debug directory.

The utility has several parameters:

- n [number of requests]
- l [package size (bites)]
- w [timeout (milliseconds)]
- a [hostname or ip-address]

Example:

```
ping2020 -n 3 -l 64 -w 5 -a google.com
```

You can only specify hostname or ip-address, the other parameters will be set by default.
