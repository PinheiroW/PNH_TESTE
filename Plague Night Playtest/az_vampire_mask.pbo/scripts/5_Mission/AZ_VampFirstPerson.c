


class AZ_VampFP
{




	static bool OwnEyeView(PlayerBase p)
	{
		if (!p || p.IsInThirdPerson())
			return false;
		int hb = p.GetBoneIndexByName("Head");
		if (hb < 0)
			return true;
		return vector.Distance(GetGame().GetCurrentCameraPosition(), p.GetBonePositionWS(hb)) < 0.18;
	}
}

modded class MissionGameplay
{
	EntityAI m_AZVampFPItem;
	bool m_AZVampFPHidden;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		EntityAI item;
		if (p)
			item = p.FindAttachmentBySlotName("Mask");
		if (item && !item.IsKindOf("AZ_VampireMask_Base"))
			item = null;
		bool hide = item && AZ_VampFP.OwnEyeView(p);

		if (item != m_AZVampFPItem)
		{
			if (m_AZVampFPItem && m_AZVampFPHidden)
				m_AZVampFPItem.SetInvisible(false);
			m_AZVampFPItem = item;
			m_AZVampFPHidden = false;
		}
		if (item && hide != m_AZVampFPHidden)
		{
			item.SetInvisible(hide);
			m_AZVampFPHidden = hide;
			Print("[AZ_Vamp] fp fangs hidden " + hide + " cam to head " + vector.Distance(GetGame().GetCurrentCameraPosition(), p.GetBonePositionWS(p.GetBoneIndexByName("Head"))));
		}
	}
}
