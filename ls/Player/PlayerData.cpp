#include "stdafx.h"
#include "PlayerData.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceUtil.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "Effect/EffectFile.h"
#include "Indicate/IndicateData.h"

namespace
{
	pstr	NODE_PLAYER						= "Player";
	pstr	NODE_BLADE						= "Blade";
	pstr	NODE_ENERGY						= "Energy";
	pstr	NODE_SMASH						= "Smash";
	pstr	NODE_OVERDRIVE					= "Overdrive";
	pstr	NODE_DASH						= "Dash";
	pstr	NODE_SPLASH						= "Splash";
	pstr	NODE_SHIELD						= "Shield";
	pstr	NODE_SPEAR						= "Spear";
	pstr	NODE_RULER						= "Ruler";
	pstr	NODE_EFFECT						= "Effect";
	pstr	NODE_INDICATE					= "Indicate";

	pstr	ATTR_PATH						= "Path";
	pstr	ATTR_SPRITE						= "Sprite";
	pstr	ATTR_EFFECT						= "Effect";
	pstr	ATTR_HIT_EFFECT					= "HitEffect";
	pstr	ATTR_REPULSE_EFFECT				= "RepulseEffect";
	pstr	ATTR_REPULSE_HIT_EFFECT			= "RepulseHitEffect";
	pstr	ATTR_REPULSE_END_EFFECT			= "RepulseEndEffect";
	pstr	ATTR_CRASH_EFFECT				= "CrashEffect";
	pstr	ATTR_BREAK_EFFECT				= "BreakEffect";
	pstr	ATTR_RECOVERY_EFFECT			= "RecoveryEffect";
	pstr	ATTR_RULER_EFFECT				= "RulerEffect";
	pstr	ATTR_RULER_HIT_EFFECT			= "RulerHitEffect";
	pstr	ATTR_PLAYER_EFFECT				= "PlayerEffect";
	pstr	ATTR_LOCK_EFFECT				= "LockEffect";
	pstr	ATTR_CHARGE_EFFECT				= "ChargeEffect";
	pstr	ATTR_MAX_EFFECT					= "MaxEffect";

	pstr	ATTR_MOVE_SPEED					= "MoveSpeed";
	pstr	ATTR_SLOW_SPEED					= "SlowSpeed";
	pstr	ATTR_DECEL						= "Decel";
	pstr	ATTR_CRASH_VEL					= "CrashVel";
	pstr	ATTR_BREAK_VEL					= "BreakVel";
	pstr	ATTR_KNOCKBACK_TIME				= "KnockbackTime";
	pstr	ATTR_REVERSAL_TIME				= "ReversalTime";
	pstr	ATTR_REPULSE_INVINCIBLE			= "RepulseInvincible";
	pstr	ATTR_RULER_MOVE_SPEED			= "RulerMoveSpeed";

	pstr	ATTR_HIT_RADIUS					= "HitRadius";
	pstr	ATTR_ITEM_COLLECT_RADIUS		= "ItemCollectRadius";
	pstr	ATTR_REGEN_ENERGY				= "RegenEnergy";
	pstr	ATTR_ITEM_ENERGY				= "ItemEnergy";

	pstr	ATTR_LENGTH						= "Length";
	pstr	ATTR_RADIUS						= "Radius";
	pstr	ATTR_NORMAL_DELAY				= "NormalDelay";
	pstr	ATTR_HIT_DELAY					= "HitDelay";
	pstr	ATTR_CANCEL_TIME				= "CancelTime";
	pstr	ATTR_NOT_EXHAUST_TIME			= "NotExhaustTime";
	pstr	ATTR_NOT_RELOAD_TIME			= "NotReloadTime";

	pstr	ATTR_CHARGE_START_TIME			= "ChargeStartTime";
	pstr	ATTR_CHARGE_TIME				= "ChargeTime";
	pstr	ATTR_CHARGE_MAX_TIME			= "ChargeMaxTime";
	pstr	ATTR_RULER_CHARGE_TIME			= "RulerChargeTime";
	pstr	ATTR_EXHAUST_RATE				= "ExhaustRate";
	pstr	ATTR_CHARGE_EXHAUST_ENERGY		= "ChargeExhaustEnergy";
	pstr	ATTR_CHARGE_EXHAUST_LIMIT		= "ChargeExhaustLimit";
	pstr	ATTR_COLLIDE_RADIUS				= "CollideRadius";

