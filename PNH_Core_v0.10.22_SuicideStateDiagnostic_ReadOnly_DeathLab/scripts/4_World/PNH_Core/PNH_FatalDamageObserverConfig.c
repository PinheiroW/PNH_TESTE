// v0.10.18 - opt-in diagnostics; missing DiagnosticMode in older JSON defaults false.
class PNH_FatalDamageObserverConfig
{
    int SchemaVersion = 1;
    bool Enabled = false;
    bool DiagnosticMode = false;
};
