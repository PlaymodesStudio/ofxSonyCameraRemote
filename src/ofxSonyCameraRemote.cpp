#include "ofxSonyCameraRemote.h"
#include <dlfcn.h>
#include <iomanip>

// Sony's vendor ID
const uint16_t SONY_VENDOR_ID = 0x054C;

// Some known Sony camera product IDs (partial list)
const std::vector<uint16_t> SONY_PRODUCT_IDS = {
    0x0994, // Sony Alpha series
    0x0C28, // Alpha 7 series
    0x0C29, // Other Alpha models
    // Add more as identified
};

ofxSonyCameraRemote::ofxSonyCameraRemote()
    : mEnumCameraObjInfo(nullptr)
    , mDeviceHandle(0)
    , mConnected(false)
    , mUsbContext(nullptr)
    , mLibUsbHandle(nullptr)
    , fn_libusb_init(nullptr)
    , fn_libusb_exit(nullptr)
    , fn_libusb_get_device_list(nullptr)
    , fn_libusb_free_device_list(nullptr)
    , fn_libusb_get_device_descriptor(nullptr)
    , fn_libusb_get_bus_number(nullptr)
    , fn_libusb_get_device_address(nullptr)
    , fn_libusb_open(nullptr)
    , fn_libusb_close(nullptr)
    , fn_libusb_get_string_descriptor_ascii(nullptr)
    , fn_libusb_claim_interface(nullptr)
    , fn_libusb_release_interface(nullptr)
    , fn_libusb_error_name(nullptr) {
}

ofxSonyCameraRemote::~ofxSonyCameraRemote() {
    exit();
}

