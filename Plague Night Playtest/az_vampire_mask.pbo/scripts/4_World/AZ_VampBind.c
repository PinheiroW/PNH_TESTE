


class AZ_VampBind
{

	static void Bind(PlayerBase p)
	{
		if (!p || !GetGame().IsServer() || !p.IsAlive())
			return;
		p.m_AZTorpor = false;
		p.m_AZDownUntil = 0;
		p.m_AZTorporWeak = false;
		p.m_AZTorporWeakUntil = 0;
		p.m_AZVampire = true;
		p.SetSynchDirty();
		EntityAI worn = p.FindAttachmentBySlotName("Mask");
		if (worn && worn.IsKindOf("AZ_VampireMask_Base"))
			return;
		if (worn && !p.ServerDropEntity(worn))
			GetGame().ObjectDelete(worn);
		if (!p.GetInventory().CreateAttachment("AZ_VampireMask"))
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(BindRetry, 500, false, p);
		Print("[AZ_Vamp] bound " + AZ_VampState.Uid(p));
	}

	static void BindRetry(PlayerBase p)
	{
		if (p && p.IsAlive() && p.m_AZVampire && !p.FindAttachmentBySlotName("Mask"))
			p.GetInventory().CreateAttachment("AZ_VampireMask");
	}

	static void Unbind(PlayerBase p)
	{
		if (!p || !GetGame().IsServer())
			return;
		if (p.m_AZTorpor && p.IsAlive())
			AZ_VampDown.Wake(p);
		p.m_AZTorpor = false;
		p.m_AZTorporWeak = false;
		p.m_AZTorporWeakUntil = 0;
		p.m_AZHungerNoHealSaid = false;
		p.m_AZVampire = false;
		p.SetSynchDirty();
	}

	static bool LockedOn(EntityAI parent)
	{
		PlayerBase pb = PlayerBase.Cast(parent);
		if (!pb && parent)
			pb = PlayerBase.Cast(parent.GetHierarchyRootPlayer());
		return pb && pb.m_AZVampire;
	}
}

class AZ_VampireMask_Base extends Clothing
{
	override bool CanDetachAttachment(EntityAI parent)
	{
		if (AZ_VampBind.LockedOn(parent))
			return false;
		return super.CanDetachAttachment(parent);
	}

	override bool CanSwitchDuringAttach(EntityAI parent)
	{
		if (AZ_VampBind.LockedOn(parent))
			return false;
		return super.CanSwitchDuringAttach(parent);
	}

	override bool CanBeFSwaped()
	{
		if (AZ_VampBind.LockedOn(GetHierarchyParent()))
			return false;
		return super.CanBeFSwaped();
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (AZ_VampBind.LockedOn(GetHierarchyParent()))
			return false;
		return super.CanPutInCargo(parent);
	}
}

modded class PlayerBase
{
	bool m_AZVampire;


	int m_AZCfgBits;

	override void Init()
	{
		super.Init();
		RegisterNetSyncVariableBool("m_AZVampire");
		RegisterNetSyncVariableInt("m_AZCfgBits", 0, 31);
	}

	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (m_AZVampire && attachment && attachment.IsKindOf("AZ_VampireMask_Base"))
			return false;
		return super.CanReleaseAttachment(attachment);
	}
}
