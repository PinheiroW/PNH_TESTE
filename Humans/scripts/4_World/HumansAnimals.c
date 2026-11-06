// Only overrides animals while a Human is the selected visible threat.
// Predators attack; prey use their native threat locomotion to escape.
modded class AnimalBase
{
 int m_HumansAnimalCommand;
 bool HumansAnimalAttacking(){return m_HumansAnimalCommand==DayZAnimalConstants.COMMANDID_ATTACK;}
 Human_Base m_HumansAnimalTarget;
 float m_HumansAnimalScan;
 float m_HumansAnimalAttackAt;
 float m_HumansAnimalAttackStarted;
 float m_HumansAnimalPathAt;
 bool m_HumansAnimalDriving;
 ref PGFilter m_HumansAnimalFilter;
 ref array<vector> m_HumansAnimalPath={};
 int m_HumansAnimalWaypoint;
 void HumansAnimalRelease()
 {
  if(!m_HumansAnimalDriving)return;
  DayZAnimalInputController input=GetInputController();
  if(input)
  {
   input.OverrideMovementSpeed(false,0);input.OverrideHeading(false,0);input.OverrideTurnSpeed(false,0);
   input.OverrideAlertLevel(false,false,0,0);input.OverrideBehaviourSlot(false,0);input.OverrideBehaviourAction(false,0);
  }
  m_HumansAnimalDriving=false;
 }
 void HumansAnimalContact()
 {
  Human_Base victim=m_HumansAnimalTarget;
  if(!IsAlive() || !HumansAnimalAttacking() || !victim || !victim.IsAlive() || vector.DistanceSq(GetPosition(),victim.GetPosition())>4 || !victim.HumanSight(this))return;
  if(victim.m_HumanAnimalLastSource==this && victim.m_HumanAnimalLastHit>=m_HumansAnimalAttackStarted)return;
  // Native damage spheres remain primary. This covers the engine's missing
  // infected-body contact, with a matching human-side debounce against doubles.
  DamageSystem.CloseCombatDamageName(this,victim,victim.GetHitComponentForAI(),"MeleeWolf",victim.ModelToWorld(victim.GetDefaultHitPosition()));
 }
 override bool ModCommandHandlerInside(float pDt,int pCurrentCommandID,bool pCurrentCommandFinished)
 {
  m_HumansAnimalCommand=pCurrentCommandID;
  if(super.ModCommandHandlerInside(pDt,pCurrentCommandID,pCurrentCommandFinished))return true;
  if(!GetGame().IsServer() && GetGame().IsMultiplayer())return false;
  if(!IsAlive()){HumansAnimalRelease();return false;}
  float now=GetGame().GetTime()*0.001;
  if(now>=m_HumansAnimalScan)
  {
   m_HumansAnimalScan=now+0.5;
   float best=900;Human_Base chosen;
   foreach(Human_Base human:Human_Base.HumanPopulation)
   {
    if(!human || !human.IsAlive() || human.HumanIsTrader())continue;
    float distance=vector.DistanceSq(GetPosition(),human.GetPosition());
    if(distance<best && human.HumanSight(this)){chosen=human;best=distance;}
   }
   array<Man> players={};GetGame().GetPlayers(players);
   foreach(Man player:players){if(player && player.IsAlive() && vector.DistanceSq(player.GetPosition(),GetPosition())<best){chosen=null;break;}}
   m_HumansAnimalTarget=chosen;
  }
  if(!m_HumansAnimalTarget || !m_HumansAnimalTarget.IsAlive()){HumansAnimalRelease();return false;}
  DayZAnimalInputController input=GetInputController();if(!input)return false;
  m_HumansAnimalDriving=true;
  if(pCurrentCommandID!=DayZAnimalConstants.COMMANDID_MOVE)return false;
  vector goal=m_HumansAnimalTarget.GetPosition();float range=vector.Distance(GetPosition(),goal);
  bool predator=IsDanger();
  if(!predator)
  {
   vector away=GetPosition()-goal;away[1]=0;away.Normalize();goal=GetPosition()+away*12;
   input.OverrideBehaviourSlot(true,DayZAnimalBehaviourSlot.SCARED);
   input.OverrideBehaviourAction(true,DayZAnimalBehaviourAction.TRAVELING_INPUT);
  }
  else
  {
   input.OverrideBehaviourSlot(true,DayZAnimalBehaviourSlot.ENRAGED);
   input.OverrideBehaviourAction(true,DayZAnimalBehaviourAction.CHARGING);
  }
  input.OverrideAlertLevel(true,true,4,1);
  if(predator && range<1.8 && now>=m_HumansAnimalAttackAt)
  {
   input.OverrideMovementSpeed(true,0);
   m_HumansAnimalAttackStarted=now;m_HumansAnimalAttackAt=now+2;
   StartCommand_Attack();SignalAIAttackStarted();
   GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(HumansAnimalContact,600,false);
   return true;
  }
  if(!m_HumansAnimalFilter)
  {
   m_HumansAnimalFilter=new PGFilter;
   m_HumansAnimalFilter.SetFlags(PGPolyFlags.WALK|PGPolyFlags.INSIDE,PGPolyFlags.DISABLED|PGPolyFlags.SWIM,0);
  }
  if(now>=m_HumansAnimalPathAt)
  {
   m_HumansAnimalPathAt=now+1;m_HumansAnimalPath.Clear();m_HumansAnimalWaypoint=1;
   GetGame().GetWorld().GetAIWorld().FindPath(GetPosition(),goal,m_HumansAnimalFilter,m_HumansAnimalPath);
  }
  while(m_HumansAnimalWaypoint<m_HumansAnimalPath.Count() && vector.DistanceSq(GetPosition(),m_HumansAnimalPath[m_HumansAnimalWaypoint])<0.36)m_HumansAnimalWaypoint++;
  if(m_HumansAnimalWaypoint>=m_HumansAnimalPath.Count()){input.OverrideMovementSpeed(true,0);return true;}
  vector direction=m_HumansAnimalPath[m_HumansAnimalWaypoint]-GetPosition();direction[1]=0;direction.Normalize();
  input.OverrideTurnSpeed(true,Math.PI2);input.OverrideHeading(true,Math.Atan2(direction[0],direction[2]));
  input.OverrideMovementSpeed(true,3*Math.Max(0,vector.Dot(direction,GetDirection())));
  return false;
 }
 override void EEDelete(EntityAI parent)
 {
  GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(HumansAnimalContact);
  super.EEDelete(parent);
 }
}

