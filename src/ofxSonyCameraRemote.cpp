#include "ofxSonyCameraRemote.h"

ofxSonyCameraRemote::ofxSonyCameraRemote() 
    : mEnumCameraObjInfo(nullptr)
    , mDeviceHandle(0)
    , mConnected(false) {
}

ofxSonyCameraRemote::~ofxSonyCameraRemote() {
    exit();
}
bool ofxSonyCameraRemote::setup() {
    // Initialize the Sony SDK
    bool result = SCRSDK::Init();
    if (!result) {
        ofLogError("ofxSonyCameraRemote") << "Failed to initialize Sony Camera Remote SDK";
        return false;
    }
    // Create callback handler
    mCallback = std::make_unique<ofxSonyCameraCallback>();
    
    return true;
}

void ofxSonyCameraRemote::exit() {
    // Disconnect if connected
    if (mConnected) {
        disconnect();
    }
    
    // Clean up device info list
    mDeviceInfoList.clear();
    
    // Release enumeration object
    if (mEnumCameraObjInfo) {
        mEnumCameraObjInfo->Release();
        mEnumCameraObjInfo = nullptr;
    }
    
    // Release SDK resources
    SCRSDK::Release();
}

bool ofxSonyCameraRemote::enumerateDevices() {
    // Clear previous device list
    mDeviceInfoList.clear();
    
    // Release previous enumeration
    if (mEnumCameraObjInfo) {
        mEnumCameraObjInfo->Release();
        mEnumCameraObjInfo = nullptr;
    }
    
    // Enumerate connected cameras
    CrError err = SCRSDK::EnumCameraObjects(&mEnumCameraObjInfo);
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to enumerate camera devices: " << err;
        return false;
    }
    
    // Get count of connected cameras
    int count = mEnumCameraObjInfo->GetCount();
    if (count == 0) {
        ofLogNotice("ofxSonyCameraRemote") << "No cameras found";
        return false;
    }
    
    // Store camera info objects
    for (int i = 0; i < count; i++) {
        const ICrCameraObjectInfo* camera = mEnumCameraObjInfo->GetCameraObjectInfo(i);
        mDeviceInfoList.push_back(camera);
    }
    
    ofLogNotice("ofxSonyCameraRemote") << "Found " << count << " camera(s)";
    return true;
}

bool ofxSonyCameraRemote::connect(int deviceIndex) {
    // Check if already connected
    if (mConnected) {
        ofLogWarning("ofxSonyCameraRemote") << "Already connected to a camera";
        return false;
    }
    
    // Check if we have devices in the list
    if (mDeviceInfoList.empty()) {
        if (!enumerateDevices()) {
            return false;
        }
    }
    
    // Check device index
    if (deviceIndex < 0 || deviceIndex >= mDeviceInfoList.size()) {
        ofLogError("ofxSonyCameraRemote") << "Invalid device index: " << deviceIndex;
        return false;
    }
    
    // Connect to the camera
    const ICrCameraObjectInfo* camera = mDeviceInfoList[deviceIndex];
    // SDK requires non-const pointer even though it shouldn't modify it
    CrError err = SCRSDK::Connect(
        const_cast<ICrCameraObjectInfo*>(camera), // Camera info (cast const away)
        mCallback.get(),              // Callback handler
        &mDeviceHandle,               // Output device handle
        CrSdkControlMode_Remote,      // Remote control mode
        CrReconnecting_ON     // Auto reconnect
    );
    
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to connect to camera: " << err;
        return false;
    }
    
    mConnected = true;
    ofLogNotice("ofxSonyCameraRemote") << "Connected to camera: " << camera->GetModel();
    
    // Load initial properties
    loadProperties();
    
    return true;
}

bool ofxSonyCameraRemote::disconnect() {
    if (!mConnected) {
        ofLogWarning("ofxSonyCameraRemote") << "Not connected to any camera";
        return false;
    }
    
    // Disconnect from the camera
    CrError err = SCRSDK::Disconnect(mDeviceHandle);
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to disconnect from camera: " << err;
        return false;
    }
    
    // Release device
    err = SCRSDK::ReleaseDevice(mDeviceHandle);
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to release camera: " << err;
    }
    
    mConnected = false;
    mDeviceHandle = 0;
    
    ofLogNotice("ofxSonyCameraRemote") << "Disconnected from camera";
    return true;
}

bool ofxSonyCameraRemote::isConnected() const {
    return mConnected;
}

bool ofxSonyCameraRemote::capturePhoto() {
    if (!mConnected) {
        ofLogError("ofxSonyCameraRemote") << "Not connected to any camera";
        return false;
    }
    
    // Send shutter command
    CrError err = SCRSDK::SendCommand(
        mDeviceHandle,                // Device handle
        CrCommandId_Release,  // Shutter command
        CrCommandParam_Down   // Press shutter
    );
    
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to capture photo: " << err;
        return false;
    }
    
    return true;
}

