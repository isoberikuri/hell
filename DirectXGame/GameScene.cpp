#include "GameScene.h"
#include "MyMath.h"
#include "Bullet.h"
#include <cstdlib>
#include <ctime>

using namespace KamataEngine;

void GameScene::Initialize()
{ 
	// ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("IMG_0564.JPG");
	//3Dモデルの生成
	modelPlayer_ = Model::CreateFromOBJ("bullet4");
	modelBlock_ = Model::CreateFromOBJ("newBlocks");
	modelSkydome_ = Model::CreateFromOBJ("SkyDome",true);
	modelEnemy_ = Model::CreateFromOBJ("bullet5", true);
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
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(100 + i, 18);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}
	// ゲームプレイフェーズから開始
	phase_ = Phase::kPlay;
	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// 弾オブジェクトを先に作成する
	bullet_ = new Bullet();
	modelBullet_ = Model::CreateFromOBJ("bullet1", true);

	// ===== 弾生成 =====
	Vector3 bulletPos = {0, 0, 0};
	bullet_->Initialize(modelBullet_, &camera_, bulletPos);

	// セレクトカード画像読み込み
	bulletSelect1Handle_ = TextureManager::Load("BulletCard/NormalBulletCard.png");
	bulletSelect1Sprite_ = Sprite::Create(bulletSelect1Handle_, {bulletSelect1PosX_, bulletSelect1PosY_});
	bulletSelect2Handle_ = TextureManager::Load("BulletCard/NormalBulletCard.png");
	bulletSelect2Sprite_ = Sprite::Create(bulletSelect2Handle_, {bulletSelect2PosX_, bulletSelect2PosY_});
	std::srand((unsigned int)std::time(nullptr));

	// ★ ここで必ず SetPlayer する！
	bullet_->SetPlayer(player_);

	//====================//
	//  ゲームクリア用   //
	//====================//
	gameClearTexture_ = TextureManager::Load("GameClear.png");
	gameClearSprite_ = Sprite::Create(gameClearTexture_, {640.0f, 360.0f});
	gameClearSprite_->SetAnchorPoint({0.5f, 0.5f});

	//====================//
	//        BGM         //
	//====================//
	audio_ = KamataEngine::Audio::GetInstance();

	// BGM 読み込み（Resources/audio フォルダ想定）
	bgmHandle_ = audio_->LoadWave("BGM00.wav");

	// ループ再生（true が重要）
	audio_->PlayWave(bgmHandle_, true, 0.25f);

}

//=========================//
//     カード画像変更      //
//=========================//

void GameScene::SetCardTexture(Sprite* sprite, int cardNo)
{ 
	sprite->SetTextureHandle(TextureManager::Load(GetCardPath(cardNo)));
}
//=========================//
//       モデル切替        //
//=========================//

void GameScene::ChangeBulletModel(int type)
{
	delete modelBullet_;
	switch (type) {
	case 1:
		modelBullet_ = Model::CreateFromOBJ("bullet1", true);
		break;
	case 2:
		modelBullet_ = Model::CreateFromOBJ("bullet2", true);
		break;
	case 3:
		modelBullet_ = Model::CreateFromOBJ("bullet3", true);
		break;
	case 4:
		modelBullet_ = Model::CreateFromOBJ("bullet4", true);
		break;
	case 5:
		modelBullet_ = Model::CreateFromOBJ("bullet5", true);
		break;
	}
	bullet_->SetModel(modelBullet_);
}
std::string GameScene::GetCardPath(int type) {
	switch (type) {
	case 1:
		return "BulletCard/NormalBulletCard.png";
	case 2:
		return "BulletCard/ReflectBulletCard.png";
	case 3:
		return "BulletCard/OperationBulletCard.png";
	case 4:
		return "BulletCard/SpeedChangeBulletCard.png";
	case 5:
		return "BulletCard/TridentBulletCard.png";
	}
	return "BulletCard/NormalBulletCard.png";
}

