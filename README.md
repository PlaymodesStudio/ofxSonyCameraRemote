# ofxSonyCameraRemote

An OpenFrameworks addon for controlling Sony cameras via the Sony Camera Remote SDK.

## Features

- Connect to Sony cameras via USB
- Change camera settings (aperture, ISO, shutter speed)
- Capture photos
- Event-based communication with the camera

## Supported Cameras

- Sony Alpha 7s III (initial focus)
- Other compatible cameras (see Sony Camera Remote SDK documentation)

## Dependencies

- Sony Camera Remote SDK v1.14.00 or later
- OpenFrameworks 0.11.0 or later

## Installation

1. Clone this repository to your OpenFrameworks addons folder:
   ```
   cd openFrameworks/addons
   git clone https://github.com/eloimaduell/ofxSonyCameraRemote.git
   ```

2. Copy the Sony Camera Remote SDK files:
   - Copy header files from `CrSDK_vX.XX.XX_XXXXXXXX_Mac/app/CRSDK/` to `ofxSonyCameraRemote/libs/CRSDK/`
   - Copy binary files from `CrSDK_vX.XX.XX_XXXXXXXX_Mac/external/crsdk/` to `ofxSonyCameraRemote/external/crsdk/`

3. Add the addon to your project using the Project Generator or by adding `ofxSonyCameraRemote` to your project's addons.make file.
## Library Path Issues

The Sony Camera Remote SDK dynamic libraries contain hardcoded absolute paths that reference the original developer's system. 

as ex :
/Users/muramatsu/Projects/sony/_repository/Network-VericRxLibrary_231H/RxLib/core/build/bin/macosArm64/releaseShared/libmonitor_protocol.dylib

These paths need to be fixed for the libraries to work correctly on your system.

### Using the fix_dylib_paths.sh Script

After installing the SDK files, you need to run the included script to fix the library paths:

```bash
cd openFrameworks/addons/ofxSonyCameraRemote
chmod +x fix_dylib_paths.sh
./fix_dylib_paths.sh
```

This script:
1. Changes the install names of the main libraries to use @rpath
2. Updates the inter-library dependencies to use relative paths
3. Fixes all libraries in the CrAdapter folder

Without running this script, you may encounter errors like "dyld: Library not loaded" when trying to use the addon, as the libraries will be looking for paths that exist only on the original developer's system.

## Usage


### Basic Example

```cpp
#include "ofxSonyCameraRemote.h"

class ofApp : public ofBaseApp {
public:
    ofxSonyCameraRemote camera;
    
    void setup() {
        // Initialize the camera SDK
        camera.setup();
        
        // Register callbacks
        camera.registerConnectCallback([this]() {
            ofLogNotice("ofApp") << "Camera connected!";
        });
        
        camera.registerDisconnectCallback([this](int error) {
            ofLogNotice("ofApp") << "Camera disconnected, error: " << error;
        });
        
        // Find and connect to the camera
        if (camera.enumerateDevices()) {
            camera.connect();
        }
    }
    
    void update() {
        // Update camera status if needed
    }
    
    void draw() {
        // Draw camera status and controls
    }
    
    void keyPressed(int key) {
        if (key == ' ' && camera.isConnected()) {
            // Capture a photo
            camera.capturePhoto();
        }
    }
    
    void exit() {
        // Clean up camera resources
        camera.disconnect();
        camera.exit();
    }
};
```

### Camera Properties

To get and set camera properties:

```cpp
// Get ISO value
SCRSDK::CrInt64u isoValue;
if (camera.getProperty(SCRSDK::CrDeviceProperty_ISO, isoValue)) {
    ofLogNotice("ISO") << isoValue;
}

// Set ISO value
camera.setProperty(SCRSDK::CrDeviceProperty_ISO, 100); // Example ISO value
// Or use the convenience method
camera.setIso(100);
```

## License

This addon is distributed under the MIT License. The Sony Camera Remote SDK has its own licensing terms which must be respected.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
