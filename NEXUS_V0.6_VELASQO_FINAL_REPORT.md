# NEXUS V0.6.0 — VELASQO ENVIRONMENT RECONSTRUCTION

Date: 2026-09-22  
Engine: UE 5.8.2  
Playable map: **`LV_Velasqo_Greybox` (conservée)**  
`LV_Velasqo_Final`: **non créée** — l’enveloppe n’est pas encore un kit mesh unique.

Smoke: `-game -nullrhi -NexusSmokeTest`  
Log: `Saved/Logs/NexusV060Smoke2.log`  
Exit: **0** (plus de crash `TestUltimates`)

**Ce n’est pas la ville finale.**  
Le greybox n’est plus **rendu**. Autour des mêmes volumes : pavés, façades, toits, fenêtres, fontaine, portes habillées — toujours des **cubes / cylindres / cônes moteur** instanciés (NoCollision). Collision, spawns, sites, Recast, portes gameplay, wallbang **inchangés**.

---

## Reconstruction (exécutée)

| Étape | Résultat smoke |
|---|---|
| Conceal greybox ISM | **PASS** hiddenISM=7 visibleGreyISM=0 (collision+nav gardées) |
| Pavement 5 m | 576 tuiles |
| Architecture wrap | modules=264 windows=225 |
| Fontaine overlay | NoCollision sur cover existant |
| Portes NEXUS | 11 acteurs habillés (même système) |
| Total ISM visuel | **1484** (`ARCH INSTANCES` PASS) |
| Callouts | 25 volumes |
| Rooftop tanks | **0** |
| Alley dressing | 90 |
| Backdrop | 35 hors ±60 m |

Gameplay volumes **non** déplacés. NavMesh **non** recuit.

---

## Crash smoke (corrigé)

Cause : gadgets jammer **Destroy** pendant `TActorIterator` + timer `Expire` après destroy.  
Fix : collect-then-destroy, `EndPlay` ClearTimer, spawn gadgets espacés, cleanup après `TestUltimates`.  
`ULTIMATE Activate` **PASS** (before=0 after=1 pts=0).

---

## Validation (cette exécution)

```
MAP LOAD          PASS   LV_Velasqo_Greybox
BOUNDS            PASS
SPAWNS            PASS   15 / 10
SITES             PASS   A+B
ROUTES            PASS   validator
LOS spawn→site    PASS   50
DOORS             PASS   11
DESTRUCTION       PASS   20
WALLBANG table    PASS
LEVEL VALIDATOR   PASS   16/16
GREYBOX HIDDEN    PASS   7 ISM
ARCH INSTANCES    PASS   1484
Spawn → A         PASS   CONNECTED 127.1 m
Spawn → B         PASS   CONNECTED 95.8 m
A → B             FAIL   PARTIAL 1.9 m / err 49.7 m
B → A             FAIL   PARTIAL 6.0 m / err 46.9 m
OPERATORS 10      PASS
GADGET / ULTIMATE PASS
PLANT / DEFUSE    PASS   PlantComplete + DefuseComplete
OVERTIME          PASS   12-12 → Overtime
HUD CanvasDraw    FAIL   DrawCount=0 sous -nullrhi
LISTEN SERVER     NOT VERIFIED   NetMode=0
FIRST PERSON      NOT VERIFIED   nullrhi
PERFORMANCE       NOT VERIFIED
SM_Vel_* unique   FAIL / not shipped
LV_Velasqo_Final  not created
```

NavMesh **non modifié** pour masquer A↔B.

---

## Definition of Done (honnête)

- [x] Greybox n’est plus **rendu** (collision conservée)
- [partial] Bâtiments = enveloppes cubes, pas d’archi unique
- [partial] Façades / fenêtres / toits procéduraux
- [partial] Rues = grille de pavement
- [ ] Intérieurs meublés Blue/East House
- [ ] Toits walkables intentionnels (tanks=0)
- [partial] Market (toiles/goods existants)
- [partial] Plaza + fontaine visuelle
- [partial] Site A / B visuels (wrap, pas kit unique)
- [ ] Caravanserai mesh landmark
- [partial] South Alleys dressing
- [partial] Props / véhicules / végétation / électrique
- [x] Matériaux MID + destruction existante
- [x] Lighting evening + 8 practical
- [ ] Audio spatial zones
- [ ] NavMesh A↔B
- [ ] Bots A↔B
- [x] Gameplay préservé (spawns/sites/portes/wallbang smoke)
- [ ] Performance / FPS viewport / multiplayer 2 clients

---

## Remaining

1. Kit `SM_Vel_*` (murs 1/2/3 m, arches, appuis) **sans** changer collision greybox.
2. Intérieurs + toits contestables.
3. Diagnostiquer A↔B (preuve) avant tout Recast.
4. PIE first-person + listen client + `stat unit`.
5. Dupliquer `LV_Velasqo_Final` seulement quand le cube kit n’est plus le visuel.

**Gameplay > art. Aucune geo / NavMesh / spawn / site changée pour cacher un FAIL.**
