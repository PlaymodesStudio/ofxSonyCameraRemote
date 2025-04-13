#pragma once

#include "ofMain.h"
#include "../libs/CRSDK/CameraRemote_SDK.h"
#include "../libs/CRSDK/IDeviceCallback.h"
#include <functional>

/**
 * @brief Callback handler for Sony Camera events
 * 
 * This class implements the SCRSDK::IDeviceCallback interface to receive
 * events from the Sony Camera Remote SDK and forward them to the application.
 */
class ofxSonyCameraCallback : public SCRSDK::IDeviceCallback {
public:
    ofxSonyCameraCallback();
    virtual ~ofxSonyCameraCallback();
    
    // Register callbacks
    void setConnectCallback(std::function<void()> callback);
    void setDisconnectCallback(std::function<void(SCRSDK::CrInt32u)> callback);
    void setPropertyChangeCallback(std::function<void()> callback);
    void setErrorCallback(std::function<void(SCRSDK::CrInt32u)> callback);
    
    // IDeviceCallback implementation
    virtual void OnConnected(SCRSDK::DeviceConnectionVersioin version) override;
    virtual void OnDisconnected(SCRSDK::CrInt32u error) override;
    virtual void OnPropertyChanged() override;
    virtual void OnPropertyChangedCodes(SCRSDK::CrInt32u num, SCRSDK::CrInt32u* codes) override;
    virtual void OnLvPropertyChanged() override;
    virtual void OnLvPropertyChangedCodes(SCRSDK::CrInt32u num, SCRSDK::CrInt32u* codes) override;
    virtual void OnCompleteDownload(SCRSDK::CrChar* filename, SCRSDK::CrInt32u type) override;
    virtual void OnNotifyContentsTransfer(SCRSDK::CrInt32u notify, SCRSDK::CrContentHandle handle, SCRSDK::CrChar* filename) override;
    virtual void OnWarning(SCRSDK::CrInt32u warning) override;
    virtual void OnError(SCRSDK::CrInt32u error) override;
    
private:
    std::function<void()> mConnectCallback;
    std::function<void(SCRSDK::CrInt32u)> mDisconnectCallback;
    std::function<void()> mPropertyChangeCallback;
    std::function<void(SCRSDK::CrInt32u)> mErrorCallback;
};