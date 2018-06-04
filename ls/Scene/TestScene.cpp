#include "stdafx.h"
#include "TestScene.h"

#include "TitleScene.h"
#include "WorldScene.h"

#include "SpriteFrame/SpriteFrameFile.h"
#include "SpriteFrame/SpriteFrame.h"

#include "App/App.h"
#include "Resource/ResourceManager.h"
#include "Resource/FontResource.h"
#include "Resource/SoundResource.h"

#include "Effect/EffectManager.h"
#include "Effect/Effect.h"
#include "Effect/EffectFile.h"

#include "BGM/BGMManager.h"

namespace
{
	pstr	SPRITE_PATH			= "Data/Sprite/Door.xml";
	pstr	EFFECT_PATH_1		= "Data/Effect/Muzzle.xml";
	pstr	EFFECT_PATH_2		= "Data/Effect/BombC.xml";
}


///////////////////////////////////////////////////////////////////////////////
//
//	�e�X�g�V�[��
//
///////////////////////////////////////////////////////////////////////////////
TestScene::TestScene( Label Name )
:Scene( Name )
{
	m_pSpriteFrameFile	= NULL;
	m_pSpriteFrame		= NULL;
	m_pFontRes			= NULL;
	m_pFontSprite		= NULL;
	m_pEffectRes		= NULL;
	m_pEffectRes2		= NULL;
	m_pSERes			= NULL;
	m_pBGMRes			= NULL;

	m_WheelAng			= 0.0f;
	m_IntpTime			= 0;

	m_SpritePath		= SPRITE_PATH;
	m_EffectPath		= EFFECT_PATH_1;
	m_EffectPath2		= EFFECT_PATH_2;

	m_BackColor			= Color::Black();
	
	m_BaseMat			= Matrix3::Trans( 320, 240 );
}
TestScene::~TestScene()
{
	SafeDelete( m_pSpriteFrame );
	SafeDelete( m_pSpriteFrameFile );

	SafeDelete( m_pFontSprite );
	SafeRelease( m_pFontRes );

	EffectDelete( m_pEffect );
	SafeRelease( m_pEffectRes );
	SafeRelease( m_pEffectRes2 );

	SafeStop( m_pSE );
	SafeStop( m_pBGM );
	SafeRelease( m_pSERes );
	SafeRelease( m_pBGMRes );
}

//*****************************************************************************
//	������
//*****************************************************************************
void TestScene::OnInit( void )
{
	//	�t�H���g
//	m_pFontRes = GetApp()->GetResMng()->LoadResource<FontResource>( "Data/Font/AngelS.xml" );
	if( m_pFontRes )
	{
		m_pFontSprite = new FontSprite( m_pFontRes->GetData() );

		m_pFontSprite->SetText( 
"abcdefghi\n"
"jklmnopqr\n"
"stuvwxyz\n"
"ABCDEFGHI\n"
"JKLMNOPQR\n"
"STUVWXYZ_\n"
"0123456789\n"
" !?&();:,.\n" );
		m_pFontSprite->SetAlign( FONT_ALIGN_CC );
		m_pFontSprite->SetColor( Color( 255, 255, 255 ) );
		m_pFontSprite->SetBlend( BLEND_ALPHA );

		m_pFontSprite->SetMatrix( Matrix3::Scale( 1.0f,1.0f ) * Matrix3::Trans( 320,240 ) );
	}

	//	�w�i�F�ύX
	P5::GetGraphix()->GetDevice()->SetScreenColor( m_BackColor );

	//	BGM
	GetApp()->GetBGMMng()->Stop();
	
	P5::GetWindow()->SetDragPath("");
}

