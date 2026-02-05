#pragma once
#include "Bullet.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include <vector>

// ゲームシーン
class GameScene {
public:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// 3Dモデル
	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;
	// モデル敵
	KamataEngine::Model* modelEnemy_ = nullptr;
	// デスパーティークル
	KamataEngine::Model* modelDeathparticles_ = nullptr;
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTrensform_;
	// カメラ
	KamataEngine::Camera camera_;
	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	// 自キャラ
	Player* player_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_;
	CameraController* cameraController_ = nullptr;
	// 敵
	std::list<Enemy*> enemies_;

	// 全ての当たり判定を行う
	void CheckAllCollisions();

	DeathParticles* deathParticles_ = nullptr;

	// ゲームのフェーズ（型）
	enum class Phase {
		kFadeIn,   // フェードイン
		kPlay,     // ゲームプレイ
		kDeath,    // デス演出
		kFadeOut,  // フェードアウト
		kGameClear // ★ゲームクリア（追加）
	};

	// ゲームの現在フェーズ（変数）
	Phase phase_;

	// フェーズの切り替え
	void ChangePhase();

	// 終了フラグ
	bool finished_ = false;

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }
	// フェード
	Fade* fade_ = nullptr;

	// ===== 弾 =====
	Bullet* bullet_ = nullptr;
	KamataEngine::Model* modelBullet_ = nullptr;

	//=============//
	//   弾選択用  //
	//=============//

	void SetCardTexture(Sprite* sprite, int cardNo);
	void ChangeBulletModel(int type);
	std::string GetCardPath(int type);
	// 弾の描画変更用
	int prevBulletType_ = 0;
	int bulletSelectPoint_ = 0;
	int bulletSelectTime_ = 60; // 1800 = 30秒
	// セレクトカード選択
	int Select1Point_ = 0;
	int Select2Point_ = 0;
	int SelectSet_ = 1;
	// セレクトカード画像ハンドル
	uint32_t bulletSelect1Handle_ = 0;
	uint32_t bulletSelect2Handle_ = 0;
	// セレクトカードスプライト
	Sprite* bulletSelect1Sprite_ = nullptr;
	Sprite* bulletSelect2Sprite_ = nullptr;
	// セレクトカードの現在の座標（X位置は固定、Yが上下移動）
	float bulletSelect1PosX_ = 197.0f;
	float bulletSelect1PosY_ = 40.0f;
	float bulletSelect2PosX_ = 680.0f;
	float bulletSelect2PosY_ = 40.0f;
	//
	int currentType = 0;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	//====================//
	//  ゲームクリア用   //
	//====================//
	int gameClearTimer_ = 60 * 30;  // ★30秒（60FPS想定）
	uint32_t gameClearTexture_ = 0; // ★ゲームクリア画像
	Sprite* gameClearSprite_ = nullptr;

	//====================//
	//       BGM          //
	//====================//
	KamataEngine::Audio* audio_ = nullptr;
	uint32_t bgmHandle_ = 0;

	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBilocks();
};