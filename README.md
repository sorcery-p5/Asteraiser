# Asteraiser
近接斬撃戦闘型 2Dブレードアクションシューティング Asteraiser  
2014　OBLIQUEGLASS  
    
    
### □ はじめに

これは、サークルOBLIQUEGLASSで2011年に頒布した、  
ブレードSTG「Asteraiser」のソースコードをまとめたものです。
http://d.hatena.ne.jp/sorcery/20141227
詳しい解説はこちらをお読みください

### □ ビルド環境
- VisualStudio2010を使用しています。
- DirectX9.0c（2010 june）が必要です。
- libOggが必要です。


### □ 各部の構成
　　
### ファイル構成

#### ls　ゲーム本体のコードです

- App                  アプリケーションクラス。ゲーム全体を統括するルート。
- Attack               敵の攻撃オブジェクト。弾やブレード・シールドなど。
- BGM                  BGM再生や停止管理。フェードアウトも行う。

- Collide              当たり判定。
	- Shape            当たり形状。円・矩形・直線など。
	- CollideCaster    当てる側の検出クラス。
	- CollideObject    当たられる側の検出クラス。
	- CollideSystem    オブジェクトを登録し、当たり検出を行うグローバルクラス。
	- CollideListener  当たり判定の結果を受け取るインタフェース。

- Config               コンフィグ設定を保持するクラス。保存と読み出し。

- Controller           コントローラー。自機やメニューの操作を管理する。
	- ManualController   通常操作用のコントローラー。リプレイの記録も仲介する。
	- ReplayController   リプレイ再生をするコントローラー。
	- SystemController   システム用コントローラー。メニュー画面の操作を受け持つ。

- Effect               エフェクト。VFXの制御。
	- Move               エフェクトの動きを管理する。慣性移動・追従移動・旋回移動など。
	- Effect             エフェクトの実体クラス。パーティクル群の発生と寿命を管理。
	- EffectDrawObj      描画物クラス。スプライト・直線・フォントなど。
	- EffectParticle     パーティクル。エフェクトの構成要素一つ一つを表す。

- Enemy                敵の管理。
	- State              ステートマシン制御のクラス。移動や攻撃アクションやターゲッティング。
	- Enemy              敵の実体クラス。表示用のスプライト・当たり判定・ステートマシンなどを持つ。
	- EnemyIndicate      敵の体力バー表示を管理する。
　
- Fade                 画面フェードアウトを管理する。
- Ground               背景と地形の管理。
- Indicate             画面の表示物。各種ゲージや得点表示など。
- Item                 アイテム管理。破片アイテムの発生・取得。
- Main                 main関数を格納している。

- Menu                 メニュー管理。
	- Object             メニュー内の表示物クラス。カーソル・スライダー・スクロールバーなど。
	- Menu               メニュー一揃いを表す実体クラス。
	- MenuLayer          メニュー内のウィンドウ1枚に相当するレイヤー。
	- MenuListener       メニューで発生した各種通知を受け取るためのインタフェース。

- Player               自機管理。
	- Equip              自機の各種武装を表すクラス。ブレード・スピア・ルーラー・シールドなど。

- Replay               リプレイ管理。リプレイの保存と再生。
- Resource             リソース管理。Xmlやテクスチャをまとめて持ち、寿命の管理や一斉開放をする。
- SaveData             セーブデータの読み込み・保存をし、値を保持するグローバルクラス。
- Scene                シーンクラス。場面の切り替え管理と、各場面ごとの実装クラス。
- Score                スコアリング。得点のカウントとレート・活性度計算。

- SpriteFrame          スプライトフレーム。多関節のスプライトを持つ仕組み。
	- SpriteFrame        スプライトフレームの実体クラス。関節情報とアニメ情報を持つ。
	- SpriteFrameAnime   アニメ再生クラス。フレームごとの位置情報とキーイベントの再生。
	- SpriteFrameBone    関節一つを表すクラス。表示物と位置情報、色情報を持つ。

