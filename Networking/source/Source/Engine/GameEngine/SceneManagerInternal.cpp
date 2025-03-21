#include "../Engine.pch.h"

#include "SceneManagerInternal.h"

#include <rapidjson-master/include/rapidjson/filewritestream.h>
#include <rapidjson-master/include/rapidjson/filereadstream.h>
#include <rapidjson-master/include/rapidjson/document.h>
#include "rapidjson-master/include/rapidjson/prettywriter.h"

#include "../Engine/GameEngine/Components/GameObject.h"

#include "wingdi.h"

#include "../GameEngine/MainSingleton.h"

#include "../GameEngine/Components/GameObject.h"

#include "Console.h"

#include <cstdio>

#include "../GameEngine/Components/MeshRenderer.h"
#include "ModelSprite/ModelFactory.h"

#include "../GameEngine/Components/DirectionalLight.h"

#include "string"
#include <memory>

#include "Components\Transform.h"

#include "../GraphicsEngine/Shader.h"
#include "Components\Animator.h"

#undef GetObject

void SceneManagerInternal::SaveScene(Scene* aSceneToBeSaved, std::string aPath)
{
    rapidjson::Document document;
    document.SetObject();

    rapidjson::Value obj(rapidjson::kObjectType);

    rapidjson::Value sceneNameInJsonFormat(aSceneToBeSaved->name.c_str(), document.GetAllocator());

    document.AddMember("name", sceneNameInJsonFormat, document.GetAllocator());

    for (auto& gameobject : aSceneToBeSaved->gameObjects)
    {
        std::shared_ptr<GameObject> currentGameObject = gameobject.second;

        std::string nName = currentGameObject->GetName();

        rapidjson::Value jsonName(nName.c_str(), document.GetAllocator());

        rapidjson::Value object(rapidjson::kObjectType);

        object.AddMember("name", jsonName, document.GetAllocator());

        rapidjson::Value Components(rapidjson::kObjectType);

        rapidjson::Value transform(rapidjson::kObjectType);

        rapidjson::Value position(rapidjson::kArrayType);
        position.PushBack(currentGameObject->transform->position.x, document.GetAllocator());
        position.PushBack(currentGameObject->transform->position.y, document.GetAllocator());
        position.PushBack(currentGameObject->transform->position.z, document.GetAllocator());

        rapidjson::Value rotation(rapidjson::kArrayType);
        rotation.PushBack(currentGameObject->transform->rotation.x, document.GetAllocator());
        rotation.PushBack(currentGameObject->transform->rotation.y, document.GetAllocator());
        rotation.PushBack(currentGameObject->transform->rotation.z, document.GetAllocator());

        rapidjson::Value scale(rapidjson::kArrayType);
        scale.PushBack(currentGameObject->transform->scale.x, document.GetAllocator());
        scale.PushBack(currentGameObject->transform->scale.y, document.GetAllocator());
        scale.PushBack(currentGameObject->transform->scale.z, document.GetAllocator());

        transform.AddMember("Position", position, document.GetAllocator());
        transform.AddMember("Rotation", rotation, document.GetAllocator());
        transform.AddMember("Scale", scale, document.GetAllocator());

        object.AddMember("Transform", transform, document.GetAllocator());

        MeshRenderer* meshRenderer = currentGameObject->GetComponent<MeshRenderer>();
        
        if (meshRenderer)
        {
            rapidjson::Value meshrenderer(rapidjson::kObjectType);

            std::string meshNameString = currentGameObject->GetComponent<MeshRenderer>()->myMesh->Name;
            rapidjson::Value meshName(meshNameString.c_str(), document.GetAllocator());
            meshrenderer.AddMember("Mesh", meshName, document.GetAllocator());

            std::shared_ptr<Shader>& pxShader = currentGameObject->GetComponent<MeshRenderer>()->GetMaterial().GetPSShader();

            std::wstring pxshaderPath = pxShader->GetShaderPath();
            std::wstring vxshaderPath = currentGameObject->GetComponent<MeshRenderer>()->GetMaterial().GetVXShader()->GetShaderPath();

            std::string pxPath = std::filesystem::path(pxshaderPath).string();
            std::string vxPath = std::filesystem::path(vxshaderPath).string();

            rapidjson::Value PXshader(pxPath.c_str(), document.GetAllocator());
            rapidjson::Value VXshader(vxPath.c_str(), document.GetAllocator());

            meshrenderer.AddMember("Pixel shader", PXshader, document.GetAllocator());
            meshrenderer.AddMember("Vertex shader", VXshader, document.GetAllocator());

            std::string diffusePath = pxShader->GetDiffusePath();
            rapidjson::Value diffuse(diffusePath.c_str(), document.GetAllocator());
            meshrenderer.AddMember("Diffuse texture", diffuse, document.GetAllocator());

            std::string normalPath = pxShader->GetNormalPath();
            rapidjson::Value normal(normalPath.c_str(), document.GetAllocator());
            meshrenderer.AddMember("Normal texture", normal, document.GetAllocator());

            std::string roughnessPath = pxShader->GetRoughnessPath();
            rapidjson::Value roughness(roughnessPath.c_str(), document.GetAllocator());
            meshrenderer.AddMember("Roughness texture", roughness, document.GetAllocator());

            std::string metallicPath = pxShader->GetMetallicPath();
            rapidjson::Value metallic(metallicPath.c_str(), document.GetAllocator());
            meshrenderer.AddMember("Metallic texture", metallic, document.GetAllocator());

            std::string aoPath = pxShader->GetAOPath();
            rapidjson::Value ao(aoPath.c_str(), document.GetAllocator());
            meshrenderer.AddMember("AO texture", ao, document.GetAllocator());

            rapidjson::Value albedo(rapidjson::kArrayType);

            albedo.PushBack(meshRenderer->GetMaterial().GetColor().x, document.GetAllocator());
            albedo.PushBack(meshRenderer->GetMaterial().GetColor().y, document.GetAllocator());
            albedo.PushBack(meshRenderer->GetMaterial().GetColor().z, document.GetAllocator());
            albedo.PushBack(meshRenderer->GetMaterial().GetColor().w, document.GetAllocator());

            meshrenderer.AddMember("Albedo", albedo, document.GetAllocator());

            rapidjson::Value invertedNormalsValue(rapidjson::kTrueType);

            meshrenderer.AddMember("Inverted normals", invertedNormalsValue, document.GetAllocator());

            Components.AddMember("MeshRenderer", meshrenderer, document.GetAllocator());
        }

        Animator* animator = currentGameObject->GetComponent<Animator>();

        if (animator)
        {
            rapidjson::Value Anim(rapidjson::kObjectType);

            std::string animationNameString = currentGameObject->GetComponent<Animator>()->myCurrentAnimation->Name;

            rapidjson::Value animationName(animationNameString.c_str(), document.GetAllocator());

            Anim.AddMember("animation", animationName, document.GetAllocator());

            Components.AddMember("Animator", Anim, document.GetAllocator());
        }

        DirectionalLight* dirLightC = currentGameObject->GetComponent<DirectionalLight>();

        if (dirLightC)
        {
            rapidjson::Value dirLight(rapidjson::kObjectType);

            rapidjson::Value dirLightIntensity(rapidjson::kNumberType);
            float lightIntensity = currentGameObject->GetComponent<DirectionalLight>()->myIntensity;

            dirLightIntensity.SetFloat(lightIntensity);

            CommonUtilities::Vector4<float> lightColor = currentGameObject->GetComponent<DirectionalLight>()->myLightColor;

            rapidjson::Value dirLightColor(rapidjson::kArrayType);
            dirLightColor.PushBack(lightColor.x, document.GetAllocator());
            dirLightColor.PushBack(lightColor.y, document.GetAllocator());
            dirLightColor.PushBack(lightColor.z, document.GetAllocator());
            dirLightColor.PushBack(lightColor.w, document.GetAllocator());

            dirLight.AddMember("light intensity", dirLightIntensity, document.GetAllocator());
            dirLight.AddMember("light color", dirLightColor, document.GetAllocator());

            Components.AddMember("Directional Light", dirLight, document.GetAllocator());

        }


        object.AddMember("Components", Components, document.GetAllocator());


        obj.AddMember("Object", object, document.GetAllocator());
    }

    document.AddMember("Gameobjects", obj, document.GetAllocator());

    char writeBuffer[65536];
    FILE* fp;

    errno_t err;

    std::string name = aPath.substr(aPath.find_last_of(".") + 1);

    std::string test = aPath + ".no";

    if (name == "no")
    {
        err = fopen_s(&fp, (aPath).c_str(), "w");
    }
    else
    {
        err = fopen_s(&fp, (aPath + ".no").c_str(), "w");
    }

    
    if (err != 0) 
    {
        PrintError("could not save file to: " + aPath + aSceneToBeSaved->name + ".no");

        return;
    }

    rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));

    rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

    document.Accept(writer);

    fclose(fp);
}

