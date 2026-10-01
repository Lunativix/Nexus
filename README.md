# NEXUS

FPS tactique compétitif **5v5**, développé en C++ sous **Unreal Engine 5.8**.

Deux équipes (**Vanguard** / **Sentinel**) s’affrontent sur un round plant / defuse, avec économie, opérateurs, gadgets et wallbang par matériau.

Ce dépôt est le projet Unreal : ouvrir `Game1.uproject`.

---

## État actuel

Prototype jouable en éditeur (PIE), pas une alpha compétitive packagée.

- Gameplay (rounds, bombe, bots 5v5, hitscan, portes, destruction) est en place.
- Les 3 cartes V1.0 existent en greybox + habillage visuel **NoCollision** (Cube / Cylinder / Sphere / Plane).
- Ce n’est **pas** le rendu photoréaliste du GDD.
- NavMesh Velasqo A↔B : **FAIL** (clusters Recast disjoints) — volontairement non « corrigé » en éditant la géométrie.
- EXE Shipping / listen server 2 clients : **non vérifiés**.

---

## Prérequis

- [Unreal Engine 5.8](https://www.unrealengine.com/) (association `5.8` dans `Game1.uproject`)
- Visual Studio 2022 (toolchain MSVC) pour compiler `Game1Editor`

Double-cliquer `Game1.uproject`, ou compiler :

```bat
Engine\Build\BatchFiles\Build.bat Game1Editor Win64 Development -Project="...\Game1.uproject" -WaitMutex
```

Carte par défaut : `/Game/NEXUS/Maps/Velasqo/LV_Velasqo_Greybox`

---

## Cartes (GDD V1.0)

| Carte | Thème | Taille | Package | Sites |
|---|---|---|---|---|
| **Velasqo** | Ville du Moyen-Orient | 120 × 120 m | `LV_Velasqo_Greybox` | A Place / B Entrepôt |
| **Skyline** | Ville moderne | 130 × 130 m | `LV_Skyline` | A Tour / B Parking |
| **Outpost** | Base de ravitaillement | 140 × 140 m | `LV_Outpost` | A Hangar / B Entrepôt |

En jeu (console) :

```
NexusOpenMap Skyline
NexusOpenMap Outpost
```

Téléport vers un spot GDD :

```
nexus.VelasqoTeleport marche
nexus.VelasqoTeleport mosquee
nexus.VelasqoTeleport helipad
nexus.VelasqoTeleport hangar
```

Spots Velasqo : `marche` · `mosquee` · `toits` · `ruelle` · `interieur` · `siteb`  
Spots Skyline : `hall` · `bureaux` · `helipad` · `parking` · `passerelle` · `a`  
Spots Outpost : `hangar` · `conteneurs` · `tourgard` · `ravitaillement` · `siteb`

---

## Gameplay

| Élément | Valeur |
|---|---|
| Format | 5v5, freeze → live → plant / defuse |
| Victoire | 13 rounds (max 24, overtime) |
| Bombe | plant 4 s · defuse 7 s · timer 40 s |
| Économie | start 800 · win 3000 · loss 1900 · kill 200 |
| Factions | Vanguard / Sentinel |
| Opérateurs | Aze, Brutus, Nyx, Vanta, Titan, Warden, Kraken, Echo, Bulwark, Hawk |

Match 10 bots : `NexusStartBotMatch`

---

## Structure

```
Game1.uproject
Config/                 réglages moteur + input
Content/NEXUS/          umaps + matériaux
Source/NEXUS/           runtime C++ (gameplay, cartes, art)
Source/NEXUSEditor/     commandlets (build maps, bake nav)
```

Les dossiers `Binaries/`, `Intermediate/`, `Saved/` ne sont pas versionnés.

---

## Smoke test

```bat
UnrealEditor.exe Game1.uproject -game -nullrhi -unattended -NexusSmokeTest
```

Charge Skyline / Outpost en ajoutant le chemin de carte :

```
"/Game/NEXUS/Maps/Skyline/LV_Skyline"
"/Game/NEXUS/Maps/Outpost/LV_Outpost"
```

---

## Licence

Projet privé / scolaire pour l’instant. Tous droits réservés sauf mention contraire.
