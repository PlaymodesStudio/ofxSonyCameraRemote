#include "ofApp.h"

// Import necessary enum values
using SCRSDK::CrDeviceProperty_IsoSensitivity;
using SCRSDK::CrDeviceProperty_FNumber;
using SCRSDK::CrDeviceProperty_ShutterSpeed;

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(40);
    
    // Load fonts
    font.load(OF_TTF_SANS, 12);
    titleFont.load(OF_TTF_SANS, 24);
    
    // Initialize camera variables
    connected = false;
    cameraModel = "Not Connected";
    isoValue = "Unknown";
    apertureValue = "Unknown";
    shutterSpeedValue = "Unknown";
    
    // Initialize the Sony Camera Remote SDK
    bool success = camera.setup();
    if (!success) {
        ofLogError("ofApp") << "Failed to initialize Camera Remote SDK";
        return;
    }
    
    // Register callbacks for camera events
    camera.registerConnectCallback([this]() { this->onCameraConnected(); });
    camera.registerDisconnectCallback([this](CrInt32u reason) { this->onCameraDisconnected(reason); });
    camera.registerErrorCallback([this](CrInt32u error) { this->onCameraError(error); });
    
    // Get SDK version
    CrInt32u sdkVersion = camera.getSDKVersion();
    sdkVersionString = "SDK Version: " + ofToString(sdkVersion);
    
    // Automatically try to find and connect to a camera
    if (camera.enumerateDevices()) {
        ofLogNotice("ofApp") << "Found " << camera.getDeviceCount() << " camera(s)";
        camera.connect();
    } else {
        ofLogNotice("ofApp") << "No cameras found. Connect a camera and press 'c' to try again.";
    }
}

//--------------------------------------------------------------
void ofApp::update() {
    // Update logic can be added here if needed
}

//--------------------------------------------------------------
void ofApp::draw() {
    int margin = 20;
    int y = margin;
    
    // Draw title
    titleFont.drawString("Sony Camera Remote Example", margin, y + 24);
    y += 50;
    
    // Draw SDK version
    font.drawString(sdkVersionString, margin, y);
    y += 20;
    
    // Draw connection status
    font.drawString("Status: " + std::string(connected ? "Connected" : "Disconnected"), margin, y);
    y += 20;
    
    if (connected) {
        font.drawString("Camera: " + cameraModel, margin, y);
        y += 30;
        
        // Draw properties
        drawPropertyList(margin, y);
        y += 120;
        
        // Draw instructions
        drawInstructions(margin, y);
    } else {
        font.drawString("Press 'c' to connect to camera", margin, y + 30);
    }
}

//--------------------------------------------------------------
void ofApp::drawPropertyList(int x, int y) {
    font.drawString("ISO: " + isoValue, x, y);
    font.drawString("Aperture: " + apertureValue, x, y + 20);
    font.drawString("Shutter Speed: " + shutterSpeedValue, x, y + 40);
}

//--------------------------------------------------------------
void ofApp::drawInstructions(int x, int y) {
    font.drawString("Controls:", x, y);
    font.drawString("SPACE - Capture Photo", x, y + 20);
    font.drawString("i - Change ISO", x, y + 40);
    font.drawString("a - Change Aperture", x, y + 60);
    font.drawString("s - Change Shutter Speed", x, y + 80);
    font.drawString("d - Disconnect", x, y + 100);
    font.drawString("c - Connect", x, y + 120);
}

//--------------------------------------------------------------
void ofApp::exit() {
    // Clean up camera resources
    camera.disconnect();
    camera.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    switch (key) {
        case 'c':
            // Connect to camera
            if (!connected) {
                if (camera.enumerateDevices()) {
                    camera.connect();
                }
            }
            break;
            
        case 'd':
            // Disconnect from camera
            if (connected) {
                camera.disconnect();
            }
            break;
            
        case ' ':
            // Capture photo
            if (connected) {
                ofLogNotice("ofApp") << "Capturing photo...";
                camera.capturePhoto();
            }
            break;
            
        case 'i':
            // Change ISO (simplified example, would need a proper UI in a real app)
            if (connected) {
                camera.setIso(100); // Example ISO value
                ofLogNotice("ofApp") << "Setting ISO to 100";
                updateCameraProperties();
            }
            break;
            
        case 'a':
            // Change aperture (simplified example)
            if (connected) {
                camera.setAperture(2.8); // Example aperture value
                ofLogNotice("ofApp") << "Setting aperture to f/2.8";
                updateCameraProperties();
            }
            break;
            
        case 's':
            // Change shutter speed (simplified example)
            if (connected) {
                camera.setShutterSpeed(1.0/125.0); // Example shutter speed value
                ofLogNotice("ofApp") << "Setting shutter speed to 1/125";
                updateCameraProperties();
            }
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::onCameraConnected() {
    connected = true;
    ofLogNotice("ofApp") << "Camera connected!";
    
    // Get camera model
    cameraModel = camera.getDeviceModel();
    
    // Update property display
    updateCameraProperties();
}

//--------------------------------------------------------------
void ofApp::onCameraDisconnected(CrInt32u reason) {
    connected = false;
    ofLogNotice("ofApp") << "Camera disconnected, reason code: " << reason;
    
    cameraModel = "Not Connected";
    isoValue = "Unknown";
    apertureValue = "Unknown";
    shutterSpeedValue = "Unknown";
}

//--------------------------------------------------------------
void ofApp::onCameraError(CrInt32u error) {
    ofLogError("ofApp") << "Camera error, code: " << error;
}

//--------------------------------------------------------------
void ofApp::updateCameraProperties() {
    // Get current property values
    CrInt64u value;
    
    if (camera.getProperty(CrDeviceProperty_IsoSensitivity, value)) {
        isoValue = ofToString(value); // In a real app, this would map to a human-readable ISO value
    }
    
    if (camera.getProperty(CrDeviceProperty_FNumber, value)) {
        // Convert to a more readable format (this is simplified)
        double fNumber = value / 100.0;
        apertureValue = "f/" + ofToString(fNumber, 1);
    }
    
    if (camera.getProperty(CrDeviceProperty_ShutterSpeed, value)) {
        // Convert to a more readable format (this is simplified)
        shutterSpeedValue = "1/" + ofToString(value);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
