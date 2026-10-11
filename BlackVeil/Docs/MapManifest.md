# Map Manifest

The game expects **14 levels**. The names are not free choices: they are
referenced by the mission data assets (`MapAsset`), by `DT_Missions.csv`, and
hard-coded in the C++ that opens them (`BVBriefingWidget`, `BVMissionCompleteWidget`,
`BVMainMenuWidget`, `BVPauseMenuWidget`).

## Levels

| Map asset | Type | Built by | Contents |
|-----------|------|----------|----------|
| `/Game/Maps/MainMenu` | Front end | `BuildAllMaps.build_frontend()` | Empty; the main menu widget fills it |
| `/Game/Maps/Briefing` | Front end | `BuildAllMaps.build_frontend()` | Empty; briefing + equipment select widgets fill it |
| `/Game/Maps/M01` | Mission 1 | `BuildAllMaps.build_one(1)` | Dead Drop - 6 rooms, 19 cover props, 8 lights, 6 guards |
| `/Game/Maps/M02` | Mission 2 | `BuildAllMaps.build_one(2)` | Blackout - 6 rooms, 14 cover props, 7 lights, 7 guards |
| `/Game/Maps/M03` | Mission 3 | `BuildAllMaps.build_one(3)` | Ghost Protocol - 5 rooms, 12 cover props, 6 lights, 6 guards |
| `/Game/Maps/M04` | Mission 4 | `BuildAllMaps.build_one(4)` | Cold Harbor - 6 rooms, 20 cover props, 7 lights, 7 guards |
| `/Game/Maps/M05` | Mission 5 | `BuildAllMaps.build_one(5)` | Paper Trail - 6 rooms, 22 cover props, 7 lights, 8 guards |
| `/Game/Maps/M06` | Mission 6 | `BuildAllMaps.build_one(6)` | Deep Cover - 6 rooms, 11 cover props, 6 lights, 6 guards |
| `/Game/Maps/M07` | Mission 7 | `BuildAllMaps.build_one(7)` | Dead Signal - 6 rooms, 11 cover props, 7 lights, 8 guards |
| `/Game/Maps/M08` | Mission 8 | `BuildAllMaps.build_one(8)` | Silent Depths - 7 rooms, 10 cover props, 7 lights, 8 guards |
| `/Game/Maps/M09` | Mission 9 | `BuildAllMaps.build_one(9)` | The Exchange - 5 rooms, 14 cover props, 7 lights, 8 guards |
| `/Game/Maps/M10` | Mission 10 | `BuildAllMaps.build_one(10)` | Broken Arrow - 6 rooms, 16 cover props, 9 lights, 9 guards |
| `/Game/Maps/M11` | Mission 11 | `BuildAllMaps.build_one(11)` | No Witnesses - 6 rooms, 16 cover props, 9 lights, 10 guards |
| `/Game/Maps/M12` | Mission 12 | `BuildAllMaps.build_one(12)` | Black Veil - 6 rooms, 16 cover props, 8 lights, 12 guards |

## Mission map detail

| # | Mission | Location | Map | Rooms | Cover | Lights | Guards | Layout spec |
|---|---------|----------|-----|-------|-------|--------|--------|-------------|
| 1 | Dead Drop | Kostov Steelworks, Ukraine | `M01` | 6 | 19 | 8 | 6 | `Content/Data/Layouts/M01_Layout.json` |
| 2 | Blackout | Vasiliev Power Station, Georgia | `M02` | 6 | 14 | 7 | 7 | `Content/Data/Layouts/M02_Layout.json` |
| 3 | Ghost Protocol | Halcyon Tower, Singapore | `M03` | 5 | 12 | 6 | 6 | `Content/Data/Layouts/M03_Layout.json` |
| 4 | Cold Harbor | Port of Kalamis, Cyprus | `M04` | 6 | 20 | 7 | 7 | `Content/Data/Layouts/M04_Layout.json` |
| 5 | Paper Trail | Directorate Archive, Vienna | `M05` | 6 | 22 | 7 | 8 | `Content/Data/Layouts/M05_Layout.json` |
| 6 | Deep Cover | Verdant Gala, Monaco | `M06` | 6 | 11 | 6 | 6 | `Content/Data/Layouts/M06_Layout.json` |
| 7 | Dead Signal | Kestrel Relay Station, Caucasus | `M07` | 6 | 11 | 7 | 8 | `Content/Data/Layouts/M07_Layout.json` |
| 8 | Silent Depths | Anvil Deep Research Station, Arctic | `M08` | 7 | 10 | 7 | 8 | `Content/Data/Layouts/M08_Layout.json` |
| 9 | The Exchange | Marchetti Compound, Sicily | `M09` | 5 | 14 | 7 | 8 | `Content/Data/Layouts/M09_Layout.json` |
| 10 | Broken Arrow | Site 14 Research Complex, Kazakhstan | `M10` | 6 | 16 | 9 | 9 | `Content/Data/Layouts/M10_Layout.json` |
| 11 | No Witnesses | UMBRA Operations Center, Prague | `M11` | 6 | 16 | 9 | 10 | `Content/Data/Layouts/M11_Layout.json` |
| 12 | Black Veil | Sorn Estate, Black Sea Coast | `M12` | 6 | 16 | 8 | 12 | `Content/Data/Layouts/M12_Layout.json` |

## How to build them

```python
import sys
sys.path.append(r"<path>/BlackVeil/Tools")
import BuildAllMaps

BuildAllMaps.build_all()      # creates all 14 maps and builds M01..M12
BuildAllMaps.build_one(3)     # rebuild just mission 3
BuildAllMaps.build_frontend() # just MainMenu + Briefing
```

Each mission map is built in four passes: `BlockoutBuilder` (space),
`LevelSetup` (objectives, spawns, routes, intel, security), `SecuritySystemSetup`
(camera/terminal/alarm network), `MissionObjectiveSetup` (objective volumes).

## Gameplay flow between maps

```
MainMenu --StartNewGame/Continue--> Briefing --LaunchMission--> M01
M01 --results screen, Continue--> M02 --...--> M12
any mission --PauseMenu ReturnToMainMenu--> MainMenu
any mission --death/restart--> same map (checkpoint restore, or reload)
```