	pstr	ATTR_ATTACK						= "Attack";
	pstr	ATTR_REPULSE_ATTACK				= "RepulseAttack";
	pstr	ATTR_ATTACK_ENERGY_RATE			= "AttackEnergyRate";
	pstr	ATTR_ATTACK_LIMIT_RATE			= "AttackLimitRate";
	pstr	ATTR_ATTACK_VEL					= "AttackVel";
	pstr	ATTR_KNOCKBACK					= "Knockback";
	pstr	ATTR_REPULSE_INTERVAL			= "RepulseInterval";
	
	pstr	ATTR_RULER_ATTACK				= "RulerAttack";
	pstr	ATTR_RULER_DELAY				= "RulerDelay";
	pstr	ATTR_RULER_LENGTH				= "RulerLength";
	pstr	ATTR_RULER_RADIUS				= "RulerRadius";

	pstr	ATTR_EXHAUST_ENERGY_RATE		= "ExhaustEnergyRate";
	pstr	ATTR_EXHAUST_LIMIT_RATE			= "ExhaustLimitRate";
	pstr	ATTR_NORMAL_REGEN				= "NormalRegen";
	pstr	ATTR_NORMAL_LIMIT_REGEN			= "NormalLimitRegen";
	pstr	ATTR_ITEM_REGEN					= "ItemRegen";
	pstr	ATTR_ITEM_LIMIT_REGEN			= "ItemLimitRegen";
	pstr	ATTR_RELOAD_REGEN				= "ReloadRegen";
	pstr	ATTR_CRASH_REGEN				= "CrashRegen";
	pstr	ATTR_CANCEL_ENERGY_DECAY		= "CancelEnergyDecay";
	pstr	ATTR_USE_REGEN_ENERGY			= "UseRegenEnergy";
	pstr	ATTR_USE_LIMIT_ENERGY			= "UseLimitEnergy";
	pstr	ATTR_USE_RELOAD_ENERGY			= "UseReloadEnergy";

	pstr	ATTR_SPEED						= "Speed";
	pstr	ATTR_TIME						= "Time";
	pstr	ATTR_GROUND_EXHAUST				= "GroundExhaust";

	pstr	ATTR_ENERGY						= "Energy";
	pstr	ATTR_ENERGY_MAX					= "EnergyMax";
	pstr	ATTR_ENERGY_ITEMRATE			= "EnergyItemRate";
	pstr	ATTR_ENERGY_DECAYRATE			= "EnergyDecayRat";
	pstr	ATTR_ATTACK_MAX					= "AttackMax";
	pstr	ATTR_ATTACK_ITEMRATE			= "AttackItemRate";
	pstr	ATTR_ATTACK_DECAYRATE			= "AttackDecayRate";
	pstr	ATTR_RADIUS_MAX					= "RadiusMax";
	pstr	ATTR_RADIUS_ITEMRATE			= "RadiusItemRate";
	pstr	ATTR_TIME_MAX					= "TimeMax";
	pstr	ATTR_TIME_ITEMRATE				= "TimeItemRate";

	pstr	ATTR_MOVE_LENGTH				= "MoveLength";
	pstr	ATTR_DELAY						= "Delay";
	pstr	ATTR_HIT_INVINCIBLE				= "HitInvincible";

	pstr	ATTR_DAMAGE_INVINCIBLE			= "DamageInvincible";
	pstr	ATTR_BREAK_INVINCIBLE			= "BreakInvincible";
	pstr	ATTR_REVIVE_INVINCIBLE			= "ReviveInvincible";
	pstr	ATTR_DAMAGE_NO_MOVE				= "DamageNoMove";
	pstr	ATTR_BREAK_TIME					= "BreakTime";
	pstr	ATTR_SERIAL_DAMAGE_TIME			= "SerialDamageTime";
	pstr	ATTR_NORMAL_RECOVERY			= "NormalRecovery";
	pstr	ATTR_ITEM_RECOVERY				= "ItemRecovery";
	pstr	ATTR_BREAK_NUM_MAX				= "BreakNumMax";
	pstr	ATTR_BREAK_NUM_DECAY_RATE		= "BreakNumDecayRate";

