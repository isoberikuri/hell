
#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include <vector>

using namespace KamataEngine::MathUtility;

class Player;
class CameraController
{
//private:
public:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// kamera
	KamataEngine::Camera cameraController_;
	KamataEngine::Camera camera_;
	Player* target_ = nullptr;

	void SetTarget(Player* target) { target_ = target; }
	KamataEngine::Vector3 targetOffset_ = {0, 0, -15.0f};
	const KamataEngine::Camera& GetViewProjection() const { return camera_; }
	struct Rect
	{
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};
	Rect movableArea_ = {0, 100, 0, 100};
	void SetMovableArea(Rect area) { movableArea_ = area; }

//public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	void Reset();

	// 描画
	//void Draw();
};