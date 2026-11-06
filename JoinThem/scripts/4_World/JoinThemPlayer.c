// 0 survivor, 1 pending choice, 2 joined player, 3 reserved, 4 chosen Respawn, 5 joined death.
modded class PlayerBase
{
 int m_JTState;
 bool m_JTVisual;
 bool m_JTSkinned;
 float m_JTIdleVoiceAt;
 ref EffectSound m_JTVoice;
 ref EffectSound m_JTSkinSound;
 float m_JTAttackAt;
 float m_JTNextAI;
 ref PGFilter m_JTFilter;
 ref array<vector> m_JTPath = {};
 void PlayerBase()
 {
  RegisterNetSyncVariableInt("m_JTState",0,5);
  RegisterNetSyncVariableBool("m_JTSkinned");
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(JTTick,100,true);
 }
 override bool IsSkinned()
 {
  return m_JTSkinned || super.IsSkinned();
 }
 override void SetAsSkinned()
 {
  super.SetAsSkinned();
  if(GetGame().IsServer()){m_JTSkinned=true;SetSynchDirty();}
 }
 override void EEKilled(Object killer)
 {
  if(m_JTState==2 || m_JTState==3)
  {
   if(GetGame().IsServer()){JTVoice(2);m_JTState=5;SetSynchDirty();}
   m_CorpseState=0;
  }
  super.EEKilled(killer);
 }
 override void EEDelete(EntityAI parent)
 {
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(JTTick);
  if(m_JTSkinSound)m_JTSkinSound.SoundStop();
  super.EEDelete(parent);
 }
 override bool EEOnDamageCalculated(TotalDamageResult damageResult,int damageType,EntityAI source,int component,string dmgZone,string ammo,vector modelPos,float speedCoef)
 {
  if(GetGame().IsServer() && m_JTState==1)return false;
  if(GetGame().IsServer() && m_JTState==0 && IsAlive() && GetIdentity())
  {
   bool lethal=damageResult.GetDamage("","Health")>=GetHealth("","Health");
   if(dmgZone=="Head" || dmgZone=="Torso")lethal=lethal || damageResult.GetDamage(dmgZone,"Health")>=GetHealth(dmgZone,"Health");
   if(lethal){JTDown();return false;}
  }
  return super.EEOnDamageCalculated(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
 }
 void JTDown()
 {
  if(!GetGame().IsServer() || m_JTState!=0 || !IsAlive())return;
  m_JTState=1;SetHealth("","Health",1);SetHealth("","Shock",0);RequestUnconsciousness(true);SetAllowDamage(false);SetSynchDirty();
 }
 void JTWake(int state)
 {
  SetAllowDamage(true);
  m_JTState=state;
  m_JTIdleVoiceAt=GetGame().GetTime()*0.001+Math.RandomFloatInclusive(12,28);
  array<string> zones={};GetDamageZones(zones);
  foreach(string zone:zones)
  {
   float maximum=GetMaxHealth(zone,"Health");
   if(maximum>0)SetHealth(zone,"Health",maximum);
  }
  SetHealth("","Health",GetMaxHealth("","Health"));
  SetHealth("","Blood",GetMaxHealth("","Blood"));
  SetHealth("","Shock",GetMaxHealth("","Shock"));
  GetStatEnergy().Set(PlayerConstants.SL_ENERGY_MAX);
  GetStatWater().Set(PlayerConstants.SL_WATER_MAX);
  SetBrokenLegs(eBrokenLegs.NO_BROKEN_LEGS);
  ForceUpdateInjuredState();RequestUnconsciousness(false);
  if(GetBleedingManagerServer())GetBleedingManagerServer().RemoveAllSources();
  SetAllowDamage(true);SetSynchDirty();JTEmptyHands();
 }
 void JTEmptyHands()
 {
  EntityAI held=GetHumanInventory().GetEntityInHands();
  if(held && Edible_Base.Cast(held))return;
  if(held && !GetInventory().TakeEntityToInventory(InventoryMode.SERVER,FindInventoryLocationType.CARGO,held))GetInventory().DropEntity(InventoryMode.SERVER,this,held);
 }
 void JTChoose(bool join)
 {
  if(!GetGame().IsServer() || m_JTState!=1 || !IsAlive())return;
  if(join)
  {
   Print("[JoinThem] Join selected: reviving existing character.");
   JTWake(2);
   return;
  }
  // Ordinary native player death: keep the original corpse and inventory.
  // EEKilled/InsertCorpse handle CleanupLifetimeDeadPlayer from globals.xml.
  Print("[JoinThem] Respawn selected: normal player death, original gear retained.");
  m_JTState=4;
  SetAllowDamage(true);SetSynchDirty();SetHealth("","Health",0);
 }
 override bool CanReceiveItemIntoHands(EntityAI item_to_hands)
 {
  if(m_JTState==1)return false;
  if(m_JTState==2 && !Edible_Base.Cast(item_to_hands))return false;
  return super.CanReceiveItemIntoHands(item_to_hands);
 }

 override void OnItemInHandsChanged()
 {
  if(m_JTState==2)
  {
   GetItemAccessor().EnableAutoAnimInstUpdateOnHandsChange(true);
   m_JTVisual=false;
  }
  super.OnItemInHandsChanged();
 }
 override bool CanJump()
 {
  if(m_JTState==2 || m_JTState==3)
  {
   return false;
  }
  return super.CanJump();
 }
 override void OnRPC(PlayerIdentity sender,int rpc_type,ParamsReadContext ctx)
 {
  super.OnRPC(sender,rpc_type,ctx);
  if(rpc_type==981304)
  {
   Param1<bool> skinSoundData;
   if(!GetGame().IsServer() && ctx.Read(skinSoundData))JTPlaySkinSound(skinSoundData.param1);
   return;
  }
  if(rpc_type==981303)
  {
   Param1<int> voice;
   if(!GetGame().IsServer() && ctx.Read(voice))JTPlayVoice(voice.param1);
   return;
  }
  if(rpc_type!=981301 && rpc_type!=981302)return;
  if(!GetGame().IsServer() || !sender || !GetIdentity() || sender.GetId()!=GetIdentity().GetId())return;
  Param1<bool> data;
  if(!ctx.Read(data))return;
  if(rpc_type==981301)JTChoose(data.param1);
  else if(m_JTState==2)JTAttack(false);
 }
 void JTAttack(bool heavy)
 {
  float now=GetGame().GetTime()*0.001;
  if(GetHumanInventory().GetEntityInHands())return;
  if(!IsAlive() || IsUnconscious() || (!GetCommand_Move() && !GetCommand_Melee2()) || now<m_JTAttackAt || (m_JTState!=2 && m_JTState!=3))return;
  m_JTAttackAt=now+0.9;if(heavy)m_JTAttackAt=now+1.5;
  JTVoice(1);
  m_JTIdleVoiceAt=now+Math.RandomFloatInclusive(12,28);
  EntityAI target=JTTarget(1.9,true);
  int hit=0;if(heavy)hit=1;
  if(GetCommand_Move())StartCommand_Melee2(target,hit,0,GetPosition()+GetDirection()+"0 1 0");
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(JTContact,350,false,target,heavy);
 }
 EntityAI JTTarget(float radius,bool cone)
 {
  ref array<Object> objects={};ref array<CargoBase> cargo={};
  GetGame().GetObjectsAtPosition3D(GetPosition(),radius,objects,cargo);
  EntityAI best;float score=radius*radius;
  foreach(Object o:objects)
  {
   EntityAI entity=EntityAI.Cast(o);if(!entity || entity==this || !entity.IsAlive())continue;
   PlayerBase p=PlayerBase.Cast(entity);
   if(p && p.m_JTState>=1 && p.m_JTState<=3)continue;
   if(!p && !ZombieBase.Cast(entity))continue;
   vector toward=entity.GetPosition()-GetPosition();toward[1]=0;
   if(cone && vector.Dot(GetDirection(),toward.Normalized())<0.35)continue;
   float d=vector.DistanceSq(GetPosition(),entity.GetPosition());
   if(d<score && JTSight(entity)){best=entity;score=d;}
  }
  return best;
 }
 bool JTSight(EntityAI target)
 {
  ref array<ref RaycastRVResult> hits={};
  RaycastRVParams ray=new RaycastRVParams(GetPosition()+"0 1.4 0",target.GetPosition()+"0 1.1 0",this);
  ray.type=ObjIntersectView;ray.flags=CollisionFlags.NEARESTCONTACT;
  if(!DayZPhysics.RaycastRVProxy(ray,hits) || hits.Count()==0)return true;
  return hits[0].obj==target || hits[0].parent==target;
 }
 void JTContact(EntityAI target,bool heavy)
 {
  if(!GetGame().IsServer() || !IsAlive() || !GetCommand_Melee2() || !target || !target.IsAlive() || vector.DistanceSq(GetPosition(),target.GetPosition())>3.61 || !JTSight(target))return;
  PlayerBase player=PlayerBase.Cast(target);
  if(player && player.GetMeleeFightLogic().IsInBlock())
  {
   vector incoming=GetPosition()-player.GetPosition();incoming[1]=0;
   if(vector.Dot(player.GetDirection(),incoming.Normalized())>0.5)return;
  }
  JTStrike(target);
 }
 void JTStrike(EntityAI target)
 {
  if(!GetGame().IsServer() || !target || !target.IsAlive())return;
  PlayerBase victim=PlayerBase.Cast(target);
  if(victim)
  {
   // Vanilla police/press armor reduces the 40-HP strike to roughly 28-30 HP.
   // Strong plate protection gets an armor-piercing claw profile: about 20 HP.
   string ammo="JoinThemClaw";
   EntityAI vest=victim.FindAttachmentBySlotName("Vest");
   if(vest)
   {
    string path="CfgVehicles "+vest.GetType()+" DamageSystem GlobalArmor Melee Health damage";
    if(GetGame().ConfigIsExisting(path) && GetGame().ConfigGetFloat(path)<=0.35)ammo="JoinThemClawPlate";
   }
   DamageSystem.CloseCombatDamageName(this,target,"dmgZone_torso",ammo,target.ModelToWorld(target.GetDefaultHitPosition()));
   return;
  }
  DamageSystem.CloseCombatDamageName(this,target,target.GetHitComponentForAI(),"MeleeFist",target.ModelToWorld(target.GetDefaultHitPosition()));
 }
 // Animation event contacts are driven once by JTContact for the new clips.
 void JTApplyVisual()
 {
  if(m_JTVisual || (m_JTState!=2 && m_JTState!=3) || IsUnconscious() || !GetCommand_Move())return;
  if(Edible_Base.Cast(GetHumanInventory().GetEntityInHands()))return;
  if(m_DecayedTexture!="")
  {
   m_CorpseState=-PlayerConstants.CORPSE_STATE_DECAYED;
   UpdateCorpseStateVisual();
  }
  GetItemAccessor().EnableAutoAnimInstUpdateOnHandsChange(false);
  SetAnimationInstanceByName("JoinThem/anims/Joined.asi",0.25);
  m_JTVisual=true;
 }
 void JTVoice(int cue)
 {
  if(!GetGame().IsServer())return;
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(this,981303,new Param1<int>(cue),true);
  else JTPlayVoice(cue);
 }
 void JTPlayVoice(int cue)
 {
  if(GetGame().IsDedicatedServer())return;
  bool female=GetGame().ConfigGetInt("CfgVehicles "+GetType()+" woman")==1;
  string soundSetName="ZmbM_CitizenASkinny_Base_CalmMove_Soundset";
  if(female)soundSetName="ZmbF_CitizenANormal_Base_CalmMove_Soundset";
  if(cue==1)
  {
   soundSetName="ZmbM_Normal_Attack_Soundset";
   if(female)soundSetName="ZmbF_Normal_Attack_Soundset";
  }
  if(cue==2)
  {
   // Use the vanilla infected heavy-hit voice bank for the death cue.
   soundSetName="ZmbM_Normal_HeavyHit_Soundset";
   if(female)soundSetName="ZmbF_Normal_HeavyHit_Soundset";
  }
  if(m_JTVoice)m_JTVoice.SoundStop();
  m_JTVoice=SEffectManager.PlaySoundOnObject(soundSetName,this);
  if(m_JTVoice)m_JTVoice.SetAutodestroy(true);
 }
 override void CommandHandler(float pDt,int pCurrentCommandID,bool pCurrentCommandFinished)
 {
  bool joined=m_JTState==2 && IsAlive() && !GetHumanInventory().GetEntityInHands();
  if(joined && GetInputController())GetInputController().OverrideRaise(HumanInputControllerOverrideType.ONE_FRAME,false);
  super.CommandHandler(pDt,pCurrentCommandID,pCurrentCommandFinished);
  if(joined && !IsUnconscious() && GetCommand_Move())
  {
   // Visual gait only: do not change health, HUD injury level, or damage stats.
   GetCommandModifier_Additives().SetInjured(1.0,true);
  }
 }
 override void SetActions(out TInputActionMap InputActionMap)
 {
  super.SetActions(InputActionMap);
  AddAction(ActionJTSkinSurvivor,InputActionMap);
 }
 bool JTCanEatMeat(Edible_Base food)
 {
  if(!food || !food.IsMeat() || !food.CanBeConsumed())return false;
  if(food.IsFoodRaw())return true;
  return food.IsKindOf("HumanSteakMeat") && (food.IsFoodBaked() || food.IsFoodBoiled() || food.IsFoodDried());
 }
 override bool CanConsumeFood(ConsumeConditionData data = null)
 {
  if(m_JTState==2 && data && data.m_ConsumedItem)
  {
   Edible_Base edible=Edible_Base.Cast(data.m_ConsumedItem);
   if(!edible)return false;
   if(!edible.IsLiquidContainer() && !JTCanEatMeat(edible))return false;
  }
  return super.CanConsumeFood(data);
 }
 override bool Consume(PlayerConsumeData data)
 {
  if(m_JTState==2 && data && data.m_Source && (data.m_Type==EConsumeType.ITEM_SINGLE_TIME || data.m_Type==EConsumeType.ITEM_CONTINUOUS))
  {
   Edible_Base edible=Edible_Base.Cast(data.m_Source);
   if(!edible)return false;
   if(!edible.IsLiquidContainer())
   {
    if(!JTCanEatMeat(edible))return false;
    if(edible.IsKindOf("HumanSteakMeat"))
    {
     if(!GetGame().IsServer())return false;
     float amount=Math.Clamp(data.m_Amount,0,edible.GetQuantity());
     if(amount<=0 || edible.GetQuantityMax()<=0)return false;
     float fraction=amount/edible.GetQuantityMax();
     if(!edible.IsFoodRaw())fraction*=0.5;
     // Reward per full-sized steak, not per bite. Do not also add vanilla
     // human-meat nutrition or cannibal agents through the stomach.
     GetStatEnergy().Set(Math.Min(PlayerConstants.SL_ENERGY_MAX,GetStatEnergy().Get()+PlayerConstants.SL_ENERGY_MAX*0.30*fraction));
     SetHealth("","Health",Math.Min(GetMaxHealth("","Health"),GetHealth("","Health")+25*fraction));
     ForceUpdateInjuredState();
     edible.Consume(amount,this);
     return true;
    }
   }
  }
  return super.Consume(data);
 }
 void JTSkinSound(bool start)
 {
  if(!GetGame().IsServer())return;
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(this,981304,new Param1<bool>(start),true);
  else JTPlaySkinSound(start);
 }
 void JTPlaySkinSound(bool start)
 {
  if(GetGame().IsDedicatedServer())return;
  if(m_JTSkinSound){m_JTSkinSound.SoundStop();m_JTSkinSound=null;}
  if(start)m_JTSkinSound=SEffectManager.PlaySoundOnObject("animalSkinning_SoundSet",this,0,0,true);
 }
 void JTTick()
 {
  if(!GetGame() || !IsAlive())return;
  JTApplyVisual();
  if(!GetGame().IsServer())return;
  if(m_JTState==1){SetHealth("","Shock",0);return;}
  if(m_JTState!=2 && m_JTState!=3)return;
  JTEmptyHands();
  float voiceTime=GetGame().GetTime()*0.001;
  if(!IsUnconscious() && GetCommand_Move() && GetVelocity(this).LengthSq()<0.04 && voiceTime>=m_JTIdleVoiceAt)
  {
   JTVoice(0);m_JTIdleVoiceAt=voiceTime+Math.RandomFloatInclusive(12,28);
  }
 }
}







