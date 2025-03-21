#pragma once
#include <utility>

#include "Component.h"
#include "../Engine/GraphicsEngine/Buffers/AnimationBuffer.h"

#include <../ImGui/ImGui/ImVector.h>

#include <../ImGuizmo\ImCurveEdit.h>
#include <../ImGuizmo\ImSequencer.h>

#include <vector>

class MeshRenderer;
class RenderMeshCommand;

enum class AnimationState
{
	NotStarted = 1,
	Finished = 2,
	Playing = 4,
	Paused = 8
};

static const char* SequencerItemTypeNames[] = { "Animation" };

struct RampEdit : public ImCurveEdit::Delegate
{
    RampEdit(): mPointCount{}, mbVisible{}
    {
	    //mPointCount[0]++;
	    //mbVisible[0] = true;

	    //mPts[0].push_back(ImVec2(0, 0));
    }

    size_t GetCurveCount() override
    {
        return 1;
    }

    bool IsVisible(size_t curveIndex) override
    {
        return mbVisible[curveIndex];
    }
    size_t GetPointCount(size_t curveIndex) override
    {
        return mPointCount[curveIndex];
    }

    uint32_t GetCurveColor(size_t curveIndex) override
    {
        uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
        return cols[curveIndex];
    }
    ImVec2* GetPoints(size_t curveIndex) override
    {
        return mPts[curveIndex].data();
    }

    ImVec2& GetPoint(size_t aIndex)
    {
        return mPts[0][aIndex];
    }

    void BeginEdit(int /*index*/) override
    {
        std::cout << "test";
    }

    ImCurveEdit::CurveType GetCurveType(size_t /*curveIndex*/) const override { return ImCurveEdit::CurveSmooth; }

    int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
    {
        mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
        SortValues(curveIndex);
        for (size_t i = 0; i < GetPointCount(curveIndex); i++)
        {
            if (mPts[curveIndex][i].x == value.x)
                return (int)i;
        }
        return pointIndex;
    }

    void AddPoint(size_t curveIndex, ImVec2 value) override
    {
        mPointCount[0]++;
        mbVisible[0] = true;

        mPts[curveIndex].push_back(value);
        SortValues(curveIndex);
    }

    ImVec2& GetMax() override { return mMax; }
    ImVec2& GetMin() override { return mMin; }
    unsigned int GetBackgroundColor() override { return 0; }
    std::vector<ImVec2> mPts[1];
    size_t mPointCount[1];
    bool mbVisible[1];
    ImVec2 mMin;
    ImVec2 mMax;
private:
    void SortValues(size_t curveIndex)
    {
        auto b = std::begin(mPts[curveIndex]);
        auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
        std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

    }
};

struct MySequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    int GetFrameMin() const override
    {
        return mFrameMin;
    }

    int GetFrameMax() const override
    {
        return mFrameMax;
    }

    int GetItemCount() const override { return (int)myItems.size(); }

    int GetItemTypeCount() const override { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
    const char* GetItemTypeName(int typeIndex) const override { return SequencerItemTypeNames[typeIndex]; }

    const char* GetItemLabel(int index) const override
    {
        static char tmps[512];
        snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[myItems[index].mType]);
        return tmps;
    }

    void Get(int index, int** start, int** end, int* type, unsigned int* color) override
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }

    void Add(int type) override { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
    void Del(int index) override { myItems.erase(myItems.begin() + index); }
    void Duplicate(int index) override { myItems.push_back(myItems[index]); }

    size_t GetCustomHeight(int index) override { return myItems[index].mExpanded ? 300 : 0; }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(0) {}
    int mFrameMin, mFrameMax;
    struct MySequenceItem
    {
        int mType;
        int mFrameStart, mFrameEnd;
        bool mExpanded;
    };
    std::vector<MySequenceItem> myItems;
    RampEdit rampEdit;

    RampEdit& GetRampEdit()
    {
        return rampEdit;
    }

    void DoubleClick(int index) override
    {
        if (myItems[index].mExpanded)
        {
            myItems[index].mExpanded = false;
            return;
        }
        for (auto& item : myItems)
            item.mExpanded = false;
        myItems[index].mExpanded = !myItems[index].mExpanded;
    }

    void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect) override;

    void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect) override;
};

