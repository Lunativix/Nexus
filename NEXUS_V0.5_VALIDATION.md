# NEXUS V0.5 VALIDATION

Date: 2026-09-22  
Engine: UE 5.8.2  
Target compiled: `Game1Editor Win64 Development` — **PASS** (UBT Result: Succeeded)  
Smoke: `-game -nullrhi -NexusSmokeTest` → `Saved/Logs/NexusV05Smoke.log`  
Map: `/Game/NEXUS/Maps/Velasqo/LV_Velasqo_Greybox`

**V0.5 is not accepted as a playable competitive alpha.**  
Systems were hardened and several runtime tests ran. Full menu→match-end in listen+client, packaged EXE, and A↔B bot rotation were **not** proven.

---

## 1. Build status

| Item | Status | Details |
|---|---|---|
| Game1Editor Win64 Development | PASS | UBT Succeeded after restoring `ENexusTeam` (was missing from `NexusTypes.h`) |
| Editor / map load (headless -game) | PASS | LV_Velasqo_Greybox loaded; smoke exited cleanly |
| Interactive Editor launch | NOT VERIFIED | this run used `-nullrhi -unattended` |
| Packaged Win64 Development/Shipping EXE | NOT VERIFIED | no cook/package this session; no `.exe` found under StagedBuilds |

## 2. Files modified / added (V0.5)

- `Source/NEXUS/Public/NexusTypes.h` — Faction, round-end reason, device state; **restored `ENexusTeam`**
- `Source/NEXUS/Public/NexusCombatTypes.h` — bot states/roles/difficulty, OT series config
- `Source/NEXUS/Public/NexusGameplaySettings.h` — bot settings, hit/hitbox/wallbang debug flags
- `Source/NEXUS/NEXUS.h` / `NEXUS.cpp` — log categories
- `NexusPlayerState` / `NexusGameState` / `NexusGameMode` — faction scoring, rematch, cleanup, OT, economy
- `NexusCharacter` — server fire-rate gate, footsteps timer
- `NexusHitscanComponent` — hit / wallbang debug
- `NexusDoor` / `NexusDestructibleSurface` — round reset
- `NexusAIController` — plant/defuse/retake/roles
- `NexusHUD` — faction scoreboard, spectator, match end, overtime
- `NexusPlayerController` — shipping-guarded debug execs
- `Debug/NexusGameplayTests.*` — operators, ultimates, spawns, network dump, economy panel
- This report

Geometry / Recast / weapons catalogue / operators / maps: **not rewritten**.

## 3. Systems implemented (extend, not duplicate)

| System | Status | Details |
|---|---|---|
| Match / round SM | PASS (code+smoke OT) | Buy→Freeze→Live→Planted→PostRound→Overtime; `SetMatchPhase` logs |
| Side switch | NOT VERIFIED | code at round 12; no 12-round session this run |
| Overtime | PASS (forced) | `nexus.ForceOvertime` path: 12-12 → `bOvertime=1` Phase=Overtime |
| Bomb plant/defuse | PASS (timed hold in smoke) | PlantComplete planted=1; DefuseComplete planted=0 Defenders win |
| Spectator HUD | NOT VERIFIED | overlay exists; no death→spectate in interactive PIE |
| Server fire validation | PASS (code) | min interval 0.82×(60/RPM); client cannot apply hit |
| Economy values | PASS (config dump) | start=800 win=3000 loss=1900 kill=200 plant/defuse=300 max=9000 |
| Buy duplicate weapon | PASS (code) | skip spend if already owning slot |
| Bot 5v5 fill | PASS | Attackers=5 Defenders=5 ops assigned=10 |
| Scoreboard TAB | NOT VERIFIED | faction columns + ping in HUD; CanvasDraw=0 under nullrhi |
| Rematch command | NOT VERIFIED | `nexus.Rematch` compiled; not exercised after MatchEnd |
| Main menu / settings / FOV | NOT VERIFIED | not implemented this pass; GameDefaultMap is Velasqo |

## 4. Systems fixed

