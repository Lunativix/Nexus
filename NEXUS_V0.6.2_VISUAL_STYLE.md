# NEXUS V0.6.2 — CS-STYLE READABILITY (NOT GREYBOX / NOT MINECRAFT)

Date: 2026-09-22  
Gameplay layout: **unchanged**  
Smoke: `Saved/Logs/NexusV062Smoke.log`

**This is still not a unique-mesh competitive map.**  
It is a readability pass: continuous streets, recessed windows, cover dressed as objects. Primitive engine shapes remain. First-person PIE **NOT VERIFIED** (`-nullrhi`).

---

## What changed (visual only)

| Problem | Change | Status |
|---|---|---|
| Tile/grid floor | 1 asphalt plane 120×120 + 4 zone slabs (plaza / B / market / alleys) | **PARTIAL** |
| Cube cover actors | `ANexusCover` mesh hidden; collision kept; overlay planter / jersey / crate | **PARTIAL** |
| Windows glued on walls | Recess cavity + frame + glass inset + sill; 3 size variants | **PARTIAL** (not `SM_Vel_Window_*`) |
| Floating roof slabs | Thin deck + 4 parapet edges | **PARTIAL** |
| Market stands as boxes | Counter + cloth, not a solid cube | **PARTIAL** |
| Cone trees | Foliage = cubes (no cones) | **PARTIAL** — still primitive |

Collision, spawns, sites, Recast, door interaction: **not modified**.

---

## Smoke

See log for PASS/FAIL. Expected: validator 16/16, A↔B still FAIL, first person NOT VERIFIED.

---

## Honest vs target

Target: “early playable professional competitive FPS.”  
Current: “modular cube kit with better depth and no floor grid.”

Someone can still tell it is engine primitives. Unique window/door/cover meshes + albedo/normal materials are required next.

Screenshots 1–8: **NOT VERIFIED** this session.
