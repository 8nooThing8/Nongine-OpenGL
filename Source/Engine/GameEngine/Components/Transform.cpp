#include "../../Engine.pch.h"

#include "Transform.h"

#include "GameObject.h"

#include "InputHandler.h"

#include <Vector\Vector.hpp>
#include <../ImGuizmo/ImGuizmo.h>

#include "../Camera.h"
#include "../Gamecamera.h"

#include "../MainSingleton.h"

CommonUtilities::Matrix4x4<float> Transform::GetFinalTransform()
{
	CommonUtilities::Matrix4x4<float> returnMX = CommonUtilities::Matrix4x4<float>();

	returnMX(1, 1) = scale.x;
	returnMX(2, 2) = scale.y;
	returnMX(3, 3) = scale.z;

	CommonUtilities::Matrix4x4<float> positionRotation;

	positionRotation = CommonUtilities::Matrix4x4<float>::CreateRotationAsQuerternion(CommonUtilities::Vector3<float>(rotation).ToQuart());

	positionRotation.SetPosition(position);

	returnMX *= positionRotation;

	return returnMX;
}

CommonUtilities::Matrix4x4<float> Transform::GetFinalTransformNoScale()
{
	CommonUtilities::Matrix4x4<float> returnMX = CommonUtilities::Matrix4x4<float>();

	CommonUtilities::Matrix4x4<float> positionRotation;

	positionRotation = CommonUtilities::Matrix4x4<float>::CreateRotationAsQuerternion(CommonUtilities::Vector3<float>(rotation).ToQuart());

	positionRotation.SetPosition(position);

	return positionRotation;
}

void Transform::UpdateImGuizmoMatrix()
{
	CommonUtilities::Vector3<float> positions = myObjBuffer.world.GetPosition();
	CommonUtilities::Vector3<float> rotations = myObjBuffer.world.GetRotationAsQuerternion().ToEuler();
	CommonUtilities::Vector3<float> scales = myObjBuffer.world.GetScale();

	float matrixTranslation[3] = { positions.x, positions.y, positions.z };
	float matrixRotation[3] = { rotation.x, rotation.y, rotation.z };
	float matrixScale[3] = { scale.x, scale.y, scale.z };

	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectMatrix);
}

void Transform::RenderImGUIZMO()
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

	if (ImGuizmo::Manipulate(static_cast<float*>(transform), static_cast<float*>(GameCamera::main->GetProjectionMatrix()), MainSingleton::Get()->gizmoOperation, ((MainSingleton::Get()->gizmoOperation == ImGuizmo::SCALE) ? ImGuizmo::LOCAL : MainSingleton::Get()->gizmoMode), objectMatrix))
	{
		ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

		position = { matrixTranslation[0], matrixTranslation[1], matrixTranslation[2] };
		rotation = { matrixRotation[0], matrixRotation[1], matrixRotation[2] };
		scale = { matrixScale[0], matrixScale[1], matrixScale[2] };
	}
}

