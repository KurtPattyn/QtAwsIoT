# QtAwsIoT

##### Author: [Kurt Pattyn](https://github.com/kurtpattyn).

QtAwsIoT is Qt library that implements a client for [Amazon AWS IoT](https://aws.amazon.com/iot/).

#### Features
- MQTT over secure WebSockets support, including SigV4 token authentication support
- Keep-alive (aka ping)
- Subscribing to topics
- Publishing and receiving messages

#### Missing features
- Native MQTT connectivity  
  Currently only supports MQTT over secure Websockets.
- Thing Shadows
- Reconnection and resubscription
- Queuing of messages

## Building and installing

### Prerequisites
1. **Qt version 5**  
2. **CMake v3.5** or higher.  
3. [QtMqtt](https://github.com/KurtPattyn/QtMqtt/).

### Building
```bash
# Check out the sources.
git clone https://github.com/KurtPattyn/QtAwsIoT.git

# Go into the checked out directory
cd QtAwsIoT

# Create build directory  
mkdir build

# Go into the build directory  
cd build

# Create the build files  
cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_SHARED_LIBS=OFF ..

# To make a release build, change `-DCMAKE_BUILD_TYPE=debug` to `-DCMAKE_BUILD_TYPE=release`  
# To make a dynamic library, change `-DBUILD_SHARED_LIBS=OFF` to `-DBUILD_SHARED_LIBS=ON`

# Build the library  
make
```
### Run unit tests
`make test`

> To enable testing of internal code, add `-DPRIVATE_TESTS_ENABLED` (default: OFF) to the `cmake` command line.

### Installing
`make install`

> This will install the library and the headers to `CMAKE_INSTALL_PREFIX`.  
> `CMAKE_INSTALL_PREFIX` defaults to `/usr/local` on UNIX/macOS and `c:/Program Files` on Windows.  
> To install in another location, add `-DCMAKE_INSTALL_PREFIX="<custom location>"` to the `cmake` command line.

## Usage
Include the following in your `CMakeLists.txt` file  
```CMake
find_package(Qt5AwsIoT)

target_link_libraries(<target> Qt5::AwsIoT)
```

In your C++ source file include the QtAwsIoT module  
```C++
#include <QtAwsIoT>
```

## Examples
See the `examples` directory for examples how to use this library.

### Enabling debug output

To enable debugging information of QtAwsIoT, the following environment variable can be defined.    
`QT_LOGGING_RULES="QtAwsIoT.*.debug=true"`
