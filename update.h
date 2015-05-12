#ifndef _UPDATE_
#define _UPDATE_

void Move_Ent(Entity *thisEnt, double xAmnt, double yAmnt);
void Draw_Ent (Entity *thatEnt);
void Draw_All ();
void update();
void DrawUI();
void editMode();
void LevelTower(int towerNum);
void pathCol();
void doWave();
void bulletToBloonCol (Entity *tempEnt);
void towerToTowerCol (Entity *tempEnt);

Uint32 TIME;

#endif
