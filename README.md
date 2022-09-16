# suckless-rp

[![sampctl](https://img.shields.io/badge/sampctl-suckless%20rp-2f2f2f.svg?style=for-the-badge)](https://github.com/sundayproject/sunday)

[![sampctl](https://github.com/duydang2311/suckless-rp/actions/workflows/sampctl.yml/badge.svg)](https://github.com/duydang2311/suckless-rp/actions/workflows/sampctl.yml)

## Information

This gamemode was started in early 2021, for a local Vietnamese Role-Play server but then stopped in late 2021 as I moved to GTA:V.

[@tavenguyen](https://github.com/tavenguyen) joined as a new developer later in 2022 and helped making the features.

We decided to make this public as it is no longer developed and maintained.

## What we've done in this gamemode?

- Player module.

  - Essential data, event handling.

  - Inventory, hand components.
  
  - Permissions.

- Item module.

  - Gun, melee.
  
  - Sim card (integrating into the phone item).
  
  - Radio (comes with a frequency management system).
  
  - Backpack (using item group for containing items).
  
  - Whisk (for the farmer job).
  
  - Phone:
    
    - Dynamic model. We made one, which is Nokia.
    
    - System functionalities that is pre-installed with all types of phone (think of it like OS API).
    
    - Registering apps to be featured on the system App Store.
    
  - Item adjustable attachment, drop, pick, carry.
    
- Job.
  
  - Farmer (with moving NPC cows, feeding, loading on car and selling).
  
- Door, house modules are ready to use.

- Faction module is only ready at the core-level, such as faction essential info, rank, division. Your own development is required to build a specific faction, like features for a faction of type police.

- Frequency module: a point where radio item connects to and works.

- Other modules: Subtitle UI, Street, Weapon, Commands...

## Notes

- The source code of item module introduces a code pattern, which overall aims to simulate the inheritance in OOP (in a non-OOP language like Pawn), because there are so many types of item. The pattern is basically a top-level function (for public use by other modules) would make a `CallLocalFunction` and all types of item would listen to that call (use hooks), then modify the `ref` argument if the written condition inside is met. After all of the hooks are executed, the top-level function returns the value of the modified argument as its result.

- All the modules written early, store all the data in memory dynamically (thanks to PawnPlus). Later on, we see this as bad practice. Except for **very** frequently used data, we don't store any at all but just query the database every time we need it, and this is very handy to do thanks to PawnPlus task.

- No .sql file is needed because we create tables on gamemode run. Remember to run the gamemode twice so all the tables are created, due to table referencing another table and script execution order.
