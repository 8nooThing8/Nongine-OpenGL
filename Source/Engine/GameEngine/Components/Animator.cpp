#include "../Engine/Engine.pch.h"

#include "Animator.h"

#include "../ModelSprite/ModelFactory.h"

#include "GameObject.h"
#include "MeshRenderer.h"
#include "InputHandler.h"
#include "../Console.h"

#include "../MainSingleton.h"

#include "../GameCamera.h"

#include "../Engine/GraphicsEngine/GraphicsCommandList.h"

#include <Matrix\Matrix4x4.hpp>

#include "TGAFbx.h"
#include "Transform.h"

#include "ImGui/imgui_internal.h"

void Animator::UpdateImGuizmoMatrix()
{
	float matrixTranslation[3] = { position.x, position.y, position.z };
	float matrixRotation[3] = { rotation.x, rotation.y, rotation.z };
	float matrixScale[3] = { scale.x, scale.y, scale.z };

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectMatrix);
}

void Animator::RenderImGUIZMO()
{
	UpdateImGuizmoMatrix();

	float matrixTranslation[3];
	float matrixRotation[3];
	float matrixScale[3];

	CommonUtilities::Matrix4x4<float> transform = CommonUtilities::Matrix4x4<float>();

	float camRotX = GameCamera::main->rotation.x;
	float camRotY = GameCamera::main->rotation.y;

	transform.SetPosition(-GameCamera::main->position);

	transform *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(CommonUtilities::DegToRad(camRotY)).GetInverse();
	transform *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(CommonUtilities::DegToRad(camRotX));

	if (ImGuizmo::Manipulate(static_cast<float*>(transform.myData), 
		GameCamera::main->GetProjectionMatrix(), 
		MainSingleton::Get()->gizmoOperation, 
		((MainSingleton::Get()->gizmoOperation == ImGuizmo::SCALE) ? ImGuizmo::LOCAL : MainSingleton::Get()->gizmoMode), 
		objectMatrix))
	{
		ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

		position = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
		rotation = { matrixRotation[0], matrixRotation[1], matrixRotation[2] };
		scale = { matrixScale[0], matrixScale[1], matrixScale[2] };

		for (int i = 0; i < 16; i++)
		{
			mySkeleton.Bones[selectedBone].BindPoseInverse.Data[i] = objectMatrix[i];
		}
	}
}

TGA::FBX::Matrix LerpTGAMatrix(const TGA::FBX::Matrix& aMinMatrix, const TGA::FBX::Matrix& aMaxMatrix, float aT)
{
	TGA::FBX::Matrix newMatrix;

	for (int x = 1; x < 5; x++)
	{
		for (int y = 1; y < 5; y++)
		{
			newMatrix(x, y) = std::lerp(aMinMatrix(x, y), aMaxMatrix(x, y), aT);
		}
	}

	return newMatrix;
}