class Animator : public Component
{
    struct LayeredAnimationData
    {
        std::string myBoneToAffect;
        TGA::FBX::Animation* myAnimation;

        float myAnimTime = 0;
        float myFrameTime = 0;
        unsigned myCurrentFrame = 0;
    };

public:
	friend class MeshRenderer;
	friend class RenderMeshCommand;

    Animator()
    {
        Component();

        myCurrentAnimationstate = AnimationState::NotStarted;
        mySequence.mFrameMin = 0;
        mySequence.mFrameMax = 1000;

        mySequence.myItems.push_back(MySequence::MySequenceItem{ 0, mySequence.mFrameMin, mySequence.mFrameMax, true });
    }

	~Animator();

	void Stop() override;

	void Update(float aDeltaTime) override;
    void AddLayeredAnimation(const std::string& boneName);

    void AddedAsComponent() override;

    void RenderImGUI() override;

    void UpdateImGuizmoMatrix();
    void RenderImGUIZMO();

    void UpdateAnimator(size_t aCurrentFrame1, size_t aCurrentFrame2, unsigned aJointID, const TGA::FBX::Matrix& aParentJointTransform, float alpha, bool noTransformation = false);

	void UpdateAnimatorLayered(size_t aCurrentFrame1, size_t aCurrentFrame2, unsigned aJointID, const TGA::FBX::Matrix& aParentJointTransform, int aIndex, const float& alpha);

	int GetBoneIndexFromString(std::string& aBoneName);

	void UpdateEditor(float aDeltaTime) override;

	void SetAnimation(const std::string& animationPath, TGA::FBX::Mesh* aMesh = nullptr);
	void SetLayeredAnimation(const std::string& aAnimationPath, int aIndex);
	void SetSecondAnimation(const std::string& aAnimationPath);

	void SetMesh(TGA::FBX::Mesh* aMesh);

	void PlayAnimation() { myCurrentAnimationstate = AnimationState::Paused; }
	void PauseAnimation() { myCurrentAnimationstate = AnimationState::Paused; }

	void StopAnimation() { myCurrentAnimationstate = AnimationState::Finished; }

	void Start();
    void OpenBoneChildren(int aIndex, int aCount);

    TGA::FBX::Animation* myCurrentAnimation;

	bool forward = true;

    void HandleSave(rapidjson::Value& outJson, rapidjson::Document& document) override;
    void HandleLoad(const rapidjson::Value& outJson) override;

    //MyGraphDelegate myanimationGraph;

    //GraphEditor::Options options;  // Use default options
    //GraphEditor::ViewState viewState;
    //GraphEditor::FitOnScreen fit = GraphEditor::Fit_AllNodes;

private:
    CommonUtilities::Vector4<float> position;
    CommonUtilities::Vector4<float> rotation;
    CommonUtilities::Vector4<float> scale;

    float objectMatrix[16] =
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    ImVec2 currentEdit = ImVec2(0, 0);

    // let's create the sequencer
    int selectedEntry = -1;
    int selectedNode = -1;
    int firstFrame = 0;
    bool expanded = true;
    int currentFrame = 0;

    // Im guizmo ankmation blend editor
    MySequence mySequence;

    bool renderBones;

	float t = 0.f;
	bool switchAniamtion = false;
	std::string secondAnimationPath;

	AnimationState myCurrentAnimationstate;

    std::vector<LayeredAnimationData> myAnimationLayers;

    TGA::FBX::Animation* mySecondAnimation;
	unsigned mySecondAnimCurrentFrame;
	float mySecondAnimTime;
	float mySecondAnimFrameTime;


    TGA::FBX::Animation* myAnimation;
	unsigned myCurrentFrame;
	float myAnimTime;
	float myCurrentFrameTime;

    char bName[256] = "";

	AnimationBuffer myAnimationBuffer;

	TGA::FBX::Skeleton mySkeleton;

	TGA::FBX::Mesh* myCurrentMesh;

	int myCurrentAnimationNumber = 0;

	std::string animationPath;

	bool myIsStopped = false;

    std::string boneToAttachTo;

   std::vector<std::pair<std::shared_ptr<GameObject>, MeshRenderer*>> myRenderBones;
   int selectedBone;
};

