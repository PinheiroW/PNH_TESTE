#ifdef EXPANSIONMODAI
// Expansion evaluates native infected mind-state. Our custom Human controller
// can be fighting while that native state still reports CALM, yielding no threat.
modded class eAIZombieTargetInformation
{
 override bool IsAcuteDanger(eAIBase ai=null)
 {
  Human_Base human=Human_Base.Cast(GetEntity());
  if(human && human.IsAlive() && ai && human.m_HumanEnemy==ai)return true;
  return super.IsAcuteDanger(ai);
 }
 override float CalculateThreat(eAIBase ai=null,eAITargetInformationState state=null)
 {
  float threat=super.CalculateThreat(ai,state);
  Human_Base human=Human_Base.Cast(GetEntity());
  if(!human || !human.IsAlive() || !ai || human.m_HumanEnemy!=ai)return threat;
  if(!human.HumanSight(ai) || vector.DistanceSq(human.GetPosition(),ai.GetPosition())>6400)return threat;
  // Register the Human's actual hostility; do not alter other infected/factions.
  return Math.Max(threat,0.8);
 }
}
#endif