void Animator::UpdateAnimator(size_t aCurrentFrame1, size_t aCurrentFrame2, unsigned aJointID, const TGA::FBX::Matrix& aParentJointTransform, float alpha, bool noTransformation)
{
	std::string name = mySkeleton.Bones[aJointID].Name;

	for (auto& myAnimationLayer : myAnimationLayers)
	{
		if (myAnimationLayer.myAnimation)
		{
			if (name == myAnimationLayer.myBoneToAffect)
				return;
		}
	}

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"BoneCalcUpdate");

	auto& currentFrame1 = myCurrentAnimation->Frames[aCurrentFrame1];
	auto& nextFrame1 = myCurrentAnimation->Frames[(aCurrentFrame1 + 1) % myCurrentAnimation->Frames.size()];

	auto& currentJointTransform1 = currentFrame1.LocalTransforms[name];
	auto& nextJointTransform1 = nextFrame1.LocalTransforms[name];

	TGA::FBX::Matrix interpolatedTransform1 = CommonUtilities::Lerp(currentJointTransform1, nextJointTransform1, alpha);

	TGA::FBX::Matrix finalTransform1 = interpolatedTransform1 * aParentJointTransform;

	TGA::FBX::Matrix jointTransform1 = mySkeleton.Bones[aJointID].BindPoseInverse * (noTransformation ? TGA::FBX::Matrix() : finalTransform1);

	CommonUtilities::Vector4<float> positionB = { finalTransform1.m14, finalTransform1.m24, finalTransform1.m34, 0 };

	CommonUtilities::Vector4<float> ppositionB = { aParentJointTransform.m14, aParentJointTransform.m24, aParentJointTransform.m34, 0 };


	PIXScopedEvent(PIX_COLOR_INDEX(1), L"BoneView");

	myRenderBones[aJointID].first->transform->position = positionB;

	CommonUtilities::Vector4<float> forwardB = ppositionB - positionB;

	float scaleAll = CommonUtilities::Vector4<float>(forwardB.x, forwardB.y, forwardB.z, 1).Length() * 0.1f;

	myRenderBones[aJointID].first->transform->scale = CommonUtilities::Vector4<float>(scaleAll, scaleAll, scaleAll, 1);

	CommonUtilities::Vector4<float> forwardNorm = forwardB.GetNormalized();

	float pitch = asin(forwardNorm.y);
	float yaw = atan2(forwardNorm.x, forwardNorm.z);
	float roll = 0.0f;

	CommonUtilities::Vector4<float> boneRotation = { -pitch, yaw, roll };

	//if (name == boneToAttachTo)
	//{
	//	CommonUtilities::Vector4<float> boneMid = positionB + forwardB * 0.5f;

	//	MainSingleton::Get()->objectToAttach->position = boneMid;
	//	MainSingleton::Get()->objectToAttach->rotation = CommonUtilities::Vector4<float>(-boneRotation.x, boneRotation.y, -boneRotation.z, 0);

	//	MainSingleton::Get()->objectToAttach->position = MainSingleton::Get()->objectToAttach->position * gameObject->transform->GetFinalTransform();

	//	MainSingleton::Get()->objectToAttach->position += gameObject->transform->position;

	//	MainSingleton::Get()->objectToAttach->Update(0);
	//}

	if (myCurrentAnimation && renderBones)
	//if (true)
	{
		myRenderBones[aJointID].first->transform->rotation = boneRotation;

		myRenderBones[aJointID].first->transform->Update(0);

		MainSingleton::Get()->mySkeletonCommandList->Enqueue<RenderMeshCommand>(myRenderBones[aJointID].second);
		MainSingleton::Get()->mySkeletonCommandList->myObjects.emplace_back(myRenderBones[aJointID].first.get());
	}

	

	if (mySecondAnimation)
	{
		auto& currentFrame2 = mySecondAnimation->Frames[aCurrentFrame2];
		auto& nextFrame2 = mySecondAnimation->Frames[(aCurrentFrame2 + 1) % mySecondAnimation->Frames.size()];

		auto& currentJointTransform2 = currentFrame2.LocalTransforms[name];
		auto& nextJointTransform2 = nextFrame2.LocalTransforms[name];

		TGA::FBX::Matrix interpolatedTransform2 = CommonUtilities::Lerp(currentJointTransform2, nextJointTransform2, alpha);

		TGA::FBX::Matrix finalTransform2 = interpolatedTransform2 * aParentJointTransform;

		TGA::FBX::Matrix jointTransform2 = mySkeleton.Bones[aJointID].BindPoseInverse * (noTransformation ? TGA::FBX::Matrix() : finalTransform2);

		myAnimationBuffer.JointTransforms[aJointID] = LerpTGAMatrix(jointTransform1, jointTransform2, t);

		for (auto& child : mySkeleton.Bones[aJointID].Children)
		{
			UpdateAnimator(aCurrentFrame1, aCurrentFrame2, child, LerpTGAMatrix(finalTransform1, finalTransform2, t), alpha, noTransformation);
		}
	}
	else
	{
		myAnimationBuffer.JointTransforms[aJointID] = jointTransform1;

		for (auto& child : mySkeleton.Bones[aJointID].Children)
		{
			UpdateAnimator(aCurrentFrame1, aCurrentFrame2, child, finalTransform1, alpha, noTransformation);
		}
	}
}