void ofxSonyCameraRemote::loadProperties() {
    if (!mConnected) {
        ofLogError("ofxSonyCameraRemote") << "Cannot load properties: Not connected";
        return;
    }
    
    // Get all device properties
    CrDeviceProperty* properties = nullptr;
    CrInt32 numOfProperties = 0;
    CrError err = SCRSDK::GetDeviceProperties(
        mDeviceHandle,    // Device handle
        &properties,      // Output properties
        &numOfProperties  // Output count
    );
    
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to get device properties: " << err;
        return;
    }
    
    // Log property information
    ofLogNotice("ofxSonyCameraRemote") << "Loaded " << numOfProperties << " properties";
    
    // Release properties when done
    SCRSDK::ReleaseDeviceProperties(mDeviceHandle, properties);
}

// Property getter and setter implementation
bool ofxSonyCameraRemote::getProperty(CrInt32u code, CrInt64u& value) {
    if (!mConnected) {
        ofLogError("ofxSonyCameraRemote") << "Cannot get property: Not connected";
        return false;
    }
    
    // Get specific device properties
    CrDeviceProperty* properties = nullptr;
    CrInt32 numOfProperties = 0;
    CrError err = SCRSDK::GetSelectDeviceProperties(
        mDeviceHandle,    // Device handle
        1,                // Number of property codes
        &code,            // Property code
        &properties,      // Output properties
        &numOfProperties  // Output count
    );
    
    if (err != CrError_None || numOfProperties == 0) {
        ofLogError("ofxSonyCameraRemote") << "Failed to get property " << code << ": " << err;
        return false;
    }
    
    // Extract the value
    value = properties[0].GetCurrentValue();
    
    // Release properties
    ReleaseDeviceProperties(mDeviceHandle, properties);
    
    return true;
}

bool ofxSonyCameraRemote::setProperty(CrInt32u code, CrInt64u value) {
    if (!mConnected) {
        ofLogError("ofxSonyCameraRemote") << "Cannot set property: Not connected";
        return false;
    }
    
    // Create property to set
    CrDeviceProperty prop;
    prop.SetCode(code);
    prop.SetCurrentValue(value);
    prop.SetValueType(CrDataType_UInt64);
    
    // Set the property
    CrError err = SCRSDK::SetDeviceProperty(
        mDeviceHandle,  // Device handle
        &prop           // Property to set
    );
    
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to set property " << code << ": " << err;
        return false;
    }
    
    return true;
}

// Helper methods for specific properties
bool ofxSonyCameraRemote::setIso(int isoValue) {
    // Convert ISO value to SDK format and set the property
    // This would need a proper mapping from ISO values to SDK enum values
    // For now, we just use the value directly
    CrInt64u sdkValue = isoValue;
    return setProperty(CrDeviceProperty_IsoSensitivity, sdkValue);
}

bool ofxSonyCameraRemote::setShutterSpeed(double seconds) {
    // Convert shutter speed to SDK format and set the property
    // This would need a proper mapping from seconds to SDK enum values
    // For now, we use a placeholder value
    CrInt64u sdkValue = static_cast<CrInt64u>(1.0 / seconds);
    return setProperty(CrDeviceProperty_ShutterSpeed, sdkValue);
}

bool ofxSonyCameraRemote::setAperture(double fNumber) {
    // Convert aperture to SDK format and set the property
    // This would need a proper mapping from f-number to SDK enum values
    // For now, we use a placeholder value
    CrInt64u sdkValue = static_cast<CrInt64u>(fNumber * 100);
    return setProperty(CrDeviceProperty_FNumber, sdkValue);
}

// Callback registration
void ofxSonyCameraRemote::registerConnectCallback(std::function<void()> callback) {
    if (mCallback) {
        mCallback->setConnectCallback(callback);
    }
}

void ofxSonyCameraRemote::registerDisconnectCallback(std::function<void(CrInt32u)> callback) {
    if (mCallback) {
        mCallback->setDisconnectCallback(callback);
    }
}

void ofxSonyCameraRemote::registerErrorCallback(std::function<void(CrInt32u)> callback) {
    if (mCallback) {
        mCallback->setErrorCallback(callback);
    }
}

int ofxSonyCameraRemote::getDeviceCount() const {
    return mDeviceInfoList.size();
}

std::string ofxSonyCameraRemote::getDeviceModel(int deviceIndex) const {
    if (deviceIndex < 0 || deviceIndex >= mDeviceInfoList.size()) {
        return "Unknown";
    }
    return mDeviceInfoList[deviceIndex]->GetModel();
}

CrInt32u ofxSonyCameraRemote::getSDKVersion() const {
    return SCRSDK::GetSDKVersion();
}

