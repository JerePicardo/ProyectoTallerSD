/*
 * Brazo.h
 *
 *  Created on: 4 jun 2026
 *      Author: PC-Casa
 */

#ifndef INC_BRAZO_H_
#define INC_BRAZO_H_

#include <BrazoFSM1.h>
#include <pca9685.h>


void UpdateBrazo(uint8_t posicion[7]);
void Brazo_MoveJoint(uint8_t posicion[7],uint8_t ch, uint8_t deg);
void Brazo_MoveCartesian(uint8_t posicion[7],uint8_t x, uint8_t y, uint8_t z);
void park(uint8_t posicion[7]);

#endif /* INC_BRAZO_H_ */
