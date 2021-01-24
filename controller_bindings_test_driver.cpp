///////////////////////////////////////////////////////////////////
// controller_bindings_test_driver.cpp
//
///////////////////////////////////////////////////////////////////
#include "dprintf.h"
#include <openvr_driver.h>
#include <string>
#include "skeleton_constants.h"

using namespace std;
using namespace vr;

class SingleTrackedDevice : public vr::ITrackedDeviceServerDriver
{
public:
	SingleTrackedDevice(const string &device_class_name, const std::string &device_instance_id)
		: m_device_class_name(device_class_name),m_device_instance_id(device_instance_id)
	{}

	// 
	// The following six methods override vr::ITrackedDeviceServerDriver pure virtuals
	//
	EVRInitError Activate(uint32_t unObjectId) override
	{
		m_property_container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
		DoSetDeviceProperties();
		DoConstructDeviceComponents();
	}
	void Deactivate() override {}
	void EnterStandby() override {}
	void *GetComponent(const char *pchComponentNameAndVersion) override
	{
		// e.g. 12/27/2018 the vrserver was asking for:
		//IVRDisplayComponent_002
		//IVRDriverDirectModeComponent_005
		//IVRCameraComponent_003
		//IVRVirtualDisplay_002
		//IVRControllerComponent_001 <-- this the legacy one that was removed from openvr_driver.h
		return NULL;
	}
	void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override {}

	DriverPose_t GetPose() override
	{
		return DoGetPose();
	}

public:
	// not using a per device thread since, for testing purposes, we want to be able to control the order in which 
	// devicecomponents are updated 
	void UpdateNow() { DoUpdateDeviceComponentStates(); }

private:
	void DoSetDeviceProperties()
	{
		vector<PropertyWrite_t> properties = {
			{ Prop_ModelNumber_String,PropertyWrite_Set,TrackedProp_Success, (void *)m_device_class_name.c_str(), (uint32_t)m_device_class_name.size() + 1 , k_unStringPropertyTag },
			{ Prop_SerialNumber_String,PropertyWrite_Set,TrackedProp_Success, (void *)m_device_instance_id.c_str(), (uint32_t)m_device_instance_id.size() + 1 , k_unStringPropertyTag }
		};

		ETrackedPropertyError result = VRPropertiesRaw()->WritePropertyBatch(m_property_container, properties.data(), (uint32_t)properties.size());
		if (result != TrackedProp_Success)
		{
			dprintf("cbt failed to write property batch %d\n", result);
		}
	}

	VRInputComponentHandle_t DoConstructDeviceComponents()
	{
		{
			string component_name = string("/input/") + m_device_instance_id + "bool";
			EVRInputError result = VRDriverInput()->CreateBooleanComponent(
				m_property_container,
				component_name.c_str(),
				&m_bool);
			if (result != VRInputError_None)
			{
				dprintf("cbt failed to create boolean component. %d\n", result);
			}
		}
		
		{
			string component_name = string("/input/") + m_device_instance_id + "float";
			EVRInputError result = VRDriverInput()->CreateScalarComponent(
				m_property_container,
				component_name.c_str(),
				&m_float,
				vr::VRScalarType_Absolute,
				vr::VRScalarUnits_NormalizedOneSided);
			if (result != VRInputError_None)
			{
				dprintf("cbt failed to create scalar component. %d\n", result);
			}
		}

		{
			string component_name = string("/input/") + m_device_instance_id + "float";
			EVRInputError result = VRDriverInput()->CreateScalarComponent(
				m_property_container,
				component_name.c_str(),
				&m_float,
				vr::VRScalarType_Absolute,
				vr::VRScalarUnits_NormalizedOneSided);
			if (result != VRInputError_None)
			{
				dprintf("cbt failed to create scalar component. %d\n", result);
			}
		}

		{
			string component_name = string("/input/") + m_device_instance_id + "skeleton";	// try choosing an arbitrary name. despite
																							// https://github.com/ValveSoftware/openvr/wiki/Creating-a-Skeletal-Input-Driver
																							// says this must be /input/skeleton/right or /input/skeleton/left
			EVRInputError result = VRDriverInput()->CreateSkeletonComponent(
				m_property_container,
				component_name.c_str(),
				"/skeleton/hand/right",
				"/pose_raw",
				VRSkeletalTracking_Partial,
				k_fist_skeleton.data(),
				(uint32_t)k_fist_skeleton.size(),
				&m_skeleton);
			if (result != VRInputError_None)
			{
				dprintf("cbt failed to create scalar component. %d\n", result);
			}
		}
	}

	void DoUpdateDeviceComponentStates();
	DriverPose_t DoGetPose();

private:
	string m_device_class_name;
	string m_device_instance_id;
	PropertyContainerHandle_t m_property_container;
	VRInputComponentHandle_t m_bool;
	VRInputComponentHandle_t m_float;
	VRInputComponentHandle_t m_skeleton;

};


class DeviceUpdater
{

};

class DeviceProvider : public vr::IServerTrackedDeviceProvider
{
	// override IServerTrackedDeviceProvider pure virtual methods
	EVRInitError Init(IVRDriverContext *pDriverContext) override
	{
		// read settings
	}
	void Cleanup() override;
	const char * const *GetInterfaceVersions() override; 
	void RunFrame() override;
	bool ShouldBlockStandbyMode() override;
	void EnterStandby() override;
	void LeaveStandby() override;

};

class WatchdogProvider : IVRWatchdogProvider
{
	// override IVRWatchdogProvider pure virtual methods
	EVRInitError Init(IVRDriverContext *pDriverContext) override;
	void Cleanup() override;
};

