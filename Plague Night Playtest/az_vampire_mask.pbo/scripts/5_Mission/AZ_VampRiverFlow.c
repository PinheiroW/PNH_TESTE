



modded class MissionGameplay
{
	protected static const int AZ_FLOW_FRAMES = 16;
	protected static const float AZ_FLOW_STEP = 0.14;
	protected ref array<EntityAI> m_AZRiver = new array<EntityAI>;
	protected float m_AZRiverScan;
	protected float m_AZRiverT;
	protected int m_AZRiverK;
	protected bool m_AZRiverGreen;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		AZRiverFlow(timeslice);
	}

	void AZRiverFlow(float dt)
	{
		PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
		if (!p || !(AZ_VampCryptLook.s_In || AZ_VampCrypt.Inside(p.GetPosition())))
		{
			m_AZRiverScan = 0;
			return;
		}
		m_AZRiverScan -= dt;
		if (m_AZRiverScan <= 0)
		{


			m_AZRiverScan = 10;
			m_AZRiver.Clear();
			array<Object> near = new array<Object>;
			GetGame().GetObjectsAtPosition3D(AZ_VampCrypt.Nave(28, 0), 32, near, null);
			foreach (Object o : near)
			{
				EntityAI e = EntityAI.Cast(o);
				if (e && (e.IsKindOf("AZ_Crypt_Blood_Nave") || e.IsKindOf("AZ_Crypt_Blood_RiverEnd")))
					m_AZRiver.Insert(e);
			}

			string seen = "";
			if (m_AZRiver.Count() > 0)
			{
				TStringArray mats = m_AZRiver[0].GetHiddenSelectionsMaterials();
				if (mats && mats.Count() > 0)
					seen = mats[0];
			}
			if (m_AZRiver.Count() >= 9)
				m_AZRiverScan = 120;
			Print("[AZ_Vamp] river flow: " + m_AZRiver.Count() + " surfaces of " + near.Count() + " objects near the hall, frame " + m_AZRiverK + ", config material " + seen);
		}



		if (!AZ_VampBand.Is("F"))
		{
			if (m_AZRiverK != 0 || m_AZRiverGreen)
			{
				m_AZRiverK = 0;
				m_AZRiverGreen = false;
				foreach (EntityAI still : m_AZRiver)
				{
					if (still)
					{
						still.SetObjectTexture(0, "azo\\az_vampire_mask\\data\\decor\\az_decor_cryptblood_co.paa");
						still.SetObjectMaterial(0, "azo\\az_vampire_mask\\data\\decor\\az_decor_cryptblood_flow_00.rvmat");
					}
				}
			}
			return;
		}
		m_AZRiverT -= dt;
		if (m_AZRiverT > 0)
			return;
		m_AZRiverT = AZ_FLOW_STEP;
		m_AZRiverK = (m_AZRiverK + 1) % AZ_FLOW_FRAMES;
		string k = m_AZRiverK.ToString();
		if (m_AZRiverK < 10)
			k = "0" + k;
		string mat = "azo\\az_vampire_mask\\data\\decor\\az_decor_cryptblood_flow_" + k + ".rvmat";



		bool swap = AZ_VAMP_HALLOWEEN_TEST && AZ_VampShotRpc.s_Label == "river_swap";
		foreach (EntityAI r : m_AZRiver)
		{
			if (!r)
				continue;
			if (swap)
				r.SetObjectTexture(0, "#(argb,8,8,3)color(0,1,0,1,co)");
			else
			{
				if (m_AZRiverGreen)
					r.SetObjectTexture(0, "azo\\az_vampire_mask\\data\\decor\\az_decor_cryptblood_co.paa");
				r.SetObjectMaterial(0, mat);
			}
		}
		m_AZRiverGreen = swap;
		if (swap && m_AZRiverK == 0)
			Print("[AZ_Vamp] river flow: swap test, " + m_AZRiver.Count() + " surfaces painted green");
	}
}
