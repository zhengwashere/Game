# Black Veil - Progress Report

**Status date:** current build. **Engine target:** UE 5.3+.

This report is deliberately literal. It distinguishes **implemented** (code and
data exist and are complete), **authored** (content written, requires the editor to
become playable) and **not done**.

---

## 1. Headline numbers

| Metric | Count |
|--------|-------|
| C++ header files | 60 |
| C++ implementation files | 56 |
| Total C++ files | 116 |
| Mission data assets authored | 12 |
| Level design documents | 12 |
| Dialogue scripts | 12 |
| Data tables (CSV) | 5 |
| Python tools | 4 |
| Automation tests authored | 4 |
| Automation tests executed | 0 |
| Missions playtested | 0 |

---

## 2. System-by-system status

Legend: **I** = implemented in code. **A** = authored content. **V** = verified by execution.

| System | I | A | V | Notes |
|--------|---|---|---|-------|
| Third-person character + camera | Y | Y | N | Full movement suite; Enhanced Input bindings declared; needs Input Action assets |
| Walk / jog / sprint / crouch / prone | Y | Y | N | Speed tiers with per-tier noise emission |
| Silent movement | Y | Y | N | Forces silent tier, suppresses noise radius |
| Cover + corner peek | Y | Y | N | Forward/left/right probes, stance classification |
| Wall hug | Y | Y | N | Shares the cover system; camera offset on lean |
| Lean around corners | Y | Y | N | Lateral camera offset + visibility penalty |
| Vault / climb / mantle | Y | Y | N | Knee-height and chest-height traces |
| Ledge hang | Y | Y | N | Trace to ledge lip, MOVE_Flying hang state |
| Surface-based footstep noise | Y | Y | N | Physical material to EBVSurfaceType mapping |
| Takedowns (behind / above / corner) | Y | Y | N | Angle and elevation classification |
| Carry and hide bodies | Y | Y | N | Carry forces silent movement; concealment spots |
| Interaction framework | Y | Y | N | Interface + component, weighted focus selection |
| Doors: quiet / normal / force | Y | Y | N | Multi-verb, lock types, power state |
| Locks: lockpick / hack / keycard / biometric | Y | Y | N | Biometric deliberately unpickable |
| Light and shadow visibility system | Y | Y | N | Samples real light components with occlusion |
| Dynamic / destructible lighting | Y | Y | N | Switchable, shootable, EMP-able, flickering |
| Visibility HUD indicator | Y | Y | N | Meter + shadow state + detection flash |
| Noise propagation with occlusion | Y | Y | N | Wall attenuation halves radius per blocker |
| AI perception (sight + hearing) | Y | Y | N | Light-adjusted sight validation |
| AI patrol routes | Y | Y | N | Waypoints, wait, look-around, conversations |
| AI suspicion / alert ladder | Y | Y | N | Unaware to Combat with timed de-escalation |
| AI investigation and search | Y | Y | N | Ring search projected onto NavMesh |
| AI squad communication | Y | Y | N | Approximate contact sharing, teamwork radius |
| AI coordinated search sectors | Y | Y | N | Sector fan-out by squad manager |
| AI reinforcements | Y | Y | N | Alarm raises squad; mission logic spawns wave |
| AI body discovery | Y | Y | N | Periodic LOS check for concealed/unconcealed bodies |
| AI cover use in combat | Y | Y | N | Cover scoring by line-of-sight break |
| Guard archetypes (6) | Y | Y | N | Standard, Armored, Tech, Surveillance, Commander, Elite |
| Guard personality model | Y | Y | N | Vigilance, bravery, teamwork, reaction, memory, patience |
| Suppressed pistol | Y | Y | N | 700 cm noise radius |
| Suppressed rifle | Y | Y | N | Automatic, 1100 cm noise radius |
| Non-lethal projector | Y | Y | N | Incapacitates without a kill |
| Noise maker | Y | Y | N | Pulses for 12 s so guards commit |
| Flashbang | Y | Y | N | Facing check; blind cuts sight to 8% |
| Smoke grenade | Y | Y | N | Spawns tagged sight blocker |
| EMP device | Y | Y | N | Disables all electronic devices in radius |
| Sticky camera | Y | Y | N | Scene capture feed + distraction noise |
| Hacking device | Y | Y | N | Tier-scaled duration; minigame widget |
| Lockpick | Y | Y | N | Difficulty-scaled, low noise |
| Fiber-optic camera | Y | Y | N | Under-door deployment |
| Surveillance detector | Y | Y | N | Passive coverage reveal |
| Equipment inventory + loadout | Y | Y | N | 4 gadget slots, ID-to-class registry |
| Security cameras | Y | Y | N | Sweep, cone, light-aware detection, alarm group |
| Security terminals | Y | Y | N | Kill cameras / unlock doors / kill lights |
| Alarm panels + sabotage | Y | Y | N | Group alarm, sabotage, reinforcement call |
| Hiding spots | Y | Y | N | Body concealment and player hiding |
| Mission data asset framework | Y | Y | N | Data-driven, 12 assets authored |
| Mission manager | Y | Y | N | Objectives, prerequisites, checkpoints, scoring |
| Runtime enemy spawning | Y | Y | N | Difficulty-scaled from mission data |
| Checkpoint system | Y | Y | N | Transform + mission time restore |
| Intel collection | Y | Y | N | 36 items authored across 12 missions |
| Main menu | Y | Y | N | New/Continue/Mission Select/Options/Quit |
| Mission select + grades | Y | Y | N | Unlock gating, per-mission grade display |
| Mission briefing screen | Y | Y | N | Loads from mission data asset |
| Equipment selection screen | Y | Y | N | Unlocked-only, 4 slots + weapon |
| Difficulty settings | Y | Y | N | 4 tiers, applied to enemy count/perception |
| Mission complete screen + grading | Y | Y | N | S/A/B/C/D from detections, alarms, kills, intel, optionals |
| Save / load campaign | Y | Y | N | USaveGame, auto-save on progression |
| Mission replay | Y | Y | N | Replay button; best grade retained |
| HUD: crosshair, objectives, ammo, gadgets | Y | Y | N | Separate widgets, C++ data binding |
| HUD: alert + threat bearing | Y | Y | N | Directional pip to nearest hunting guard |
| Subtitle / radio dialogue display | Y | Y | N | Speaker tag + auto-dismiss |
| Audio manager | Y | Y | N | Alert music states, surface footsteps, dialogue |
| Spatial audio for gameplay events | Y | Y | N | All gameplay sounds played at world location |
| Gameplay tags | Y | Y | N | Native tags + config declaration |
| Automated tests | Y | N | N | 4 tests authored, none executed |
| 12 campaign missions | Y | Y | N | Distinct setting, layout, objectives, enemy comp, set piece |
| Level design docs | N | Y | N | One per mission |
| Dialogue scripts | N | Y | N | One per mission |
| Story bible | N | Y | N | Characters, factions, arc, three endings |
| Level construction Python tools | Y | Y | N | LevelSetup, SecuritySystemSetup, MissionObjectiveSetup, validator |

