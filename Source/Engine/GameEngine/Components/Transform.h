#pragma once

#include "Component.h"
#include <Matrix/Matrix4x4.hpp>

#include <Vector/Vector4.hpp>

class Transform : public Component
{
public:
    Transform() : myPosition({ 0.f, 0.f, 0.f, 1.f }), myRotation({ 0.f, 0.f, 0.f, 1.f }), myScale({ 1.f, 1.f, 1.f }), myIsDirty(true)
    {
        Component();
    }

    ~Transform() = default;

    friend class Editor;
    friend class NetworkTransform;

    void Start() override;
    void Update(float aDeltaTime) override;

    const CommonUtilities::Matrix4x4<float>& GetMatrix();
    const CommonUtilities::Matrix4x4<float>& GetMatrixNoScale();

    void SetPosition(const CU::Vector3<float>& aPosition);
    void SetRotation(const CU::Vector3<float>& aRotation);
    void SetScale(const CU::Vector3<float>& aScale);

    void SetPosition(const CU::Vector4<float>& aPosition);
    void SetRotation(const CU::Vector4<float>& aRotation);
    void SetScale(const CU::Vector4<float>& aScale);

    const CU::Vector4<float>& GetPosition();
    const CU::Vector4<float>& GetRotation();
    const CU::Vector4<float>& GetScale();

    bool GetIsDirty();
    void SetIsDirty();
    void RemoveIsDirty();

private:
    CU::Vector4<float> myPosition;
    CU::Vector4<float> myRotation;
    CU::Vector4<float> myScale;

    CommonUtilities::Matrix4x4<float> myObjectMatrix;
    CommonUtilities::Matrix4x4<float> myObjectNoScaleMatrix;

    bool myIsDirty;
};