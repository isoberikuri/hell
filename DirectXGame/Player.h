#pragma once
#include "KamataEngine.h"
#include <vector>

class Player
{
//private:
public:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	KamataEngine::Camera* camera_ = nullptr;

	//KamataEngine::Vector3& position_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 0.5f;
	enum class LRDirection
	{
		kRight,
		kLeft,
	};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.9f;

	bool onGround_ = true;
	static inline const float kGravityAcceleration = 0.5f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 2.0f;

	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	

public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

};
