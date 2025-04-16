#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize()
{ 
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("IMG_0564.JPG");
	//3Dモデルの生成
	model_ = Model::Create();
	//ワールドトランスフォームの初期化
	worldTrensform_.Initialize();
	//カメラの初期化
	camera_.Initialize();
}

void GameScene::Update()
{

}

void GameScene::Draw() 
{
	//DirectXCommonインストラクタの習得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	//3Dモデル描画
	model_->Draw(worldTrensform_, camera_, textureHandle_);
	//3Dモデル描画後処理
	Model::PostDraw();
}

GameScene::~GameScene()
{
	delete model_;
}