	pstr	ATTR_HIT_ITEM_NUM				= "HitItemNum";
	pstr	ATTR_REPULSE_ITEM_NUM			= "RepulseItemNum";
	pstr	ATTR_CRASH_ITEM_NUM				= "CrashItemNum";
	pstr	ATTR_RELOAD_ITEM_NUM			= "ReloadItemNum";
	pstr	ATTR_BREAK_ITEM_NUM				= "BreakItemNum";
	pstr	ATTR_ITEM_SPEED					= "ItemSpeed";
}


////////////////////////////////////////////////////////////////////////////////
//
//	プレイヤーデータ
//
////////////////////////////////////////////////////////////////////////////////
PlayerData::PlayerData( void )
{

}

PlayerData::~PlayerData()
{
	ReleaseMap( m_mapSpriteRes );
	ReleaseMap( m_mapEffectRes );
	ReleaseMap( m_mapIndicateRes );
}

//******************************************************************************
//	読み込み
//******************************************************************************
bool PlayerData::Load( pstr pPath )
{
	//	ファイル読み込む
	XML Xml;
	if( !Xml.Load( pPath ) )
	{
		return false;
	}

	//	エフェクト
	const XML::Node* pEffectNode = Xml.GetRoot().FindChild( NODE_EFFECT );
	if( pEffectNode ) _LoadEffect( pEffectNode );

	//	インジケート
	const XML::Node* pIndicateNode = Xml.GetRoot().FindChild( NODE_INDICATE );
	if( pIndicateNode ) _LoadIndicate( pIndicateNode );

	//	パラメータ
	const XML::Node* pParamNode = Xml.GetRoot().FindChild( NODE_PLAYER );
	if( pParamNode ) _LoadParam( pParamNode );

	//	ブレード
	const XML::Node* pBladeNode = Xml.GetRoot().FindChild( NODE_BLADE );
	if( pBladeNode ) _LoadBlade( m_Param.Blade, pBladeNode );

	//	オーヴァドライヴ
	const XML::Node* pOverdriveNode = Xml.GetRoot().FindChild( NODE_OVERDRIVE );
	if( pOverdriveNode ) _LoadOverdrive( m_Param.Overdrive, pOverdriveNode );

	//	ダッシュ
	const XML::Node* pDashNode = Xml.GetRoot().FindChild( NODE_DASH );
	if( pDashNode ) _LoadDash( m_Param.Dash, pDashNode );

	//	シールド
	const XML::Node* pShieldNode = Xml.GetRoot().FindChild( NODE_SHIELD );
	if( pShieldNode ) _LoadShield( m_Param.Shield, pShieldNode );

	return true;
}

//******************************************************************************
//	スプライトファイル取得
//******************************************************************************
const SpriteFrameFile* PlayerData::GetSpriteFrameFile( Label Name ) const
{
	return FindResourceMap<SpriteFrameFile, SpriteFrameResource>( m_mapSpriteRes, Name );
}

//******************************************************************************
//	エフェクトファイルの取得
//******************************************************************************
const EffectFile* PlayerData::GetEffectFile( Label Name ) const
{
	return FindResourceMap<EffectFile, EffectResource>( m_mapEffectRes, Name );
}

//******************************************************************************
//	インジケートデータの取得
//******************************************************************************
const IndicateData* PlayerData::GetIndicateData( Label Name ) const
{
	return FindResourceMap<IndicateData, IndicateResource>( m_mapIndicateRes, Name );
}