- Round win credits use **Faction**, not round role (side-switch safe)
- Round cleanup: gadgets destroyed; doors/surfaces `ResetForNewRound`
- Fire-rate anti double-fire on authority
- Spectator cycle uses **Faction** (not enemy after switch)
- UHT: `ENexusTeam` restored (compile blocker)
- C4458 `FactionForRole(Role)` renamed `RoundRole`
- Missing `LastFootstepTime`

## 5. Tests executed

Command: `UnrealEditor.exe Game1.uproject -game -nullrhi -unattended -NexusSmokeTest`  
Log: `Game1/Saved/Logs/NexusV05Smoke.log`

## 6. PASS

| SYSTEM | STATUS | DETAILS |
|---|---|---|
| Compile Game1Editor | PASS | Development Win64 |
| Level validator | PASS | 16/16 including no spawn→site LOS (50 traces) |
| Spawn validator | PASS | atk=15 def=10 overlap=0 |
| Spawn → Site A / B nav | PASS | CONNECTED 127.1m / 95.8m |
| Weapon wallbang math | PASS | wood/plaster/stone/structural as expected |
| Round phase present | PASS | Live t=180 |
| 5v5 roster | PASS | 5+5 |
| Operator defs + gadget spawn | PASS | AZE–HAWK; 10 spawn, 0 leftover |
| Ultimate activate | PASS | charge spent, actor spawned |
| Live damage / death / armor | PASS | HP 78.7 then 0; armor 50→36 |
| Gadget deploy | PASS | count increased |
| Plant start / interrupt / 4s complete | PASS | PlantComplete planted=1 Planted phase |
| 7s defuse complete | PASS | PostRound Defenders |
| Overtime force 12-12 | PASS | Phase=Overtime |
| Door replication **flag** | PASS | 11 doors `GetIsReplicated()` |
| Economy config dump | PASS | V0.3 values |

## 7. FAIL

| SYSTEM | STATUS | DETAILS |
|---|---|---|
| Nav A → B | FAIL | PARTIAL ~1.9–3.4m, endErr ~46–57m. Diagnostic only; **NavMesh/layout not modified**. |
| Nav B → A | FAIL | PARTIAL ~6.0m, endErr ~46.9m. Same. |
| HUD CanvasDraw (this smoke) | FAIL | DrawCount=0 under `-nullrhi` (no Canvas tick). Does **not** prove HUD broken in PIE with viewport. |

Connectivity matrix: CONNECTED=19 PARTIAL=11 FAILED=0. Spawn can reach both sites; **site-to-site Recast path is partial** (likely island / door / gap — not hidden by rebake).

## 8. NOT VERIFIED

| SYSTEM | STATUS | DETAILS |
|---|---|---|
| Mouse / PIE look | NOT VERIFIED | headless |
| HUD pixels / scoreboard on screen | NOT VERIFIED | nullrhi |
| Listen server NM_ListenServer | NOT VERIFIED | NetMode=Standalone (0) |
| Client replication (2nd process) | NOT VERIFIED | no remote PC |
| NetSim 50–200ms gameplay | NOT VERIFIED | CVar wired; not playtested |
| Full 13-round match | NOT VERIFIED | smoke ≠ 13 rounds |
| Side switch at round 12 | NOT VERIFIED | code only |
| OT series to match end | NOT VERIFIED | force-OT only |
| Rematch after MatchEnd | NOT VERIFIED | |
| Menu / PLAY / SETTINGS / QUIT | NOT VERIFIED | no menu map |
| FOV 80–110 / keybinds save | NOT VERIFIED | not implemented |
| Spectator after death (Q/E/wheel) | NOT VERIFIED | |
| Bot combat / plant / defuse / retake in a live round | NOT VERIFIED | spawn PASS; A↔B FAIL blocks retake proof |
| All 16 weapons fire-rate/ADS in PIE | NOT VERIFIED | catalogue exists; no per-gun live fire suite |
| Door open visible on client | NOT VERIFIED | replicate flag only |
| Destruction reset mid-match visual | NOT VERIFIED | |
| Packaged EXE launch / packaged MP | NOT VERIFIED | not built |
| 1080p 60 FPS | NOT VERIFIED | no stat unit |
| `nexus.AutoMatchTest` 13-round proof | NOT VERIFIED | command starts match only; log states it does not simulate 24 rounds |

