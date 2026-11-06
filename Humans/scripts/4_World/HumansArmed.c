// Armed Humans keep the infected rig; all shots and decisions are server-owned.
class HumanArmed_Base extends Human_Base
{
 Weapon_Base m_ArmedPistol;
 Magazine m_ArmedMagazine;
 int m_ArmedRounds=15;
 bool m_ArmedDrawn=true;
 bool m_ArmedCrouched;
 bool m_ArmedBandaging;
 bool m_ArmedBleeding;
 float m_ArmedNextShot;
 float m_ArmedReloadUntil;
 float m_ArmedActionUntil;
 int m_ArmedAction;
 float m_ArmedBandageUntil;
 float m_ArmedBandageAt;
 float m_ArmedSuppressedUntil;
 float m_ArmedCoverAt;
 float m_ArmedCoverUntil;
 vector m_ArmedCover;
 vector m_ArmedPeek;
 vector m_ArmedCoverThreat;
 bool m_ArmedHasCover;
 bool m_ArmedPeeking;
 float m_ArmedPeekAt;
 float m_ArmedExposeUntil;
 float m_ArmedBleedTick;
 int m_ArmedShotCount;
 int m_ArmedReloadCount;
 static const int ARMED_FX_RPC=874140;
 void HumanArmed_Base()
 {
  RegisterNetSyncVariableBool("m_ArmedDrawn");
  RegisterNetSyncVariableBool("m_ArmedCrouched");
  RegisterNetSyncVariableBool("m_ArmedBandaging");
  RegisterNetSyncVariableBool("m_ArmedPoseHidden");
  RegisterNetSyncVariableInt("m_ArmedRounds",0,15);
 }
 override void HumanInitLoadout()
 {
  if(!HumanAuthority() || !IsAlive())return;
  HumanEquip(0);
  if(m_ArmedPistol)return;
  int hand=GetBoneIndexByName("RightHand_Dummy");
  if(hand<0)return;
  m_ArmedPistol=Weapon_Base.Cast(GetGame().CreateObjectEx("Glock19","0 0 0",ECE_CREATEPHYSICS|ECE_NOLIFETIME|ECE_DYNAMIC_PERSISTENCY));
  if(!m_ArmedPistol)return;
  vector grip[4];
  grip[0]="0.990232667 -0.086590665 -0.109276359";
  grip[1]="0.090191821 -0.199861629 0.975664268";
  grip[2]="-0.106323568 -0.975990464 -0.190099744";
  grip[3]="-0.028270891 0.020225109 -0.01";
  m_ArmedPistol.SetTransform(grip);
  if(!AddChild(m_ArmedPistol,hand)){GetGame().ObjectDelete(m_ArmedPistol);m_ArmedPistol=null;return;}
  m_ArmedPistol.SetTakeable(false);m_ArmedPistol.SetAllowDamage(false);m_ArmedPistol.SetJammed(false);
  m_ArmedMagazine=Magazine.Cast(m_ArmedPistol.GetInventory().CreateAttachment("Mag_Glock_15Rnd"));
  if(m_ArmedMagazine){m_ArmedMagazine.ServerSetAmmoCount(m_ArmedRounds);m_ArmedMagazine.SetAllowDamage(false);}
  GetGame().RemoteObjectTreeDelete(m_ArmedPistol);GetGame().RemoteObjectTreeCreate(m_ArmedPistol);
  SetSynchDirty();
 }
 override int HumanActiveWeapon(){return 0;}
 override bool HumanCanRest(){return false;}
 vector m_ArmedRetreatGoal;
 float m_ArmedRetreatUntil;
 float m_ArmedRetreatAt;
 bool ArmedStartRetreat()
 {
  if(!HumanHostile(m_HumanEnemy) || m_HumanClock<m_ArmedRetreatAt || !GetCommand_Move())return false;
  vector away=GetPosition()-m_HumanEnemy.GetPosition();away[1]=0;away.Normalize();
  vector side=Vector(away[2],0,-away[0]);
  for(int i=0;i<3;i++)
  {
   vector wanted=GetPosition()+away*3.8;
   if(i==1)wanted=wanted+side*2;
   if(i==2)wanted=wanted-side*2;
   if(!HumanNavigation().GroundPoint(GetPosition(),wanted,m_ArmedRetreatGoal,true,false))continue;
   m_ArmedRetreatUntil=m_HumanClock+2.5;m_ArmedRetreatAt=m_HumanClock+4;m_HumanSwingTarget=null;
   return true;
  }
  m_ArmedRetreatAt=m_HumanClock+1;return false;
 }

