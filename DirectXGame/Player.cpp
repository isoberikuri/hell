#define NOMINMAX
#include "Player.h"
#include "MyMath.h"
#include <numbers>
#include <vector>
#include <algorithm>



void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	//NULLポインタチェック
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	camera_ = camera;

}

void Player::Update()
{
	using namespace KamataEngine::MathUtility;
	//行列を定数バッファに転送
	worldTransform_.matWorld_ = 
	MakeAffineMatrix
	(
	worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_
	);
	worldTransform_.TransferMatrix();
	//移動入力
	bool landing = false;
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}
	if (onGround_)
	{
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT) || KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT))
		{
			KamataEngine::Vector3 acceleration = {};
			if (KamataEngine::Input::GetInstance()->PushKey(DIK_RIGHT))
			{
				acceleration.x += kAcceleration;
				if (velocity_.x < 0.0f)
				{
					velocity_.x *= (1.0f - kAcceleration);
				}
				if (lrDirection_ != LRDirection::kRight)
				{
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_LEFT))
			{
				acceleration.x -= kAcceleration;
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAcceleration);
				}
				if (lrDirection_ != LRDirection::kLeft)
				{
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_ += acceleration;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
		else
		{
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (turnTimer_ > 1.0f / 60.0f)
		{
			--turnTimer_;
			float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
		}
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP))
		{
			velocity_ += KamataEngine::Vector3(0, kJumpAcceleration, 0);
		}
		if (velocity_.y > 0.0f)
		{
			onGround_ = false;
		}
	}
	else
	{
		velocity_ += KamataEngine::Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
	//移動
	worldTransform_.translation_ += velocity_;
	//行列更新

	
}

void Player::Draw() 
{ 
	model_->Draw(worldTransform_, *camera_);
}