//------------------------------------------------------------------------------
//	パラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadParam( const XML::Node* pNode )
{
	m_Param.SpriteName = AddResourceMap< SpriteFrameResource >( m_mapSpriteRes, pNode->GetAttr( ATTR_SPRITE ).GetStr(), GetApp()->GetResMng() );

	m_Param.MoveSpeed			= pNode->GetAttr( ATTR_MOVE_SPEED ).GetFloat();
	m_Param.SlowSpeed			= pNode->GetAttr( ATTR_SLOW_SPEED ).GetFloat();
	m_Param.Decel				= pNode->GetAttr( ATTR_DECEL ).GetFloat();
	m_Param.CrashVel			= pNode->GetAttr( ATTR_CRASH_VEL ).GetFloat();
	m_Param.BreakVel			= pNode->GetAttr( ATTR_BREAK_VEL ).GetFloat();

	m_Param.KnockbackTime		= pNode->GetAttr( ATTR_KNOCKBACK_TIME ).GetInt();
	m_Param.ReversalTime		= pNode->GetAttr( ATTR_REVERSAL_TIME ).GetInt();
	m_Param.RepulseInvincible	= pNode->GetAttr( ATTR_REPULSE_INVINCIBLE ).GetInt();

	m_Param.RulerMoveSpeed		= pNode->GetAttr( ATTR_RULER_MOVE_SPEED ).GetFloat();

	m_Param.HitRadius			= pNode->GetAttr( ATTR_HIT_RADIUS ).GetFloat();
	m_Param.ItemCollectRadius	= pNode->GetAttr( ATTR_ITEM_COLLECT_RADIUS ).GetFloat();

	m_Param.RegenEnergy			= pNode->GetAttr( ATTR_REGEN_ENERGY ).GetFloat();
	m_Param.ItemEnergy			= pNode->GetAttr( ATTR_ITEM_ENERGY ).GetFloat();
}

//------------------------------------------------------------------------------
//	ブレードパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadBlade( BLADE_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクト
	Param.EffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.HitEffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RepulseEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_REPULSE_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RepulseHitEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_REPULSE_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RepulseEndEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_REPULSE_END_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.CrashEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_CRASH_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RulerEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_RULER_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RulerHitEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_RULER_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.Length				= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.Radius				= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.NormalDelay			= pNode->GetAttr( ATTR_NORMAL_DELAY ).GetInt();
	Param.HitDelay				= pNode->GetAttr( ATTR_HIT_DELAY ).GetInt();
	Param.CancelTime			= pNode->GetAttr( ATTR_CANCEL_TIME ).GetInt();
	Param.NotExhaustTime		= pNode->GetAttr( ATTR_NOT_EXHAUST_TIME ).GetInt();
	Param.NotReloadTime			= pNode->GetAttr( ATTR_NOT_RELOAD_TIME ).GetInt();

	Param.Attack				= pNode->GetAttr( ATTR_ATTACK ).GetFloat();
	Param.RepulseAttack			= pNode->GetAttr( ATTR_REPULSE_ATTACK ).GetFloat();
	Param.AttackEnergyRate		= pNode->GetAttr( ATTR_ATTACK_ENERGY_RATE ).GetFloat();
	Param.AttackLimitRate		= pNode->GetAttr( ATTR_ATTACK_LIMIT_RATE ).GetFloat();
	Param.AttackVel				= pNode->GetAttr( ATTR_ATTACK_VEL ).GetFloat();
	Param.Knockback				= pNode->GetAttr( ATTR_KNOCKBACK ).GetInt();
	Param.RepulseInterval		= Max( pNode->GetAttr( ATTR_REPULSE_INTERVAL ).GetInt(), 1 );

	Param.RulerAttack			= pNode->GetAttr( ATTR_RULER_ATTACK ).GetFloat();
	Param.RulerDelay			= pNode->GetAttr( ATTR_RULER_DELAY ).GetInt();
	Param.RulerLength			= pNode->GetAttr( ATTR_RULER_LENGTH ).GetFloat();
	Param.RulerRadius			= pNode->GetAttr( ATTR_RULER_RADIUS ).GetFloat();

	Param.HitItemNum			= pNode->GetAttr( ATTR_HIT_ITEM_NUM ).GetInt();
	Param.RepulseItemNum		= pNode->GetAttr( ATTR_REPULSE_ITEM_NUM ).GetInt();
	Param.CrashItemNum			= pNode->GetAttr( ATTR_CRASH_ITEM_NUM ).GetInt();
	Param.ReloadItemNum			= pNode->GetAttr( ATTR_RELOAD_ITEM_NUM ).GetInt();
	Param.ItemSpeed				= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();

	//	エネルギー
	const XML::Node* pEnergyNode = pNode->FindChild( NODE_ENERGY );
	if( pEnergyNode ) _LoadBladeEnergy( Param.EnergyParam, pEnergyNode );

	//	スマッシュ
	const XML::Node* pSmashNode = pNode->FindChild( NODE_SMASH );
	if( pSmashNode ) _LoadBladeSmash( Param.SmashParam, pSmashNode );
}