 float m_ArmedPoseReadyAt;
 bool m_ArmedPoseHidden;
 void ArmedMovePose()
 {
  if (!m_ArmedDrawn && !m_ArmedBandaging) return;
  int stance=0;if(m_ArmedCrouched)stance=1;if(m_ArmedBandaging)stance=3;
  // Native command completion restores this field before the next AI tick.
  m_StanceVariation=stance;
  DayZInfectedCommandMove move=GetCommand_Move();
  if(move){move.SetStanceVariation(stance);move.SetIdleState(2);}
 }
 void ArmedPresentation()
 {
  if(HumanAuthority())
  {
   bool hidden=m_ArmedBandaging || m_HumanVaulting || m_HumanVaultPending || m_HumanDoorAction || m_HumanClock<m_ArmedPoseReadyAt;
   if(hidden!=m_ArmedPoseHidden){m_ArmedPoseHidden=hidden;SetSynchDirty();}
   ArmedMovePose();
  }
  ArmedVisual();
 }
 override void OnVariablesSynchronized()
 {
  super.OnVariablesSynchronized();ArmedVisual();
 }
 void ArmedVisual()
 {
  if(!m_ArmedPistol)
  {
   IEntity child=GetChildren();
   while(child){Weapon_Base gun=Weapon_Base.Cast(child);if(gun && gun.IsKindOf("Glock19")){m_ArmedPistol=gun;break;}child=child.GetSibling();}
  }
  if(m_ArmedPistol)m_ArmedPistol.SetInvisible(IsAlive() && (!m_ArmedDrawn || m_ArmedBandaging || m_ArmedPoseHidden));
 }
 override bool ModCommandHandlerBefore(float pDt,int pCurrentCommandID,bool pCurrentCommandFinished)
 {
  bool result=super.ModCommandHandlerBefore(pDt,pCurrentCommandID,pCurrentCommandFinished);
  ArmedPresentation();
  return result;
 }
 void ArmedPose(bool drawn,bool crouched,bool bandaging=false)
 {
  if(m_ArmedDrawn==drawn && m_ArmedCrouched==crouched && m_ArmedBandaging==bandaging)return;
  if(m_ArmedBandaging && !bandaging){m_ArmedPoseReadyAt=m_HumanClock+0.5;m_ArmedPoseHidden=true;}
  m_ArmedDrawn=drawn;m_ArmedCrouched=crouched;m_ArmedBandaging=bandaging;
  ArmedMovePose();
  SetSynchDirty();ArmedVisual();
 }
 override bool HumanClearSegment(vector start,vector end,float radius=0.25,bool ignoreTraffic=false)
 {
  array<Object> ignore={this};if(m_ArmedPistol)ignore.Insert(m_ArmedPistol);
  if(ignoreTraffic)
  {
   foreach(Human_Base human:HumanPopulation)
   {
    if(!human || human==this)continue;ignore.Insert(human);
    if(human.m_HumanWeapon)ignore.Insert(human.m_HumanWeapon);
    HumanArmed_Base armed=HumanArmed_Base.Cast(human);if(armed && armed.m_ArmedPistol)ignore.Insert(armed.m_ArmedPistol);
   }
   if(m_HumanEnemy)ignore.Insert(m_HumanEnemy);
  }
  array<ref RaycastRVResult> hits={};RaycastRVParams ray=new RaycastRVParams(start,end,this,radius);ray.type=ObjIntersectGeom;
  return !DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0;
 }
 override bool HumanSight(EntityAI target,bool groupScan=false)
 {
  if(!target)return false;
  array<Object> ignore={this};if(m_ArmedPistol)ignore.Insert(m_ArmedPistol);
  if(groupScan)
  {
   foreach(Human_Base human:HumanPopulation)
   {
    if(!human || human==this || human==target)continue;ignore.Insert(human);
    if(human.m_HumanWeapon)ignore.Insert(human.m_HumanWeapon);
    HumanArmed_Base armed=HumanArmed_Base.Cast(human);if(armed && armed.m_ArmedPistol)ignore.Insert(armed.m_ArmedPistol);
   }
  }
  vector origin=GetPosition()+"0 1.5 0";if(m_ArmedCrouched)origin=GetPosition()+"0 1 0";
  vector aim=target.ModelToWorld(target.GetDefaultHitPosition());if(PlayerBase.Cast(target))aim=target.GetPosition()+"0 1.1 0";
  array<ref RaycastRVResult> hits={};RaycastRVParams ray=new RaycastRVParams(origin,aim,this);ray.flags=CollisionFlags.NEARESTCONTACT;ray.type=ObjIntersectView;
  if(!DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0)return true;
  return hits[0].obj==target || hits[0].parent==target;
 }
 static float ArmedSpread(float distance,float accuracy)
 {
  // 0 is very inaccurate; 10 is tight aim. Geometry still decides every hit.
  return 0.015+distance*(0.002+(10-Math.Clamp(accuracy,0,10))*0.006);
 }
 bool ArmedLineClear(vector from,vector to)
 {
  array<Object> ignore={this};if(m_ArmedPistol)ignore.Insert(m_ArmedPistol);
  // The ray starts at the threat; test intervening cover, not its own body.
  if(m_HumanEnemy)ignore.Insert(m_HumanEnemy);
  array<ref RaycastRVResult> hits={};RaycastRVParams ray=new RaycastRVParams(from,to,this);
  ray.type=ObjIntersectFire;ray.flags=CollisionFlags.NEARESTCONTACT;
  if(!DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0)return true;
  return vector.DistanceSq(hits[0].pos,to)<0.04;
 }
 bool ArmedHidden()
 {
  if(!HumanHostile(m_HumanEnemy))return m_HumanClock>=m_ArmedSuppressedUntil;
  return !ArmedLineClear(m_HumanEnemy.GetPosition()+"0 1.5 0",GetPosition()+"0 0.8 0") && !HumanSight(m_HumanEnemy);
 }
 bool ArmedFindCover()
 {
  if(!HumanHostile(m_HumanEnemy))return false;
  m_ArmedCoverAt=m_HumanClock+2;
  vector start=GetPosition();vector threat=m_HumanLastKnown+"0 1.5 0";
  vector away=start-m_HumanLastKnown;away[1]=0;away.Normalize();
  vector side=Vector(away[2],0,-away[0]);float best=100000;bool found;
  int navCandidates;int hiddenCandidates;int peekCandidates;
  for(int i=0;i<36;i++)
  {
   int sector = i % 12;
   float angle = sector * Math.PI2 / 12;
   float radius = 2 + Math.Floor(i / 12) * 2;
   vector radial = away * Math.Cos(angle);
   radial = radial + side * Math.Sin(angle);
   vector candidate;vector wanted = start + radial * radius;
   if(!HumanNavigation().GroundPoint(start,wanted,candidate,false,false))continue;
   navCandidates++;
   if(ArmedLineClear(threat,candidate+"0 0.8 0"))continue;
   hiddenCandidates++;
   vector peek;bool hasPeek;
   for(int j=0;j<6;j++)
   {
    int sideIndex=j % 2;float sign=1;if(sideIndex==1)sign=-1;
    float peekDistance=1+Math.Floor(j/2);
    if(!HumanNavigation().GroundPoint(candidate,candidate+side*sign*peekDistance,peek,true,false))continue;
    peekCandidates++;
    if(ArmedLineClear(threat,peek+"0 1.5 0")){hasPeek=true;break;}
   }
   if(!hasPeek)continue;
   float score=vector.DistanceSq(start,candidate)+vector.DistanceSq(candidate,m_HumanLastKnown)*0.015;
   if(score<best){best=score;m_ArmedCover=candidate;m_ArmedPeek=peek;found=true;}
  }
  if(found)
  {
   m_ArmedHasCover=true;m_ArmedPeeking=false;m_ArmedCoverUntil=m_HumanClock+18;
   m_ArmedCoverThreat=m_HumanLastKnown;m_ArmedPeekAt=m_HumanClock+1.5;
  }
#ifdef DIAG_DEVELOPER
  if(!found)Print("[HumansCover] no cover: reachable="+navCandidates.ToString()+" hidden="+hiddenCandidates.ToString()+" reachablePeeks="+peekCandidates.ToString());
#endif
  return found;
 }
 void ArmedReactToShot(EntityAI shooter)
 {
  if(!HumanAuthority() || !IsAlive() || !HumanHostile(shooter))return;
  m_ArmedSuppressedUntil=m_HumanClock+10;m_ArmedCoverAt=0;m_ArmedBandageUntil=0;
  if(HumanSight(shooter))
  {
   m_HumanEnemy=shooter;m_HumanSeen=m_HumanClock;m_HumanLastKnown=shooter.GetPosition();
   HumanTacticalBrain().Remember(shooter,m_HumanLastKnown);
  }
  else HumanHear(shooter.GetPosition());
 }
 bool ArmedBandageTick()
 {
  if(m_ArmedBandageUntil>0)
  {
   if(!ArmedHidden()) {m_ArmedBandageUntil=0;ArmedPose(true,true);return false;}
   ArmedPose(false,true,true);
   if(m_HumanClock>=m_ArmedBandageUntil)
   {
    m_ArmedBleeding=false;m_ArmedBandageUntil=0;m_ArmedBandageAt=m_HumanClock+30;
    AddHealth("","Health",Math.Min(HumansSettings.Get().ArmedBandageHeal,GetMaxHealth("","Health")-GetHealth("","Health")));
    ArmedPose(true,true);
   }
   return true;
  }
  if((m_ArmedBleeding || GetHealth01("","Health")<0.65) && m_HumanClock>=m_ArmedBandageAt && ArmedHidden())
  {
   m_ArmedBandageUntil=m_HumanClock+HumansSettings.Get().ArmedBandageSeconds;
   m_HumanHitDone=true;ArmedPose(false,true,true);return true;
  }
  return false;
 }
 bool ArmedCoverTick()
 {
  if(m_ArmedHasCover && (m_HumanClock>m_ArmedCoverUntil || vector.DistanceSq(m_HumanLastKnown,m_ArmedCoverThreat)>36))m_ArmedHasCover=false;
  if(!m_ArmedHasCover && m_HumanClock>=m_ArmedCoverAt && (m_HumanClock<m_ArmedSuppressedUntil || m_ArmedBleeding))ArmedFindCover();
  if(!m_ArmedHasCover)return false;
  vector destination=m_ArmedCover;
  if(m_ArmedPeeking)destination=m_ArmedPeek;
  if(vector.DistanceSq(GetPosition(),destination)>0.16)
  {
   ArmedPose(true,!m_ArmedPeeking);HumanTravel(destination,2);return true;
  }
  if(!m_ArmedPeeking)
  {
   ArmedPose(true,true);
   if(m_ArmedRounds<=0 && m_ArmedReloadUntil<=0){ArmedReload();return true;}
   if(ArmedBandageTick() || m_ArmedReloadUntil>m_HumanClock)return true;
   if(m_HumanClock<m_ArmedPeekAt)return true;
   m_ArmedPeeking=true;m_ArmedExposeUntil=m_HumanClock+2.6;
   return true;
  }
  if(m_HumanClock>=m_ArmedExposeUntil)
  {
   m_ArmedPeeking=false;m_ArmedPeekAt=m_HumanClock+Math.RandomFloatInclusive(1,2.2);return true;
  }
  ArmedPose(true,false);return false;
 }
 override bool HumanRangedTick(float dt,int command)
 {
  if(!m_ArmedPistol){HumanInitLoadout();return false;}
  if(m_ArmedBleeding)
  {
   m_ArmedBleedTick+=dt;
   if(m_ArmedBleedTick>=1){AddHealth("","Health",-0.25*m_ArmedBleedTick);m_ArmedBleedTick=0;}
  }
  if(m_ArmedAction>0 && m_HumanClock>=m_ArmedActionUntil)
  {
   if(GetCommand_Attack())StartCommand_Move();m_ArmedAction=0;
  }
  if(!HumanHostile(m_HumanEnemy))
  {
   m_ArmedHasCover=false;
   if(ArmedBandageTick())return true;
   ArmedPose(true,false);return false;
  }
  float distance=vector.Distance(GetPosition(),m_HumanEnemy.GetPosition());
  // Finish a close punch, then use a clear navigation route to make pistol space.
  if(!m_ArmedDrawn && m_HumanSwingTarget==m_HumanEnemy && m_HumanHitDone && GetCommand_Move() && !HumanIsBlocking())ArmedStartRetreat();
  if(m_ArmedRetreatUntil>m_HumanClock && distance<4 && GetCommand_Move() && !HumanIsBlocking())
  {
   ArmedPose(false,false);HumanTravel(m_ArmedRetreatGoal,2);return true;
  }
  if(distance>=4)m_ArmedRetreatUntil=0;
  float closeRange=2.6;if(!m_ArmedDrawn)closeRange=3.5;
  if(distance<closeRange)
  {
   if(m_ArmedAction>0 && GetCommand_Attack())StartCommand_Move();
   m_ArmedAction=0;m_ArmedReloadUntil=0;m_ArmedBandageUntil=0;m_ArmedHasCover=false;
   ArmedPose(false,false);return false; // Existing fist/block/escape controller.
  }
  m_HumanWake=true;m_HumanRestPhase=3;
  if(command!=DayZInfectedConstants.COMMANDID_MOVE)return true;
  if(m_ArmedReloadUntil>0 && m_HumanClock>=m_ArmedReloadUntil)
  {
   m_ArmedReloadUntil=0;m_ArmedRounds=15;m_ArmedReloadCount++;
   if(m_ArmedMagazine)m_ArmedMagazine.ServerSetAmmoCount(15);
   SetSynchDirty();
  }
  if(m_ArmedBandageUntil>0 && ArmedBandageTick())return true;
  if(m_HumanClock<m_ArmedPoseReadyAt)return true;
  if(ArmedCoverTick())return true;
  bool crouched=(m_HumanSocialId%3)!=0;
  if(m_ArmedHasCover && m_ArmedPeeking)crouched=false;
  ArmedPose(true,crouched);
  HumanFace(m_HumanLastKnown);
  if(distance>HumansSettings.Get().ArmedFireRange || !HumanSight(m_HumanEnemy))
  {
   HumanTravel(m_HumanLastKnown,2);return true;
  }
  if(m_ArmedReloadUntil>m_HumanClock)return true;
  if(m_ArmedRounds<=0){ArmedReload();return true;}
  if(m_HumanClock>=m_ArmedNextShot)
  {
   vector toward=m_HumanEnemy.GetPosition()-GetPosition();toward[1]=0;toward.Normalize();
   if(vector.Dot(GetDirection(),toward)>0.95)ArmedShoot();
  }
  return true;
 }
 void ArmedReload()
 {
  if(!HumanAuthority() || !IsAlive() || m_ArmedReloadUntil>m_HumanClock)return;
  m_ArmedReloadUntil=m_HumanClock+HumansSettings.Get().ArmedReloadSeconds;
  m_ArmedAction=2;m_ArmedActionUntil=m_ArmedReloadUntil;
  m_HumanHitDone=true;m_HumanSwingSoundDone=true;
  StartCommand_Attack(m_HumanEnemy,2,1);
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(this,ARMED_FX_RPC,new Param1<int>(2),true);else ArmedEffect(2);
 }
 void ArmedShoot()
 {
  if(!HumanAuthority() || !IsAlive() || !HumanHostile(m_HumanEnemy) || m_ArmedRounds<=0 || m_ArmedReloadUntil>m_HumanClock || m_ArmedBandaging || m_HumanClock<m_ArmedPoseReadyAt)return;
  if(vector.DistanceSq(GetPosition(),m_HumanEnemy.GetPosition())>HumansSettings.Get().ArmedFireRange*HumansSettings.Get().ArmedFireRange)return;
  m_ArmedRounds--;m_ArmedShotCount++;m_ArmedNextShot=m_HumanClock+Math.RandomFloatInclusive(HumansSettings.Get().ArmedShotIntervalMin,HumansSettings.Get().ArmedShotIntervalMax);
  if(m_ArmedMagazine)m_ArmedMagazine.ServerSetAmmoCount(m_ArmedRounds);
  m_ArmedPistol.SetJammed(false);m_ArmedPistol.SetHealthMax("","Health");
  m_HumanHitDone=true;m_HumanSwingSoundDone=true;
  m_ArmedAction=1;m_ArmedActionUntil=m_HumanClock+0.5;
  // Keep the native crouched aim pose while firing; a full-body erect
  // fire clip would stand the actor up on every shot.
  if(!m_ArmedCrouched)StartCommand_Attack(m_HumanEnemy,1,1);
  SetSynchDirty();
  if(GetGame().IsMultiplayer())GetGame().RPCSingleParam(this,ARMED_FX_RPC,new Param1<int>(1),true);else ArmedEffect(1);
  vector origin=GetPosition()+"0 1.35 0";
  if(m_ArmedCrouched)origin=GetPosition()+"0 1 0";
  vector aim=m_HumanEnemy.ModelToWorld(m_HumanEnemy.GetDefaultHitPosition());
  PlayerBase player=PlayerBase.Cast(m_HumanEnemy);
  if(player)aim=player.GetPosition()+"0 1.1 0";
  vector direction=vector.Direction(origin,aim);direction.Normalize();
  vector right=Vector(direction[2],0,-direction[0]);right.Normalize();vector up=right*direction;up.Normalize();
  float spread=ArmedSpread(vector.Distance(origin,aim),HumansSettings.Get().ArmedAimAccuracy);
  float radius=Math.Sqrt(Math.RandomFloat01())*spread;float angle=Math.RandomFloatInclusive(0,Math.PI2);
  aim=aim+right*Math.Cos(angle)*radius+up*Math.Sin(angle)*radius;
  direction=vector.Direction(origin,aim);direction.Normalize();
  array<Object> ignore={this,m_ArmedPistol};array<ref RaycastRVResult> hits={};
  RaycastRVParams ray=new RaycastRVParams(origin,origin+direction*(HumansSettings.Get().ArmedFireRange+1),this);
  ray.type=ObjIntersectFire;ray.flags=CollisionFlags.NEARESTCONTACT;
  if(!DayZPhysics.RaycastRVProxy(ray,hits,ignore) || hits.Count()==0)return;
  EntityAI hit=EntityAI.Cast(hits[0].obj);if(!hit)hit=EntityAI.Cast(hits[0].parent);
  if(hit){EntityAI root=EntityAI.Cast(hit.GetHierarchyRoot());if(root)hit=root;}
  if(!HumanHostile(hit))return; // First obstruction wins; never shoot through cover/friends.
  ArmedApplyBullet(hit,hits[0].pos);
 }
 bool ArmedApplyBullet(EntityAI target,vector point)
 {
  if(!HumanAuthority() || !IsAlive() || !HumanHostile(target))return false;
  // Global health route: fixed 15 HP, with no head multiplier or armor-dependent spike.
  target.ProcessDirectDamage(DamageType.FIRE_ARM,this,"","HumansGlockHit",target.WorldToModel(point),1);
  return true;
 }
 void ArmedEffect(int cue)
 {
  if(GetGame().IsDedicatedServer())return;
  if(cue==1)
  {
   HumansGunAudio.PlayShot(this);
   ArmedVisual();if(m_ArmedPistol)ItemBase.PlayFireParticles(m_ArmedPistol,0,"Bullet_9x19",m_ArmedPistol,null,"CfgWeapons");
  }
  if(cue==2)
  {
   EffectSound reload=SEffectManager.PlaySoundOnObject("Humans_GlockReload_SoundSet",this);if(reload)reload.SetAutodestroy(true);
  }
 }
 override void OnRPC(PlayerIdentity sender,int rpc_type,ParamsReadContext ctx)
 {
  super.OnRPC(sender,rpc_type,ctx);
  if(rpc_type==ARMED_FX_RPC && !GetGame().IsServer())
  {
   Param1<int> cue;if(ctx.Read(cue))ArmedEffect(cue.param1);
  }
 }
 override bool HandleMindStateChange(int pCurrentCommandID,DayZInfectedInputController pInputController,float pDt)
 {
  if(m_ArmedDrawn || m_ArmedBandaging)
  {
   DayZInfectedCommandMove move=GetCommand_Move();
   if(move)
   {
    int stance=0;if(m_ArmedCrouched)stance=1;if(m_ArmedBandaging)stance=3;
    m_StanceVariation=stance;move.SetStanceVariation(stance);move.SetIdleState(2);
   }
   return false;
  }
  return super.HandleMindStateChange(pCurrentCommandID,pInputController,pDt);
 }
 override bool FightLogic(int pCurrentCommandID,DayZInfectedInputController pInputController,float pDt)
 {
  if(m_ArmedDrawn || m_ArmedBandaging || m_ArmedAction>0)return true;
  return super.FightLogic(pCurrentCommandID,pInputController,pDt);
 }
 override void EEHitBy(TotalDamageResult damageResult,int damageType,EntityAI source,int component,string dmgZone,string ammo,vector modelPos,float speedCoef)
 {
  super.EEHitBy(damageResult,damageType,source,component,dmgZone,ammo,modelPos,speedCoef);
  if(!HumanAuthority() || !IsAlive())return;
  m_ArmedBandageUntil=0;
  if((damageType==DamageType.FIRE_ARM || ammo.Contains("Sharp")) && damageResult.GetDamage("","Health")>0)m_ArmedBleeding=true;
  EntityAI attacker=source;if(source && source.GetHierarchyRootPlayer())attacker=source.GetHierarchyRootPlayer();
  ArmedReactToShot(attacker);
 }
 override void EEKilled(Object killer)
 {
  m_ArmedBandageUntil=0;m_ArmedReloadUntil=0;
  if(HumanAuthority() && m_ArmedPistol)
  {
   Weapon_Base loot=m_ArmedPistol;GetGame().RemoteObjectTreeDelete(loot);RemoveChild(loot,false);
   vector tm[4];Math3D.MatrixIdentity4(tm);tm[3]=GetPosition()+"0 0.15 0";loot.SetTransform(tm);
   loot.SetInvisible(false);loot.SetTakeable(true);loot.SetAllowDamage(true);loot.SetJammed(false);loot.PlaceOnSurface();
   if(m_ArmedMagazine)m_ArmedMagazine.SetAllowDamage(true);
   loot.SetLifetime(14400);loot.SetLifetimeMax(14400);GetGame().RemoteObjectTreeCreate(loot);m_ArmedPistol=null;m_ArmedMagazine=null;
  }
  super.EEKilled(killer);
 }
 override void EEDelete(EntityAI parent)
 {
  if(HumanAuthority() && m_ArmedPistol){RemoveChild(m_ArmedPistol);GetGame().ObjectDelete(m_ArmedPistol);m_ArmedPistol=null;}
  super.EEDelete(parent);
 }
}
class HumanArmedM_HunterOld_Summer extends HumanArmed_Base {}
class HumanArmedM_Jacket_grey extends HumanArmed_Base {}
class HumanArmedM_JournalistSkinny extends HumanArmed_Base {}
class HumanArmedF_SurvivorNormal_Blue extends HumanArmed_Base {}
class HumanArmedM_SoldierNormal extends HumanArmed_Base {}
class HumanArmedM_SoldierNormal_Female extends HumanArmed_Base {}

modded class Weapon_Base
{
 override void EEFired(int muzzleType,int mode,string ammoType)
 {
  super.EEFired(muzzleType,mode,ammoType);
  if(!GetGame().IsServer())return;
  EntityAI shooter=GetHierarchyRootPlayer();if(!shooter)return;
  foreach(Human_Base human:Human_Base.HumanPopulation)
  {
   HumanArmed_Base armed=HumanArmed_Base.Cast(human);if(!armed || !armed.IsAlive())continue;
   vector toward=armed.GetPosition()-shooter.GetPosition();toward[1]=0;
   if(toward.LengthSq()>6400)continue;toward.Normalize();
   if(vector.Dot(shooter.GetDirection(),toward)>0.75)armed.ArmedReactToShot(shooter);
  }
 }
}