//*****************************************************************************
//	�X�V
//*****************************************************************************
void TestScene::Update( void )
{
	const Mouse* pMouse = GetSign()->GetMouse();
	const Keyboard* pKey = P5::GetSign()->GetKeyboard();

	bool bSpriteReload = pKey->IsKeyInput( VK_RETURN, INPUT_TRIG );
	bool bEffectReload = pKey->IsKeyInput( VK_CONVERT, INPUT_TRIG );

	//	�h���b�O�p�X���擾
	String256 DragPath = P5::GetWindow()->GetDragPath();
	if( DragPath.Length() )
	{
		if( DragPath.Find( "Sprite" ) )
		{
			m_SpritePath = DragPath;
			bSpriteReload = true;
		}
		if( DragPath.Find( "Effect" ) )
		{
			if( !pKey->IsKeyInput( VK_SHIFT, INPUT_PRESS ) )
			{
				m_EffectPath = DragPath;
			}
			else
			{
				m_EffectPath2 = DragPath;
			}
			bEffectReload = true;
		}

		P5::GetWindow()->SetDragPath("");
	}

	//	��ʒu
	if( pMouse->IsButtonInput( MOUSE_M, INPUT_TRIG ) )
	{
		m_WheelAng	= 0;
		m_BaseMat	= Matrix3::Trans( 320, 240 );
	}

	if( pMouse->IsButtonInput( MOUSE_R, INPUT_PRESS ) )
	{
		m_WheelAng += Angle::ToRad( pMouse->GetWheel() * 20.0f );

		m_BaseMat = Matrix3::TransRotZ( pMouse->GetPos(), m_WheelAng );

		if( m_pEffect )
		{
			m_pEffect->SetMatrix( m_BaseMat );
		}	
	}

	//	�X�v���C�g�����[�h
	if( bSpriteReload )
	{
		GetApp()->GetEffectMng()->DeleteAll();

		SafeDelete( m_pSpriteFrame );
		SafeDelete( m_pSpriteFrameFile );

		m_pSpriteFrameFile = new SpriteFrameFile;
		if( m_pSpriteFrameFile )
		{
			m_pSpriteFrameFile->Load( m_SpritePath.c_str() );
		
			m_pSpriteFrame = new SpriteFrame;
			m_pSpriteFrame->Init( m_pSpriteFrameFile, m_BaseMat );
		}
	}

	//	�X�v���C�g�폜
	if( pKey->IsKeyInput( VK_BACK, INPUT_TRIG ) )
	{
		GetApp()->GetEffectMng()->DeleteAll();

		SafeDelete( m_pSpriteFrame );
		SafeDelete( m_pSpriteFrameFile );
	}

	//	�G�t�F�N�g�����[�h
	if( bEffectReload ||
		!m_pEffectRes && !m_pEffectRes2 )
	{
		EffectDelete( m_pEffect );
		GetApp()->GetEffectMng()->DeleteAll();

		SafeRelease( m_pEffectRes );
		SafeRelease( m_pEffectRes2 );
		m_pEffectRes	= GetApp()->GetResMng()->LoadResource<EffectResource>( m_EffectPath.c_str() );
		m_pEffectRes2	= GetApp()->GetResMng()->LoadResource<EffectResource>( m_EffectPath2.c_str() );
	}

	//	�G�t�F�N�g
	if( pKey->IsKeyInput( VK_SPACE, INPUT_TRIG ) ||
		pKey->IsKeyInput( VK_NONCONVERT, INPUT_PRESS ) )
	{
		if( !pKey->IsKeyInput( VK_SHIFT, INPUT_PRESS ) )
		{
			if( m_pEffectRes )
			{		
				if( pKey->IsKeyInput( VK_SPACE, INPUT_TRIG ) )
				{
					EffectDelete( m_pEffect );
					m_pEffect = GetApp()->GetEffectMng()->CreateEffect( m_pEffectRes->GetData(), m_BaseMat );
				}
				else
				{
					GetApp()->GetEffectMng()->CreateEffect( m_pEffectRes->GetData(), m_BaseMat );
				}
			}
		}
		else
		{
			if( m_pEffectRes2 )
			{
				if( pKey->IsKeyInput( VK_SPACE, INPUT_TRIG ) )
				{
					EffectDelete( m_pEffect );
					m_pEffect = GetApp()->GetEffectMng()->CreateEffect( m_pEffectRes2->GetData(), m_BaseMat );
				}
				else
				{
					GetApp()->GetEffectMng()->CreateEffect( m_pEffectRes2->GetData(), m_BaseMat );
				}
			}
		}
	}

	else if( pKey->IsKeyInput( VK_INSERT, INPUT_TRIG ) )
	{
		EffectEndAppear( m_pEffect );
	}

	else if( pKey->IsKeyInput( VK_DELETE, INPUT_TRIG ) )
	{
		EffectDelete( m_pEffect );
	}
	
	//	�A�j��
	if( m_pSpriteFrame )
	{
		byte Key = pKey->GetInputKey( INPUT_TRIG );
		if( Key >= 'A' && Key <= 'Z' )
		{
			//	����������Đ�
			int Mode = pKey->IsKeyInput( VK_SHIFT, INPUT_PRESS ) &&	pKey->IsKeyInput( VK_CONTROL, INPUT_PRESS )? 3 :
						pKey->IsKeyInput( VK_SHIFT, INPUT_PRESS )? 1 :
						pKey->IsKeyInput( VK_CONTROL, INPUT_PRESS )? 2 : 0;
			int Count = 0;
			for( int i = 0; i < m_pSpriteFrame->GetAnimeNum(); i++ )
			{
				Label Name = m_pSpriteFrame->GetAnimeName( i );
				if( *Name.GetStr() == Key )
				{
					if( Count == Mode )
					{
						m_pSpriteFrame->ChangeAnime( Name, m_IntpTime );
						break;
					}
					else
					{
						Count++;
					}
				}
			}
		}
		else if( Key >= '0' && Key <= '9' )
		{
			//	�C���f�N�X����Đ�
			int i = Key - '0';
			if( i < m_pSpriteFrame->GetAnimeNum() )
			{
				Label Name = m_pSpriteFrame->GetAnimeName( i );
				m_pSpriteFrame->ChangeAnime( Name, m_IntpTime );
			}
		}
		else if( Key >= VK_F1 && Key <= VK_F24  )
		{
			//	�C���f�N�X����Đ�
			int i = Key - VK_F1 + 10;
			if( i < m_pSpriteFrame->GetAnimeNum() )
			{
				Label Name = m_pSpriteFrame->GetAnimeName( i );
				m_pSpriteFrame->ChangeAnime( Name, m_IntpTime );
			}
		}
		else if( Key >= VK_NUMPAD0 && Key <= VK_NUMPAD9 )
		{
			//	��Ԏ��Ԃ�ύX
			m_IntpTime = (Key - VK_NUMPAD0) * 5;
		}

	}
	
	//	�w�i�̐F
	if( pKey->IsKeyInput( VK_PRIOR, INPUT_TRIG ) )
	{
		m_BackColor += Color::Gray(16);
		P5::GetGraphix()->GetDevice()->SetScreenColor( m_BackColor );
	}
	if( pKey->IsKeyInput( VK_NEXT, INPUT_TRIG ) )
	{
		m_BackColor -= Color::Gray(16);
		P5::GetGraphix()->GetDevice()->SetScreenColor( m_BackColor );
	}
	
	//	�t�H���g
	SafePtr(m_pFontSprite)->Draw();

	if(m_pSpriteFrame)
	{
		m_pSpriteFrame->Update( NULL );
		m_pSpriteFrame->Draw( Matrix3::Identity() );
	}

	//	�I��
	if( pKey->IsKeyInput( VK_ESCAPE, INPUT_TRIG ) )
	{
		ChangeScene( new TitleScene( "Title" ) );
		return;
	}

	if( pKey->IsKeyInput( VK_PAUSE, INPUT_TRIG ) )
	{
		ChangeScene( new WorldScene( "World", "Data/World/Test.xml" ) );
		return;
	}
}