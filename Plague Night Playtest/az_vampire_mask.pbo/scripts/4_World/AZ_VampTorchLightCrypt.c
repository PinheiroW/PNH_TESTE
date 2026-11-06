

modded class Torch
{
	override void UpdateLight()
	{
		super.UpdateLight();
		if (m_Light)
			m_Light.AZ_UpdateCrypt(this);
	}
}

modded class TorchLight
{
	static const float CRYPT_RADIUS = 9;
	static const float CRYPT_BRIGHTNESS = 1.6;
	protected Torch m_AZTorch;
	protected bool m_AZInCrypt;

	void AZ_UpdateCrypt(Torch source)
	{
		m_AZTorch = source;
		bool inside = AZ_VampCryptLights.InCryptBox(source.GetPosition());
		bool leaving = m_AZInCrypt && !inside;
		m_AZInCrypt = inside;
		if (!inside && !leaving)
			return;

		float fuelScale = 1;
		if (source.GetQuantity() <= source.m_StartFadeOutOfLightAtQuantityEx)
			fuelScale = Math.Max(source.GetQuantity() / source.m_StartFadeOutOfLightAtQuantityEx, source.m_StartFadeOutOfLightAtQuantityEx / 10);
		float radius = m_TorchRadius;
		float brightness = m_TorchBrightness;
		if (inside)
		{
			radius = CRYPT_RADIUS;
			brightness = CRYPT_BRIGHTNESS;
		}
		SetRadiusTo(radius * fuelScale);
		SetBrightnessTo(brightness * fuelScale);
	}

	override void OnFrameLightSource(IEntity other, float timeSlice)
	{
		super.OnFrameLightSource(other, timeSlice);
		if (!m_AZTorch || !m_AZTorch.GetCompEM() || !m_AZTorch.GetCompEM().IsWorking())
			return;
		bool inside = AZ_VampCryptLights.InCryptBox(m_AZTorch.GetPosition());
		if (inside != m_AZInCrypt)
			AZ_UpdateCrypt(m_AZTorch);
	}
}
