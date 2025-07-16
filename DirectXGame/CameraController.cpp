#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize()
{
	cameraController_.Initialize();
}

void CameraController::Update()
{ 
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	cameraController_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	//範囲制限
	camera_.translation_.x = max(camera_.translation_.x, movableArea_.left);
	camera_.translation_.x = min(camera_.translation_.x, movableArea_.right);
	camera_.translation_.y = max(camera_.translation_.y, movableArea_.bottom);
	camera_.translation_.y = min(camera_.translation_.y, movableArea_.top);

	cameraController_.UpdateMatrix();
}

void CameraController::Reset()
{ 
	const KamataEngine::WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	cameraController_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}
