#pragma once

#include "Component.h"
#include "../../GraphicsEngine/Material/Material.h"

struct Sprite;
struct ObjectBuffer;

class SpriteRenderer : public Component
{
public:
    SpriteRenderer(Sprite* aSprite, Material aMaterial);
    SpriteRenderer(const SpriteRenderer& aSprite);
    SpriteRenderer();
    ~SpriteRenderer();

    void SetCastShadow(bool aValue);
    void LateUpdateEditor(float aDeltaTime) override;
    void Draw();
    void Destroy();
    void RenderImGUI() override;

    void HandleSave(rapidjson::Value& /*aOutJson*/, rapidjson::Document& /*aDocument*/) override;

    void HandleLoad(const rapidjson::Value& /*aObj*/) override;

    Sprite*& GetSprite();
    Material& GetMaterial();

private:
    Material myMaterial;
    Sprite* mySprite = nullptr;
    bool myCastShadow;

    char PXshaderPath[256] = "";
    char VXshaderPath[256] = "";
};
