class HumanTraderF_Journalist extends Human_Base
{
 static ref array<HumanTraderF_Journalist> Traders={};
 ref array<PlayerBase> HTAggressors={};
 PlayerBase HTSpeaker;
 vector HTAnchor;
 int HTToken;
 int HTSequence;
 float HTLease;
 float HTKeepAlive;
 ref EffectSound HTVoice;
 float HTNextSit=45;
 PlayerBase HTHeardPlayer;
 vector HTHeardPosition;
 float HTAttentionUntil;
 float HTHearAt;


 void HumanTraderF_Journalist(){Traders.Insert(this);}
 override bool HumanIsTrader(){return true;}
 override bool IsDanger(){return false;}
 override void HumanInitLoadout()
 {
  if(!HumanAuthority())return;
  HumanEquip(0);
  if(!GetInventory().FindAttachment(InventorySlots.MASK))GetInventory().CreateAttachmentEx("Shemag_Brown",InventorySlots.MASK);
 }
 override bool CanReceiveAttachment(EntityAI attachment,int slotId)
 {
  return slotId==InventorySlots.MASK && attachment && attachment.IsKindOf("Shemag_ColorBase");
 }
 override bool CanReleaseAttachment(EntityAI attachment){return !IsAlive();}
 override void HumanUpdateGroup(){m_HumanLeader=null;m_HumanWasGrouped=false;}
 override bool HumanLootTick(){return false;}
 override void HumanHear(vector position){}
 array<Object> TraderRayIgnore(bool traffic=false)
 {
  array<Object> ignore={this};IEntity child=GetChildren();
  while(child){Object object=Object.Cast(child);if(object)ignore.Insert(object);child=child.GetSibling();}
  if(traffic)
  {
   foreach(Human_Base human:HumanPopulation)
   {
    if(!human || human==this)continue;ignore.Insert(human);
    if(human.m_HumanWeapon)ignore.Insert(human.m_HumanWeapon);
   }
  }
  return ignore;
 }
 override bool HumanSight(EntityAI target,bool groupScan=false)
 {
  if(!target)return false;
  array<Object> ignore=TraderRayIgnore(groupScan);
  // A traffic scan may include the target; it is still a valid endpoint.
  ignore.RemoveItem(target);
  array<ref RaycastRVResult> hits={};
  RaycastRVParams ray=new RaycastRVParams(GetPosition()+"0 1.5 0",target.GetPosition()+"0 1.1 0",this);
  ray.flags=CollisionFlags.NEARESTCONTACT;ray.type=ObjIntersectView;
  if(!DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0)return true;
  return hits[0].obj==target || hits[0].parent==target;
 }
 override bool HumanClearSegment(vector start,vector end,float radius=0.25,bool ignoreTraffic=false)
 {
  array<Object> ignore=TraderRayIgnore(ignoreTraffic);array<ref RaycastRVResult> hits={};
  RaycastRVParams ray=new RaycastRVParams(start,end,this,radius);ray.type=ObjIntersectGeom;
  return !DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0;
 }
 override bool HumanCanRest()
 {
  if(HTSpeaker || m_HumanEnemy || HTAggressors.Count()>0 || m_HumanClock<HTAttentionUntil)return false;
  return GetGame().GetWeather().GetRain().GetActual()<0.1;
 }
 // Pick a routine only while fully standing. Sitting uses the original
 // entry / seated loop / exit phases; no transform or pose teleport.
 void ChooseRoutine(int roll=-1)
 {
  if(roll<0)roll=Math.RandomInt(0,100);
  m_HumanStageEnd=m_HumanClock+Math.RandomFloatInclusive(30,75);
  if(roll<25 && m_HumanClock>=HTNextSit && HumanCanRest())
  {
   m_HumanStage=1;m_HumanRestStyle=1;m_HumanRestPhase=1;m_HumanRestTime=0;m_HumanWake=false;
   m_HumanHeading=GetInputController().GetHeading();m_HumanStageEnd=m_HumanClock+Math.RandomFloatInclusive(20,40);
   HTNextSit=m_HumanClock+Math.RandomFloatInclusive(120,240);return;
  }
  if(roll<65)
  {
   m_HumanStage=3;
   float radius=HumanTraderEconomy.Load().WanderRadius;
   m_HumanGoal=HTAnchor+Vector(Math.RandomFloatInclusive(-radius,radius),0,Math.RandomFloatInclusive(-radius,radius));
   m_HumanGoal[1]=GetGame().SurfaceY(m_HumanGoal[0],m_HumanGoal[2]);return;
  }
  m_HumanStage=0;
 }
 void ClearAttention()
 {
  HTAttentionUntil=0;HTHeardPlayer=null;
  DayZInfectedInputController input=GetInputController();if(input)input.OverrideLookAt(false,vector.Zero);
 }
 bool HearFootsteps()
 {
  HumanTradersConfig config=HumanTraderEconomy.Load();
  if(config.HearingRange<=0)return false;
  if(m_HumanClock>=HTHearAt)
  {
   HTHearAt=m_HumanClock+0.4;float nearest=config.HearingRange*config.HearingRange;
   array<Man> players={};GetGame().GetPlayers(players);
   foreach(Man man:players)
   {
    PlayerBase player=PlayerBase.Cast(man);
    if(!player || !player.GetIdentity() || !player.IsAlive() || player.IsUnconscious() || player.GetCommand_Vehicle())continue;
    float distance=vector.DistanceSq(player.GetPosition(),GetPosition());
    HumanMovementState state=new HumanMovementState;player.GetMovementState(state);
    if(distance>nearest || !HumanAudibleMovement(state.m_iStanceIdx,state.m_iMovement,distance))continue;
    nearest=distance;HTHeardPlayer=player;HTHeardPosition=player.GetPosition();HTAttentionUntil=m_HumanClock+4;
   }
  }
  if(!HTHeardPlayer || !HTHeardPlayer.IsAlive() || m_HumanClock>=HTAttentionUntil)return false;
  m_HumanWake=true;if(m_HumanRestPhase<3)return false;
  HumanFace(HTHeardPosition);
  GetInputController().OverrideMovementSpeed(true,0);
  return true;
 }
 override void EOnInit(IEntity other,int extra)
 {
  super.EOnInit(other,extra);HTAnchor=GetPosition();
  if(HumanAuthority()){if(GetAIAgent())GetAIAgent().SetKeepInIdle(true);SetLifetime(3888000);SetLifetimeMax(3888000);UpdateSimulationPrecision(0);}
 }
 void PruneAggressors()
 {
  float radius=HumanTraderEconomy.Load().DefenseRadius;
  for(int i=HTAggressors.Count() - 1;i>=0;i--)
  {
   PlayerBase player=HTAggressors[i];
   if(!player || !player.IsAlive() || vector.DistanceSq(player.GetPosition(),HTAnchor)>radius*radius)HTAggressors.Remove(i);
  }
  if(m_HumanEnemy && HTAggressors.Find(PlayerBase.Cast(m_HumanEnemy))<0)
  {
   HumanTacticalBrain().Memories.Clear();HumanTacticalBrain().DropFocus();m_HumanInvestigateUntil=0;
  }
 }
 override bool HumanHostile(EntityAI target)
 {
  PlayerBase player=PlayerBase.Cast(target);
  if(!player || !player.IsAlive() || HTAggressors.Find(player)<0)return false;
  float radius=HumanTraderEconomy.Load().DefenseRadius;
  return vector.DistanceSq(player.GetPosition(),HTAnchor)<=radius*radius;
 }
 override void HumanScan()
 {
  PruneAggressors();float best=100000;PlayerBase chosen;
  foreach(PlayerBase attacker:HTAggressors)
  {
   float distance=vector.DistanceSq(GetPosition(),attacker.GetPosition());
   if(distance<best && HumanSight(attacker)){chosen=attacker;best=distance;}
  }
  if(chosen){m_HumanEnemy=chosen;m_HumanLastKnown=chosen.GetPosition();m_HumanSeen=m_HumanClock;HumanTacticalBrain().Remember(chosen,m_HumanLastKnown);}
 }
 override void HumanTravel(vector goal,float speed)
 {
  if(HTSpeaker)return;
  float radius=HumanTraderEconomy.Load().WanderRadius;if(m_HumanEnemy)radius=HumanTraderEconomy.Load().DefenseRadius;
  vector direction=goal-HTAnchor;direction[1]=0;
  if(radius<=0 && !m_HumanEnemy)return;
  if(direction.Length()>radius){direction.Normalize();goal=HTAnchor+direction*radius;goal[1]=GetGame().SurfaceY(goal[0],goal[2]);}
  super.HumanTravel(goal,speed);
 }
 override bool HumanRangedTick(float dt,int command)
 {
  PruneAggressors();
  if(m_HumanClock>=HTKeepAlive){HTKeepAlive=m_HumanClock+5;if(GetAIAgent())GetAIAgent().SetKeepInIdle(true);SetLifetime(3888000);SetLifetimeMax(3888000);UpdateSimulationPrecision(0);}
  if(HTSpeaker)
  {
   if(!HTSpeaker.IsAlive() || !HTSpeaker.GetIdentity() || vector.DistanceSq(HTSpeaker.GetPosition(),GetPosition())>25 || GetGame().GetTime()*0.001>HTLease || HTAggressors.Count()>0)
   {EndConversation(true);return false;}
   m_HumanWake=true;m_HumanStage=0;m_HumanStageEnd=m_HumanClock+20;
   if(m_HumanRestPhase<3)return false; // Complete the civilian get-up animation.
   GetInputController().OverrideMovementSpeed(true,0);HumanFace(HTSpeaker.GetPosition());
   DayZInfectedCommandMove move=GetCommand_Move();if(move){m_StanceVariation=2;move.SetStanceVariation(2);move.SetIdleState(2);}
   return true;
  }
  if(!m_HumanEnemy)
  {
   if(HearFootsteps())return true;
   if(m_HumanRestPhase==3 && m_HumanClock>=m_HumanStageEnd)ChooseRoutine();
  }
  return false;
 }
 bool CanTalk(PlayerBase player)
 {
  return player && player.IsAlive() && !player.IsUnconscious() && IsAlive() && HumanTraderEconomy.Load().traderspawn==1 && HTAggressors.Count()==0 && vector.DistanceSq(GetPosition(),player.GetPosition())<=16 && HumanSight(player);
 }
 void Reply(PlayerBase player,string message,bool closed=false)
 {
  if(!player || !player.GetIdentity())return;
  HTReply response;
  if(closed){response=new HTReply;response.Closed=true;response.Message=message;response.Token=HTToken;GetNetworkID(response.TraderLow,response.TraderHigh);}
  else response=HumanTraderEconomy.Snapshot(player,this,message);
  if(!GetGame().IsMultiplayer()){HTClient.Reply=response;HTClient.Ready=true;}
  else GetGame().RPCSingleParam(player,HTClient.REPLY_RPC,new Param1<ref HTReply>(response),true,player.GetIdentity());
 }
 void Voice(int cue)
 {
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(this,HTClient.VOICE_RPC,new Param1<int>(cue),true);else PlayVoice(cue);
 }
 void PlayVoice(int cue)
 {
  if(GetGame().IsDedicatedServer())return;
  if(HTVoice)HTVoice.SoundStop();
  string sound="Humans_TraderFGreet"+cue.ToString()+"_SoundSet";
  if(cue==5)sound="Humans_TraderFGoodbye1_SoundSet";
  HTVoice=SEffectManager.PlaySoundOnObject(sound,this);if(HTVoice)HTVoice.SetAutodestroy(true);
 }
 override void OnRPC(PlayerIdentity sender,int rpc_type,ParamsReadContext ctx)
 {
  super.OnRPC(sender,rpc_type,ctx);
  if(rpc_type==HTClient.VOICE_RPC && !GetGame().IsServer()){Param1<int> cue;if(ctx.Read(cue))PlayVoice(cue.param1);}
 }
 void EndConversation(bool notify=false)
 {
  PlayerBase speaker=HTSpeaker;HTSpeaker=null;HTLease=0;
  if(speaker){if(notify)Reply(speaker,"Conversation ended.",true);Voice(5);}
  m_HumanStageEnd=m_HumanClock+Math.RandomFloatInclusive(5,12);
 }
 void Request(PlayerBase player,HTRequest request)
 {
  if(!HumanAuthority() || !player || !player.GetIdentity())return;
  if(request.Operation==0)
  {
   PruneAggressors();
   if(!CanTalk(player)){Reply(player,"She cannot trade right now.",true);return;}
   if(HTSpeaker && HTSpeaker!=player){Reply(player,"She is speaking with someone else.",true);return;}
   if(!HTSpeaker){ClearAttention();HTSpeaker=player;HTToken=Math.RandomIntInclusive(1,2000000000);HTSequence=0;Voice(Math.RandomIntInclusive(1,4));}
   HTLease=GetGame().GetTime()*0.001+20;m_HumanWake=true;HumanCancelLoot();Reply(player,"Welcome. What do you need?");return;
  }
  if(HTSpeaker!=player || request.Token!=HTToken)return;
  if(request.Sequence<=HTSequence)return;
  HTSequence=request.Sequence;
  if(request.Operation==4){EndConversation(false);return;}
  if(!CanTalk(player)){EndConversation(true);return;}
  HTLease=GetGame().GetTime()*0.001+20;
  HTWallet wallet=HumanTraderEconomy.Wallet(HumanTraderEconomy.PlayerId(player));
  string message="";
  if(request.Operation==1)message=HumanTraderEconomy.Buy(player,request.Index,wallet);
  else if(request.Operation==2)
  {
   ItemBase item=ItemBase.Cast(GetGame().GetObjectByNetworkId(request.ItemLow,request.ItemHigh));
   message=HumanTraderEconomy.Sell(player,item,wallet,request.ExpectedSalePrice);
  }
  else if(request.Operation!=3)return;
  Reply(player,message);
 }
 override bool EEOnDamageCalculated(TotalDamageResult damageResult,int damageType,EntityAI source,int component,string dmgZone,string ammo,vector modelPos,float speedCoef)
 {
  EntityAI root=source;if(source && source.GetHierarchyRootPlayer())root=source.GetHierarchyRootPlayer();
  if(ZombieBase.Cast(root) || AnimalBase.Cast(root) || Human_Base.Cast(root))return false;
  return super.EEOnDamageCalculated(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
 }
 override void EEHitBy(TotalDamageResult damageResult,int damageType,EntityAI source,int component,string dmgZone,string ammo,vector modelPos,float speedCoef)
 {
  PlayerBase attacker;if(source)attacker=PlayerBase.Cast(source.GetHierarchyRootPlayer());if(!attacker)attacker=PlayerBase.Cast(source);
  if(HumanAuthority() && attacker){ClearAttention();if(HTAggressors.Find(attacker)<0)HTAggressors.Insert(attacker);EndConversation(true);}
  super.EEHitBy(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
 }
 override void EEKilled(Object killer)
 {
  if(HumanAuthority()){EndConversation(true);if(HumanTraderSpawner.Instance)HumanTraderSpawner.Instance.Died(this);}
  super.EEKilled(killer);
 }
 override void EEDelete(EntityAI parent)
 {
  if(HumanAuthority())EndConversation(true);
  if(HTVoice)HTVoice.SoundStop();Traders.RemoveItem(this);super.EEDelete(parent);
 }
}
class HumanTraderSpawner
{
 static ref HumanTraderSpawner Instance;
 HumanTraderF_Journalist NPC;
 vector Position;
 float Orientation;
 float SpawnAt;
 bool Waiting;
 bool AutoSurface;
 void Start()
 {
  HumanTradersConfig config=HumanTraderEconomy.Load();if(config.traderspawn!=1 || config.Locations.Count()==0)return;
  int index=0;if(config.traderrandomlocations==1)index=Math.RandomInt(0,config.Locations.Count());
  HTLocation location=config.Locations[index];if(!location)return;
  Position=location.Position;Orientation=location.Orientation;AutoSurface=Position[1]==0;
  if(Position[1]==0)Position[1]=GetGame().SurfaceY(Position[0],Position[2]);
  Spawn();GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Tick,1000,true);
 }
 void Spawn()
 {
  int flags=ECE_INITAI|ECE_NOLIFETIME;if(AutoSurface)flags=flags|ECE_PLACE_ON_SURFACE;
  NPC=HumanTraderF_Journalist.Cast(GetGame().CreateObjectEx("HumanTraderF_Journalist",Position,flags));
  if(NPC){NPC.SetOrientation(Vector(Orientation,0,0));NPC.HTAnchor=NPC.GetPosition();Waiting=false;Print("[HumanTraders] Trader spawned at "+Position.ToString());}
  else {Waiting=true;SpawnAt=GetGame().GetTime()*0.001+30;Print("[HumanTraders] Spawn failed; retrying in 30 seconds.");}
 }
 void Died(HumanTraderF_Journalist trader)
 {
  if(trader!=NPC || Waiting)return;
  Waiting=true;SpawnAt=GetGame().GetTime()*0.001+HumanTraderEconomy.Load().RespawnMinutes*60;
  Print("[HumanTraders] Trader died; respawn scheduled at the same restart-selected location.");
 }
 void Tick()
 {
  if(Waiting){if(GetGame().GetTime()*0.001>=SpawnAt)Spawn();return;}
  if(NPC && !NPC.IsAlive()){Died(NPC);return;}
  if(!NPC){Print("[HumanTraders] Live trader was removed externally; restoring at selected location.");Spawn();return;}
  NPC.SetLifetime(3888000);NPC.UpdateSimulationPrecision(0);
 }
 void Stop(){GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Tick);}
}