bool ofxSonyCameraRemote::setup() {
    // Debug library loading
    ofLogNotice("ofxSonyCameraRemote") << "Testing library loading...";
    
    // Test loading core libraries
    void* lib1 = dlopen("libCr_Core.dylib", RTLD_LAZY);
    if (!lib1) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libCr_Core.dylib: " << dlerror();
        lib1 = dlopen("../../../../../libs/CRSDK/lib/libCr_Core.dylib", RTLD_LAZY);
        if (!lib1) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/libCr_Core.dylib";
            dlclose(lib1);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libCr_Core.dylib";
        dlclose(lib1);
    }
    
    // Check if adapter libraries can be loaded
    void* lib2 = dlopen("libCr_PTP_IP.dylib", RTLD_LAZY);
    if (!lib2) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libCr_PTP_IP.dylib: " << dlerror();
        lib2 = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libCr_PTP_IP.dylib", RTLD_LAZY);
        if (!lib2) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/CrAdapter/libCr_PTP_IP.dylib";
            dlclose(lib2);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libCr_PTP_IP.dylib";
        dlclose(lib2);
    }
    
    // Check if USB adapter library can be loaded
    void* lib3 = dlopen("libCr_PTP_USB.dylib", RTLD_LAZY);
    if (!lib3) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libCr_PTP_USB.dylib: " << dlerror();
        lib3 = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libCr_PTP_USB.dylib", RTLD_LAZY);
        if (!lib3) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/CrAdapter/libCr_PTP_USB.dylib";
            dlclose(lib3);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libCr_PTP_USB.dylib";
        dlclose(lib3);
    }
    
    // Check if libusb can be loaded
    void* lib4 = dlopen("libusb-1.0.dylib", RTLD_LAZY);
    if (!lib4) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libusb-1.0.dylib: " << dlerror();
        lib4 = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.dylib", RTLD_LAZY);
        if (!lib4) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.dylib";
            dlclose(lib4);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libusb-1.0.dylib";
        dlclose(lib4);
    }
    
    // Check for monitor protocol libraries
    void* lib5 = dlopen("libmonitor_protocol.dylib", RTLD_LAZY);
    if (!lib5) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libmonitor_protocol.dylib: " << dlerror();
        lib5 = dlopen("../../../../../libs/CRSDK/lib/libmonitor_protocol.dylib", RTLD_LAZY);
        if (!lib5) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/libmonitor_protocol.dylib";
            dlclose(lib5);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libmonitor_protocol.dylib";
        dlclose(lib5);
    }
    
    // Check monitor protocol platform-specific library
    void* lib6 = dlopen("libmonitor_protocol_pf.dylib", RTLD_LAZY);
    if (!lib6) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libmonitor_protocol_pf.dylib: " << dlerror();
        lib6 = dlopen("../../../../../libs/CRSDK/lib/libmonitor_protocol_pf.dylib", RTLD_LAZY);
        if (!lib6) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/libmonitor_protocol_pf.dylib";
            dlclose(lib6);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libmonitor_protocol_pf.dylib";
        dlclose(lib6);
    }
    
    // Check SSH library (used for network connections)
    void* lib7 = dlopen("libssh2.dylib", RTLD_LAZY);
    if (!lib7) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libssh2.dylib: " << dlerror();
        lib7 = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libssh2.dylib", RTLD_LAZY);
        if (!lib7) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/CrAdapter/libssh2.dylib";
            dlclose(lib7);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libssh2.dylib";
        dlclose(lib7);
    }
    
    // Check alternate libusb filename pattern
    void* lib8 = dlopen("libusb-1.0.0.dylib", RTLD_LAZY);
    if (!lib8) {
        ofLogError("ofxSonyCameraRemote") << "Failed to load libusb-1.0.0.dylib: " << dlerror();
        lib8 = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.0.dylib", RTLD_LAZY);
        if (!lib8) {
            ofLogError("ofxSonyCameraRemote") << "Failed with relative path too: " << dlerror();
        } else {
            ofLogNotice("ofxSonyCameraRemote") << "Loaded with relative path ../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.0.dylib";
            dlclose(lib8);
        }
    } else {
        ofLogNotice("ofxSonyCameraRemote") << "Successfully loaded libusb-1.0.0.dylib";
        dlclose(lib8);
    }
    
    // Initialize the Sony SDK with error diagnostic
    ofLogNotice("ofxSonyCameraRemote") << "Initializing Sony Camera Remote SDK...";
    bool result = SCRSDK::Init();
    if (!result) {
        ofLogError("ofxSonyCameraRemote") << "Failed to initialize Sony Camera Remote SDK";
        return false;
    }
    
	
    
	ofLogNotice("ofxSonyCameraRemote") << "SDK initialized successfully";
    
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
    
    // Clean up USB resources
    if (mUsbContext && fn_libusb_exit) {
        fn_libusb_exit(mUsbContext);
        mUsbContext = nullptr;
    }
    
    // Close library handle
    if (mLibUsbHandle) {
        dlclose(mLibUsbHandle);
        mLibUsbHandle = nullptr;
    }
    
    // Clear function pointers
    fn_libusb_init = nullptr;
    fn_libusb_exit = nullptr;
    fn_libusb_get_device_list = nullptr;
    fn_libusb_free_device_list = nullptr;
    fn_libusb_get_device_descriptor = nullptr;
    fn_libusb_get_bus_number = nullptr;
    fn_libusb_get_device_address = nullptr;
    fn_libusb_open = nullptr;
    fn_libusb_close = nullptr;
    fn_libusb_get_string_descriptor_ascii = nullptr;
    fn_libusb_claim_interface = nullptr;
    fn_libusb_release_interface = nullptr;
    fn_libusb_error_name = nullptr;
    
    // Clear data
    mUsbDeviceInfoList.clear();
}

