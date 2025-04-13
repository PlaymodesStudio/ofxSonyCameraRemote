#pragma once

#include "ofMain.h"
#include "ofxSonyCameraRemote.h"

// CrInt32u is defined in the global namespace, not in SCRSDK

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Sony Camera Remote instance
    ofxSonyCameraRemote camera;
    
    // Camera status
    bool connected;
    
    // Camera properties
    std::string cameraModel;
    std::string isoValue;
    std::string apertureValue;
    std::string shutterSpeedValue;
    
    // UI elements
    ofTrueTypeFont font;
    ofTrueTypeFont titleFont;
    
    // Camera event callbacks
    void onCameraConnected();
    void onCameraDisconnected(CrInt32u reason);
    void onCameraError(CrInt32u error);
    
    // Helper methods
    void updateCameraProperties();
    void drawPropertyList(int x, int y);
    void drawInstructions(int x, int y);
};