void Animator::UpdateAnimatorLayered(size_t aCurrentFrame1, size_t aCurrentFrame2, unsigned aJointID, const TGA::FBX::Matrix& aParentJointTransform, int aIndex, const float& alpha)
{
	std::string name = mySkeleton.Bones[aJointID].Name;

	auto& currentFrame1 = myAnimationLayers[aIndex].myAnimation->Frames[aCurrentFrame1];
	auto& nextFrame1 = myAnimationLayers[aIndex].myAnimation->Frames[(aCurrentFrame1 + 1) % myAnimationLayers[aIndex].myAnimation->Frames.size()];

	auto& currentJointTransform1 = currentFrame1.LocalTransforms[name];
	auto& nextJointTransform1 = nextFrame1.LocalTransforms[name];

	TGA::FBX::Matrix interpolatedTransform1 = CommonUtilities::Lerp(currentJointTransform1, nextJointTransform1, alpha);

	TGA::FBX::Matrix finalTransform1 = interpolatedTransform1 * aParentJointTransform;

	CommonUtilities::Vector4<float> positionB = { finalTransform1.m14, finalTransform1.m24, finalTransform1.m34, 0 };
	CommonUtilities::Vector4<float> ppositionB = { aParentJointTransform.m14, aParentJointTransform.m24, aParentJointTransform.m34, 0 };

	if (myCurrentAnimation && renderBones)
	{
		myRenderBones[aJointID].first->transform->position = positionB;

		CommonUtilities::Vector4<float> forwardB = ppositionB - positionB;

		float scaleAll = CommonUtilities::Vector4<float>(forwardB.x, forwardB.y, forwardB.z, 1).Length() * 0.1f;

		myRenderBones[aJointID].first->transform->scale = CommonUtilities::Vector4<float>(scaleAll, scaleAll, scaleAll, 1);

		CommonUtilities::Vector4<float> forwardNorm = forwardB.GetNormalized();

		float pitch = asin(forwardNorm.y);
		float yaw = atan2(forwardNorm.x, forwardNorm.z);
		float roll = 0.0f;

		CommonUtilities::Vector4<float> boneRotation = { -pitch, yaw, roll };

		myRenderBones[aJointID].first->transform->rotation = boneRotation;

		myRenderBones[aJointID].first->transform->Update(0);

		MainSingleton::Get()->mySkeletonCommandList->Enqueue<RenderMeshCommand>(myRenderBones[aJointID].second);
		MainSingleton::Get()->mySkeletonCommandList->myObjects.emplace_back(myRenderBones[aJointID].first.get());
	}

	//TGA::FBX::Matrix jointTransform1 = mySkeleton.Bones[aJointID].BindPoseInverse * finalTransform1;

	//if (mySecondAnimation)
	//{
	//	auto& currentFrame2 = mySecondAnimation->Frames[aCurrentFrame2];
	//	auto& nextFrame2 = mySecondAnimation->Frames[(aCurrentFrame2 + 1) % mySecondAnimation->Frames.size()];

	//	auto& currentJointTransform2 = currentFrame2.LocalTransforms[name];
	//	auto& nextJointTransform2 = nextFrame2.LocalTransforms[name];

	//	TGA::FBX::Matrix interpolatedTransform2 = CommonUtilities::Lerp(currentJointTransform2, nextJointTransform2, alpha);

	//	TGA::FBX::Matrix finalTransform2 = interpolatedTransform2 * aParentJointTransform;

	//	TGA::FBX::Matrix jointTransform2 = mySkeleton.Bones[aJointID].BindPoseInverse * finalTransform2;

	//	myAnimationBuffer.JointTransforms[aJointID] = LerpTGAMatrix(jointTransform1, jointTransform2, t);

	//	for (auto& child : mySkeleton.Bones[aJointID].Children)
	//	{
	//		UpdateAnimatorLayered(aCurrentFrame1, aCurrentFrame2, child, LerpTGAMatrix(finalTransform1, finalTransform2, t), aIndex, alpha);
	//	}
	//}
	//else
	//{
	myAnimationBuffer.JointTransforms[aJointID] = mySkeleton.Bones[aJointID].BindPoseInverse * finalTransform1;

	for (auto& child : mySkeleton.Bones[aJointID].Children)
	{
		UpdateAnimatorLayered(aCurrentFrame1, aCurrentFrame2, child, finalTransform1, aIndex, alpha);
	}
	//}
}

