///////////////////////////////////////////////////////////////////
// controller_bindings_test_app.cpp
///////////////////////////////////////////////////////////////////
#include <openvr.h>
#include <assert.h>
#include <iostream>
#include <chrono>
#include <thread>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace vr;
using namespace std::literals::chrono_literals;

#define CHECK_RESULT(x) assert((x) == 0);

// given a starting position, a bone length and a quaternion - calculate the end position
void calculate_next_position(HmdQuaternionf_t q)
{
	float R[9];
	R[0] = 1 - 2 * q.y*q.y - 2 * q.z*q.z; R[1] = 2 * q.x*q.y - 2 * q.w*q.z;     R[2] = 2 * q.x*q.z + 2 * q.w*q.y;
	R[3] = 2 * q.x*q.y + 2 * q.w*q.z;     R[4] = 1 - 2 * q.x*q.x - 2 * q.z*q.z; R[5] = 2 * q.y*q.z - 2 * q.w*q.x;
	R[6] = 2 * q.x*q.z - 2 * q.w*q.y;     R[7] = 2 * q.y*q.z + 2 * q.w*q.x;     R[8] = 1 - 2 * q.x*q.x - 2 * q.y*q.y;
}


static void toEulerAngle(const HmdQuaternionf_t& q, double& roll, double& pitch, double& yaw)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
	double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w * q.y - q.z * q.x);
	if (fabs(sinp) >= 1)
		pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
	double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
	yaw = atan2(siny_cosp, cosy_cosp);
}


#define RAD2DEG( a ) ( (a) * (180.0f/M_PI) )




int main(int argc, char **argv)
{
	EVRInitError error;
	VR_Init(&error, VRApplication_Scene);
	assert(error == 0);
	   
	std::string app_action_manifest_path = "C:\\projects\\controller_bindings_priority_experiment\\actions_manifest1.json";
	CHECK_RESULT(VRInput()->SetActionManifestPath(app_action_manifest_path.c_str()));

	VRActionSetHandle_t action_set_handle;
	CHECK_RESULT(VRInput()->GetActionSetHandle("/actions/default", &action_set_handle));

	VRActionHandle_t hand_skeleton_action;
	CHECK_RESULT(VRInput()->GetActionHandle("/actions/default/in/SkeletonRightHand", &hand_skeleton_action));

	uint32_t num_sets = 1;
	VRActiveActionSet_t active_set;
	active_set.ulActionSet = action_set_handle;
	active_set.ulRestrictedToDevice = k_ulInvalidInputValueHandle;
	active_set.nPriority = 0;
	int update_count = 0;
	
	bool got_parent_h = false;

	static const int kNumBones = 31;
	BoneIndex_t parent_indices[kNumBones];

	while (!got_parent_h)
	{
		// set the application active set to update
		CHECK_RESULT(VRInput()->UpdateActionState(&active_set, sizeof(active_set), num_sets));
		update_count++;

		
		EVRInputError r = VRInput()->GetBoneHierarchy(hand_skeleton_action, parent_indices, kNumBones);
		if (r == VRInputError_None)
		{
			got_parent_h = true;
			printf("BoneIndex_t parent_indices[] = {");
			for (int i = 0; i < 31; i++)
			{
				printf("%d,", parent_indices[i]);
			}
			printf("};\n");
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		
	}
	

	while (1)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		static const int kNumBones = 31;
		VRBoneTransform_t transforms[kNumBones];
		// parent means the transforms are all parent relative - so distances should be easy to calculate
		EVRInputError r = VRInput()->GetSkeletalBoneData(hand_skeleton_action, VRSkeletalTransformSpace_Model, VRSkeletalMotionRange_WithoutController, transforms, kNumBones);

		// 1/12/2018  - now getting no data here.  blah
		if (r == VRInputError_NoData)
		{
			// using ces2019demos worked better
			//
		}

		// sean try this to see what bone lengths I get
		//r = VRInput()->GetSkeletalReferenceTransforms(hand_skeleton_action, VRSkeletalTransformSpace_Model, VRSkeletalReferencePose_BindPose, transforms, kNumBones);

		if (r == VRInputError_None)
		{
			double root[3];
			double wrist[3];
			toEulerAngle(transforms[0].orientation, root[0], root[1], root[2]);
			toEulerAngle(transforms[1].orientation, wrist[0], wrist[1], wrist[2]);

			wrist[0] = RAD2DEG(wrist[0]) + 360.0;
			wrist[1] = RAD2DEG(wrist[1]) + 360.0;
			wrist[2] = RAD2DEG(wrist[2]) + 360.0;


			float total_bone_len = 0.0f;
			for (int i = 1; i < 26; i++)
			{
				int parent_index = parent_indices[i];
				float d1 = transforms[i].position.v[0] - transforms[parent_index].position.v[0];
				float d2 = transforms[i].position.v[1] - transforms[parent_index].position.v[1];
				float d3 = transforms[i].position.v[2] - transforms[parent_index].position.v[2];
				float bone_length = sqrt(d1*d1 + d2 * d2 + d3 * d3);
				printf("%4.4f,", bone_length);
				total_bone_len += bone_length;
			}
			printf("%4.4f\n", total_bone_len);
		}
	}
}


// sample results

// bind pose, 
// VRSkeletalTransformSpace_Model
// 0.0006, 0.0426, 0.0404, 0.0325

// animation frames
// VRSkeletalTransformSpace_Model
// 0.1721,0.0395,0.0404,0.0325
// pressing the b button
// 0.1721, 0.0366, 0.0404,0.0325

// BoneIndex_t parent_indices[] = {-1,0,1,2,3,4,1,6,7,8,9,1,11,12,13,14,1,16,17,18,19,1,21,22,23,24,0,0,0,0,0,};
// float bone_lengths[] = 
