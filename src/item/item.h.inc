#if defined item_h_included
    #endinput
#endif

#define item_h_included

forward Item_OnNameGet(ability, Task:task);
public Item_OnNameGet(ability, Task:task) { return 1; }