//------------------------------------------------------------------------------
//	ブレード：エネルギーパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadBladeEnergy( BLADE_ENERGY_PARAM& Param, const XML::Node* pNode )
{
	Param.ExhaustEnergyRate		= pNode->GetAttr( ATTR_EXHAUST_ENERGY_RATE ).GetFloat();
	Param.ExhaustLimitRate		= pNode->GetAttr( ATTR_EXHAUST_LIMIT_RATE ).GetFloat();

	Param.NormalRegen			= pNode->GetAttr( ATTR_NORMAL_REGEN ).GetFloat();
	Param.NormalLimitRegen		= pNode->GetAttr( ATTR_NORMAL_LIMIT_REGEN ).GetFloat();
	Param.ItemRegen				= pNode->GetAttr( ATTR_ITEM_REGEN ).GetFloat();
	Param.ItemLimitRegen		= pNode->GetAttr( ATTR_ITEM_LIMIT_REGEN ).GetFloat();
	Param.ReloadRegen			= pNode->GetAttr( ATTR_RELOAD_REGEN ).GetFloat();
	Param.CrashRegen			= pNode->GetAttr( ATTR_CRASH_REGEN ).GetFloat();
	Param.CancelEnergyDecay		= pNode->GetAttr( ATTR_CANCEL_ENERGY_DECAY ).GetFloat();

	Param.UseRegenEnergy		= pNode->GetAttr( ATTR_USE_REGEN_ENERGY ).GetFloat();
	Param.UseLimitEnergy		= pNode->GetAttr( ATTR_USE_LIMIT_ENERGY ).GetFloat();
	Param.UseReloadEnergy		= pNode->GetAttr( ATTR_USE_RELOAD_ENERGY ).GetFloat();
}

//------------------------------------------------------------------------------
//	ブレード：スマッシュパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadBladeSmash( BLADE_SMASH_PARAM& Param, const XML::Node* pNode )
{
	Param.HitEffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.ChargeStartTime		= pNode->GetAttr( ATTR_CHARGE_START_TIME ).GetInt();
	Param.ChargeTime			= pNode->GetAttr( ATTR_CHARGE_TIME ).GetInt();
	Param.ChargeMaxTime			= pNode->GetAttr( ATTR_CHARGE_MAX_TIME ).GetInt();
	Param.RulerChargeTime		= pNode->GetAttr( ATTR_RULER_CHARGE_TIME ).GetInt();
	Param.Length				= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.Radius				= pNode->GetAttr( ATTR_RADIUS ).GetFloat();

	Param.Attack				= pNode->GetAttr( ATTR_ATTACK ).GetFloat();
	Param.AttackVel				= pNode->GetAttr( ATTR_ATTACK_VEL ).GetFloat();
	Param.Knockback				= pNode->GetAttr( ATTR_KNOCKBACK ).GetInt();

	Param.ExhaustRate			= pNode->GetAttr( ATTR_EXHAUST_RATE ).GetFloat();
	Param.ChargeExhaustEnergy	= pNode->GetAttr( ATTR_CHARGE_EXHAUST_ENERGY ).GetFloat();
	Param.ChargeExhaustLimit	= pNode->GetAttr( ATTR_CHARGE_EXHAUST_LIMIT ).GetFloat();
}

//------------------------------------------------------------------------------
//	オーヴァドライヴパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadOverdrive( OVERDRIVE_PARAM& Param, const XML::Node* pNode )
{
	Param.NormalRegen			= pNode->GetAttr( ATTR_NORMAL_REGEN ).GetFloat();
	Param.ItemRegen				= pNode->GetAttr( ATTR_ITEM_REGEN ).GetFloat();
	Param.UseRegenEnergy		= pNode->GetAttr( ATTR_USE_REGEN_ENERGY ).GetFloat();
	Param.Delay					= pNode->GetAttr( ATTR_DELAY ).GetInt();

	//	エフェクト
	Param.ChargeEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_CHARGE_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.MaxEffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_MAX_EFFECT ).GetStr(), GetApp()->GetResMng() );

	for( int i = 0; i < OVERDRIVE_EFFECT_NUM; i++ )
	{
		String256 Str;
		Str.Format( "%s%d", ATTR_EFFECT, i + 1 );
		Param.EffectName[i]		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( Str.c_str() ).GetStr(), GetApp()->GetResMng() );
	}

	//	スピア
	const XML::Node* pSpearNode = pNode->FindChild( NODE_SPEAR );
	if( pSpearNode ) _LoadSpear( Param.Spear, pSpearNode );
	
	//	ルーラー
	const XML::Node* pRulerNode = pNode->FindChild( NODE_RULER );
	if( pRulerNode ) _LoadRuler( Param.Ruler, pRulerNode );
}