void GameScene::Update()
{

	// フェード
	fade_->Update();
	ChangePhase();

	switch (phase_)
	{
	case Phase::kPlay:
		// ゲームプレイフェーズの処理////////////////////////////////////
		
		//====================//
		//  ゲームクリア判定 //
		//====================//
		gameClearTimer_--;
		if (gameClearTimer_ <= 0)
		{
			phase_ = Phase::kGameClear;
		}

		//=====================//
		//    弾・モデル切替   //
		//=====================//
		currentType = bullet_->GetCurrentBulletType();

		if (currentType != prevBulletType_)
		{
			ChangeBulletModel(currentType);
		}
		prevBulletType_ = currentType;

		//=======================//
		//   カード選択フェーズ  //
		//=======================//

		if (bulletSelectPoint_ == 1)
		{
			if (Input::GetInstance()->TriggerKey(DIK_A))
				SelectSet_ = 1;

			if (Input::GetInstance()->TriggerKey(DIK_D))
				SelectSet_ = 2;

			// ★ どちらを選択しているかによって拡大率変更
			if (SelectSet_ == 1)
			{
				bulletSelect1Sprite_->SetSize({336 * 1.2f, 528 * 1.2f});
				bulletSelect2Sprite_->SetSize({336, 528});
			} else {
				bulletSelect1Sprite_->SetSize({336, 528});
				bulletSelect2Sprite_->SetSize({336 * 1.2f, 528 * 1.2f});
			}

			// SPACE 決定
			if (Input::GetInstance()->TriggerKey(DIK_RETURN))
			{
				int chosenCard = (SelectSet_ == 1) ? Select1Point_ : Select2Point_;

				bullet_->SetBulletType(chosenCard);
				ChangeBulletModel(chosenCard);

				bulletSelectPoint_ = 0;
				bulletSelectTime_ = 300;
			}
		}
		
		// 天球の更新
		skydome_->Update();
		//===========================//
		//      プレイフェーズ       //
		//===========================//
		if (bulletSelectPoint_ == 0)
		{
			bulletSelectTime_--;

			if (bulletSelectTime_ <= 0)
			{
				bulletSelectPoint_ = 1;
				bullet_->ResetBulletState();

				// ランダムカード生成
				Select1Point_ = (std::rand() % 5) + 1;
				Select2Point_ = (std::rand() % 5) + 1;

				// カード画像切替
				SetCardTexture(bulletSelect1Sprite_, Select1Point_);
				SetCardTexture(bulletSelect2Sprite_, Select2Point_);

				SelectSet_ = 1;
			}
			//==========//
			//	その他  //
			//==========//
			// 自キャラの更新
			player_->Update();
			// 敵の更新
			for (Enemy* enemy : enemies_)
			{
				enemy->Update();
			}
			// 弾
			if (bullet_)
			{
				bullet_->Update();
			}
			camera_.TransferMatrix();
		}
		// カメラコントローラーの更新
		cameraController_->Update();
		// カメラの更新
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
		// 全ての当たり判定
		CheckAllCollisions();

		//敵脂肪
		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
		});

		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;

			fade_->Start(Fade::Status::FadeOut, 1.0f);
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
	case Phase::kFadeIn:
		// ゲームプレイフェーズの処理////////////////////////////////////

		// 天球の更新
		skydome_->Update();
		
		//==========//
		//	その他  //
		//==========//
		// 自キャラの更新
		player_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_)
		{
			enemy->Update();
		}
		// 弾
		if (bullet_)
		{
			bullet_->Update();
		}
		camera_.TransferMatrix();
		// カメラコントローラーの更新
		cameraController_->Update();
		// カメラの更新
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
		// 全ての当たり判定
		CheckAllCollisions();

		break;

		case Phase::kGameClear:
		//====================//
		//  ゲームクリア中   //
		//====================//

		// BGM 停止（1回だけでOK）
		audio_->StopWave(bgmHandle_);

		// SPACE でタイトルへ
		if (Input::GetInstance()->TriggerKey(DIK_SPACE))
		{
			finished_ = true;
		}
		break;
	}
}

void GameScene::Draw() 
{
	//DirectXCommonインストラクタの習得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	//3Dモデル描画前処理

	//=========================//
	//    3Dスプライト描画     //
	//=========================//

	Model::PreDraw(dxCommon->GetCommandList());
	// 自キャラの描画
	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn)
	{
		player_->Draw();
	}
	for (Enemy* enemy : enemies_)
	{
		enemy->Draw();
	}
	// 弾
	if (bullet_)
	{
		bullet_->Draw();
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
	// フェード
	fade_->Draw();

	//=========================//
	//    2Dスプライト描画     //
	//=========================//
	if (bulletSelectPoint_ == 1) // ←追加
	{
		Sprite::PreDraw(dxCommon->GetCommandList());

		// セレクトカードを描画
		bulletSelect1Sprite_->Draw();
		bulletSelect2Sprite_->Draw();

		// スプライト描画後処理
		Sprite::PostDraw();
	}
	Sprite::PreDraw(dxCommon->GetCommandList());

	//=========================//
	//    ゲームクリア表示     //
	//=========================//
	if (phase_ == Phase::kGameClear) {
		Sprite::PreDraw(dxCommon->GetCommandList());
		gameClearSprite_->Draw();
		Sprite::PostDraw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

}

void GameScene::CheckAllCollisions()
{
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
			enemy->OnCollisionEP(player_);
		}
	}

	 if (bullet_->GetCurrentBulletType() != 5 )
	 {

		AABB bulletAABB = bullet_->GetAABB();

		for (Enemy* enemy : enemies_)
		{
			AABB enemyAABB = enemy->GetAABB();

			if (IsCollision(bulletAABB, enemyAABB))
			{
				bullet_->OnCollision(enemy);
				enemy->OnCollisionEB(bullet_);
			}
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
void GameScene::ChangePhase()
{ ///////////////////////////////////////////////////////////
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

	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			// ゲームプレイ
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		// シーン終了
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

GameScene::~GameScene()
{

	// BGM 停止
	audio_->StopWave(bgmHandle_);

	//自キャラの解放
	delete player_;
	delete modelPlayer_;
	delete modelBlock_;
	delete modelSkydome_;
	delete mapChipField_;
	delete deathParticles_;
	// 弾
	delete bullet_;
	delete modelBullet_;
	// フェード
	delete fade_;
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
