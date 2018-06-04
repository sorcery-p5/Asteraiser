#pragma once


////////////////////////////////////////////////////////////////////////////////
//
//	リソース型定義
//
////////////////////////////////////////////////////////////////////////////////
enum RES_TYPE
{
	RES_WORLD,				//	ワールド

	RES_SCORE,				//	スコア
	RES_ITEM,				//	アイテム
	RES_PLAYER,				//	プレーヤー
	RES_STAGE,				//	ステージ
	RES_ENEMY,				//	敵
	RES_ATTACK,				//	攻撃
	RES_INDICATE,			//	インジケート
	RES_MENU,				//	メニュー

	RES_EFFECT,				//	エフェクト
	RES_SPRITE_FRAME,		//	スプライトフレーム
	RES_SOUND,				//	サウンド
	RES_FONT,				//	フォント
	RES_TEX,				//	テクスチャ

	RES_TYPE_NUM,
	RES_NULL			= -1,
};