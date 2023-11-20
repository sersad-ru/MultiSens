#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The FindPin plugin searches for msPin number for selected wire.
* You should connect the wire to ground, then start FindPin plugin
* to see which msPin number is atteached to this wire.
*/

void plgFindPin(MultiSensCore& core);