int Animator::GetBoneIndexFromString(std::string& aBoneName)
{
	for (int i = 0; i < mySkeleton.Bones.size(); i++)
	{
		if (mySkeleton.Bones[i].Name == aBoneName)
		{
			return i;
		}
	}

	PrintError("The bone name that has been provided for the second animation to be played is invalid or the bone does not exist");

	return 0;
}

void Animator::UpdateEditor(float /*aDeltaTime*/)
{

}

void MySequence::CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect,
	const ImRect& clippingRect, const ImRect& legendClippingRect)
{
	static const char* labels[] = { "Animation" };

	rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
	rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
	draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
	for (int i = 0; i < 1; i++)
	{
		ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
		ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
		draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
		if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
			rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
	}
	draw_list->PopClipRect();

	ImGui::SetCursorScreenPos(rc.Min);
	ImCurveEdit::Edit(rampEdit, ImVec2(rc.Max.x - rc.Min.x, rc.Max.y - rc.Min.y), 137 + index, &clippingRect);
}

void MySequence::CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
{
	rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
	rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
	draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < rampEdit.mPointCount[i]; j++)
		{
			float p = rampEdit.mPts[i][j].x;
			if (p < myItems[index].mFrameStart || p > myItems[index].mFrameEnd)
				continue;
			float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
			float x = ImLerp(rc.Min.x, rc.Max.x, r);
			draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
		}
	}
	draw_list->PopClipRect();
}

Animator::~Animator()
{
	myIsStopped = true;
	Stop();
}

void Animator::Stop()
{
	if (gameObject)
	{
		gameObject->GetComponent<MeshRenderer>()->GetMaterial().SetVertexShader(L"Assets/Shaders/Default_VS.hlsl");
	}
}

