#include "AnimationBlender.h"

using namespace Ogre;
//=============================================================================
//						Constructor
//
/// Sets entity_ to entity.
AnimationBlender::AnimationBlender( Entity *entity ) : entity_(entity) 
{
}

//=============================================================================
//						Initialize
//
/// This sets the source animation to animation and disables all other
/// animation states and sets their weight and position to 0.
void AnimationBlender::Initialize(const String &animation, bool loop)
{
	AnimationStateSet *set = entity_->getAllAnimationStates();
	AnimationStateIterator it = set->getAnimationStateIterator();

	while( it.hasMoreElements() )
	{
		AnimationState *anim = it.getNext();
		anim->setEnabled(false);
		anim->setWeight(0);
		anim->setTimePosition(0);
	}

	source_ = entity_->getAnimationState( animation );
	source_->setEnabled(true);
	source_->setWeight(1);

	timeleft_ = 0;
	duration_ = 1;
	target_ = 0;
	complete_ = false;
	loop_ = loop;
} 

//=============================================================================
//						Blend
//
/// Blend to a new animation or switch to it depending on the BlendingTransition
/// passed to the function.
void AnimationBlender::Blend(
		const String &animation,
		BlendingTransition transition,
		Real duration,
		bool loop,
		Real targetTime)
{
	loop_ = loop;
	targetTime_ = targetTime;
	if( transition == AnimationBlender::BlendSwitch )
	{
		if( source_ != 0 )
		{
			source_->setEnabled(false);
		}

		source_ = entity_->getAnimationState( animation );
		source_->setEnabled(true);
		source_->setWeight(1);
		source_->setTimePosition(0);
		timeleft_ = 0;
	} 
	else 
	{ 
		AnimationState* newTarget = entity_->getAnimationState( animation );

		if( timeleft_ > 0 )
		{
			// oops, weren't finished yet
			if( newTarget == target_ )
			{
				// nothing to do! (ignoring duration here)
			}
			else if( newTarget == source_ )
			{
				// going back to the source state, so let's switch
				source_ = target_;
				target_ = newTarget;
				timeleft_ = duration_ - timeleft_; // i'm ignoring the new duration here
			}
			else
			{
				// ok, newTarget is really new, so either we simply replace the target with this one, or
				// we make the target the new source
				if( timeleft_ < duration_ * 0.5 )
				{
					// simply replace the target with this one
					target_->setEnabled(false);
					target_->setWeight(0);
				}
				else
				{
					// old target becomes new source
					source_->setEnabled(false);
					source_->setWeight(0);
					source_ = target_;
				} 

				target_ = newTarget;
				target_->setEnabled(true);
				target_->setWeight( 1.0 - timeleft_ / duration_ );
				target_->setTimePosition(0);
			}

		}
		else
		{
			// assert( target == 0, "target should be 0 when not blending" )
			// source_->setEnabled(true);
			// source_->setWeight(1);

			transition_ = transition;
			timeleft_ = duration_ = duration;
			target_ = newTarget;
			target_->setEnabled(true);
			target_->setWeight(0);
			target_->setTimePosition(0);
			
		}
	}
}


void AnimationBlender::AddTime( Real time )
{
	if( source_ != 0 )
	{
		if( timeleft_ > 0 )
		{
			timeleft_ -= time;
			if( timeleft_ < 0 )
			{
				// finish blending
				source_->setEnabled(false);
				source_->setWeight(0);
				source_ = target_;
				source_->setEnabled(true);
				source_->setWeight(1);
				target_ = 0;
			}
			else
			{
				// still blending, advance weights
				source_->setWeight(timeleft_ / duration_);
				target_->setWeight(1.0 - timeleft_ / duration_);

				if(transition_ == AnimationBlender::BlendWhileAnimating)
				{
					target_->addTime(time);
				}

			}
		}

		if (source_->getTimePosition() >= source_->getLength())
		{
			complete_ = true;
		}
		else
		{
			complete_ = false;
		}

		
		if (source_->getTimePosition() < targetTime_)
		{
			source_->addTime(time);
		}
		else if(loop_)
		{
			source_->setTimePosition(0);
		}

		source_->setLoop(loop_);
	}
}


