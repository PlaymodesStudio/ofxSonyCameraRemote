#pragma once

#include "ofMain.h"
#include "../libs/CRSDK/CameraRemote_SDK.h"
#include "ofxSonyCameraCallback.h"
#include <vector>
#include <memory>

/**
 * @brief Main class for interacting with Sony cameras via the Camera Remote SDK
 * 
 * This class provides methods to connect to Sony cameras, control camera settings,
 * capture photos, and handle camera events.
 */
class ofxSonyCameraRemote {
public:
    ofxSonyCameraRemote();
    ~ofxSonyCameraRemote();
    
    /**
     * @brief Initialize the Sony Camera Remote SDK
     * 
     * This must be called before any other methods.
     * 
     * @return true if initialization was successful, false otherwise
     */
    bool setup();
    
    /**
     * @brief Clean up SDK resources
     * 
     * This should be called when finished using the camera.
     */
    void exit();
    
    /**
     * @brief Discover available cameras
     * 
     * @return true if at least one camera was found, false otherwise
     */
    bool enumerateDevices();
    
    /**
     * @brief Connect to a camera
     * 
     * @param deviceIndex The index of the camera in the enumerated devices list (default: 0)
     * @return true if connection was successful, false otherwise
     */
    bool connect(int deviceIndex = 0);
    
    /**
     * @brief Disconnect from the camera
     * 
     * @return true if disconnection was successful, false otherwise
     */
    bool disconnect();
    
    /**
     * @brief Check if connected to a camera
     * 
     * @return true if connected, false otherwise
     */
    bool isConnected() const;
    
    /**
     * @brief Capture a photo with the current settings
     * 
     * @return true if the capture command was sent successfully, false otherwise
     */
    bool capturePhoto();
    
    /**
     * @brief Get a camera property value
     * 
     * @param code The property code (from SCRSDK::CrDeviceProperty enum)
     * @param value Reference to store the property value
     * @return true if the property was retrieved successfully, false otherwise
     */
    bool getProperty(SCRSDK::CrInt32u code, SCRSDK::CrInt64u& value);
    
    /**
     * @brief Set a camera property value
     * 
     * @param code The property code (from SCRSDK::CrDeviceProperty enum)
     * @param value The value to set
     * @return true if the property was set successfully, false otherwise
     */
    bool setProperty(SCRSDK::CrInt32u code, SCRSDK::CrInt64u value);
    
    /**
     * @brief Set ISO sensitivity
     * 
     * @param isoValue The ISO value to set
     * @return true if successful, false otherwise
     */
    bool setIso(int isoValue);
    
    /**
     * @brief Set shutter speed
     * 
     * @param seconds The shutter speed in seconds
     * @return true if successful, false otherwise
     */
    bool setShutterSpeed(double seconds);
    
    /**
     * @brief Set aperture (f-number)
     * 
     * @param fNumber The aperture value (f-number)
     * @return true if successful, false otherwise
     */
    bool setAperture(double fNumber);
    
    /**
     * @brief Register a callback for camera connection events
     * 
     * @param callback The function to call when the camera connects
     */
    void registerConnectCallback(std::function<void()> callback);
    
    /**
     * @brief Register a callback for camera disconnection events
     * 
     * @param callback The function to call when the camera disconnects
     */
    void registerDisconnectCallback(std::function<void(SCRSDK::CrInt32u)> callback);
    
    /**
     * @brief Register a callback for camera error events
     * 
     * @param callback The function to call when a camera error occurs
     */
    void registerErrorCallback(std::function<void(SCRSDK::CrInt32u)> callback);
    
    /**
     * @brief Get the number of enumerated devices
     * 
     * @return The number of cameras found during enumeration
     */
    int getDeviceCount() const;
    
    /**
     * @brief Get the model name of a device
     * 
     * @param deviceIndex The index of the device in the enumerated list
     * @return The model name as a string
     */
    std::string getDeviceModel(int deviceIndex = 0) const;
    
private:
    // SDK handles
    SCRSDK::ICrEnumCameraObjectInfo* mEnumCameraObjInfo;
    std::vector<SCRSDK::ICrCameraObjectInfo*> mDeviceInfoList;
    SCRSDK::CrDeviceHandle mDeviceHandle;
    
    // Callback handler
    std::unique_ptr<ofxSonyCameraCallback> mCallback;
    
    // Connection status
    bool mConnected;
    
    // Helper methods for SDK interaction
    void loadProperties();
};