//------------------------------------------------------------------------------
//	スピアパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadSpear( SPEAR_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクト
	Param.EffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.HitEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.CrashEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_CRASH_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.LockEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_LOCK_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.Length			= pNode->GetAttr( ATTR_LENGTH ).GetFloat();
	Param.Radius			= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.Speed				= pNode->GetAttr( ATTR_SPEED ).GetFloat();
	Param.Time				= pNode->GetAttr( ATTR_TIME ).GetInt();
	Param.AttackVel			= pNode->GetAttr( ATTR_ATTACK_VEL ).GetFloat();
	Param.Knockback			= pNode->GetAttr( ATTR_KNOCKBACK ).GetInt();
	Param.GroundExhaust		= pNode->GetAttr( ATTR_GROUND_EXHAUST ).GetFloat();

	Param.Energy			= pNode->GetAttr( ATTR_ENERGY ).GetFloat();
	Param.EnergyMax			= pNode->GetAttr( ATTR_ENERGY_MAX ).GetFloat();
	Param.EnergyItemRate	= pNode->GetAttr( ATTR_ENERGY_ITEMRATE ).GetFloat();
	Param.EnergyDecayRate	= pNode->GetAttr( ATTR_ENERGY_DECAYRATE ).GetFloat();

	Param.Attack			= pNode->GetAttr( ATTR_ATTACK ).GetFloat();
	Param.AttackMax			= pNode->GetAttr( ATTR_ATTACK_MAX ).GetFloat();
	Param.AttackItemRate	= pNode->GetAttr( ATTR_ATTACK_ITEMRATE ).GetFloat();
	Param.AttackDecayRate	= pNode->GetAttr( ATTR_ATTACK_DECAYRATE ).GetFloat();
	
	Param.HitItemNum		= pNode->GetAttr( ATTR_HIT_ITEM_NUM ).GetInt();
	Param.CrashItemNum		= pNode->GetAttr( ATTR_CRASH_ITEM_NUM ).GetInt();
	Param.ItemSpeed			= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();
}

//------------------------------------------------------------------------------
//	ルーラーパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadRuler( RULER_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクト
	Param.EffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.CrashEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_CRASH_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.PlayerEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_PLAYER_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.Radius			= pNode->GetAttr( ATTR_RADIUS ).GetFloat();
	Param.RadiusMax			= pNode->GetAttr( ATTR_RADIUS_MAX ).GetFloat();
	Param.RadiusItemRate	= pNode->GetAttr( ATTR_RADIUS_ITEMRATE ).GetFloat();
	Param.Time				= pNode->GetAttr( ATTR_TIME ).GetInt();
	Param.TimeMax			= pNode->GetAttr( ATTR_TIME_MAX ).GetInt();
	Param.TimeItemRate		= pNode->GetAttr( ATTR_TIME_ITEMRATE ).GetFloat();
	
	Param.CrashItemNum		= pNode->GetAttr( ATTR_CRASH_ITEM_NUM ).GetInt();
	Param.ItemSpeed			= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();
}

//------------------------------------------------------------------------------
//	ダッシュパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadDash( DASH_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクト
	Param.EffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.MoveLength			= pNode->GetAttr( ATTR_MOVE_LENGTH ).GetFloat();
	Param.Delay					= pNode->GetAttr( ATTR_DELAY ).GetInt();
	Param.RulerDelay			= pNode->GetAttr( ATTR_RULER_DELAY ).GetInt();
	
	//	払い抜け
	const XML::Node* pSplashNode = pNode->FindChild( NODE_SPLASH );
	if( pSplashNode ) _LoadDashSplash( Param.Splash, pSplashNode );
}

