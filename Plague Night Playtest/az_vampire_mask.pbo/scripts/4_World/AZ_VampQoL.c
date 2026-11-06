



class AZ_VampReportLog
{
	static const string FILE = "$profile:AZ_Vamp_bugreports.log";
	static const int COOLDOWN_S = 20;
	static const int DAY_MAX = 15;

	protected static ref map<string, ref array<int>> s_Recent;
	protected static ref map<string, int> s_DenyAt;
	protected static int s_Seq;

	static void Submit(PlayerBase player, PlayerIdentity id, int kind, string text)
	{
		if (!id || kind < 0 || kind >= AZ_VampQoL.KIND_COUNT)
			return;
		text.Replace("\n", " ");
		text.Replace("\r", " ");
		text.Replace("\t", " ");
		text.Replace("|", "/");
		text = text.Trim();
		if (text.LengthUtf8() < AZ_VampQoL.TEXT_MIN || text.LengthUtf8() > AZ_VampQoL.TEXT_MAX)
			return;

		if (!s_Recent)
			s_Recent = new map<string, ref array<int>>;
		string steam = id.GetPlainId();
		int now = GetGame().GetTime() / 1000;
		array<int> times = s_Recent.Get(steam);
		if (!times)
		{
			times = new array<int>;
			s_Recent.Insert(steam, times);
		}
		for (int i = times.Count() - 1; i >= 0; i--)
		{
			if (now - times[i] > 86400)
				times.Remove(i);
		}
		if ((times.Count() > 0 && now - times[times.Count() - 1] < COOLDOWN_S) || times.Count() >= DAY_MAX)
		{

			if (!s_DenyAt)
				s_DenyAt = new map<string, int>;
			if (!s_DenyAt.Contains(steam) || now - s_DenyAt.Get(steam) >= 30)
			{
				s_DenyAt.Set(steam, now);
				NotificationSystem.SendNotificationToPlayerIdentityExtended(id, 6, "Report not sent", "Too many reports. Try again in a minute.");
			}
			return;
		}
		times.Insert(now);

		int y, mo, d, h, mi, s;
		GetYearMonthDay(y, mo, d);
		GetHourMinuteSecond(h, mi, s);
		s_Seq++;
		string stamp = y.ToStringLen(4) + "-" + mo.ToStringLen(2) + "-" + d.ToStringLen(2) + " " + h.ToStringLen(2) + ":" + mi.ToStringLen(2) + ":" + s.ToStringLen(2);
		string pos = "";
		if (player)
			pos = player.GetPosition().ToString();
		string name = id.GetName();
		name.Replace("\n", " ");
		name.Replace("\r", " ");
		name.Replace("\t", " ");
		name.Replace("|", "/");
		string line = stamp + " | #" + s_Seq + " | " + AZ_VampQoL.KindLabel(kind) + " | " + steam + " | " + name + " | " + pos + " | " + text;

		FileHandle fh = OpenFile(FILE, FileMode.APPEND);
		if (fh != 0)
		{
			FPrintln(fh, line);
			CloseFile(fh);
		}
		else
		{
			Print("[AZ_VampQoL] Could not open " + FILE);
		}
		NotificationSystem.SendNotificationToPlayerIdentityExtended(id, 8, AZ_VampQoL.KindLabel(kind) + " received", "Thanks. The Plague Night team reads every one.");
	}
};

modded class PlayerBase
{
	int m_AZReportRpcAt;

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		if (rpc_type == AZ_VampQoL.RPC_REPORT)
		{
			if (GetGame().IsServer())
			{

				PlayerIdentity self = GetIdentity();
				if (!sender || !self || sender.GetId() != self.GetId())
					return;


				int rnow = GetGame().GetTime();
				if (rnow - m_AZReportRpcAt < 1500)
					return;
				m_AZReportRpcAt = rnow;
				int kind;
				string text;
				if (!ctx.Read(kind) || !ctx.Read(text))
					return;
				AZ_VampReportLog.Submit(this, sender, kind, text);
			}
			return;
		}
		super.OnRPC(sender, rpc_type, ctx);
	}


	override void CheckLiftWeapon()
	{
		if (GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT)
		{
			if (m_LiftWeapon_player || m_ObstructWeapon_player > 0)
				SendLiftWeaponSync(false);
		}
	}


	override void OnCommandSwimStart()
	{
		super.OnCommandSwimStart();
		GameInventory inv = GetInventory();
		if (inv)
			inv.UnlockInventory(LOCK_FROM_SCRIPT);
	}

	override bool CanReceiveItemIntoHands(EntityAI item_to_hands)
	{
		if (IsSwimming())
		{
			ItemBase item;
			if (ItemBase.CastTo(item, item_to_hands) && item.IsHeavyBehaviour())
				return false;
			return true;
		}
		return super.CanReceiveItemIntoHands(item_to_hands);
	}
};


modded class BandageDressing
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class DuctTape
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class EpoxyPutty
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class SewingKit
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class LeatherSewingKit
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class ElectronicRepairKit
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class WeaponCleaningKit
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};

modded class TireRepairKit
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
};
