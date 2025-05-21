#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Skydome.h"
#include "MapChipField.h"
#include <vector>

//ゲームシーン
class GameScene
{
	public:


	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//3Dモデル
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTrensform_;
	//カメラ
	KamataEngine::Camera camera_;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	//デストラクタ
	~GameScene();


	//初期化
	void Initialize();

	//更新
	void Update();

	//描画
	void Draw();

	void GenerateBilocks();

};