//------------------------------------------------------------------------------
//	ダッシュ：払い抜けパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadDashSplash( DASH_SPLASH_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクト
	Param.EffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.HitEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.CollideRadius		= pNode->GetAttr( ATTR_COLLIDE_RADIUS ).GetFloat();
	Param.RulerRadius		= pNode->GetAttr( ATTR_RULER_RADIUS ).GetFloat();
	Param.Attack			= pNode->GetAttr( ATTR_ATTACK ).GetFloat();
	Param.AttackVel			= pNode->GetAttr( ATTR_ATTACK_VEL ).GetFloat();
	Param.Knockback			= pNode->GetAttr( ATTR_KNOCKBACK ).GetInt();
	Param.ExhaustRate		= pNode->GetAttr( ATTR_EXHAUST_RATE ).GetFloat();
	Param.HitInvincible		= pNode->GetAttr( ATTR_HIT_INVINCIBLE ).GetInt();
}

//------------------------------------------------------------------------------
//	シールドパラメータの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadShield( SHIELD_PARAM& Param, const XML::Node* pNode )
{
	//	エフェクt
	Param.HitEffectName			= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_HIT_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.BreakEffectName		= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_BREAK_EFFECT ).GetStr(), GetApp()->GetResMng() );
	Param.RecoveryEffectName	= AddResourceMap< EffectResource >( m_mapEffectRes, pNode->GetAttr( ATTR_RECOVERY_EFFECT ).GetStr(), GetApp()->GetResMng() );

	Param.DamageInvincible		= pNode->GetAttr( ATTR_DAMAGE_INVINCIBLE ).GetInt();
	Param.BreakInvincible		= pNode->GetAttr( ATTR_BREAK_INVINCIBLE ).GetInt();
	Param.ReviveInvincible		= pNode->GetAttr( ATTR_REVIVE_INVINCIBLE ).GetInt();
	Param.DamageNoMove			= pNode->GetAttr( ATTR_DAMAGE_NO_MOVE ).GetInt();
	Param.BreakTime				= pNode->GetAttr( ATTR_BREAK_TIME ).GetInt();
	Param.SerialDamageTime		= pNode->GetAttr( ATTR_SERIAL_DAMAGE_TIME ).GetInt();

	Param.NormalRegen			= pNode->GetAttr( ATTR_NORMAL_REGEN ).GetFloat();
	Param.ItemRegen				= pNode->GetAttr( ATTR_ITEM_REGEN ).GetFloat();
	Param.NormalRecovery		= pNode->GetAttr( ATTR_NORMAL_RECOVERY ).GetFloat();
	Param.ItemRecovery			= pNode->GetAttr( ATTR_ITEM_RECOVERY ).GetFloat();

	Param.BreakNumMax			= pNode->GetAttr( ATTR_BREAK_NUM_MAX ).GetInt();
	Param.BreakNumDecayRate		= pNode->GetAttr( ATTR_BREAK_NUM_DECAY_RATE ).GetFloat();
	Param.UseRegenEnergy		= pNode->GetAttr( ATTR_USE_REGEN_ENERGY ).GetFloat();
	
	Param.BreakItemNum			= pNode->GetAttr( ATTR_BREAK_ITEM_NUM ).GetInt();
	Param.ItemSpeed				= pNode->GetAttr( ATTR_ITEM_SPEED ).GetFloat();
}

//------------------------------------------------------------------------------
//	エフェクトの読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadEffect( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< EffectResource >( m_mapEffectRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}

//------------------------------------------------------------------------------
//	インジケートデータ読み込み
//------------------------------------------------------------------------------
void PlayerData::_LoadIndicate( const XML::Node* pNode )
{
	for( int i = 0; i < pNode->GetChildNum(); i++ )
	{
		const XML::Node* pFileNode = pNode->GetChild( i );

		//	読み込む
		const XML::Value* pValue = pFileNode->FindAttr( ATTR_PATH );
		if( pValue && !pValue->IsEmpty() )
		{
			AddResourceMap< IndicateResource >( m_mapIndicateRes, pValue->GetStr(), GetApp()->GetResMng() );
		}
	}
}