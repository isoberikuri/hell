#pragma once
#include "KamataEngine.h"

enum class MapChipType
{
	kBlank, // 空白
	kBlock, // ブロック
};

struct MapChipData // グローバル
{
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField
{
public:
	// １ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	// 1読み込みデータをリセットする関数
	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

};