## 9. BLOCKED

| SYSTEM | STATUS | DETAILS |
|---|---|---|
| Bot A↔B rotate / retake | BLOCKED_BY_NAVIGATION | A→B and B→A Recast paths PARTIAL. No geometry/NavMesh change per V0.5 rules. |
| Competitive listen+client loop | BLOCKED | needs PIE listen + 2nd client (not this smoke) |
| Packaged bot match / packaged MP | BLOCKED | no package this session |

## 10. Known issues

1. **A↔B Recast**: paths stop ~3–6m from start with ~50m residual. Spawn→A and Spawn→B work. Diagnose further (NavLink, door collision, floor island) before any mesh edit.
2. **Human PlayerState faction vs role** in smoke: PC was `Team=Defenders` and `Faction=Vanguard` (faction assigned on first team, then roster fill moved role). Side-switch identity is intentional; **initial** assignment can desync if the pawn is re-teamed.
3. **HUD DrawHUD** does not run under `-nullrhi`.
4. **`nexus.AutoMatchTest` / `StartBotMatch`** fill 5v5 and start flow; they do **not** auto-simulate a 13-round match.
5. Debug execs compiled out of Shipping (`#if !UE_BUILD_SHIPPING`).
6. No dedicated main-menu map; default map is the greybox match.

## 11. Performance

NOT VERIFIED (`stat fps` / `stat unit` not captured).

## 12. Network results

| Check | Status |
|---|---|
| NetMode in smoke | NM_Standalone (0) — **NOT VERIFIED** for Listen/Client |
| `nexus.NetworkTest` | logs AuthGM + replicate flags; remote client NOT VERIFIED |
| `nexus.NetSim` | sets `NetPktLag` — playtest NOT VERIFIED |

## 13. Bot results

| Check | Status |
|---|---|
| Spawn 5v5 | PASS |
| Navigate spawn→A/B | PASS (path query) |
| Navigate A↔B | FAIL |
| Combat / plant / defuse / retake as AI | NOT VERIFIED / retake BLOCKED_BY_NAVIGATION |

## 14. Packaged build result

**NOT VERIFIED.** No packaged binary produced or launched this session.

## 15. Next recommended version

**V0.5.7–0.5.10 remain open:** diagnose A↔B Recast (still no layout rewrite until the island/door/gap is identified), then bot rotate/retake.

**V0.5.6 / 0.5.16:** PIE Listen Server + 1 client; then Development package + launch `.exe`.

**V0.5.13 UX:** main menu map (PLAY local / bot match / quit) without replacing Velasqo.

**Do not start V0.6** until A↔B is diagnosed, listen+client is PASSed, and packaged launch is PASSed.

---

### Acceptance checklist (honest)

- [x] Project compiles (Editor Development)
- [x] Headless map load
- [x] Level validator PASS
- [x] Spawn validator PASS
- [x] Spawn→A / Spawn→B navigation PASS
- [ ] A→B navigation **FAIL**
- [ ] B→A navigation **FAIL**
- [x] Wallbang math PASS
- [x] Damage / armor / death PASS (server ApplyDamage in smoke)
- [x] Bomb plant PASS (4s)
- [x] Bomb defuse PASS (7s)
- [ ] Spectator **NOT VERIFIED**
- [x] Economy config PASS
- [ ] Buy in UI **NOT VERIFIED** (server buy exists)
- [x] Operator gadget spawn PASS
- [x] Ultimate spawn PASS
- [ ] Doors on client **NOT VERIFIED**
- [ ] Destruction visual **NOT VERIFIED**
- [x] Bots spawn PASS
- [ ] Bots navigate A↔B **FAIL**
- [ ] Bots combat/plant/defuse/retake **NOT VERIFIED / BLOCKED**
- [ ] Listen server **NOT VERIFIED**
- [ ] Client replication **NOT VERIFIED**
- [ ] Latency playtest **NOT VERIFIED**
- [ ] Side switch **NOT VERIFIED**
- [x] Overtime force PASS
- [ ] Full match **NOT VERIFIED**
- [ ] Match end + rematch **NOT VERIFIED**
- [ ] Packaged EXE **NOT VERIFIED**
