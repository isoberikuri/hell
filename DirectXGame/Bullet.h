#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "Player.h"
#include "Enemy.h"

class Player;
class Enemy;
class Bullet {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	// ワールド座標の取得
	KamataEngine::Vector3 GetWorldPosition();

	// ワールド取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return bulletWorldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return bulletVelocity_; }
	
	//弾をプレイヤーの位置に持っていく用
	void SetPlayer(Player* player) { player_ = player; }

	//==========//
	//  弾種類  //
	//==========//
	//普通
	int bullet1Normal_ = false;
	//反射
	int bullet2Reflect_ = false;
	//操作
	int bullet3Operation_ = false;
	//連射　変更->　スピード変化
	int bullet4SpeedChange_ = false;

	float currentSpeed_ = 0.0f;          // 現在の弾速
	float bullet4Acceleration_ = 0.05f;  // 加速度
	float maxSpeed_ = 1.5f;              // 最大速度
	float minSpeed_ = -1.0f;             // 最小速度

	//3方向
	int bullet5Trident_ = false;
	KamataEngine::WorldTransform bullet5WorldTransform_[3];     // 基本データ
	KamataEngine::Vector3 bullet5Velocity_[3] = {};             // 基本データ
	KamataEngine::Vector3 bullet5GetWorldPosition(int i);       // ワールド座標の取得

	//==========//
	//  全弾用  //
	//==========//
	int bulletPoint_ = true;
	float bulletSpeed_ = 0.0f;
	void SetModel(KamataEngine::Model* model);

	//=============//
	//   弾選択用  //
	//=============//
	
	 // 現在の弾種を返す（1〜5。未設定なら 0 を返す）
	int GetCurrentBulletType() const;
	// 弾種をセットする（1〜5）。0 を渡すと全フラグをリセットします
	void SetBulletType(int type);
	//リフレクション ->トライデント エラー用
	void ResetBulletState()
	{
		bulletVelocity_ = {0.0f, 0.0f};
		bulletPoint_ = 1;
		bulletWorldTransform_.translation_.y = -1000.0f;
		for (int j = 0; j < 3; j++)
		{
			bullet5WorldTransform_[j].translation_.y = -1000.0f;
			bullet5Velocity_[j] = {0.0f, 0.0f};
		}
	}


	//=================//
	//   弾当たり判定  //
	//=================//
	
	// AABBを取得
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Enemy* enemy);

private:
	// ===== 基本データ =====
	KamataEngine::WorldTransform bulletWorldTransform_;
	KamataEngine::Camera* bulletCamera_ = nullptr;
	KamataEngine::Model* bulletModel_ = nullptr;

	// ===== 移動 =====
	KamataEngine::Vector3 bulletVelocity_ = {};

	// 弾の加速度（速度を増やすときの増加量）
	static inline const float kBulletAcceleration = 0.1f;
	// 弾の減速度（速度を減らすときの減少量、慣性の減衰などに利用）
	static inline const float kBulletAttenuation = 0.1f;
	// 弾の速度の最大値（速度の上限を制限して速くなりすぎないようにする）
	static inline const float kBulletLimitSpeed = 0.2f;

	// 弾をプレイヤーの位置に持っていく用
	Player* player_ = nullptr;

};