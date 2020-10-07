# resoca - REmoteSOcketCAn
---

Provides a TCP/IP interface for sending and receiving CAN and CANFD frames.

Resoca uses Protobuf for (De-) Serialization of CAN-Frames.

## resoca-server (Server)
#### Compile
```
pip install conan
mkdir build
cd build
conan install .. --build
cmake ..
make
```
#### Usage:
```
resoca-server v1.2.0 
server options:
  -h [ --help ]          show help message
  -v [ --verbosity ] arg set log level; highest: 0, default: 4
  -c [ --can ] arg       can device to listen on
  --host arg             Host on which to listen to; default: 0.0.0.0
  --port arg             Port on which to listen to; default: 23636

Written by Péter Bohner (xzvf), 2020
```

## python client

A small testing client, written in python, located in the `python` directory.
Run with `python -i test_client.py`.
`stest()` creates a connection to a server on localhost.

## Windows
Windows client is located in `src/windows/ResocaClient`.
It consists of `ResocaClientLib`, which is the client shared library (.dll),
aswell as `ResocaClient`, which is a WPF App, which uses `ResocaClientLib`.
It serves as a working example for usage of the `ResocaClientLib`.

# Documentation.

See the `/doc` folder.

# License
All of the software is licensed under the MIT license:

Copyright 2020 Péter Bohner (a.k.a xzvf, x-zvf)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
