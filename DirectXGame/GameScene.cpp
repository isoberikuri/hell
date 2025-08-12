#include "GameScene.h"
#include "MyMath.h"

using namespace KamataEngine;

void GameScene::Initialize()
{ 
	// ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("IMG_0564.JPG");
	//3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("player");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("SkyDome",true);
	modelEnemy_ = Model::CreateFromOBJ("Enemy");
	//ワールドトランスフォームの初期化
	worldTrensform_.Initialize();
	//カメラの初期化
	camera_.Initialize();
	//自キャラの生成
	player_ = new Player();
	skydome_ = new Skydome();
	mapChipField_ = new MapChipField;
	//自キャラの初期化
	//player_->Initialize(modelPlayer_, &camera_);
	skydome_->Initialize(modelSkydome_,& camera_);
	////要素数
	//const uint32_t kNumBlockVirtical = 10;
	//const uint32_t kNumBlockHorizontal = 20;
	////ブロック1個分の横幅
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;
	////要素数を変更する
	//worldTransformBlocks_.resize(kNumBlockVirtical);
	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i)
	//{
	//	worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	//}
	////キューブの生成
	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i)
	//{
	//	for (uint32_t j = 0; j < kNumBlockHorizontal; ++j)
	//	{
	//		if ((j + i) % 2 == 0)
	//			continue;

	//		worldTransformBlocks_[i][j] = new WorldTransform();
	//		worldTransformBlocks_[i][j]->Initialize();
	//		worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
	//		worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
	//	}
	//}
	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBilocks();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	//カメラコントロール
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	//範囲指定
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	//自キャラの生成と初期化
	player_->SetMapChipField(mapChipField_);
	// 敵
	enemy_ = new Enemy();
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(40, 18);
	enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);


}

void GameScene::Update()
{ 
	//自キャラの更新
	player_->Update(); 
	skydome_->Update();
	cameraController_->Update();
	enemy_->Update();
	//ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->scale_;
			worldTransformBlock->rotation_;
			worldTransformBlock->translation_;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}

	// デバックカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG

	// カメラの処理
	if (isDebugCameraActive_)
	{
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else
	{
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の更新と転送
		camera_.TransferMatrix();
	}

}

void GameScene::Draw() 
{
	//DirectXCommonインストラクタの習得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	// 自キャラの描画
	player_->Draw();
	enemy_->Draw();
	skydome_->Draw();
	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}
	//3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::GenerateBilocks()
{ 
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}
	for (uint32_t i = 0; i < numBlockVirtical; ++i)
	{
		for (uint32_t j = 0; j < numBlockHorizontal; ++j)
		{
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock)
			{
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j,i);
			}
		}
	}
}

GameScene::~GameScene()
{
	//自キャラの解放
	delete player_;
	delete modelPlayer_;
	delete modelBlock_;
	delete modelSkydome_;
	delete mapChipField_;
	delete enemy_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}