---

## 3. Mission implementation checklist

| # | Mission | Data asset | Objectives | Patrols | Intel | Checkpoints | Lighting plan | Set piece | Level built | Playtested |
|---|---------|-----------|------------|---------|-------|-------------|---------------|-----------|-------------|------------|
| 1 | Dead Drop | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 2 | Blackout | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 3 | Ghost Protocol | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 4 | Cold Harbor | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 5 | Paper Trail | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 6 | Deep Cover | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 7 | Dead Signal | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 8 | Silent Depths | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 9 | The Exchange | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 10 | Broken Arrow | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 11 | No Witnesses | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |
| 12 | Black Veil | Yes | Yes | Yes | Yes | Yes | Yes | Yes | No (needs editor) | No |

> The final two columns are the honest ones: every mission is fully authored and
> structurally validated, and none has been constructed in the editor or played.

---

## 4. What remains

| Task | Why it is not done | Effort |
|------|--------------------|--------|
| Compile the C++ module | No Unreal toolchain in the authoring environment | Hours (mostly iteration on warnings) |
| Create Blueprint assets | Requires the editor | 1-2 days |
| Build the 12 maps | Requires the editor; the Python tools generate the scaffolding | 2-4 weeks (art-dependent) |
| Author animation blueprints and meshes | Requires the editor and art | Weeks (art-dependent) |
| Record dialogue and music | Requires production | Weeks |
| Run the automation tests | Requires a compiled module | Minutes |
| Playtest all 12 missions | Requires built maps | 1-2 weeks |

---

## 5. Design decisions worth knowing

1. **The AI can never cheat.** This was treated as a hard architectural rule, not
   a preference. Guards only act on perception, sound and communicated contacts.
   Every guard-related bug fix must respect this constraint.
2. **Lighting is a gameplay input, not a visual.** Visibility is computed from the
   actual light components in the level, so a designer who places a lamp changes
   difficulty without touching a single gameplay value.
3. **Missions are data, not code.** Twelve missions exist without twelve
   subclasses. Mission 13 will not need one either.
4. **Every mechanic is C++, every asset is Blueprint.** This keeps iteration fast
   while keeping the systems testable and reviewable.
5. **Non-lethal is a first-class path.** The scoring system, the weapon set, the
   takedown system and the body-concealment system all exist to make Panther play
   as viable and as satisfying as Ghost.
