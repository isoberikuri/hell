
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
	//KamataEngine::Camera cameraController_;
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
	//カメラの目標座標
	KamataEngine::Vector3 targetPosition_;
	//座標補問割合
	static inline const float kInterpolationRate = 1.0f;
	//速度掛け率
	static inline const float kVelocityBias = 30;
	//追従対象カメラ移動範囲
	static inline const Rect targetMargin = {-9.0f, 9.0f, -5.0f, 5.0f};

//public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	void Reset();

	// 描画
	//void Draw();
};