void Transform::RenderImGUI()
{
	if (ImGui::RadioButton("Local", MainSingleton::Get()->gizmoMode == ImGuizmo::LOCAL))
		MainSingleton::Get()->gizmoMode = ImGuizmo::LOCAL;
	ImGui::SameLine();
	if (ImGui::RadioButton("World", MainSingleton::Get()->gizmoMode == ImGuizmo::WORLD))
		MainSingleton::Get()->gizmoMode = ImGuizmo::WORLD;

	float changeSpeed = 0.1f;

	if (InputHandler::GetInstance()->GetKey(keycode::CONTROL))
	{
		changeSpeed = 0.01f;
	}
	else if (InputHandler::GetInstance()->GetKey(keycode::SHIFT))
	{
		changeSpeed = 1.f;
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::Text("Position");
	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();
	if (ImGui::DragFloat3("##Position", &position[0], changeSpeed))
	{
		//UpdateImGuizmoMatrix();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::Text("Rotation");
	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	CommonUtilities::Vector3<float> angle = CommonUtilities::Vector3<float>(rotation.x, rotation.y, rotation.z);

	CommonUtilities::Vector3<float> angleDegrees;

	angleDegrees[0] = angle[0];
	angleDegrees[1] = angle[1];
	angleDegrees[2] = angle[2];

	if (ImGui::DragFloat3("##Rotation", &angleDegrees[0], changeSpeed))
	{
		angle[0] = angleDegrees[0];
		angle[1] = angleDegrees[1];
		angle[2] = angleDegrees[2];

		//UpdateImGuizmoMatrix();
	}

	rotation = angle;

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::Text("Scale");
	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();
	if (ImGui::DragFloat3("##Scale", &scale[0], changeSpeed))
	{
		//UpdateImGuizmoMatrix();
	}

	ImGui::Dummy(ImVec2(12.0f, 0.f));
	ImGui::SameLine();

	ImGui::Text("Uniform scale");
	ImGui::SameLine();
	ImGui::Checkbox("##UniformScale", &uniform);

	//if (uniform)
	//{
	//	scale[1] = scale[0];
	//	scale[2] = scale[0];
	//}

	//if (rot[0] != prevrot[0])
	//{
	//	float amount = prevrot[0] - rot[0];

	//	mySelectedObject->transform->AddToRotation(0, CommonUtilities::DegToRad(amount));
	//}
	//if (rot[1] != prevrot[1])
	//{
	//	float amount = prevrot[1] - rot[1];

	//	mySelectedObject->transform->AddToRotation(1, CommonUtilities::DegToRad(amount));
	//}
	//if (rot[2] != prevrot[2])
	//{
	//	float amount = prevrot[2] - rot[2];

	//	mySelectedObject->transform->AddToRotation(2, CommonUtilities::DegToRad(amount));
	//}
}

void Transform::Start()
{

}

void Transform::Update(const float /*aDeltaTime*/)
{
	//CommonUtilities::Vector4<float> pixelPickerColor;

	//pixelPickerColor.x = static_cast<float>((static_cast<unsigned>(gameObject->myID)) & 0xff);
	//pixelPickerColor.y = static_cast<float>((static_cast<unsigned>(gameObject->myID) >> 8) & 0xff);
	//pixelPickerColor.z = static_cast<float>((static_cast<unsigned>(gameObject->myID) >> 16) & 0xff);

	//pixelPickerColor = pixelPickerColor;

	//myObjBuffer.objectColorID = CommonUtilities::Vector4<float>(pixelPickerColor.x, pixelPickerColor.y, pixelPickerColor.z, 1.f);

	myObjBuffer.world = GetFinalTransform();

	myObjBuffer.scale = scale;

	if (gameObject->GetParent())
		myObjBuffer.world *= gameObject->GetParent()->transform->GetObjBuffer().world;

	//std::vector<std::shared_ptr<GameObject>>& children = gameObject->GetChildren();
	//for (auto& child : children)
	//{
	//	child->transform->Update(aDeltaTime);
	//}

	myObjBuffer.rotation = myObjBuffer.world.GetInverse();
}

CU::Matrix4x4<float> Transform::GetObjectBufferGlobalMat()
{
	CU::Matrix4x4<float> mat = myObjBuffer.world;

	if (auto parent = gameObject->GetParent())
		mat *= parent->transform->GetObjBuffer().world;

	return mat;
}

ObjectBuffer& Transform::GetObjBuffer()
{
	return myObjBuffer;
}

CommonUtilities::Vector3f Transform::GetPosition()
{
	CommonUtilities::Vector4f finalPosition = myObjBuffer.world.GetPosition();

	return finalPosition.ToVector3();
}

CommonUtilities::Vector3f Transform::GetLocalPosition()
{
	return position;
}

CommonUtilities::Vector3f Transform::GetRotation()
{
	CommonUtilities::Vector4f finalRotation = rotation;

	if (auto parent = gameObject->GetParent())
		finalRotation = GetObjectBufferGlobalMat().GetRotationAsQuerternion().ToEuler();

	return finalRotation;
}

CommonUtilities::Vector3f Transform::GetLocalRotation()
{
	return rotation;
}

CommonUtilities::Vector3f Transform::GetScale()
{
	CommonUtilities::Vector4f finalScale = position;

	if (auto parent = gameObject->GetParent())
		finalScale = GetObjectBufferGlobalMat().GetScale();

	return finalScale;
}

CommonUtilities::Vector3f Transform::GetLocalScale()
{
	return CommonUtilities::Vector3f();
}



void Transform::SetUniform(const bool aUniform)
{
	uniform = aUniform;
}

CommonUtilities::Vector3f Transform::GetForward()
{
	CommonUtilities::Vector3f forward;

	float cosX = cos(DegToRad(rotation.x));
	float cosY = cos(DegToRad(rotation.y));

	float sinX = sin(DegToRad(-rotation.x));
	float sinY = sin(DegToRad(rotation.y));

	forward.x = cosX * sinY;
	forward.y = -sinX;
	forward.z = cosX * cosY;

	return forward;
}

CommonUtilities::Vector3f Transform::GetRight()
{
	CommonUtilities::Vector3f right;

	float cosY = cos(DegToRad(rotation.y));

	float sinY = sin(DegToRad(rotation.y));

	right.x = cosY;
	right.z = -sinY;

	return right;
}

CommonUtilities::Vector3f Transform::GetUp()
{
	CommonUtilities::Vector3f up;

	up = GetForward().Cross(GetRight());

	return up;
}

void Transform::HandleSave(rapidjson::Value& outJsonObj, rapidjson::Document& aDocument)
{
	rapidjson::Value pos(rapidjson::kArrayType);
	pos.PushBack(position.x, aDocument.GetAllocator());
	pos.PushBack(position.y, aDocument.GetAllocator());
	pos.PushBack(position.z, aDocument.GetAllocator());

	rapidjson::Value rotat(rapidjson::kArrayType);
	rotat.PushBack(rotation.x, aDocument.GetAllocator());
	rotat.PushBack(rotation.y, aDocument.GetAllocator());
	rotat.PushBack(rotation.z, aDocument.GetAllocator());

	rapidjson::Value sca(rapidjson::kArrayType);
	sca.PushBack(scale.x, aDocument.GetAllocator());
	sca.PushBack(scale.y, aDocument.GetAllocator());
	sca.PushBack(scale.z, aDocument.GetAllocator());

	outJsonObj.AddMember("Position", pos, aDocument.GetAllocator());
	outJsonObj.AddMember("Rotation", rotat, aDocument.GetAllocator());
	outJsonObj.AddMember("Scale", sca, aDocument.GetAllocator());
}

void Transform::HandleLoad(const rapidjson::Value& aObj)
{
	position.x = aObj["Position"][0].GetFloat();
	position.y = aObj["Position"][1].GetFloat();
	position.z = aObj["Position"][2].GetFloat();

	rotation.x = aObj["Rotation"][0].GetFloat();
	rotation.y = aObj["Rotation"][1].GetFloat();
	rotation.z = aObj["Rotation"][2].GetFloat();

	scale.x = aObj["Scale"][0].GetFloat();
	scale.y = aObj["Scale"][1].GetFloat();
	scale.z = aObj["Scale"][2].GetFloat();
}
