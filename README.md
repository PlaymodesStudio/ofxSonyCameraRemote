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
