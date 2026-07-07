#ifndef ManndoFSM_h
#define ManndoFSM_h
#include "definiciones.h"
#include <Arduino.h>
#include "Hardware.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wire.h>


bool pushEvent(evento ev);
evento popEvent(void);
evento updateEvents(void);
void stateMachine(Mando * M);
const char *eventoToString(evento ev);
const char *estadoToString(estado st);
void printStatus(Mando *M);
void processManualCommands(Mando *M);
//transmitir un mensaje 
//FSM_update
//FSM Innit
//Printear Datos

#endif