- Stage                ステージ進行の管理。
	- Action             ステージ進行用のアクション。自機のイベント移動など。
	- Stage              ステージの実体クラス。ステートマシンの進行と表示物の管理をする。
	- StageState         ステージ進行用のステートマシン。
	- StageTelop         ステージ表示物。ステージ表示やボス警告など。

- TimeKey              フレームで発生するイベントの進行ユーティリティ。
- Util                 各種ユーティリティクラス。

- World                ゲーム世界クラス。インゲーム全体の管理をする。
	- World              実体クラス。自機・敵・攻撃などの管理クラスを統括する。
	- WorldContinue      コンティニュー画面とゲームオーバー表示の管理。
	- WorldPause         一時中断画面の管理。
	- WorldResult        ステージリザルトの管理。


#### P5　描画、サウンド、算術等のライブラリ群です。

- Common               共用ライブラリ。
	- Arith              ２D用算術。角度・座標・行列・矩形・乱数など。
	- Util               各種ユーティリティ。テンプレート関数やログ、色管理など。

- Graphix              グラフィックス。
	- DrawItem           描画アイテム管理。描画する形状たちを登録・ソートする。
	- Font               ビットマップフォント。文字の切り出し情報を管理する。
	- Piece              ピース。テクスチャからの切り出し座標を管理。
	- Shape              単純図形。円や直線・多角形などをワイヤフレームで描画する。
	- Sprite             スプライト。テクスチャ付きの頂点を描画。
	- Texture            テクスチャ。画像の単純な読み込み。

- Sign                 入力管理。
	- Keyboard           キーボード入力の取得。
	- Mouse              マウス入力の取得。
	- Pad                ゲームパッドの入力を取得。

- System               windows関連。
	- Timer              ゲームループの時間管理
	- WInddow            ウィンドウの生成とメッセージ管理。

- Tone                 サウンド。
	- Device             サウンドデバイスのラッパー。
	- File               Ogg/Wavのサウンドファイル。
	- Object             サウンド再生を監視する実体クラス。
 


### クラス構成

各構成は、次のような構造で作ることが多いです。

- ○○                対象のクラス（敵やエフェクトなど）
- ○○Manager         対象を配列で管理しておくグローバルクラス
- ○○Data/File       対象のクラスを生成するためのデータ。xml一つに相当
- ○○Types           対象のクラスに関連する構造体／定数の定義。


### クラスツリー構造

- App
    - ResourceManager
    - EffectManager
    - BGMManager
    - SaveDataManager
    - SceneManager
        - TitleScene
        - ConfigScene
        - ：
        - WorldScene
           - World
            - ControllerManager
            - ReplayManager
            - StageManager
            - PlayerManager		
       	     - Player
				- Blade
				- Overdrive
				- Dash
				- Shield
            - EnemyManager		
      	      - Enemy
     	       - EnemyState
            - AttackManager
  	          - Attack
            - GroundManager
         	   - Ground
            - IndicateManager	
            - ItemManager
            - ScoreManager
            - WorldPause		
            - WorldContinue		
            - WorldResult		
            - WorldReplayMenu	

- CollideSystem
- ConfigManager



### □ おやくそく

このコードの著作権はサークル[OBLIQUEGLASS]が保持しています。  

転載については、出典が明記されていれば自由に転載することができます。  
改変に関しては、上に加えて改変した内容の明記を必要とします。

このコードを利用して作られた著作物に関しては、  
商用・非商用の区別なく、制限なしに使用することができます。

このコードを使用したことで万一の損害が発生したとしても、  
その責任は負いかねます。ご了承ください。


----

### LibOgg ライセンス表示

Copyright (c) 2002-2004, Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


----

　制作・著作  
　2014. OBLIQUEGLASS  

■sorcery(代表)  
　制作/プログラム  
　
　　WEB：  
　　　http://d.hatena.ne.jp/sorcery/  