void Animator::Update(float aDeltaTime)
{
	if (!myCurrentAnimation)
		return;

	if (myCurrentAnimationstate == AnimationState::Paused)
	{
		myCurrentFrame = currentFrame;
	}

	if (myCurrentAnimationstate == AnimationState::Playing)
	{
		currentFrame = myCurrentFrame;
	}

	if (switchAniamtion)
	{
		t += aDeltaTime / 2;

		if (t >= 1)
		{
			t = 1;
			switchAniamtion = false;

			SetAnimation(secondAnimationPath);
		}
	}

	TGA::FBX::Matrix startMatrix;
	float alpha = 0;

	if (myCurrentAnimationstate != AnimationState::Paused)
	{
		/*if (!InputHandler::GetInstance()->GetMouseButton(1))
		{
			if (InputHandler::GetInstance()->GetKeyDown(keycode::SPACE))
			{
				myCurrentAnimationNumber = (myCurrentAnimationNumber + 1) % 4;

				std::string animationToBeSet;

				switch (myCurrentAnimationNumber)
				{
				case 0:
					animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Idle_Brething.fbx";
					SetSecondAnimation(animationToBeSet);
					switchAniamtion = true;
					t = 0;
					break;
				case 1:
					animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Idle_Wave.fbx";
					SetSecondAnimation(animationToBeSet);
					switchAniamtion = true;
					t = 0;
					break;
				case 2:
					animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Run.fbx";
					SetSecondAnimation(animationToBeSet);
					switchAniamtion = true;
					t = 0;
					break;
				case 3:
					animationToBeSet = "Assets/Animations/A_C_TGA_Bro_Walk.fbx";
					SetSecondAnimation(animationToBeSet);
					switchAniamtion = true;
					t = 0;
					break;
				}
			}
		}*/

		myAnimTime += aDeltaTime;
		myCurrentFrameTime = static_cast<float>(1.0 / myCurrentAnimation->FramesPerSecond);

		alpha = myAnimTime / myCurrentFrameTime;

		if (myAnimTime >= myCurrentFrameTime)
		{
			myAnimTime = 0.0;
			myCurrentFrame = (myCurrentFrame + 1) % myCurrentAnimation->Frames.size();
		}
	}

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"Layered animation");

	for (int i = 0; i < myAnimationLayers.size(); i++)
	{
		if (myAnimationLayers[i].myAnimation)
		{
			float alphaLayer = 0;

			if (myCurrentAnimationstate != AnimationState::Paused)
			{
				myAnimationLayers[i].myAnimTime += aDeltaTime;
				myAnimationLayers[i].myFrameTime = static_cast<float>(1.0 / myAnimationLayers[i].myAnimation->FramesPerSecond);

				alphaLayer = myAnimationLayers[i].myAnimTime / myAnimationLayers[i].myFrameTime;

				if (myAnimationLayers[i].myAnimTime >= myAnimationLayers[i].myFrameTime)
				{
					myAnimationLayers[i].myAnimTime = 0.0;
					myAnimationLayers[i].myCurrentFrame = (myAnimationLayers[i].myCurrentFrame + 1) % myAnimationLayers[i].myAnimation->Frames.size();
				}
			}

			const int index = GetBoneIndexFromString(myAnimationLayers[i].myBoneToAffect);

			const int parentIndex = mySkeleton.Bones[index].ParentIdx;
			const std::string parentName = mySkeleton.Bones[parentIndex].Name;

			TGA::FBX::Matrix parentMatrixLayed = myAnimationLayers[i].myAnimation->Frames[myAnimationLayers[i].myCurrentFrame].GlobalTransforms[parentName];
			const TGA::FBX::Matrix parentMatrix = myCurrentAnimation->Frames[myCurrentFrame].GlobalTransforms[parentName];

			parentMatrixLayed(4, 1) = parentMatrix(4, 1);
			parentMatrixLayed(4, 2) = parentMatrix(4, 2);
			parentMatrixLayed(4, 3) = parentMatrix(4, 3);

			UpdateAnimatorLayered(myAnimationLayers[i].myCurrentFrame, mySecondAnimCurrentFrame, index, parentMatrixLayed, i, alphaLayer);
		}
	}

	PIXScopedEvent(PIX_COLOR_INDEX(1), L"AnimationUpdate");

	if (myCurrentAnimationstate != AnimationState::Paused)
	{
		if (mySecondAnimation)
		{
			mySecondAnimTime += aDeltaTime;
			mySecondAnimFrameTime = static_cast<float>(1.0 / myCurrentAnimation->FramesPerSecond);

			if (mySecondAnimTime >= mySecondAnimFrameTime)
			{
				mySecondAnimTime = 0.0;
				mySecondAnimCurrentFrame = (mySecondAnimCurrentFrame + 1) % mySecondAnimation->Frames.size();
			}
		}
	}

	UpdateAnimator(myCurrentFrame, mySecondAnimCurrentFrame, 0, startMatrix, alpha);
}

void Animator::AddLayeredAnimation(const std::string& boneName)
{
	LayeredAnimationData data;

	data.myAnimation = nullptr;
	data.myBoneToAffect = boneName;
	data.myCurrentFrame = 0;

	myAnimationLayers.push_back(std::move(data));
}

void Animator::AddedAsComponent()
{
	//TGA::FBX::Matrix startMatrix;

	//UpdateAnimator(myCurrentFrame, mySecondAnimCurrentFrame, 0, startMatrix, 0);
}

void Animator::SetLayeredAnimation(const std::string& aAnimationPath, int aIndex)
{
	delete myAnimationLayers[aIndex].myAnimation;

	myAnimationLayers[aIndex].myAnimation = CreateAnimation(aAnimationPath);
	myAnimationLayers[aIndex].myCurrentFrame = 0;

	TGA::FBX::Matrix matrix;

	const int index = GetBoneIndexFromString(myAnimationLayers[aIndex].myBoneToAffect);

	UpdateAnimatorLayered(0, 0, index, matrix, aIndex, 1);
}

void Animator::SetSecondAnimation(const std::string& aAnimationPath)
{
	secondAnimationPath = aAnimationPath;

	mySecondAnimation = CreateAnimation(aAnimationPath);

	mySecondAnimCurrentFrame = 0;

	TGA::FBX::Matrix startMatrix;

	UpdateAnimator(myCurrentFrame, mySecondAnimCurrentFrame, 0, startMatrix, 0);
}

