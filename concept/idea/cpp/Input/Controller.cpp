/*==============================================================================
	[Controller.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Network/NetworkManager.h"
#include "../../h/Input/InputManager.h"
#include "../../h/Input/Controller.h"

using namespace std;

struct PacketData NullInputData;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Controller::Controller():
	joystickID_(0U),
	bKeyboard_(false),
	up_(0U),
	down_(0U),
	left_(0U),
	right_(0U),
	lAxisX_(0.0f),
	lAxisY_(0.0f),
	rAxisX_(0.0f),
	rAxisY_(0.0f),
	lTrigger_(0.0f),
	rTrigger_(0.0f),
	bVibration_(false),
	vibrationTime_(0U),
	cMode_(CONNECT_NONE),
	rMode_(REPLAY_NONE),
	delay_(0U)
{
	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0;
		config_[i] = i;
		keyConfig_[i] = KeyCode::KEY_CODE_UNUSED;
	}

	ZeroMemory(&inputLog_, sizeof(inputLog_));

	vector<struct InputLog>().swap(replayLog_);
}

//------------------------------------------------------------------------------
// 初期化
// 引数　：ジョイスティック番号(int JoystickID),キーボードの使用フラグ(bool bKeyboard)
// 　　　　振動の使用フラグ(bool bVibratio), キーログの保存フラグ(bool bLog)
// 　　　　遅延(unsigned int delay),リプレイモード(enum ReplayMode rMode)
// 　　　　通信モード(enum ConnectMode cMode)
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::Init(unsigned int JoystickID, bool bKeyboard, bool bVibration, bool bLog, unsigned int delay, enum ReplayMode rMode, enum ConnectMode cMode)
{
	// ゼロクリア
	up_			= 0U;
	down_		= 0U;
	left_		= 0U;
	right_		= 0U;
	lAxisX_		= 0.0f;
	lAxisY_		= 0.0f;
	rAxisX_		= 0.0f;
	rAxisY_		= 0.0f;
	lTrigger_	= 0.0f;
	rTrigger_	= 0.0f;
	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0;
		config_[i] = i;
		keyConfig_[i] = KeyCode::KEY_CODE_UNUSED;
	}
	vibrationTime_ = 0U;

	// ログの初期化
	ZeroMemory(&inputLog_, sizeof(inputLog_));
	replayLog_.clear();
	itReplayLog_ = replayLog_.begin();

	bVibration_ = bVibration;
	bLog_ = bLog;
	joystickID_ = (JoystickID <= InputManager::JOYSTIC_MAX) ? JoystickID : 0U;
	bKeyboard_ = bKeyboard;
	rMode_ = rMode;
	cMode_ = cMode;
	delay_ = (delay < 30U) ? delay : 0U;

	if(rMode_ == REPLAY_REC){
		replayLog_.resize(10800);
	}

	if(cMode_ == CONNECT_SEND){
		struct PacketData* pid = NetworkManager::Instance().GetSendData();
		for(int i = 30 - 1; i >= 0; --i){
			pid->id[i].keyData = 0;
			pid->id[i].lAxisX = 0;
			pid->id[i].lAxisY = 0;
			pid->id[i].rAxisX = 0;
			pid->id[i].rAxisY = 0;
			pid->id[i].lTrigger = 0;
			pid->id[i].rTrigger = 0;
		}
	}

	InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
}

//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::UnInit()
{
	InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
	// 使用フラグをオフに
	joystickID_ = 0U;
	bKeyboard_ = false;
	cMode_ = CONNECT_NONE;
	rMode_ = REPLAY_NONE;
	ZeroMemory(&inputLog_, sizeof(inputLog_));
}

//------------------------------------------------------------------------------
// 刺さっているジョイスティックの数の取得
// 引数　：なし
// 戻り値：刺さっているジョイスティックの数
//------------------------------------------------------------------------------
int Controller::GetJoystickMax()const
{
	return InputManager::Instance().GetJoystickNum();	// InputManagerの仲介をする
}

//------------------------------------------------------------------------------
// 入力のクリア
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::ZeroClear()
{
	// ゼロクリア
	up_			= 0U;
	down_		= 0U;
	left_		= 0U;
	right_		= 0U;
	lAxisX_		= 0.0f;
	lAxisY_		= 0.0f;
	rAxisX_		= 0.0f;
	rAxisY_		= 0.0f;
	lTrigger_	= 0.0f;
	rTrigger_	= 0.0f;
	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0;
	}
	vibrationTime_ = 0U;
}

//------------------------------------------------------------------------------
// 矢印キーの入力方向の取得(正規化済み)
// 引数　：なし
// 戻り値：矢印キーの入力方向(正規化済み)
//------------------------------------------------------------------------------
Vector2D Controller::GetArrow() const
{
	Vector2D v;

	v.x = ((up_) ? 1.0f : 0.0f + (down_) ? -1.0f : 0.0f);
	v.y = ((right_) ? 1.0f : 0.0f + (left_) ? -1.0f : 0.0f);

	return v.Normalized();
}

//------------------------------------------------------------------------------
// 入力情報の更新
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::Update()
{
	if(joystickID_ || bKeyboard_ || rMode_ == REPLAY_PLAY || cMode_ == CONNECT_RECEIVE){	// 使用フラグがオフなら何もしない
		if(rMode_ != REPLAY_PLAY && cMode_ != CONNECT_RECEIVE){
			InputManager& im = InputManager::Instance();
			Joystick& js = im.GetJoystick((DWORD)(joystickID_ - 1));

			bool bUp = (js.up || (bKeyboard_ && im.IsKeyPress(DIK_UP)));
			bool bDown = (js.down || (bKeyboard_ && im.IsKeyPress(DIK_DOWN)));
			bool bLeft = (js.left || (bKeyboard_ && im.IsKeyPress(DIK_LEFT)));
			bool bRight = (js.right || (bKeyboard_ && im.IsKeyPress(DIK_RIGHT)));
			bool bButton[BUTTON_MAX] = {};
			for(int i = BUTTON_MAX - 1; i >= 0; --i){
				bButton[i] = (js.button[config_[i]] || (bKeyboard_ && IsKeyPress(keyConfig_[i])));
			}

			lAxisX_	= js.lAxisX;
			lAxisY_	= js.lAxisY;
			rAxisX_	= js.rAxisX;
			rAxisY_	= js.rAxisY;
			lTrigger_	= js.lTrigger;
			rTrigger_	= js.rTrigger;

			// リプレイデータの保存
			if(rMode_ == REPLAY_REC){
				struct InputLog id = {0};
				if(bUp)		{ id.keyData |= 0x8000; }
				if(bDown)	{ id.keyData |= 0x4000; }
				if(bLeft)	{ id.keyData |= 0x2000; }
				if(bRight)	{ id.keyData |= 0x1000; }
				if(bButton[0]){ id.keyData |= 0x800; }
				if(bButton[1]){ id.keyData |= 0x400; }
				if(bButton[2]){ id.keyData |= 0x200; }
				if(bButton[3]){ id.keyData |= 0x100; }
				if(bButton[4]){ id.keyData |= 0x80; }
				if(bButton[5]){ id.keyData |= 0x40; }
				if(bButton[6]){ id.keyData |= 0x20; }
				if(bButton[7]){ id.keyData |= 0x10; }
				if(bButton[8]){ id.keyData |= 0x8; }
				if(bButton[9]){ id.keyData |= 0x4; }

				id.lAxisX	= (short)(lAxisX_ * 32767);
				id.lAxisY	= (short)(lAxisY_ * 32767);
				id.rAxisX	= (short)(rAxisX_ * 32767);
				id.rAxisY	= (short)(rAxisY_ * 32767);
				id.lTrigger = (short)(lTrigger_ * 32767);
				id.rTrigger = (short)(rTrigger_ * 32767);

				replayLog_.push_back(id);
			}

			// 入力データの転送
			if(cMode_ == CONNECT_SEND){
				struct PacketData* pid = NetworkManager::Instance().GetSendData();
				for(int i = 30 - 1; i >= 1; --i){
					pid->id[i].keyData = pid->id[i - 1].keyData;
					pid->id[i].lAxisX = pid->id[i - 1].lAxisX;
					pid->id[i].lAxisY = pid->id[i - 1].lAxisY;
					pid->id[i].rAxisX = pid->id[i - 1].rAxisX;
					pid->id[i].rAxisY = pid->id[i - 1].rAxisY;
					pid->id[i].lTrigger = pid->id[i - 1].lTrigger;
					pid->id[i].rTrigger = pid->id[i - 1].rTrigger;
				}

				short keyData = 0;
				if(bUp)		{ keyData |= 0x8000; }
				if(bDown)	{ keyData |= 0x4000; }
				if(bLeft)	{ keyData |= 0x2000; }
				if(bRight)	{ keyData |= 0x1000; }
				if(bButton[0]){ keyData |= 0x800; }
				if(bButton[1]){ keyData |= 0x400; }
				if(bButton[2]){ keyData |= 0x200; }
				if(bButton[3]){ keyData |= 0x100; }
				if(bButton[4]){ keyData |= 0x80; }
				if(bButton[5]){ keyData |= 0x40; }
				if(bButton[6]){ keyData |= 0x20; }
				if(bButton[7]){ keyData |= 0x10; }
				if(bButton[8]){ keyData |= 0x8; }
				if(bButton[9]){ keyData |= 0x4; }
				pid->id[0].keyData = keyData;

				pid->id[0].lAxisX	= (short)(lAxisX_ * 32767);
				pid->id[0].lAxisY	= (short)(lAxisY_ * 32767);
				pid->id[0].rAxisX	= (short)(rAxisX_ * 32767);
				pid->id[0].rAxisY	= (short)(rAxisY_ * 32767);
				pid->id[0].lTrigger = (short)(lTrigger_ * 32767);
				pid->id[0].rTrigger = (short)(rTrigger_ * 32767);
			}

			// ログの更新
			if(bLog_){
				for(int i = 30 - 1; i >= 1; --i){
					inputLog_[i].keyData = inputLog_[i - 1].keyData;
					inputLog_[i].lAxisX = inputLog_[i - 1].lAxisX;
					inputLog_[i].lAxisY = inputLog_[i - 1].lAxisY;
					inputLog_[i].rAxisX = inputLog_[i - 1].rAxisX;
					inputLog_[i].rAxisY = inputLog_[i - 1].rAxisY;
					inputLog_[i].lTrigger = inputLog_[i - 1].lTrigger;
					inputLog_[i].rTrigger = inputLog_[i - 1].rTrigger;
				}

				inputLog_[0].keyData = 0;
				if(bUp)		{ inputLog_[0].keyData |= 0x8000; }
				if(bDown)	{ inputLog_[0].keyData |= 0x4000; }
				if(bLeft)	{ inputLog_[0].keyData |= 0x2000; }
				if(bRight)	{ inputLog_[0].keyData |= 0x1000; }
				if(bButton[0]){ inputLog_[0].keyData |= 0x800; }
				if(bButton[1]){ inputLog_[0].keyData |= 0x400; }
				if(bButton[2]){ inputLog_[0].keyData |= 0x200; }
				if(bButton[3]){ inputLog_[0].keyData |= 0x100; }
				if(bButton[4]){ inputLog_[0].keyData |= 0x80; }
				if(bButton[5]){ inputLog_[0].keyData |= 0x40; }
				if(bButton[6]){ inputLog_[0].keyData |= 0x20; }
				if(bButton[7]){ inputLog_[0].keyData |= 0x10; }
				if(bButton[8]){ inputLog_[0].keyData |= 0x8; }
				if(bButton[9]){ inputLog_[0].keyData |= 0x4; }

				inputLog_[0].lAxisX = (short)(lAxisX_ * 32767);
				inputLog_[0].lAxisY = (short)(lAxisY_ * 32767);
				inputLog_[0].rAxisX = (short)(rAxisX_ * 32767);
				inputLog_[0].rAxisY = (short)(rAxisY_ * 32767);
				inputLog_[0].lTrigger = (short)(lTrigger_ * 32767);
				inputLog_[0].rTrigger = (short)(rTrigger_ * 32767);
			}
			// 入力されていたらフレーム数を加算,されていなければ0にする
			if(bLog_ && delay_){
				++up_;
				if(!(inputLog_[delay_].keyData & 0x8000)){ up_ = 0; }
				++down_;
				if(!(inputLog_[delay_].keyData & 0x4000)){ down_ = 0; }
				++left_;
				if(!(inputLog_[delay_].keyData & 0x2000)){ left_ = 0; }
				++right_;
				if(!(inputLog_[delay_].keyData & 0x1000)){ right_ = 0; }
				++button_[0];
				if(!(inputLog_[delay_].keyData & 0x800)){ button_[0] = 0; }
				++button_[1];
				if(!(inputLog_[delay_].keyData & 0x400)){ button_[1] = 0; }
				++button_[2];
				if(!(inputLog_[delay_].keyData & 0x200)){ button_[2] = 0; }
				++button_[3];
				if(!(inputLog_[delay_].keyData & 0x100)){ button_[3] = 0; }
				++button_[4];
				if(!(inputLog_[delay_].keyData & 0x80)){ button_[4] = 0; }
				++button_[5];
				if(!(inputLog_[delay_].keyData & 0x40)){ button_[5] = 0; }
				++button_[6];
				if(!(inputLog_[delay_].keyData & 0x20)){ button_[6] = 0; }
				++button_[7];
				if(!(inputLog_[delay_].keyData & 0x10)){ button_[7] = 0; }
				++button_[8];
				if(!(inputLog_[delay_].keyData & 0x8)){ button_[8] = 0; }
				++button_[9];
				if(!(inputLog_[delay_].keyData & 0x4)){ button_[9] = 0; }
				lAxisX_	= inputLog_[delay_].lAxisX;
				lAxisY_	= inputLog_[delay_].lAxisY;
				rAxisX_	= inputLog_[delay_].rAxisX;
				rAxisY_	= inputLog_[delay_].rAxisY;
				lTrigger_	= inputLog_[delay_].lTrigger;
				rTrigger_	= inputLog_[delay_].rTrigger;
			}else{
				++up_;
				if(!bUp){ up_ = 0; }
				++down_;
				if(!bDown){ down_ = 0; }
				++left_;
				if(!bLeft){ left_ = 0; }
				++right_;
				if(!bRight){ right_ = 0; }
				for(int i = BUTTON_MAX - 1; i >= 0; --i){
					++button_[i];
					if(!bButton[i]){ button_[i] = 0;}
				}
			}
		}else if(rMode_ == REPLAY_PLAY){
			if(itReplayLog_ != replayLog_.end()){
				// 入力されていたらフレーム数を加算,されていなければ0にする
				++up_;
				if(!(itReplayLog_->keyData & 0x8000)){ up_ = 0; }
				++down_;
				if(!(itReplayLog_->keyData & 0x4000)){ down_ = 0; }
				++left_;
				if(!(itReplayLog_->keyData & 0x2000)){ left_ = 0; }
				++right_;
				if(!(itReplayLog_->keyData & 0x1000)){ right_ = 0; }

				lAxisX_	= itReplayLog_->lAxisX / 32767.0f;
				lAxisY_	= itReplayLog_->lAxisY / 32767.0f;
				rAxisX_	= itReplayLog_->rAxisX / 32767.0f;
				rAxisY_	= itReplayLog_->rAxisY / 32767.0f;
				lTrigger_	= itReplayLog_->lTrigger / 32767.0f;
				rTrigger_	= itReplayLog_->rTrigger / 32767.0f;

				++button_[0];
				if(!(itReplayLog_->keyData & 0x800)){ button_[0] = 0; }
				++button_[1];
				if(!(itReplayLog_->keyData & 0x400)){ button_[1] = 0; }
				++button_[2];
				if(!(itReplayLog_->keyData & 0x200)){ button_[2] = 0; }
				++button_[3];
				if(!(itReplayLog_->keyData & 0x100)){ button_[3] = 0; }
				++button_[4];
				if(!(itReplayLog_->keyData & 0x80)){ button_[4] = 0; }
				++button_[5];
				if(!(itReplayLog_->keyData & 0x40)){ button_[5] = 0; }
				++button_[6];
				if(!(itReplayLog_->keyData & 0x20)){ button_[6] = 0; }
				++button_[7];
				if(!(itReplayLog_->keyData & 0x10)){ button_[7] = 0; }
				++button_[8];
				if(!(itReplayLog_->keyData & 0x8)){ button_[8] = 0; }
				++button_[9];
				if(!(itReplayLog_->keyData & 0x4)){ button_[9] = 0; }

				// ログの更新
				for(int i = 30 - 1; i >= 1; --i){
					inputLog_[i].keyData = inputLog_[i - 1].keyData;
					inputLog_[i].lAxisX = inputLog_[i - 1].lAxisX;
					inputLog_[i].lAxisY = inputLog_[i - 1].lAxisY;
					inputLog_[i].rAxisX = inputLog_[i - 1].rAxisX;
					inputLog_[i].rAxisY = inputLog_[i - 1].rAxisY;
					inputLog_[i].lTrigger = inputLog_[i - 1].lTrigger;
					inputLog_[i].rTrigger = inputLog_[i - 1].rTrigger;
				}
				inputLog_[0].keyData	= itReplayLog_->keyData;
				inputLog_[0].lAxisX	= itReplayLog_->lAxisX;
				inputLog_[0].lAxisY	= itReplayLog_->lAxisY;
				inputLog_[0].rAxisX	= itReplayLog_->rAxisX;
				inputLog_[0].rAxisY	= itReplayLog_->rAxisY;
				inputLog_[0].lTrigger	= itReplayLog_->lTrigger;
				inputLog_[0].rTrigger	= itReplayLog_->rTrigger;

				++itReplayLog_;
			}
		}else if(cMode_ == CONNECT_RECEIVE){
			if(NetworkManager::Instance().IsConnect()){
				struct PacketData* pid =  NetworkManager::Instance().GetReceiveData();
				// 入力されていたらフレーム数を加算,されていなければ0にする
				int f = min(29, max(-(int)delay_, (int)((long long)NetworkManager::Instance().GetReceiveData()->Serial - (long long)NetworkManager::Instance().GetSendData()->Serial)) + delay_);
				++up_;
				if(!(pid->id[f].keyData & 0x8000)){ up_ = 0; }
				++down_;
				if(!(pid->id[f].keyData & 0x4000)){ down_ = 0; }
				++left_;
				if(!(pid->id[f].keyData & 0x2000)){ left_ = 0; }
				++right_;
				if(!(pid->id[f].keyData & 0x1000)){ right_ = 0; }

				lAxisX_	= pid->id[f].lAxisX / 32767.0f;
				lAxisY_	= pid->id[f].lAxisY / 32767.0f;
				rAxisX_	= pid->id[f].rAxisX / 32767.0f;
				rAxisY_	= pid->id[f].rAxisY / 32767.0f;
				lTrigger_	= pid->id[f].lTrigger / 32767.0f;
				rTrigger_	= pid->id[f].rTrigger / 32767.0f;

				++button_[0];
				if(!(pid->id[f].keyData & 0x800)){ button_[0] = 0; }
				++button_[1];
				if(!(pid->id[f].keyData & 0x400)){ button_[1] = 0; }
				++button_[2];
				if(!(pid->id[f].keyData & 0x200)){ button_[2] = 0; }
				++button_[3];
				if(!(pid->id[f].keyData & 0x100)){ button_[3] = 0; }
				++button_[4];
				if(!(pid->id[f].keyData & 0x80)){ button_[4] = 0; }
				++button_[5];
				if(!(pid->id[f].keyData & 0x40)){ button_[5] = 0; }
				++button_[6];
				if(!(pid->id[f].keyData & 0x20)){ button_[6] = 0; }
				++button_[7];
				if(!(pid->id[f].keyData & 0x10)){ button_[7] = 0; }
				++button_[8];
				if(!(pid->id[f].keyData & 0x8)){ button_[8] = 0; }
				++button_[9];
				if(!(pid->id[f].keyData & 0x4)){ button_[9] = 0; }

				// ログの更新
				for(int i = 30 - 1; i >= 0; --i){
					inputLog_[i].keyData = pid->id[i].keyData;
					inputLog_[i].lAxisX = pid->id[i].lAxisX;
					inputLog_[i].lAxisY = pid->id[i].lAxisY;
					inputLog_[i].rAxisX = pid->id[i].rAxisX;
					inputLog_[i].rAxisY = pid->id[i].rAxisY;
					inputLog_[i].lTrigger = pid->id[i].lTrigger;
					inputLog_[i].rTrigger = pid->id[i].rTrigger;
				}

				// 入力データの保存
				if(rMode_ == REPLAY_REC){
					struct InputLog id = {0};

					if(!!up_)		{ id.keyData |= 0x8000; }
					if(!!down_)	{ id.keyData |= 0x4000; }
					if(!!left_)	{ id.keyData |= 0x2000; }
					if(!!right_)	{ id.keyData |= 0x1000; }
					if(!!button_[0]){ id.keyData |= 0x800; }
					if(!!button_[1]){ id.keyData |= 0x400; }
					if(!!button_[2]){ id.keyData |= 0x200; }
					if(!!button_[3]){ id.keyData |= 0x100; }
					if(!!button_[4]){ id.keyData |= 0x80; }
					if(!!button_[5]){ id.keyData |= 0x40; }
					if(!!button_[6]){ id.keyData |= 0x20; }
					if(!!button_[7]){ id.keyData |= 0x10; }
					if(!!button_[8]){ id.keyData |= 0x8; }
					if(!!button_[9]){ id.keyData |= 0x4; }

					id.lAxisX	= (short)(lAxisX_ * 32767);
					id.lAxisY	= (short)(lAxisY_ * 32767);
					id.rAxisX	= (short)(rAxisX_ * 32767);
					id.rAxisY	= (short)(rAxisY_ * 32767);
					id.lTrigger = (short)(lTrigger_ * 32767);
					id.rTrigger = (short)(rTrigger_ * 32767);

					replayLog_.push_back(id);
				}
			}else{
				up_		= 0;
				down_		= 0;
				left_		= 0;
				right_		= 0;
				lAxisX_	= 0.0f;
				lAxisY_	= 0.0f;
				rAxisX_	= 0.0f;
				rAxisY_	= 0.0f;
				lTrigger_	= 0.0f;
				rTrigger_	= 0.0f;
				for(int i = BUTTON_MAX - 1; i >= 0; --i){ button_[i] = 0; }

				// ログの更新
				for(int i = 30 - 1; i >= 1; --i){
					inputLog_[i].keyData = inputLog_[i - 1].keyData;
					inputLog_[i].lAxisX = inputLog_[i - 1].lAxisX;
					inputLog_[i].lAxisY = inputLog_[i - 1].lAxisY;
					inputLog_[i].rAxisX = inputLog_[i - 1].rAxisX;
					inputLog_[i].rAxisY = inputLog_[i - 1].rAxisY;
					inputLog_[i].lTrigger = inputLog_[i - 1].lTrigger;
					inputLog_[i].rTrigger = inputLog_[i - 1].rTrigger;
				}
				inputLog_[0].keyData	= 0;
				inputLog_[0].lAxisX	= 0;
				inputLog_[0].lAxisY	= 0;
				inputLog_[0].rAxisX	= 0;
				inputLog_[0].rAxisY	= 0;
				inputLog_[0].lTrigger	= 0;
				inputLog_[0].rTrigger	= 0;

				// 入力データの保存
				if(rMode_ == REPLAY_REC){
					struct InputLog id = {0};
					replayLog_.push_back(id);
				}
			}
		}
	}
	if(vibrationTime_){
		--vibrationTime_;
		if(!vibrationTime_){
			InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
		}
	}
}

//------------------------------------------------------------------------------
// ボタンの入力フレーム数の取得
// 引数　：ボタン番号(int buttonID)
// 戻り値：ボタンの入力フレーム数
//------------------------------------------------------------------------------
unsigned int Controller::GetButton(unsigned int buttonID)const
{
	if(BUTTON_MAX <= buttonID){ return 0; }	// 存在しないボタンは0を返す
	return button_[buttonID];
}

//------------------------------------------------------------------------------
// 指定したキーが押されているか
// 引数　：キーコード(enum keyCode keyCode)
// 戻り値：指定したキーが押されているか
//------------------------------------------------------------------------------
bool Controller::IsKeyPress(enum KeyCode keyCode)const
{
	return InputManager::Instance().IsKeyPress((DWORD)keyCode);	// InputManagerの仲介をする
}

//------------------------------------------------------------------------------
// 振動の開始
// 引数　：振動時間(enum keyCode keyCode),左の速度(float leftSpee)
// 　　　　右の速度(float rightSpeed)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Controller::SetVibration(unsigned int time, float leftSpeed, float rightSpeed)
{
	if(bVibration_){
		vibrationTime_ = time;
		return InputManager::Instance().SetVibration(joystickID_, leftSpeed, rightSpeed);
	}
	return false;
}

//------------------------------------------------------------------------------
// 上キー入力のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：入力されていたか
//------------------------------------------------------------------------------
bool Controller::GetLogUp(unsigned int backFrame)const
{
	if(backFrame > 30U){ return false; }
	return !!(inputLog_[backFrame].keyData & 0x8000);
}

//------------------------------------------------------------------------------
// 下キー入力のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：入力されていたか
//------------------------------------------------------------------------------
bool Controller::GetLogDown(unsigned int backFrame)const
{
	if(backFrame > 30U){ return false; }
	return !!(inputLog_[backFrame].keyData & 0x4000);
}

//------------------------------------------------------------------------------
// 左キー入力のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：入力されていたか
//------------------------------------------------------------------------------
bool Controller::GetLogLeft(unsigned int backFrame)const
{
	if(backFrame > 30U){ return false; }
	return !!(inputLog_[backFrame].keyData & 0x2000);
}

//------------------------------------------------------------------------------
// 右キー入力のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：入力されていたか
//------------------------------------------------------------------------------
bool Controller::GetLogRight(unsigned int backFrame)const
{
	if(backFrame > 30U){ return false; }
	return !!(inputLog_[backFrame].keyData & 0x1000);
}

//------------------------------------------------------------------------------
// 左アナログスティックX軸のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：軸のログ
//------------------------------------------------------------------------------
float Controller::GetLogLAxisX(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].lAxisX / 32767.0f;
}

//------------------------------------------------------------------------------
// 左アナログスティックY軸のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：軸のログ
//------------------------------------------------------------------------------
float Controller::GetLogLAxisY(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].lAxisY / 32767.0f;
}

//------------------------------------------------------------------------------
// 右アナログスティックX軸のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：軸のログ
//------------------------------------------------------------------------------
float Controller::GetLogRAxisX(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].rAxisX / 32767.0f;
}

//------------------------------------------------------------------------------
// 右アナログスティックY軸のログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：軸のログ
//------------------------------------------------------------------------------
float Controller::GetLogRAxisY(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].rAxisY / 32767.0f;
}

//------------------------------------------------------------------------------
// 左トリガーのログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：トリガーのログ
//------------------------------------------------------------------------------
float Controller::GetLogLTrigger(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].lTrigger / 32767.0f;
}

//------------------------------------------------------------------------------
// 右トリガーのログの取得
// 引数　：遡るフレーム数(unsigned int backFrame)
// 戻り値：トリガーのログ
//------------------------------------------------------------------------------
float Controller::GetLogRTrigger(unsigned int backFrame)const
{
	if(backFrame > 30U){ return 0.0f; }
	return inputLog_[backFrame].rTrigger / 32767.0f;
}

//------------------------------------------------------------------------------
// ボタン入力のログの取得
// 引数　：ボタン番号(int buttonID),遡るフレーム数(unsigned int backFrame)
// 戻り値：入力されていたか
//------------------------------------------------------------------------------
bool Controller::GetLogButton(unsigned int buttonID, unsigned int backFrame)const
{
	if(backFrame > 30U){ return false; }

	switch(buttonID){
	case 0:
		return !!(inputLog_[backFrame].keyData & 0x800);
	case 1:
		return !!(inputLog_[backFrame].keyData & 0x400);
	case 2:
		return !!(inputLog_[backFrame].keyData & 0x200);
	case 3:
		return !!(inputLog_[backFrame].keyData & 0x100);
	case 4:
		return !!(inputLog_[backFrame].keyData & 0x80);
	case 5:
		return !!(inputLog_[backFrame].keyData & 0x40);
	case 6:
		return !!(inputLog_[backFrame].keyData & 0x20);
	case 7:
		return !!(inputLog_[backFrame].keyData & 0x10);
	case 8:
		return !!(inputLog_[backFrame].keyData & 0x8);
	case 9:
		return !!(inputLog_[backFrame].keyData & 0x4);
	default:
		return false;
	}
}

//------------------------------------------------------------------------------
// 指定したジョイスティックボタンをボタン番号に関連付ける
// 引数　：コンフィグ番号(int configID),ジョイスティックボタン番号(int buttonID)
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::SetConfig(unsigned int configID, unsigned int buttonID)
{
	if(BUTTON_MAX <= buttonID || BUTTON_MAX <= configID){ return; }	// 存在しないボタンを指定されたら終了

	config_[configID] = buttonID;	// コンフィグ情報の更新
}

//------------------------------------------------------------------------------
// 指定したキーをボタン番号に関連付ける
// 引数　：コンフィグ番号(int configID),キーコード(enum keyCode keyCode)
// 戻り値：なし
//------------------------------------------------------------------------------
void Controller::SetConfig(unsigned int configID, enum KeyCode keyCode)
{
	if(BUTTON_MAX <= configID || KeyCode::KEY_CODE_MAX <= keyCode || keyCode < KeyCode::KEY_CODE_UNUSED){ return; }	// 存在しないボタンを指定されたら終了

	keyConfig_[configID] = keyCode;	// コンフィグ情報の更新
}

//------------------------------------------------------------------------------
// ボタン番号に関連付けられたジョイスティックボタンの番号を取得する
// 引数　：コンフィグ番号(int configID)
// 戻り値：ジョイスティックボタン番号
//------------------------------------------------------------------------------
int Controller::GetConfig(unsigned int configID)const
{
	if(BUTTON_MAX <= configID){ return -1; }	// 存在しないボタンを指定されたら終了

	return config_[configID];	// コンフィグ情報を返す
}

//------------------------------------------------------------------------------
// リプレイデータの保存
// 引数　：保存するファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Controller::SaveReplay(const char* pFileName)
{
	if(rMode_ != REPLAY_REC){ return false; }

	FILE* fp;
	if(fopen_s(&fp, pFileName, "wb") != 0){ return false; }

	std::vector<struct InputLog>::iterator it = replayLog_.begin(), itEnd = replayLog_.end();
	for(; it != itEnd; ++it){
		if(fwrite(&*it, sizeof(struct InputLog), 1, fp) == 0){
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	return true;
}

//------------------------------------------------------------------------------
// リプレイデータの読み込み
// 引数　：読み込むファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Controller::LoadReplay(const char* pFileName)
{
	FILE* fp;
	if(fopen_s(&fp, pFileName, "rb") != 0){ return false; }

	replayLog_.clear();

	struct InputLog buf = {0};
	while(!feof(fp)){
		ZeroMemory(&buf, sizeof(buf));
		if((fread(&buf, 1, sizeof(buf), fp)) == 0 ){ break; }
		replayLog_.push_back(buf);
	}

	fclose(fp);

	itReplayLog_ = replayLog_.begin();

	return true;
}

//------------------------------------------------------------------------------
// リプレイが終了したかどうか
// 引数　：なし
// 戻り値：終了したかどうか
//------------------------------------------------------------------------------
bool Controller::IsFinishReplay()const
{
	if(rMode_ != REPLAY_PLAY){ return true; }
	return (itReplayLog_ == replayLog_.end());
}