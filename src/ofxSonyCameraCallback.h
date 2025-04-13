#pragma once

#include "ofMain.h"
#include "../libs/CRSDK/include/CameraRemote_SDK.h"
#include "../libs/CRSDK/include/IDeviceCallback.h"

// Note: CrInt32u, CrInt32, CrChar types are defined in the global namespace in CrTypes.h
// Only types specifically defined in the SCRSDK namespace need to be qualified
using SCRSDK::DeviceConnectionVersioin;
using SCRSDK::CrContentHandle;
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
    void setDisconnectCallback(std::function<void(CrInt32u)> callback);
    void setPropertyChangeCallback(std::function<void()> callback);
    void setErrorCallback(std::function<void(CrInt32u)> callback);
    
    // IDeviceCallback implementation
    virtual void OnConnected(SCRSDK::DeviceConnectionVersioin version) override;
    virtual void OnDisconnected(CrInt32u error) override;
    virtual void OnPropertyChanged() override;
    virtual void OnPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
    virtual void OnLvPropertyChanged() override;
    virtual void OnLvPropertyChangedCodes(CrInt32u num, CrInt32u* codes) override;
    virtual void OnCompleteDownload(CrChar* filename, CrInt32u type) override;
    virtual void OnNotifyContentsTransfer(CrInt32u notify, CrContentHandle handle, CrChar* filename) override;
    virtual void OnWarning(CrInt32u warning) override;
    virtual void OnError(CrInt32u error) override;
    
private:
    std::function<void()> mConnectCallback;
    std::function<void(CrInt32u)> mDisconnectCallback;
    std::function<void()> mPropertyChangeCallback;
    std::function<void(CrInt32u)> mErrorCallback;
};