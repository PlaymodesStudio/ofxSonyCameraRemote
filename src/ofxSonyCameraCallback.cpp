#include "ofxSonyCameraCallback.h"

ofxSonyCameraCallback::ofxSonyCameraCallback() {
    // Initialize callbacks to empty functions
    mConnectCallback = []() {};
    mDisconnectCallback = [](CrInt32u) {};
    mPropertyChangeCallback = []() {};
    mErrorCallback = [](CrInt32u) {};
}

ofxSonyCameraCallback::~ofxSonyCameraCallback() {
    // Clean up any resources if needed
}

void ofxSonyCameraCallback::setConnectCallback(std::function<void()> callback) {
    mConnectCallback = callback;
}

void ofxSonyCameraCallback::setDisconnectCallback(std::function<void(CrInt32u)> callback) {
    mDisconnectCallback = callback;
}

void ofxSonyCameraCallback::setPropertyChangeCallback(std::function<void()> callback) {
    mPropertyChangeCallback = callback;
}

void ofxSonyCameraCallback::setErrorCallback(std::function<void(CrInt32u)> callback) {
    mErrorCallback = callback;
}

// IDeviceCallback implementation
void ofxSonyCameraCallback::OnConnected(DeviceConnectionVersioin version) {
    ofLogNotice("ofxSonyCameraCallback") << "Camera connected, version: " << version;
    mConnectCallback();
}

void ofxSonyCameraCallback::OnDisconnected(CrInt32u error) {
    ofLogNotice("ofxSonyCameraCallback") << "Camera disconnected, error: " << error;
    mDisconnectCallback(error);
}

void ofxSonyCameraCallback::OnPropertyChanged() {
    ofLogVerbose("ofxSonyCameraCallback") << "Camera property changed";
    mPropertyChangeCallback();
}

void ofxSonyCameraCallback::OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes) {
    ofLogVerbose("ofxSonyCameraCallback") << "Camera property changed with " << num << " code(s)";
    // Here you could add specific handling for different property codes
    mPropertyChangeCallback();
}

void ofxSonyCameraCallback::OnLvPropertyChanged() {
    ofLogVerbose("ofxSonyCameraCallback") << "Live view property changed";
    // Handle live view property changes
}

void ofxSonyCameraCallback::OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes) {
    ofLogVerbose("ofxSonyCameraCallback") << "Live view property changed with " << num << " code(s)";
    // Here you could add specific handling for different live view property codes
}

void ofxSonyCameraCallback::OnCompleteDownload(CrChar* filename, CrInt32u type) {
    ofLogNotice("ofxSonyCameraCallback") << "Download completed: " << filename << ", type: " << type;
    // Handle download completion
}

void ofxSonyCameraCallback::OnNotifyContentsTransfer(CrInt32u notify, CrContentHandle handle, CrChar* filename) {
    ofLogNotice("ofxSonyCameraCallback") << "Contents transfer notification: " << notify 
                                         << ", handle: " << handle 
                                         << ", filename: " << (filename ? filename : "null");
    // Handle content transfer notification
}

void ofxSonyCameraCallback::OnWarning(CrInt32u warning) {
    ofLogWarning("ofxSonyCameraCallback") << "Camera warning: " << warning;
    // Handle warning
}

void ofxSonyCameraCallback::OnError(CrInt32u error) {
    ofLogError("ofxSonyCameraCallback") << "Camera error: " << error;
    mErrorCallback(error);
}