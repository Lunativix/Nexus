# NEXUS V0.6.1 — FROM BLOCKOUT TO CITY DRESS

Date: 2026-09-22  
Map: `LV_Velasqo_Greybox` (layout inchangé)  
Smoke: `Saved/Logs/NexusV061Smoke.log` exit **0**

**Velasqo n’est pas art-complete.**  
Le damier debug / mélange sol-façade est corrigé. La map reste un **kit cube instancié** avec palette unie + fenêtres/cadres/sol zoné. Pas de `SM_Vel_*` unique. First-person PIE **NOT VERIFIED** (cette session = `-nullrhi`).

---

## Ce qui a changé (visuel seulement)

| Item | Status | Notes |
|---|---|---|
| Debug/grid floor | **PASS** (smoke) | Plus de `DefaultMaterial` / WorldGrid sur ISM visuels ; plus de damier façade+sol |
| Palette | **PARTIAL** | MID `BasicShapeMaterial` colorés (asphalte, plâtre, grès, béton, bois, métal, verre, tissu, céramique). `NEXUS_MI_*` uasset : commandlet crash (asset partiel) — **pas de PBR textures** |
| Façades | **PARTIAL** | Soubassement + corniche + kit fenêtre (cadre/verre/appui/volets) ; familles VQ / BH bleu / EH / B industriel |
| Fenêtres | **PARTIAL** | 683 kits procéduraux (cap levé). **SM_Vel_Window_*** **NOT IMPLEMENTED** |
| Portes | **PARTIAL** | 11 portes NEXUS existantes + cadre visuel. Pas de 2e système |
| Toits | **PARTIAL** | Parapet visuel + 2 kits AC/antenne. Pas de toits walkables nouveaux |
| Rues | **PARTIAL** | Asphalte + overlays plaza (grès) / market (bois) / Site B (béton) / ruelles |
| Market | **PARTIAL** | 24 props (toiles, caisses, câbles, enseigne) |
| Plaza / fontaine | **PARTIAL** | Bassin pierre + eau + 11 dress plaza |
| Site B | **PARTIAL** | Béton/métal + 9 warehouse + 17 colonnes caravansérail |
| Alleys | **PARTIAL** | 111 (AC, câbles, bacs, portes visuelles) corridor 2.2 m intact |
| Blue House | **PARTIAL** | Façade teinte bleu poussiéreux (`VisualMilitary`) |
| East House | **PARTIAL** | Plâtre clair vs VQ grès |
| Caravanserai | **PARTIAL** | Colonnes/linteaux NoCollision |
| Végétation / véhicules | **PARTIAL** | Cônes feuillage + 10 véhicules NoCollision (pas de cover extra) |
| Lighting | **PARTIAL** | Evening + 13 practical, fog allégé |
| Screenshots FPS | **NOT VERIFIED** | nullrhi |

Collision greybox, spawns, sites, Recast, portes gameplay, destruction : **inchangés**.

---

## Smoke (exécuté)

```
GREYBOX HIDDEN     PASS  7 ISM
ARCH INSTANCES     PASS  4405
NO DEBUG GRID MAT  PASS  debugISM=0 solidISM=16
Validator          PASS  16/16
Spawn → A / B      PASS
A → B / B → A      FAIL  (même PARTIAL Recast — mesh non recuit)
Plant / Defuse / OT PASS
FIRST PERSON       NOT VERIFIED
```

---

## Definition of Done (honnête)

- [partial] Plus de look greybox **debug/grid** (damier)
- [ ] Ville crédible unique
- [partial] Façades / fenêtres cubes
- [ ] Kit `SM_Vel_Window_*` / `SM_Vel_Door_*`
- [partial] Rues zonées
- [partial] Market / Plaza / Site B / Alleys
- [ ] Intérieurs meublés
- [ ] Toits jouables intentionnels
- [ ] Audio spatial
- [ ] Perf / FPS viewport / MP 2 clients
- [x] Gameplay layout préservé

**Prochaine étape réelle :** meshes de fenêtre/porte/toit + matériaux avec albedo/normal (pas un 2e pass de cubes).
