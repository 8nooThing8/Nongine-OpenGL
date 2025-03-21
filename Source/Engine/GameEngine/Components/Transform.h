#pragma once

#include "Component.h"
#include <Matrix/Matrix.hpp>

#include <Vector/Vector.hpp>
#include <../Engine/GraphicsEngine/GraphicsEngine.h>

#include "../Engine/GraphicsEngine/Buffers/ObjectBuffer.h"

using namespace CommonUtilities;

class Transform : public Component
{
public:
    Transform() : scale({ 1.f, 1.f, 1.f }), position({ 0.f, 0.f, 0.f, 1.f }), rotation({ 0.f, 0.f, 0.f, 1.f })
    {
        Component();

    }

    void Start() override;

    void Update(float aDeltaTime) override;

    CommonUtilities::Matrix4x4<float> GetFinalTransform();
    CommonUtilities::Matrix4x4<float> GetFinalTransformNoScale();

    void UpdateImGuizmoMatrix();
    void RenderImGUIZMO();


    void RenderImGUI() override;

    CU::Matrix4x4<float> GetObjectBufferGlobalMat();
    ObjectBuffer& GetObjBuffer();

    CommonUtilities::Vector3f GetPosition();
    CommonUtilities::Vector3f GetLocalPosition();

    CommonUtilities::Vector3f GetRotation();
    CommonUtilities::Vector3f GetLocalRotation();

    CommonUtilities::Vector3f GetScale();
    CommonUtilities::Vector3f GetLocalScale();

    void SetUniform(bool aUniform);

    CommonUtilities::Vector3f GetForward();
    CommonUtilities::Vector3f GetRight();
    CommonUtilities::Vector3f GetUp();

    void HandleSave(rapidjson::Value& outJsonObj, rapidjson::Document& aDocument)override;
    void HandleLoad(const rapidjson::Value& aObj)override;

private:

    ObjectBuffer myObjBuffer;

    bool uniform;

    float objectMatrix[16] = 
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f 
    };

public:
    Vector4<float> position;
    Vector4<float> rotation;
    Vector4<float> scale;
};