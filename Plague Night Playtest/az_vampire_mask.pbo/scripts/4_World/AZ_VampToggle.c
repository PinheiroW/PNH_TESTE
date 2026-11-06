



class AZ_VampToggle
{
	static bool s_FangsHintShown;
	static const int RPC_FANGS = 4702186;
	static const int RPC_TIE = 4702622;
	static const int RPC_FANGS_TEST = 4702187;
	static const int RPC_WALK_TEST = 4702250;

	static void Toggle(PlayerBase p)
	{
		if (!p || !p.m_AZVampire || !p.IsAlive() || p.IsUnconscious())
			return;
		if (AZ_VampFeed.Cancel(p))
			return;
		if (GetGame().GetTime() < p.m_AZFeedCancelUntil)
			return;
		if (AZ_VampShadow.TryStep(p))
			return;
		int now = GetGame().GetTime();

		if (now < p.m_AZVampBareUntil && p.m_AZVampBareUntil - now > (AZ_VampState.Cfg().bareAtWillSec - 2) * 1000)
			return;
		if (now < p.m_AZVampBareUntil)
		{
			p.m_AZVampBareUntil = 0;
			AZ_VampReveal.Update(p);
			if (p.m_AZVampHungry)
				p.Message("Too hungry to hide your fangs.", "colorAction");
			else
				p.Message("You hide your fangs.", "colorAction");
			return;
		}
		GetGame().RPCSingleParam(p, AZ_VampSound.RPC_KISS, new Param1<bool>(false), true);
		AZ_VampReveal.BareAtWill(p, AZ_VampState.Cfg().bareAtWillSec);


		if (p.GetIdentity())
			GetGame().RPCSingleParam(p, AZ_VampToggle.RPC_FANGS, new Param1<bool>(true), true, p.GetIdentity());
		p.Message("You bare your fangs.", "colorAction");
		Print("[AZ_Vamp] fangs bared by key " + AZ_VampState.Uid(p));
	}
}