void Animator::SetAnimation(const std::string& aAnimationPath, TGA::FBX::Mesh* aMesh)
{
	animationPath = aAnimationPath;
	

	MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();

	if (meshRenderer)
		myCurrentMesh = meshRenderer->GetMesh();
	else
		myCurrentMesh = aMesh;

	myCurrentFrame = 0;
	mySkeleton = myCurrentMesh->Skeleton;

	for (auto& bone : mySkeleton.Bones)
	{
		bone.BindPoseInverse = bone.BindPoseInverse.GetTranspose();
	}

	myCurrentAnimation = CreateAnimation(aAnimationPath);

	TGA::FBX::Matrix startMatrix;

	if (myCurrentMesh)
	{
		myRenderBones.clear();

		for (int i = 0; i < myCurrentMesh->Skeleton.Bones.size(); i++)
		{
			myRenderBones.push_back(std::pair<std::shared_ptr<GameObject>, MeshRenderer*>(std::make_shared<GameObject>(), nullptr));

			myRenderBones[i].second = myRenderBones[i].first->AddComponent<MeshRenderer>(false);

			myRenderBones[i].second->myMesh = MainSingleton::Get()->mySkeletonBone;

			myRenderBones[i].second->GetMaterial().SetShader(L"Assets/Shaders/Simple_Lit_PS.hlsl", L"Assets/Shaders/Default_VS.hlsl");
			myRenderBones[i].second->GetMaterial().SetColor(CommonUtilities::Vector4<float>(1, 1, 1, 1));

			gameObject->PushNewChild(myRenderBones[i].first);
		}
	}

	UpdateAnimator(myCurrentFrame, mySecondAnimCurrentFrame, 0, startMatrix, 0);
}

void Animator::SetMesh(TGA::FBX::Mesh* aMesh)
{
	myCurrentMesh = aMesh;
}

void Animator::Start()
{
	boneToAttachTo = "LeftHand";

	myCurrentAnimationstate = AnimationState::Playing;

	myAnimTime = myCurrentFrameTime;

	myCurrentFrame = 0;

	myCurrentAnimationNumber = -1;

	TGA::FBX::Matrix matrix;

	MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
	if (meshRenderer)
	{
		meshRenderer->myMaterial.SetVertexShader(L"Assets/Shaders/Animated_VS.hlsl");
		SetAnimation(animationPath);
	}
	else
	{
		SetAnimation(animationPath, myCurrentMesh);
	}

	UpdateAnimator(0, 0, 0, matrix, 1, true);
}

void Animator::OpenBoneChildren(int aIndex, int aCount)
{
	std::string ParentName = mySkeleton.Bones[aIndex].Name;

	ImGui::Dummy(ImVec2(10.f * static_cast<float>(aCount), 0));
	ImGui::SameLine();

	//ImGui::PushItemWidth(20);

	if (ImGui::CollapsingHeader(ParentName.c_str()))
	{
		ImGui::Dummy(ImVec2(10.f * static_cast<float>(aCount), 0));
		ImGui::SameLine();

		std::stringstream stream;

		stream << "Select##" << aIndex;

		if (selectedBone == aIndex)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0.7f, 0, 1));

		//ImGui::SameLine();

		bool selectedBoneWasTheSame = false;
		bool buttonPressed = ImGui::Button(stream.str().c_str());

		if (buttonPressed)
		{
			if (selectedBone == aIndex)
			{
				ImGui::PopStyleColor();
				selectedBoneWasTheSame = true;
			}


			myRenderBones[selectedBone].second->GetMaterial().SetColor(CommonUtilities::Vector4<float>(1.f, 1.f, 1.f, 1.f));
			selectedBone = aIndex;
			myRenderBones[selectedBone].second->GetMaterial().SetColor(CommonUtilities::Vector4<float>(1.f, 0.7f, 0.f, 1.f));

			for (int i = 0; i < 16; i++)
			{
				objectMatrix[i] = mySkeleton.Bones[selectedBone].BindPoseInverse.Data[i];
			}

		}

		if (selectedBone == aIndex && !selectedBoneWasTheSame && !buttonPressed)
			ImGui::PopStyleColor();

		for (auto& child : mySkeleton.Bones[aIndex].Children)
		{
			OpenBoneChildren(child, aCount + 1);
		}
	}

	//ImGui::PopItemWidth();
}

