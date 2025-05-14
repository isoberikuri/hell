#include "Player.h"

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) 
{ 
	//NULLポインタチェック
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
	camera_ = camera;
}

void Player::Update() 
{ 
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix(); 
}

void Player::Draw() 
{ 
	model_->Draw(worldTransform_, *camera_);
}