bool ofxSonyCameraRemote::enumerateDevices() {
    // Clear previous device list
    mDeviceInfoList.clear();
    
    // Release previous enumeration
    if (mEnumCameraObjInfo) {
        mEnumCameraObjInfo->Release();
        mEnumCameraObjInfo = nullptr;
    }
    
    // Detailed error logging
    ofLogNotice("ofxSonyCameraRemote") << "Enumerating camera devices...";
    ofLogNotice("ofxSonyCameraRemote") << "SDK Version: " << SCRSDK::GetSDKVersion();
    
    // Enumerate connected cameras
    CrError err = SCRSDK::EnumCameraObjects(&mEnumCameraObjInfo);
    
    // Enhanced error diagnostic based on error code
    if (err != CrError_None) {
        ofLogError("ofxSonyCameraRemote") << "Failed to enumerate camera devices: " << err;
        
        // Convert to hex for easier lookup in header file
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << err;
        ofLogError("ofxSonyCameraRemote") << "Error code in hex: 0x" << ss.str();
        
        // More detailed error analysis
        if (err >= SCRSDK::CrError_Connect && err < SCRSDK::CrError_Memory) {
            ofLogError("ofxSonyCameraRemote") << "This is a connection-related error. Check camera connectivity.";
        } else if (err >= SCRSDK::CrError_Adaptor && err < SCRSDK::CrError_Device) {
            ofLogError("ofxSonyCameraRemote") << "This is an adapter-related error. Check if the camera is in the correct mode (PC Remote or similar).";
            
            // More specific diagnostics
            if (err == SCRSDK::CrError_Adaptor_EnumDevice) {
                ofLogError("ofxSonyCameraRemote") << "Specifically an enumeration error. Make sure camera is connected and in Remote control mode.";
            }
            
            // Specific handling for error 0x8703 (34563)
            if (err == 34563) { // 0x8703
                ofLogError("ofxSonyCameraRemote") << "Error 0x8703: USB device enumeration failure detected.";
                ofLogError("ofxSonyCameraRemote") << "This usually indicates one of the following:";
                ofLogError("ofxSonyCameraRemote") << "1. The camera is not in the proper connection mode (needs to be in PC Remote or PC Connection mode)";
                ofLogError("ofxSonyCameraRemote") << "2. The camera's USB connection is not functioning correctly";
                ofLogError("ofxSonyCameraRemote") << "3. macOS permissions issues with USB device access";
                ofLogError("ofxSonyCameraRemote") << "4. Another application has exclusive access to the camera";
                
                // Additional diagnostic information specific to macOS
                ofLogError("ofxSonyCameraRemote") << "Additional troubleshooting steps:";
                ofLogError("ofxSonyCameraRemote") << "- Check System Information > USB for camera detection";
                ofLogError("ofxSonyCameraRemote") << "- Try connecting camera to a different USB port";
                ofLogError("ofxSonyCameraRemote") << "- Restart the camera and ensure it's set to PC Remote mode";
                ofLogError("ofxSonyCameraRemote") << "- Try using Image Capture app to see if macOS can detect the camera";
            }
        }
        
        ofLogNotice("ofxSonyCameraRemote") << "DEBUG TIPS:";
        ofLogNotice("ofxSonyCameraRemote") << "1. Make sure camera is connected via USB or Wi-Fi";
        ofLogNotice("ofxSonyCameraRemote") << "2. Camera should be in 'PC Remote' or 'Remote Shooting' mode in the camera menu";
        ofLogNotice("ofxSonyCameraRemote") << "3. Check if libraries are correctly loaded (see above messages)";
        ofLogNotice("ofxSonyCameraRemote") << "4. Make sure camera isn't already connected to another app";
        ofLogNotice("ofxSonyCameraRemote") << "5. Try restarting the camera";
        
        return false;
    }
    
    // Get count of connected cameras
    int count = mEnumCameraObjInfo->GetCount();
    if (count == 0) {
        ofLogNotice("ofxSonyCameraRemote") << "No cameras found. Make sure your camera is:";
        ofLogNotice("ofxSonyCameraRemote") << "1. Connected via USB or Wi-Fi";
        ofLogNotice("ofxSonyCameraRemote") << "2. In 'PC Remote' or similar transfer mode";
        ofLogNotice("ofxSonyCameraRemote") << "3. Not currently being used by another application";
        return false;
    }
    
    // Store camera info objects
    for (int i = 0; i < count; i++) {
        const ICrCameraObjectInfo* camera = mEnumCameraObjInfo->GetCameraObjectInfo(i);
        mDeviceInfoList.push_back(camera);
        
        // Enhanced logging with device details
        ofLogNotice("ofxSonyCameraRemote") << "Camera " << i << ": Model=" << camera->GetModel()
                                         << ", Serial=" << camera->GetId();
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
        ofLogNotice("ofxSonyCameraRemote") << "No cameras in device list. Attempting to enumerate...";
        if (!enumerateDevices()) {
            return false;
        }
    }
    
    // Check device index
    if (deviceIndex < 0 || deviceIndex >= mDeviceInfoList.size()) {
        ofLogError("ofxSonyCameraRemote") << "Invalid device index: " << deviceIndex;
        return false;
    }
    
    // Connect to the camera with enhanced logging
    const ICrCameraObjectInfo* camera = mDeviceInfoList[deviceIndex];
    ofLogNotice("ofxSonyCameraRemote") << "Connecting to camera: " << camera->GetModel();
    
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
        
        // Convert to hex for easier lookup in header file
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << err;
        ofLogError("ofxSonyCameraRemote") << "Error code in hex: 0x" << ss.str();
        
        // More detailed diagnostic based on error code
        if (err == SCRSDK::CrError_Connect_FailRejected) {
            ofLogError("ofxSonyCameraRemote") << "Connection rejected by camera. Check if it's in the correct mode.";
        } else if (err == SCRSDK::CrError_Connect_FailBusy) {
            ofLogError("ofxSonyCameraRemote") << "Camera is busy. It might be connected to another application.";
        } else if (err == SCRSDK::CrError_Generic_InvalidParameter) {
            ofLogError("ofxSonyCameraRemote") << "Invalid parameter. This could be an SDK compatibility issue.";
        }
        
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
    SCRSDK::ReleaseDeviceProperties(mDeviceHandle, properties);
    
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

// Function to load libusb functions
bool ofxSonyCameraRemote::loadLibUsbFunctions() {
    mUsbErrorMessages.clear();
    
    // If we already have loaded the functions, just return success
    if (fn_libusb_init != nullptr) {
        return true;
    }
    
    // Try to load the library
    mLibUsbHandle = dlopen("libusb-1.0.dylib", RTLD_LAZY);
    if (!mLibUsbHandle) {
        // Try with path
        mLibUsbHandle = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.dylib", RTLD_LAZY);
        if (!mLibUsbHandle) {
            // Try alternate filename
            mLibUsbHandle = dlopen("libusb-1.0.0.dylib", RTLD_LAZY);
            if (!mLibUsbHandle) {
                mLibUsbHandle = dlopen("../../../../../libs/CRSDK/lib/CrAdapter/libusb-1.0.0.dylib", RTLD_LAZY);
                if (!mLibUsbHandle) {
                    addUsbError("Failed to load libusb: " + std::string(dlerror()));
                    return false;
                }
            }
        }
    }
    
    // Load function pointers
    fn_libusb_init = (libusb_init_fn)dlsym(mLibUsbHandle, "libusb_init");
    fn_libusb_exit = (libusb_exit_fn)dlsym(mLibUsbHandle, "libusb_exit");
    fn_libusb_get_device_list = (libusb_get_device_list_fn)dlsym(mLibUsbHandle, "libusb_get_device_list");
    fn_libusb_free_device_list = (libusb_free_device_list_fn)dlsym(mLibUsbHandle, "libusb_free_device_list");
    fn_libusb_get_device_descriptor = (libusb_get_device_descriptor_fn)dlsym(mLibUsbHandle, "libusb_get_device_descriptor");
    fn_libusb_get_bus_number = (libusb_get_bus_number_fn)dlsym(mLibUsbHandle, "libusb_get_bus_number");
    fn_libusb_get_device_address = (libusb_get_device_address_fn)dlsym(mLibUsbHandle, "libusb_get_device_address");
    fn_libusb_open = (libusb_open_fn)dlsym(mLibUsbHandle, "libusb_open");
    fn_libusb_close = (libusb_close_fn)dlsym(mLibUsbHandle, "libusb_close");
    fn_libusb_get_string_descriptor_ascii = (libusb_get_string_descriptor_ascii_fn)dlsym(mLibUsbHandle, "libusb_get_string_descriptor_ascii");
    fn_libusb_claim_interface = (libusb_claim_interface_fn)dlsym(mLibUsbHandle, "libusb_claim_interface");
    fn_libusb_release_interface = (libusb_release_interface_fn)dlsym(mLibUsbHandle, "libusb_release_interface");
    fn_libusb_error_name = (libusb_error_name_fn)dlsym(mLibUsbHandle, "libusb_error_name");
    
    // Check if all functions were loaded
    if (!fn_libusb_init || !fn_libusb_exit || !fn_libusb_get_device_list || !fn_libusb_free_device_list ||
        !fn_libusb_get_device_descriptor || !fn_libusb_get_bus_number || !fn_libusb_get_device_address ||
        !fn_libusb_open || !fn_libusb_close || !fn_libusb_get_string_descriptor_ascii ||
        !fn_libusb_claim_interface || !fn_libusb_release_interface) {
        
        addUsbError("Failed to load one or more libusb functions: " + std::string(dlerror()));
        return false;
    }
    
    // Initialize libusb context
    int result = fn_libusb_init(&mUsbContext);
    if (result != LIBUSB_SUCCESS) {
        addUsbError("Failed to initialize libusb: " + std::string(getLibUsbErrorName(result)));
        return false;
    }
    
    return true;
}

// Implement USB debugging methods
bool ofxSonyCameraRemote::debugUsbDevices() {
    // Load libusb functions if not already loaded
    if (!loadLibUsbFunctions()) {
        return false;
    }
    
    // Enumerate all devices
    int count = enumerateAllUsbDevices();
    if (count <= 0) {
        ofLogError("ofxSonyCameraRemote") << "No USB devices found or error occurred";
        return false;
    }
    
    // Count Sony devices
    int sonyCount = countSonyDevices();
    ofLogNotice("ofxSonyCameraRemote") << "Found " << sonyCount << " Sony devices out of " << count << " total USB devices";
    
    // Print detailed info
    ofLogNotice("ofxSonyCameraRemote") << getUsbDevicesInfo();
    
    // Return if any Sony devices were found
    return (sonyCount > 0);
}

std::string ofxSonyCameraRemote::getUsbDevicesInfo() const {
    std::stringstream ss;
    
    ss << "USB Device List: " << mUsbDeviceInfoList.size() << " devices found\n";
    ss << "-----------------------------------------------------\n";
    
    bool foundAlpha7sIII = false;
    
    for (size_t i = 0; i < mUsbDeviceInfoList.size(); i++) {
        const auto& info = mUsbDeviceInfoList[i];
        
        ss << "Device " << i << ":\n";
        ss << "  Vendor ID: 0x" << std::hex << info.vendorId << std::dec << "\n";
        ss << "  Product ID: 0x" << std::hex << info.productId << std::dec << "\n";
        ss << "  Bus/Address: " << (int)info.busNumber << "/" << (int)info.deviceAddress << "\n";
        
        if (!info.manufacturer.empty()) {
            ss << "  Manufacturer: " << info.manufacturer << "\n";
        }
        
        if (!info.product.empty()) {
            ss << "  Product: " << info.product << "\n";
        }
        
        if (!info.serialNumber.empty()) {
            ss << "  Serial: " << info.serialNumber << "\n";
        }
        
        ss << "  Sony Camera: " << (info.isSonyCamera ? "YES" : "NO") << "\n";
        
        bool isA7sIII = isAlpha7sIII(info.vendorId, info.productId, info.product);
        if (isA7sIII) {
            foundAlpha7sIII = true;
        }
        
        ss << "  Alpha 7S III Match: " << (isA7sIII ? "LIKELY" : "NO") << "\n";
        ss << "  Accessible: " << (info.isAccessible ? "YES" : "NO") << "\n";
        ss << "\n";
    }
    
    // Add summary info
    if (foundAlpha7sIII) {
        ss << "FOUND POTENTIAL SONY ALPHA 7S III DEVICE(S)\n";
    } else {
        ss << "WARNING: NO SONY ALPHA 7S III DEVICES DETECTED\n";
    }
    
    return ss.str();
}

std::vector<std::string> ofxSonyCameraRemote::getUsbErrors() const {
    return mUsbErrorMessages;
}

int ofxSonyCameraRemote::enumerateAllUsbDevices() {
    if (!mUsbContext || !fn_libusb_get_device_list) {
        addUsbError("USB context not initialized or functions not loaded");
        return -1;
    }
    
    mUsbDeviceInfoList.clear();
    
    // Get device list
    libusb_device **deviceList;
    ssize_t count = fn_libusb_get_device_list(mUsbContext, &deviceList);
    
    if (count < 0) {
        addUsbError("Failed to get USB device list: " + std::string(getLibUsbErrorName(count)));
        return -1;
    }
    
    ofLogNotice("ofxSonyCameraRemote") << "Found " << count << " USB devices";
    
    // Process each device
    for (ssize_t i = 0; i < count; i++) {
        libusb_device *device = deviceList[i];
        
        // Get device descriptor
        struct libusb_device_descriptor desc;
        int result = fn_libusb_get_device_descriptor(device, &desc);
        
        if (result != LIBUSB_SUCCESS) {
            addUsbError("Failed to get device descriptor: " + std::string(getLibUsbErrorName(result)));
            continue;
        }
        
        // Create device info
        UsbDeviceInfo info;
        info.vendorId = desc.idVendor;
        info.productId = desc.idProduct;
        info.busNumber = fn_libusb_get_bus_number(device);
        info.deviceAddress = fn_libusb_get_device_address(device);
        info.isSonyCamera = isSonyCamera(desc.idVendor, desc.idProduct);
        info.isAccessible = false;
        
        // Try to open the device to get string descriptors
        libusb_device_handle *handle;
        result = fn_libusb_open(device, &handle);
        
        if (result == LIBUSB_SUCCESS) {
            info.isAccessible = true;
            
            // Get string descriptors if available
            if (desc.iManufacturer > 0) {
                info.manufacturer = getUsbDeviceString(handle, desc.iManufacturer);
            }
            
            if (desc.iProduct > 0) {
                info.product = getUsbDeviceString(handle, desc.iProduct);
            }
            
            if (desc.iSerialNumber > 0) {
                info.serialNumber = getUsbDeviceString(handle, desc.iSerialNumber);
            }
            
            // Close the device
            fn_libusb_close(handle);
        } else {
            // Device cannot be opened
            info.isAccessible = false;
            
            if (info.isSonyCamera) {
                addUsbError("Sony camera found but cannot be opened: " +
                         std::string(getLibUsbErrorName(result)));
            }
        }
        
        // Add to our list
        mUsbDeviceInfoList.push_back(info);
    }
    
    // Free the list
    fn_libusb_free_device_list(deviceList, 1);
    
    return mUsbDeviceInfoList.size();
}

int ofxSonyCameraRemote::countSonyDevices() const {
    int sonyCount = 0;
    for (const auto& info : mUsbDeviceInfoList) {
        if (info.isSonyCamera) {
            sonyCount++;
        }
    }
    return sonyCount;
}

std::string ofxSonyCameraRemote::getUsbDeviceString(libusb_device_handle* handle, uint8_t descIndex) {
    if (!handle || !fn_libusb_get_string_descriptor_ascii) return "";
    
    // Buffer for string descriptor
    unsigned char buffer[256];
    
    // Get string descriptor
    int result = fn_libusb_get_string_descriptor_ascii(
        handle, descIndex, buffer, sizeof(buffer));
    
    if (result < 0) {
        addUsbError("Failed to get string descriptor: " + std::string(getLibUsbErrorName(result)));
        return "";
    }
    
    return std::string(reinterpret_cast<char*>(buffer), result);
}

bool ofxSonyCameraRemote::isSonyCamera(uint16_t vendorId, uint16_t productId) const {
    // Check vendor ID first
    if (vendorId != SONY_VENDOR_ID) {
        return false;
    }
    
    // Check if product ID is in our list of known Sony camera IDs
    for (auto knownId : SONY_PRODUCT_IDS) {
        if (productId == knownId) {
            return true;
        }
    }
    
    // For unknown Sony devices, look for camera-related strings in the product name
    // which is already handled by checking for Sony's vendor ID
    return true; // Any Sony device could potentially be a camera
}

bool ofxSonyCameraRemote::isAlpha7sIII(uint16_t vendorId, uint16_t productId, const std::string& productString) const {
    // Sony Alpha 7S III has vendor ID 0x054C (Sony)
    // The exact product ID depends on the firmware version, but typical values include:
    // 0x0C29, 0x0C28, etc.
    
    if (vendorId != SONY_VENDOR_ID) {
        return false;
    }
    
    // Check product string first if available
    if (!productString.empty()) {
        if (productString.find("Alpha 7S III") != std::string::npos ||
            productString.find("A7S III") != std::string::npos ||
            productString.find("ILCE-7SM3") != std::string::npos) {
            return true;
        }
    }
    
    // Otherwise, check common product IDs
    // Note: This is imprecise and may need adjustment
    if (productId == 0x0C29 || productId == 0x0C28) {
        return true;
    }
    
    return false;
}

void ofxSonyCameraRemote::addUsbError(const std::string& message) const {
    ofLogError("ofxSonyCameraRemote") << message;
    // Note: We need to cast away const here because we're modifying member data
    // from a const method. This is generally not ideal, but works for error collection.
    const_cast<ofxSonyCameraRemote*>(this)->mUsbErrorMessages.push_back(message);
}

const char* ofxSonyCameraRemote::getLibUsbErrorName(int code) const {
    if (fn_libusb_error_name) {
        return fn_libusb_error_name(code);
    }
    
    // Fallback if function not available
    switch (code) {
        case LIBUSB_SUCCESS: return "LIBUSB_SUCCESS";
        case LIBUSB_ERROR_IO: return "LIBUSB_ERROR_IO";
        case LIBUSB_ERROR_INVALID_PARAM: return "LIBUSB_ERROR_INVALID_PARAM";
        case LIBUSB_ERROR_ACCESS: return "LIBUSB_ERROR_ACCESS";
        case LIBUSB_ERROR_NO_DEVICE: return "LIBUSB_ERROR_NO_DEVICE";
        case LIBUSB_ERROR_NOT_FOUND: return "LIBUSB_ERROR_NOT_FOUND";
        case LIBUSB_ERROR_BUSY: return "LIBUSB_ERROR_BUSY";
        case LIBUSB_ERROR_TIMEOUT: return "LIBUSB_ERROR_TIMEOUT";
        case LIBUSB_ERROR_OVERFLOW: return "LIBUSB_ERROR_OVERFLOW";
        case LIBUSB_ERROR_PIPE: return "LIBUSB_ERROR_PIPE";
        case LIBUSB_ERROR_INTERRUPTED: return "LIBUSB_ERROR_INTERRUPTED";
        case LIBUSB_ERROR_NO_MEM: return "LIBUSB_ERROR_NO_MEM";
        case LIBUSB_ERROR_NOT_SUPPORTED: return "LIBUSB_ERROR_NOT_SUPPORTED";
        case LIBUSB_ERROR_OTHER: return "LIBUSB_ERROR_OTHER";
        default: return "Unknown error";
    }
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
