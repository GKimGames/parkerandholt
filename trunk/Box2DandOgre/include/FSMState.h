/*=============================================================================

	FSMState.h

	Author: Matt King

	Finite State Machine State

=============================================================================*/
#ifndef FSMSTATE_H
#define FSMSTATE_H

#include "Message.h"

template <class T>
class FSMState
{
public:

  virtual ~FSMState(){}

  /// This is called when the state is entered.
  virtual void Enter(T*)=0;

  /// Update.
  virtual void Update(T*)=0;

  /// Used to handle messages.
  virtual void HandleMessage(T*, const KGBMessage message)=0;

  /// This will execute when the state is exited.
  virtual void Exit(T*)=0;
};

#endif