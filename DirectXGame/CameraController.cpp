#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize()
{
	camera_.Initialize();
}

void CameraController::Update()
{ 
	const KamataEngine::Vector3& targetVelocity = target_->GetVelocity();

	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;
	//追従対象が画面外に出ないように補正
	camera_.translation_.x = max(camera_.translation_.x, camera_.translation_.x + targetMargin.left);
	camera_.translation_.x = min(camera_.translation_.x, camera_.translation_.x + targetMargin.right);
	camera_.translation_.x = max(camera_.translation_.y, camera_.translation_.y + targetMargin.bottom);
	camera_.translation_.x = min(camera_.translation_.y, camera_.translation_.y + targetMargin.top);

	//範囲制限
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);
	//座標補問
	camera_.translation_.x = Lerp(camera_.translation_.x, targetPosition_.x, kInterpolationRate);
	camera_.translation_.y = Lerp(camera_.translation_.y, targetPosition_.y, kInterpolationRate);

	camera_.UpdateMatrix();
}

void CameraController::Reset()
{ 
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
