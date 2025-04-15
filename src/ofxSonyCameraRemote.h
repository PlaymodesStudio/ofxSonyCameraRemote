#pragma once

#include "ofMain.h"
#include "../libs/CRSDK/include/CameraRemote_SDK.h"
#include "ofxSonyCameraCallback.h"

// Note: CrInt32u, CrInt64u types are defined in the global namespace in CrTypes.h
// Only types specifically defined in the SCRSDK namespace need to be qualified
using SCRSDK::CrDeviceHandle;
using SCRSDK::ICrEnumCameraObjectInfo;
using SCRSDK::ICrCameraObjectInfo;
using SCRSDK::CrDeviceProperty;
// Note: SDK uses different enum names than in our implementation
using namespace SCRSDK; // Import all enum values from SCRSDK namespace
using SCRSDK::CrDataType_UInt64;
using SCRSDK::CrError_None;
using SCRSDK::CrCommandId_Release;
using SCRSDK::CrCommandParam_Down;
#include <vector>
#include <memory>

// Only include typedefs and constants we need from libusb
// These match the libusb-1.0 API but don't require the header
typedef struct libusb_context libusb_context;
typedef struct libusb_device libusb_device;
typedef struct libusb_device_handle libusb_device_handle;
typedef struct libusb_device_descriptor {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} libusb_device_descriptor;

// Define libusb error codes
#define LIBUSB_SUCCESS 0
#define LIBUSB_ERROR_IO -1
#define LIBUSB_ERROR_INVALID_PARAM -2
#define LIBUSB_ERROR_ACCESS -3
#define LIBUSB_ERROR_NO_DEVICE -4
#define LIBUSB_ERROR_NOT_FOUND -5
#define LIBUSB_ERROR_BUSY -6
#define LIBUSB_ERROR_TIMEOUT -7
#define LIBUSB_ERROR_OVERFLOW -8
#define LIBUSB_ERROR_PIPE -9
#define LIBUSB_ERROR_INTERRUPTED -10
#define LIBUSB_ERROR_NO_MEM -11
#define LIBUSB_ERROR_NOT_SUPPORTED -12
#define LIBUSB_ERROR_OTHER -99

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
    bool getProperty(CrInt32u code, CrInt64u& value);
    
    /**
     * @brief Set a camera property value
     * 
     * @param code The property code (from SCRSDK::CrDeviceProperty enum)
     * @param value The value to set
     * @return true if the property was set successfully, false otherwise
     */
    bool setProperty(CrInt32u code, CrInt64u value);
    
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
    void registerDisconnectCallback(std::function<void(CrInt32u)> callback);
    
    /**
     * @brief Register a callback for camera error events
     * 
     * @param callback The function to call when a camera error occurs
     */
    void registerErrorCallback(std::function<void(CrInt32u)> callback);
    
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
    
    /**
     * @brief Get the Camera Remote SDK version
     *
     * @return The SDK version as an unsigned 32-bit integer
     */
    CrInt32u getSDKVersion() const;
    
    /**
     * @brief Debug USB devices by directly using libusb
     *
     * This method bypasses the Sony SDK to directly enumerate USB devices
     * and identify any Sony cameras connected to the system.
     *
     * @return true if any Sony devices were found, false otherwise
     */
    bool debugUsbDevices();
    
    /**
     * @brief Get detailed information about USB devices
     *
     * @return A string containing detailed USB device information
     */
    std::string getUsbDevicesInfo() const;
    
    /**
     * @brief Get any USB debug error messages
     *
     * @return A vector of error message strings
     */
    std::vector<std::string> getUsbErrors() const;
    
private:
    // SDK handles
    ICrEnumCameraObjectInfo* mEnumCameraObjInfo;
    std::vector<const ICrCameraObjectInfo*> mDeviceInfoList;
    CrDeviceHandle mDeviceHandle;
    
    // Callback handler
    std::unique_ptr<ofxSonyCameraCallback> mCallback;
    
    // Connection status
    bool mConnected;
    
    // Helper methods for SDK interaction
    void loadProperties();
    
    // USB debugging data structures
    struct UsbDeviceInfo {
        uint16_t vendorId;
        uint16_t productId;
        uint8_t busNumber;
        uint8_t deviceAddress;
        std::string manufacturer;
        std::string product;
        std::string serialNumber;
        bool isSonyCamera;
        bool isAccessible;
    };
    
    // USB device list and error messages
    std::vector<UsbDeviceInfo> mUsbDeviceInfoList;
    std::vector<std::string> mUsbErrorMessages;
    
    // libusb function pointer types
    typedef int (*libusb_init_fn)(libusb_context**);
    typedef void (*libusb_exit_fn)(libusb_context*);
    typedef ssize_t (*libusb_get_device_list_fn)(libusb_context*, libusb_device***);
    typedef void (*libusb_free_device_list_fn)(libusb_device**, int);
    typedef int (*libusb_get_device_descriptor_fn)(libusb_device*, libusb_device_descriptor*);
    typedef uint8_t (*libusb_get_bus_number_fn)(libusb_device*);
    typedef uint8_t (*libusb_get_device_address_fn)(libusb_device*);
    typedef int (*libusb_open_fn)(libusb_device*, libusb_device_handle**);
    typedef void (*libusb_close_fn)(libusb_device_handle*);
    typedef int (*libusb_get_string_descriptor_ascii_fn)(libusb_device_handle*, uint8_t, unsigned char*, int);
    typedef int (*libusb_claim_interface_fn)(libusb_device_handle*, int);
    typedef int (*libusb_release_interface_fn)(libusb_device_handle*, int);
    typedef const char* (*libusb_error_name_fn)(int);
    
    // libusb function pointers
    libusb_init_fn fn_libusb_init;
    libusb_exit_fn fn_libusb_exit;
    libusb_get_device_list_fn fn_libusb_get_device_list;
    libusb_free_device_list_fn fn_libusb_free_device_list;
    libusb_get_device_descriptor_fn fn_libusb_get_device_descriptor;
    libusb_get_bus_number_fn fn_libusb_get_bus_number;
    libusb_get_device_address_fn fn_libusb_get_device_address;
    libusb_open_fn fn_libusb_open;
    libusb_close_fn fn_libusb_close;
    libusb_get_string_descriptor_ascii_fn fn_libusb_get_string_descriptor_ascii;
    libusb_claim_interface_fn fn_libusb_claim_interface;
    libusb_release_interface_fn fn_libusb_release_interface;
    libusb_error_name_fn fn_libusb_error_name;
    
    // libusb context
    libusb_context* mUsbContext;
    
    // Library handle
    void* mLibUsbHandle;
    
    // USB helper methods
    bool loadLibUsbFunctions();
    int enumerateAllUsbDevices();
    int countSonyDevices() const;
    std::string getUsbDeviceString(libusb_device_handle* handle, uint8_t descIndex);
    bool isSonyCamera(uint16_t vendorId, uint16_t productId) const;
    void addUsbError(const std::string& message) const;
    bool isAlpha7sIII(uint16_t vendorId, uint16_t productId, const std::string& productString) const;
    const char* getLibUsbErrorName(int code) const;
};
