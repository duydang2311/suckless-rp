#if defined item_h_included
    #endinput
#endif

#define item_h_included

forward Task:Item_GetAbility(id);

forward Item_OnNameGet(id, ability, Task:task);
public Item_OnNameGet(id, ability, Task:task) { return 1; }
