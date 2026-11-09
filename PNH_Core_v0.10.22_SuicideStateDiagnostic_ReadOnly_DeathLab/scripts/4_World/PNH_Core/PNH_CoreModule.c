// PNH_Core v0.9 A - CF bootstrap + diagnostics + RPC + isolated storage and life-cycle probes.
[CF_RegisterModule(PNH_CoreModule)]
class PNH_CoreModule : CF_ModuleWorld
{
    protected bool m_PNHIdentityObserverEnabled = false;
    protected bool m_PNHPlayerBindingEnabled = false;
    protected bool m_PNHLifeMonitorEnabled = false;
    protected bool m_PNHTransitionEnabled = false;
    protected bool m_PNHSafeCommitEnabled = false;
    protected bool m_PNHMultiLifeEnabled = false;
    protected bool m_PNHArchiveEnabled = false;
    protected bool m_PNHRotationEnabled = false;
    protected bool m_PNHSecondLifeEnabled = false;
    protected bool m_PNHSecondLifeTxEnabled = false;
    protected bool m_PNHLifeCycleManagerEnabled = false;
    protected bool m_PNHArchiveCoordinatorEnabled = false;
    protected bool m_PNHRotationCoordinatorEnabled = false;
    protected bool m_PNHProtectedRotationEnabled = false;
    protected bool m_PNHLifeContinuityEnabled = false;
    protected bool m_PNHLife3ReadinessEnabled = false;
    protected bool m_PNHLife3ControlledPreflightEnabled = false;
    protected bool m_PNHLife3TransitionPlanEnabled = false;
    protected bool m_PNHLife3JournalPrepEnabled = false;
    protected bool m_PNHProtectedCreationEnabled = false;
    protected bool m_PNHStagingInspectEnabled = false;
    protected bool m_PNHPublicationPreflightEnabled = false;
    override void OnInit()
    {
        super.OnInit();

        // CF registration must happen on BOTH server and client at initialization.
        GetRPCManager().AddRPC("PNH_Core", "RPC_Ping", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("PNH_Core", "RPC_Pong", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("PNH_Core", "RPC_Ack", this, SingleplayerExecutionType.Server);

        if (GetGame() && GetGame().IsServer())
        {
            Print("[PNH_CORE] Bootstrap v0.10.22 initialized on server");
            PNH_Diagnostics.Initialize();
            PNH_Diagnostics.Log("Core", "State", "v0.8 startup probe OK");
            Print("[PNH_CORE][RPC] CF handlers registered (Ping/Pong/Ack)");
            PNH_StorageProbe.Initialize();
            PNH_LifeCycleProbe.Initialize();

            // Register CF lifecycle hooks ONLY if explicitly enabled in JSON.
            // No additional per-frame work and no changes to vanilla player state.
            m_PNHIdentityObserverEnabled = PNH_IdentityObserver.Initialize();
            m_PNHPlayerBindingEnabled = PNH_PlayerBinding.Initialize();
            m_PNHLifeMonitorEnabled = PNH_LifeCycleMonitor.Initialize();
            m_PNHTransitionEnabled = PNH_TransitionJournal.Initialize();
            m_PNHSafeCommitEnabled = PNH_SafeLifeCommit.Initialize();
            m_PNHMultiLifeEnabled = PNH_MultiLifePreflight.Initialize();
            m_PNHArchiveEnabled = PNH_LifeHistoryArchive.Initialize();
            m_PNHRotationEnabled = PNH_ActiveJournalRotation.Initialize();
            m_PNHSecondLifeEnabled = PNH_SecondLifePreflight.Initialize();
            m_PNHSecondLifeTxEnabled = PNH_SecondLifeTransitionGuard.Initialize();
            m_PNHLifeCycleManagerEnabled = PNH_LifeCycleManager.Initialize();
            m_PNHArchiveCoordinatorEnabled = PNH_LifeCycleArchiveCoordinator.Initialize();
            m_PNHRotationCoordinatorEnabled = PNH_LifeCycleRotationCoordinator.Initialize();
            m_PNHProtectedRotationEnabled = PNH_ProtectedJournalRotation.Initialize();
            m_PNHLifeContinuityEnabled = PNH_LifeCycleContinuity.Initialize();
            m_PNHLife3ReadinessEnabled = PNH_Life3TransitionReadiness.Initialize();
            m_PNHLife3ControlledPreflightEnabled = PNH_Life3ControlledTransitionPreflight.Initialize();
            m_PNHLife3TransitionPlanEnabled = PNH_Life3TransitionPlan.Initialize();
            m_PNHLife3JournalPrepEnabled = PNH_Life3JournalPreparation.Initialize();
            m_PNHProtectedCreationEnabled = PNH_ProtectedJournalCreation.Initialize();
            m_PNHStagingInspectEnabled = PNH_JournalStaging.Initialize();
            PNH_LabStagingTest.Initialize();
            m_PNHPublicationPreflightEnabled = PNH_JournalPublicationPreflight.Initialize();
            PNH_DeathTransitionCandidate.Initialize();
            PNH_DeathEventObserver.Initialize();
            PNH_FatalDamageObserver.Initialize();
            PNH_FatalDamageCorrelation.Initialize();
            PNH_DeathClassification.Initialize();
            PNH_DeathMechanismObserver.Initialize();
            bool shouldEnableCFHooks = false;
            if (m_PNHIdentityObserverEnabled) shouldEnableCFHooks = true;
            if (m_PNHPlayerBindingEnabled) shouldEnableCFHooks = true;
            if (m_PNHLifeMonitorEnabled) shouldEnableCFHooks = true;
            if (m_PNHTransitionEnabled) shouldEnableCFHooks = true;
            if (m_PNHSafeCommitEnabled) shouldEnableCFHooks = true;
            if (m_PNHMultiLifeEnabled) shouldEnableCFHooks = true;
            if (m_PNHArchiveEnabled) shouldEnableCFHooks = true;
            if (m_PNHRotationEnabled) shouldEnableCFHooks = true;
            if (m_PNHSecondLifeEnabled) shouldEnableCFHooks = true;
            if (m_PNHSecondLifeTxEnabled) shouldEnableCFHooks = true;
            if (m_PNHLifeCycleManagerEnabled) shouldEnableCFHooks = true;
            if (m_PNHArchiveCoordinatorEnabled) shouldEnableCFHooks = true;
            if (m_PNHRotationCoordinatorEnabled) shouldEnableCFHooks = true;
            if (m_PNHProtectedRotationEnabled) shouldEnableCFHooks = true;
            if (m_PNHLifeContinuityEnabled) shouldEnableCFHooks = true;
            if (m_PNHLife3ReadinessEnabled) shouldEnableCFHooks = true;
            if (m_PNHLife3ControlledPreflightEnabled) shouldEnableCFHooks = true;
            if (m_PNHLife3TransitionPlanEnabled) shouldEnableCFHooks = true;
            if (m_PNHLife3JournalPrepEnabled) shouldEnableCFHooks = true;
            if (m_PNHProtectedCreationEnabled) shouldEnableCFHooks = true;
            if (m_PNHStagingInspectEnabled) shouldEnableCFHooks = true;
            if (m_PNHPublicationPreflightEnabled) shouldEnableCFHooks = true;
            if (shouldEnableCFHooks)
            {
                EnableClientNew();
                EnableClientReady();
                if (m_PNHIdentityObserverEnabled) EnableInvokeConnect();
                Print("[PNH_CORE][BIND] CF hooks enabled (New/Ready)");
            }
        }
    }

    override void OnClientNew(Class sender, CF_EventArgs args)
    {
        super.OnClientNew(sender, args);
        if (m_PNHIdentityObserverEnabled)
        {
            Print("[PNH_CORE][IDENTITY][TRACE] Callback OnClientNew fired");
            PNH_IdentityObserver.Observe("CLIENT_NEW", args);
        }
        if (m_PNHPlayerBindingEnabled) PNH_PlayerBinding.Observe("CLIENT_NEW", args);
        if (m_PNHLifeMonitorEnabled) PNH_LifeCycleMonitor.OnCFEvent("CLIENT_NEW", args);
        bool freshlyObserved = false;
        if (m_PNHTransitionEnabled) freshlyObserved = PNH_TransitionJournal.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHMultiLifeEnabled) PNH_MultiLifePreflight.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHArchiveEnabled) PNH_LifeHistoryArchive.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHRotationEnabled) PNH_ActiveJournalRotation.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHSecondLifeEnabled) PNH_SecondLifePreflight.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHSecondLifeTxEnabled) PNH_SecondLifeTransitionGuard.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLifeCycleManagerEnabled) PNH_LifeCycleManager.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHArchiveCoordinatorEnabled) PNH_LifeCycleArchiveCoordinator.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHRotationCoordinatorEnabled) PNH_LifeCycleRotationCoordinator.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHProtectedRotationEnabled) PNH_ProtectedJournalRotation.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLifeContinuityEnabled) PNH_LifeCycleContinuity.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLife3ReadinessEnabled) PNH_Life3TransitionReadiness.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLife3ControlledPreflightEnabled) PNH_Life3ControlledTransitionPreflight.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLife3TransitionPlanEnabled) PNH_Life3TransitionPlan.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHLife3JournalPrepEnabled) PNH_Life3JournalPreparation.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHProtectedCreationEnabled) PNH_ProtectedJournalCreation.OnCFEvent("CLIENT_NEW", args);
        if (m_PNHSafeCommitEnabled)
        {
            if (freshlyObserved) PNH_SafeLifeCommit.OnNewlyObserved(args);
            else PNH_SafeLifeCommit.OnReady(args); // recovery check also for a new character after restart
        }
    }

    override void OnClientReady(Class sender, CF_EventArgs args)
    {
        super.OnClientReady(sender, args);
        if (m_PNHIdentityObserverEnabled)
        {
            Print("[PNH_CORE][IDENTITY][TRACE] Callback OnClientReady fired");
            PNH_IdentityObserver.Observe("CLIENT_READY", args);
        }
        if (m_PNHPlayerBindingEnabled) PNH_PlayerBinding.Observe("CLIENT_READY", args);
        if (m_PNHLifeMonitorEnabled) PNH_LifeCycleMonitor.OnCFEvent("CLIENT_READY", args);
        if (m_PNHTransitionEnabled) PNH_TransitionJournal.OnCFEvent("CLIENT_READY", args);
        if (m_PNHSafeCommitEnabled) PNH_SafeLifeCommit.OnReady(args);
        if (m_PNHMultiLifeEnabled) PNH_MultiLifePreflight.OnCFEvent("CLIENT_READY", args);
        if (m_PNHArchiveEnabled) PNH_LifeHistoryArchive.OnCFEvent("CLIENT_READY", args);
        if (m_PNHRotationEnabled) PNH_ActiveJournalRotation.OnCFEvent("CLIENT_READY", args);
        if (m_PNHSecondLifeEnabled) PNH_SecondLifePreflight.OnCFEvent("CLIENT_READY", args);
        if (m_PNHSecondLifeTxEnabled) PNH_SecondLifeTransitionGuard.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLifeCycleManagerEnabled) PNH_LifeCycleManager.OnCFEvent("CLIENT_READY", args);
        if (m_PNHArchiveCoordinatorEnabled) PNH_LifeCycleArchiveCoordinator.OnCFEvent("CLIENT_READY", args);
        if (m_PNHRotationCoordinatorEnabled) PNH_LifeCycleRotationCoordinator.OnCFEvent("CLIENT_READY", args);
        if (m_PNHProtectedRotationEnabled) PNH_ProtectedJournalRotation.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLifeContinuityEnabled) PNH_LifeCycleContinuity.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLife3ReadinessEnabled) PNH_Life3TransitionReadiness.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLife3ControlledPreflightEnabled) PNH_Life3ControlledTransitionPreflight.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLife3TransitionPlanEnabled) PNH_Life3TransitionPlan.OnCFEvent("CLIENT_READY", args);
        if (m_PNHLife3JournalPrepEnabled) PNH_Life3JournalPreparation.OnCFEvent("CLIENT_READY", args);
        if (m_PNHProtectedCreationEnabled) PNH_ProtectedJournalCreation.OnCFEvent("CLIENT_READY", args);
        if (m_PNHStagingInspectEnabled) PNH_JournalStaging.OnCFEvent("CLIENT_READY", args);
        if (m_PNHPublicationPreflightEnabled) PNH_JournalPublicationPreflight.OnCFEvent("CLIENT_READY", args);
    }

    // Independent connection signal: can fire when an existing character joins.
    override void OnInvokeConnect(Class sender, CF_EventArgs args)
    {
        super.OnInvokeConnect(sender, args);
        if (m_PNHIdentityObserverEnabled)
        {
            Print("[PNH_CORE][IDENTITY][TRACE] Callback OnInvokeConnect fired");
            PNH_IdentityObserver.Observe("INVOKE_CONNECT", args);
        }
    }

    // Client -> server. Never trust a client-supplied player state.
    void RPC_Ping(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame() || !GetGame().IsServer() || type != CallType.Server || !sender)
            return;

        Param1<int> data;
        if (!ctx.Read(data) || !data || data.param1 != 301)
        {
            Print("[PNH_CORE][RPC][WARN] Ping rejected: invalid payload");
            return;
        }

        Print("[PNH_CORE][RPC] PING received on server");
        PNH_Diagnostics.Log("Core", "RPC", "Valid PING request received");
        GetRPCManager().SendRPC("PNH_Core", "RPC_Pong", new Param1<int>(301), true, sender);
    }

    // Server -> client. Acknowledge only valid demo payload.
    void RPC_Pong(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame() || !GetGame().IsClient() || type != CallType.Client)
            return;

        Param1<int> data;
        if (!ctx.Read(data) || !data || data.param1 != 301)
            return;

        Print("[PNH_CORE][RPC] PONG received on client");
        GetRPCManager().SendRPC("PNH_Core", "RPC_Ack", new Param1<int>(301), true);
    }

    // Client -> server. Demonstrates round trip via server log only.
    void RPC_Ack(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (!GetGame() || !GetGame().IsServer() || type != CallType.Server || !sender)
            return;

        Param1<int> data;
        if (!ctx.Read(data) || !data || data.param1 != 301)
            return;

        Print("[PNH_CORE][RPC] ACK received on server (round trip complete)");
        PNH_Diagnostics.Log("Core", "RPC", "Round trip ACK received");
    }
};