Scene* SceneManagerInternal::LoadScene(std::string aScenePath)
{
    MainSingleton::Get()->myComponentManager->myComponents.clear();

    Scene* newScene = new Scene();

    FILE* fp;
    char readBuffer[65536];

    // Open the file containing the saved scene
    errno_t err = fopen_s(&fp, aScenePath.c_str(), "r");
    if (err != 0) 
    {
        PrintError("could not find file to: " + aScenePath);
        throw std::runtime_error("Failed to open the scene file.");
    }

    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Document document;
    document.ParseStream(is);

    fclose(fp);

    if (!document.IsObject()) {
        throw std::runtime_error("Invalid JSON format: root should be an object.");
    }

    if (document.HasMember("name")) 
    {
        newScene->name = document["name"].GetString();
    }

    const rapidjson::Value& objects = document["Gameobjects"];
    if (!objects.IsObject()) 
    {
        throw std::runtime_error("Invalid JSON format: missing 'Gameobjects' object.");
    }

    for (auto& m : objects.GetObject()) 
    {
        const rapidjson::Value& object = m.value;

        std::string objectName = object["name"].GetString();

        std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(objectName);

        const rapidjson::Value& transform = object["Transform"];

        Vector3 position
        (
            transform["Position"][0].GetFloat(),
            transform["Position"][1].GetFloat(),
            transform["Position"][2].GetFloat()
        );
        Vector3 rotation
        (
            transform["Rotation"][0].GetFloat(),
            transform["Rotation"][1].GetFloat(),
            transform["Rotation"][2].GetFloat()
        );
        Vector3 scale
        (
            transform["Scale"][0].GetFloat(),
            transform["Scale"][1].GetFloat(),
            transform["Scale"][2].GetFloat()
        );

        gameObject->transform->position = position;
        gameObject->transform->rotation = rotation;
        gameObject->transform->scale = scale;

        if (object["Components"].HasMember("MeshRenderer"))
        {
            MeshRenderer* meshRenderer = gameObject->AddComponent<MeshRenderer>();

            std::string meshName = object["Components"]["MeshRenderer"]["Mesh"].GetString();

            if (!meshName.empty())
                meshRenderer->myMesh = CreateModel(meshName);
            else
                meshRenderer->myMesh = nullptr;

            meshRenderer->myDeffered = true;

            std::wstring pxShaderPath = std::filesystem::path(object["Components"]["MeshRenderer"]["Pixel shader"].GetString()).wstring();
            meshRenderer->GetMaterial().SetPixelShader(pxShaderPath);

            std::wstring vxShader = std::filesystem::path(object["Components"]["MeshRenderer"]["Vertex shader"].GetString()).wstring();
            meshRenderer->GetMaterial().SetVertexShader(vxShader);

            std::wstring diffusePath = std::filesystem::path(object["Components"]["MeshRenderer"]["Diffuse texture"].GetString()).wstring();
            meshRenderer->GetMaterial().SetDiffuseTexture(diffusePath);

            std::wstring normalPath = std::filesystem::path(object["Components"]["MeshRenderer"]["Normal texture"].GetString()).wstring();
            meshRenderer->GetMaterial().SetNormalTexture(normalPath);

            std::wstring roughnessPath = std::filesystem::path(object["Components"]["MeshRenderer"]["Roughness texture"].GetString()).wstring();
            meshRenderer->GetMaterial().SetRoughnessTexture(roughnessPath);

            std::wstring metallicPath = std::filesystem::path(object["Components"]["MeshRenderer"]["Metallic texture"].GetString()).wstring();
            meshRenderer->GetMaterial().SetMetallicTexture(metallicPath);

            std::wstring aoPath = std::filesystem::path(object["Components"]["MeshRenderer"]["AO texture"].GetString()).wstring();
            meshRenderer->GetMaterial().SetAOTexture(aoPath);

            Vector4<float> color{
                object["Components"]["MeshRenderer"]["Albedo"][0].GetFloat(),
                object["Components"]["MeshRenderer"]["Albedo"][1].GetFloat(),
                object["Components"]["MeshRenderer"]["Albedo"][2].GetFloat(),
                object["Components"]["MeshRenderer"]["Albedo"][3].GetFloat()
            };

            meshRenderer->GetMaterial().SetColor(color);
        }

        if (object["Components"].HasMember("Animator"))
        {
            Animator* animator = gameObject->AddComponent<Animator>();

            std::string animationPath = object["Components"]["MeshRenderer"]["Mesh"].GetString();

            if (!animationPath.empty())
                animator->SetAnimation(animationPath);

            animator->SetAnimation(animationPath);
        }

        if (object["Components"].HasMember("Directional Light"))
        {
            DirectionalLight* dirLight = gameObject->AddComponent<DirectionalLight>();

            Vector4<float> color{
                object["Components"]["Directional Light"]["light color"][0].GetFloat(),
                object["Components"]["Directional Light"]["light color"][1].GetFloat(),
                object["Components"]["Directional Light"]["light color"][2].GetFloat(),
                object["Components"]["Directional Light"]["light color"][3].GetFloat()
            };

            dirLight->myLightColor = color;
            dirLight->myIntensity = object["Components"]["Directional Light"]["light intensity"].GetFloat();
        }

        newScene->gameObjects[objectName] = gameObject;
    }

    return newScene;
}

Scene::Scene(std::string aName)
{
    name = aName;
    PhysicsEngine::Get().NewScene(myPhysicsScene);
}

Scene::Scene(Scene& aScene)
{
    for (auto& gameobject : aScene.gameObjects)
    {
        gameObjects.emplace(gameobject.first, gameobject.second);
    }

    name = aScene.name;
}
