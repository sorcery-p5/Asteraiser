#include "P5.h"

#include "DrawDevice.h"
#include "DrawItem/DrawItemManager.h"

#include "System/Timer.h"

namespace P5
{

///////////////////////////////////////////////////////////////////////////////
//
//	グラフィクス
//
///////////////////////////////////////////////////////////////////////////////
Graphix::Graphix( void )
{
	m_pDevice		= NULL;
	m_pDrawItemMng	= NULL;
	m_pTimer		= NULL;

	m_FrameSkip			= 0;
	m_FrameSkipCount	= 0;
}

Graphix::~Graphix()
{
	SafeDelete( m_pTimer );
	SafeDelete( m_pDrawItemMng );
	SafeDelete( m_pDevice );
}

//*****************************************************************************
//	初期化
//*****************************************************************************
bool Graphix::Init( Window* pWindow )
{
	if( !pWindow ) return false;

	Log( "　□グラフィックス初期化" );

	//	デバイス初期化
	m_pDevice = new DrawDevice;
	if( !m_pDevice ) return false;	
	if( !m_pDevice->Init( pWindow ) ) return false;

	//	スプライト管理
	m_pDrawItemMng = new DrawItemManager;
	if( !m_pDrawItemMng ) return false;

	//	タイマー
	m_pTimer = new Timer;
	if( !m_pTimer ) return false;

	Log( "　○グラフィックス初期化完了" );
	Log( "　----------------------------------------" );

	return true;
}

//*****************************************************************************
//	更新
//*****************************************************************************
bool Graphix::Update( void )
{
	bool bRes = true;

	if( m_FrameSkip <= 1 || m_FrameSkipCount == 0 )
	{
		if( m_pDevice->Begin() )
		{
			//	描画アイテム描画
			m_pDrawItemMng->Render();
		}
		bRes = m_pDevice->End();

		//	タイマーでウェイトをとる
		if( m_pDevice->IsTimerWait() ) m_pTimer->Wait();
	}
	else
	{
		//	スキップ
		m_pDrawItemMng->ClearDrawItem();
	}

	//	フレームスキップ
	if( m_FrameSkip > 1 )
	{
		m_FrameSkipCount++;
		if( m_FrameSkipCount >= m_FrameSkip ) m_FrameSkipCount = 0;
	}

	return bRes;
}

//*****************************************************************************
//	デバイスのリセット
//*****************************************************************************
void Graphix::Reset( void )
{
	//	リソースを解放
	for( VecResetListener::iterator it = m_vResetListener.begin(); it != m_vResetListener.end(); ++it )
	{
		(*it)->OnDrawDeviceReset();
	}

	//	デバイスをリセット
	m_pDevice->Reset();

	//	リソースを復帰
	for( VecResetListener::iterator it = m_vResetListener.begin(); it != m_vResetListener.end(); ++it )
	{
		(*it)->OnDrawDeviceRestore();
	}
}

//******************************************************************************
//	リセット通知インタフェースの追加
//******************************************************************************
void Graphix::AddResetListener( DeviceResetListener* pListener )
{
	if( !pListener ) return;

	m_vResetListener.push_back( pListener );
}

//******************************************************************************
//	リセット通知インタフェースの削除
//******************************************************************************
void Graphix::RemoveResetListener( DeviceResetListener* pListener )
{
	VecResetListener::iterator it = StdFind( m_vResetListener, pListener );

	if( it != m_vResetListener.end() )
	{
		m_vResetListener.erase( it );
	}
}

//******************************************************************************
//	文字列をブレンドモードへ変換
//******************************************************************************
BLEND_MODE Graphix::StrToBlendMode( pstr pStr )
{
	pstr aBlend[] =
	{
		"NONE",
		"ALPHA",
		"ADD",
		"SUB",
		"MUL",
		"REV",
	};

	return BLEND_MODE( StrToInt( pStr, aBlend, ArrayNum(aBlend), BLEND_ALPHA ) );
}

}