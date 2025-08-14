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
	modelDeathparticles_ = Model::CreateFromOBJ("deathParticle");
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
	//enemy_ = new Enemy();
	//Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(40, 18);
	//enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);
	for (int32_t i = 0; i < 5; i++) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(6 + i, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}
	// 仮の生成処理。後で消す
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelDeathparticles_, &camera_, playerPosition);
	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;
}

void GameScene::Update()
{
	ChangePhase();

	switch (phase_)
	{
	case Phase::kPlay:
		// 自キャラの更新
		player_->Update();
		skydome_->Update();
		cameraController_->Update();
		for (Enemy* enemy : enemies_)
		{
			enemy->Update();
		}
		// ブロックの更新
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
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))
		{
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
		} else {
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の更新と転送
			camera_.TransferMatrix();
		}

		// 全ての当たり判定を行う
		CheckAllCollisions();
		// モデルパーティクル
		if (deathParticles_)
		{
			deathParticles_->Update();
		}
		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_ = true;
		}
		// デス演出フェーズの処理////////////////////////////////////
		//  //天球の更新
		skydome_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// カメラの更新////////////////////
		debugCamera_->Update();
#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_0)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバックカメラの更新
			debugCamera_->Update();
			// デバックカメラのビュー行列
			camera_.matView = debugCamera_->GetCamera().matView;
			// デバックカメラのプロジェクション行列
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			camera_.matView = cameraController_->GetViewProjection().matView;
			camera_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		}
		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock) {
					continue;
				}
				// アフィン変換行列の作成
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

				/*	worldTransformBlock->matWorld_ = アフィン変換行列　*/

				// 定数バッファに転送する
				worldTransformBlock->TransferMatrix();
			}
		}
		break;
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
	for (Enemy* enemy : enemies_)
	{
		enemy->Draw();
	}
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
	// モデルパーティクル
	if (deathParticles_) {
		deathParticles_->Draw();
	}
	//3Dモデル描画後処理
	Model::PostDraw();
}

void GameScene::CheckAllCollisions() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象１と２の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_)
	{
		// 敵弾の座標
		aabb2 = enemy->GetAABB();
		// AABB同士の交差判定
		if (IsCollision(aabb1, aabb2)) {
			// 自キャラの衝突時間関数を呼び出す
			player_->OnCollision(enemy);
			// 敵弾の衝突時コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}

#pragma endregion
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
void GameScene::ChangePhase() { ///////////////////////////////////////////////////////////
	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathparticles_, &camera_, deathParticlesPosition);
		}

		break;

	case Phase::kDeath:
		// デス演出フェーズの処理

		break;
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
	delete deathParticles_;
	for (Enemy* enemy : enemies_)
	{
		delete enemy;
	}
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}