modded class PlayerBase
{
	int m_AZFangsRpcAt;
	int m_AZTieRpcAt;

	override void CommandHandler(float pDt, int pCurrentCommandID, bool pCurrentCommandFinished)
	{
		super.CommandHandler(pDt, pCurrentCommandID, pCurrentCommandFinished);
		if (!GetGame().IsClient() || !IsControlledPlayer() || !m_AZVampire)
			return;
		// Every fresh press reaches the server; release must bypass start throttles.
		if (GetGame().GetUIManager().GetMenu())
			return;
		UAInput tieKey = GetUApi().GetInputByName("UAAZVampTie");
		if (tieKey && tieKey.LocalPress())
			GetGame().RPCSingleParam(this, AZ_VampToggle.RPC_TIE, new Param1<bool>(true), true);
		UAInput key = GetUApi().GetInputByName("UAAZVampFangs");
		if (key && key.LocalPress())
		{
			if (!AZ_VampToggle.s_FangsHintShown)
			{
				AZ_VampToggle.s_FangsHintShown = true;
				Message("J shows or hides your fangs. To bite, get close and hold the on-screen Bite action.", "colorAction");
			}
			AZ_FangsPress();
		}
	}


	void AZ_FangsPress()
	{
		{






			GetGame().RPCSingleParam(this, AZ_VampToggle.RPC_FANGS, new Param1<bool>(true), true);
			Print("[AZ_Vamp] fangs key pressed");
		}
	}





	protected int m_AZFangsState;
	protected int m_AZFangsOutGen;
	static const int FANGS_OUT_WAIT_MS = 400;
	static const int FANGS_HISS_MS = 2000;

	static bool AZ_FangsOutFace(EntityAI item)
	{
		return item && (item.IsKindOf("AZ_VampireMask_Bared") || item.IsKindOf("AZ_VampireMask_Fed"));
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		if (!GetGame().IsClient() || slot_name != "Mask" || !item || !item.IsKindOf("AZ_VampireMask_Base"))
			return;
		int now = 1;
		if (AZ_FangsOutFace(item))
			now = 2;
		int was = m_AZFangsState;
		m_AZFangsState = now;
		if (was == 0 || was == now || !IsAlive())
			return;
		if (now == 2)
		{
			m_AZFangsOutGen++;
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_FangsOutSound, FANGS_OUT_WAIT_MS, false, m_AZFangsOutGen);
			return;
		}
		m_AZFangsOutGen++;
		SEffectManager.PlaySoundOnObject("AZ_VampFangsIn_SoundSet", this);
		Print("[AZ_Vamp] fangs in sound " + GetType());
	}


	void AZ_FangsOutSound(int gen)
	{
		if (gen != m_AZFangsOutGen || m_AZFangsState != 2 || !IsAlive())
			return;
		if (GetGame().GetTime() - m_AZHissAt < FANGS_HISS_MS)
			return;
		string hiss = "AZ_VampHissF_SoundSet";
		if (IsMale())
			hiss = "AZ_VampHissM_SoundSet";
		SEffectManager.PlaySoundOnObject(hiss, this);
		m_AZHissAt = GetGame().GetTime();
		Print("[AZ_Vamp] fangs out sound " + GetType());
	}


	void AZ_FangsHiss()
	{
		bool can = GetEmoteManager() && GetEmoteManager().CanPlayEmote(EmoteConstants.ID_EMOTE_TAUNTKISS);
		if (can)
			GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_TAUNTKISS);
		EntityAI cm = FindAttachmentBySlotName("Mask");
		string cmt = "none";
		if (cm)
			cmt = cm.GetType();
		Print("[AZ_Vamp] fangs hiss emote " + can + " client mask " + cmt);
	}


	void AZ_WalkTest(int ms)
	{
		HumanInputController hic = GetInputController();
		Print("[AZ_Vamp] walk test client: " + ms + " ms, input " + (hic != null));
		if (!hic)
			return;

		Camera cam = Camera.GetCurrentCamera();
		if (cam)
		{
			cam.SetActive(false);
			Print("[AZ_Vamp] walk test client: the shot camera released");
		}
		hic.OverrideMovementSpeed(HumanInputControllerOverrideType.ENABLED, 2);
		hic.OverrideMovementAngle(HumanInputControllerOverrideType.ENABLED, 0);
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_WalkTestEnd, ms, false);
	}

	void AZ_WalkTestEnd()
	{
		HumanInputController hic = GetInputController();
		if (!hic)
			return;
		hic.OverrideMovementSpeed(HumanInputControllerOverrideType.DISABLED, 0);
		hic.OverrideMovementAngle(HumanInputControllerOverrideType.DISABLED, 0);
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == AZ_VampToggle.RPC_FANGS_TEST && GetGame().IsClient() && IsControlledPlayer())
		{
			AZ_FangsPress();
			return;
		}
		if (rpc_type == AZ_VampToggle.RPC_WALK_TEST && GetGame().IsClient() && IsControlledPlayer())
		{
			Param1<int> wk = new Param1<int>(0);
			if (ctx.Read(wk))
				AZ_WalkTest(wk.param1);
			return;
		}
		if (rpc_type == AZ_VampToggle.RPC_FANGS && GetGame().IsClient() && IsControlledPlayer())
		{
			AZ_FangsHiss();
			return;
		}
		if (rpc_type == AZ_VampToggle.RPC_TIE && GetGame().IsServer())
		{
			if (!sender || !GetIdentity() || sender.GetId() != GetIdentity().GetId())
				return;
			PlayerBase tiePlayer = this;
			if (!tiePlayer.m_AZVampire || !tiePlayer.IsAlive() || tiePlayer.IsUnconscious())
				return;
			int tieNow = GetGame().GetTime();
			if (tieNow < m_AZTieRpcAt || tieNow < tiePlayer.m_AZFeedCancelUntil)
				return;
			m_AZTieRpcAt = tieNow + 300;
			// No binding while holding or chaining someone (the Fangs path cancelled the feed first, so this was never possible).
			AZ_VampShadowState tieChain = AZ_VampShadow.s_State.Get(tiePlayer);
			if (tiePlayer.m_AZVampFeedingVictim || tiePlayer.m_AZVampTimedVictim || (tieChain && (tieChain.victim || tieChain.animWaiting)))
				return;
			AZ_VampTie.Try(tiePlayer);
			return;
		}
		if (rpc_type != AZ_VampToggle.RPC_FANGS || !GetGame().IsServer())
			return;
		if (!sender || !GetIdentity() || sender.GetId() != GetIdentity().GetId())
			return;
		if (AZ_VampFeed.Cancel(this))
			return;
		int now = GetGame().GetTime();
		if (now < m_AZFangsRpcAt)
			return;
		m_AZFangsRpcAt = now + 300;
		AZ_VampToggle.Toggle(this);
	}
}
