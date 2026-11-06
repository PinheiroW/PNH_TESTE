



class AZ_VampSound
{
	static const int RPC_KISS = 4702127;
	static const int RPC_SIGH = 4702189;
	static const int SIGH_DELAY_MS = 1500;
	static const int BITE_DELAY_MS = 900;

	static bool IsVampire(PlayerBase p)
	{
		if (!p)
			return false;
		EntityAI mask = p.FindAttachmentBySlotName("Mask");
		return mask && mask.IsKindOf("AZ_VampireMask_Base");
	}
}

modded class EmoteManager
{
	override bool PlayEmote(int id)
	{
		bool started = super.PlayEmote(id);

		bool fangs = id == EmoteConstants.ID_EMOTE_THROAT || id == EmoteConstants.ID_EMOTE_TAUNTKISS;

		bool fresh = m_Player && m_Player.m_AZVampBareUntil - GetGame().GetTime() > (AZ_VampState.Cfg().bareAtWillSec - 2) * 1000;
		if (started && fangs && !fresh && GetGame().IsServer() && AZ_VampSound.IsVampire(m_Player))
		{
			GetGame().RPCSingleParam(m_Player, AZ_VampSound.RPC_KISS, new Param1<bool>(true), true);
			AZ_VampReveal.BareAtWill(m_Player, AZ_VampState.Cfg().bareAtWillSec);
			m_Player.Message("You bare your fangs.", "colorAction");
		}
		return started;
	}
}

modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (rpc_type == AZ_VampSound.RPC_SIGH && GetGame().IsClient())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampPlaySigh, AZ_VampSound.SIGH_DELAY_MS, false);
			return;
		}
		if (rpc_type != AZ_VampSound.RPC_KISS || !GetGame().IsClient())
			return;


		Param1<bool> withBite = new Param1<bool>(true);
		if (!ctx.Read(withBite))
			withBite.param1 = true;
		string hiss = "AZ_VampHissF_SoundSet";
		if (IsMale())
			hiss = "AZ_VampHissM_SoundSet";
		// F06: one-shots destroy themselves when done; without it every hiss, bite and sigh stays registered
		EffectSound hissSnd = SEffectManager.PlaySoundOnObject(hiss, this);
		if (hissSnd)
			hissSnd.SetSoundAutodestroy(true);
		m_AZHissAt = GetGame().GetTime();
		m_AZSndLogUntil = GetGame().GetTime() + 3000;
		if (withBite.param1)
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(AZ_VampPlayBite, AZ_VampSound.BITE_DELAY_MS, false);
		Print("[AZ_Vamp] kiss sounds " + hiss + " bite " + withBite.param1);
	}

	int m_AZSndLogUntil;
	int m_AZHissAt = -100000;

	override void OnSoundEvent(string pEventType, string pUserString, int pUserInt)
	{
		if (GetGame().IsClient() && GetGame().GetTime() < m_AZSndLogUntil)
			Print("[AZ_Vamp] snd anim " + GetType() + " " + pEventType + " '" + pUserString + "' " + pUserInt);
		super.OnSoundEvent(pEventType, pUserString, pUserInt);
	}

	override bool PlaySoundEventEx(EPlayerSoundEventID id, bool from_anim_system = false, bool is_from_server = false, int param = 0)
	{
		if (GetGame().IsClient() && GetGame().GetTime() < m_AZSndLogUntil)
			Print("[AZ_Vamp] snd event " + GetType() + " " + typename.EnumToString(EPlayerSoundEventID, id) + " anim " + from_anim_system + " server " + is_from_server);
		return super.PlaySoundEventEx(id, from_anim_system, is_from_server, param);
	}

	void AZ_VampPlaySigh()
	{
		string sigh = "AZ_VampSighF_SoundSet";
		if (IsMale())
			sigh = "AZ_VampSighM_SoundSet";
		EffectSound sighSnd = SEffectManager.PlaySoundOnObject(sigh, this);
		if (sighSnd)
			sighSnd.SetSoundAutodestroy(true);
	}

	void AZ_VampPlayBite()
	{
		EffectSound biteSnd = SEffectManager.PlaySoundOnObject("AZ_VampBite_SoundSet", this);
		if (biteSnd)
			biteSnd.SetSoundAutodestroy(true);
	}
}