void Animator::HandleSave(rapidjson::Value& outJson, rapidjson::Document& document)
{
	std::string mainAnimationNameString = myCurrentAnimation->Path;
	std::string SecondnimationNameString = mySecondAnimation->Path;

	rapidjson::Value mainAnimationName(mainAnimationNameString.c_str(), document.GetAllocator());
	rapidjson::Value secondAnimationName(SecondnimationNameString.c_str(), document.GetAllocator());

	outJson.AddMember("Main Animation", mainAnimationName, document.GetAllocator());
	outJson.AddMember("Layered Animation", secondAnimationName, document.GetAllocator());
}

void Animator::HandleLoad(const rapidjson::Value& outJson)
{
	Animator* animator = gameObject->AddComponent<Animator>();

	std::string mainsAnimationPath = outJson["Main Animation"].GetString();
	std::string secAnimationPath = outJson["Layered Animation"].GetString();

	if (!mainsAnimationPath.empty())
		animator->SetAnimation(mainsAnimationPath);
	if (!secAnimationPath.empty())
		animator->SetAnimation(secAnimationPath);
}

void Animator::RenderImGUI()
{
	//RenderImGUIZMO();

	if (ImGui::Begin("Animation Graph Editor"))
	{
		if (ImGui::Button("Start Blending"))
		{
			switchAniamtion = true;
			t = 0;
		}
		if (myCurrentAnimationstate == AnimationState::Paused)
		{
			if (ImGui::Button("Play"))
			{
				myCurrentAnimationstate = AnimationState::Playing;
			}
		}
		else
		{
			if (ImGui::Button("Pause"))
			{
				myCurrentAnimationstate = AnimationState::Paused;
			}
		}


		if (ImGui::Button("Addpoint"))
		{
			mySequence.GetRampEdit().AddPoint(0, ImVec2(10, 0.1f));
		}

		ImGui::PushItemWidth(130);

		ImGui::DragInt("Frame Min", &mySequence.mFrameMin);
		ImGui::SameLine();

		ImGui::DragInt("Frame ", &currentFrame);
		ImGui::SameLine();

		ImGui::DragInt("Frame Max", &mySequence.mFrameMax);
		ImGui::SameLine();

		if (ImGui::InputInt("selected", &selectedNode))
		{
			selectedNode = std::clamp(selectedNode, -1, static_cast<int>(mySequence.GetRampEdit().GetPointCount(0)) - 1);

			if (selectedNode != -1)
			{
				currentEdit = mySequence.GetRampEdit().GetPoint(selectedNode);
			}
		}



		if (selectedNode != -1)
		{
			ImGui::DragFloat("Time", &currentEdit.x);
			ImGui::SameLine();
			ImGui::DragFloat("Value", &currentEdit.y);

			mySequence.GetRampEdit().EditPoint(0, selectedNode, currentEdit);
		}

		ImGui::PopItemWidth();
		Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_EDIT_ALL | ImSequencer::SEQUENCER_CHANGE_FRAME);

		//currentFrame = static_cast<int>(t * 100.f);

		currentFrame = std::clamp(currentFrame, mySequence.mFrameMin, static_cast<int>(myCurrentAnimation->Frames.size() - 1));
		mySequence.mFrameMin = std::clamp(mySequence.mFrameMin, 0, mySequence.mFrameMax);

		// add a UI to edit that particular item
		//if (selectedEntry != -1)
		//{
		//	const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];
		//	ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
		//}
	}

	ImGui::End();

	ImGui::Text("Display");
	ImGui::Checkbox("Render bones", &renderBones);

	ImGui::Text("Current Animation");

	if (myCurrentAnimation)
	{
		if (ImGui::Button((myCurrentAnimation->Name + "##MainAnimation").c_str()))
			ImGui::OpenPopup("selectAnimationPopUp");
	}
	else
	{
		if (ImGui::Button("<None>##MainAnimation"))
			ImGui::OpenPopup("selectAnimationPopUp");
	}


	if (ImGui::BeginPopup("selectAnimationPopUp"))
	{
		std::vector<std::string> animations;

		std::string directoryPath = "Assets/Animations";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				animations.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Animations");
		for (const auto& animation : animations)
		{
			if (ImGui::Selectable(animation.c_str()))
			{
				std::string animationToBeSet = "Assets/Animations/" + animation;

				SetAnimation(animationToBeSet);
			}
		}

		ImGui::EndPopup();
	}

	ImGui::Text("Blended Animation");

	if (mySecondAnimation)
	{
		if (ImGui::Button((mySecondAnimation->Name + "##SecondAnimation").c_str()))
			ImGui::OpenPopup("selectSecondAnimationPopUp");
	}
	else
	{
		if (ImGui::Button("<None>##SecondAnimation"))
			ImGui::OpenPopup("selectSecondAnimationPopUp");
	}


	if (ImGui::BeginPopup("selectSecondAnimationPopUp"))
	{
		std::vector<std::string> animations;

		std::string directoryPath = "Assets/Animations";

		if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
		{
			PrintError("Directory for texture imgui checking does not exist or is not a directory");
		}

		for (const auto& entry : fs::directory_iterator(directoryPath))
		{
			const auto& path = entry.path();
			if (fs::is_regular_file(entry))
			{
				animations.push_back(path.filename().string());
			}
		}

		ImGui::SeparatorText("Animations  ");
		for (const auto& animation : animations)
		{
			if (ImGui::Selectable((animation + "  ").c_str()))
			{
				std::string animationToBeSet = "Assets/Animations/" + animation;

				SetSecondAnimation(animationToBeSet);
			}
		}

		ImGui::EndPopup();
	}

	if (ImGui::CollapsingHeader("Skeleton bones"))
	{
		OpenBoneChildren(0, 0);
	}

	ImGui::Text("Add layered animation");
	ImGui::InputText("Bone name", bName, IM_ARRAYSIZE(bName), ImGuiInputTextFlags_EnterReturnsTrue);
	if (ImGui::Button("Add"))
	{
		AddLayeredAnimation(bName);
		strcpy_s(bName, "");
	}

	for (int i = 0; i < myAnimationLayers.size(); i++)
	{
		ImGui::Text(std::string("Layered Animation for bone: " + myAnimationLayers[i].myBoneToAffect).c_str());

		std::stringstream iStream;

		iStream << i;

		if (myAnimationLayers[i].myAnimation)
		{
			std::stringstream stream;

			stream << myAnimationLayers[i].myAnimation->Name << "##" << i;

			if (ImGui::Button(stream.str().c_str()))
				ImGui::OpenPopup(iStream.str().c_str());
		}
		else
		{
			std::stringstream stream;

			stream << "<None>" << "##" << i;

			if (ImGui::Button(stream.str().c_str()))
				ImGui::OpenPopup(iStream.str().c_str());
		}

		ImGui::SameLine();

		std::stringstream boneStream;

		boneStream << "Change bone name" << "##" << i;

		char newbName[256] = "";

		if (ImGui::InputText(boneStream.str().c_str(), newbName, IM_ARRAYSIZE(newbName), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			myAnimationLayers[i].myBoneToAffect = newbName;
		}


		if (ImGui::BeginPopup(iStream.str().c_str()))
		{
			std::vector<std::string> animations;

			std::string directoryPath = "Assets/Animations";

			if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
			{
				PrintError("Directory for texture imgui checking does not exist or is not a directory");
			}

			for (const auto& entry : fs::directory_iterator(directoryPath))
			{
				const auto& path = entry.path();
				if (fs::is_regular_file(entry))
				{
					animations.push_back(path.filename().string());
				}
			}

			ImGui::SeparatorText("Animations ");
			for (const auto& animation : animations)
			{
				if (ImGui::Selectable((animation + " ").c_str()))
				{
					std::string animationToBeSet = "Assets/Animations/" + animation;

					SetLayeredAnimation(animationToBeSet, i);
				}
			}

			ImGui::EndPopup();
		}
	}
}
