
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the GNU Affero General Public License Version 3.0
// (the "Additional License"). You may not use these modifications except in
// compliance with the additional license. You may obtain a copy of the Additional
// License at
//
// http://www.gnu.org/licenses/agpl.html
//
// Contact Automatak, LLC for a commercial license to these modifications
//
#ifndef __EVENT_BUFFERS_H_
#define __EVENT_BUFFERS_H_

#include "BufferSetTypes.h"
#include "EventBufferBase.h"

#include <opendnp3/Visibility.h>

#include <map>
#include <set>
#include <vector>
#include <limits>

namespace opendnp3
{

/** Event buffer that only stores one event per Index:

	Note: EventType must have the public property mIndex.
	*/
template <class EventType>
class DLL_LOCAL SingleEventBuffer : public EventBufferBase<EventType, IndexSet< EventType > >
{
public:

	SingleEventBuffer(size_t aMaxEvents);

	void _Update(const EventType& arEvent);
};

/** Event buffer that stores all changes to all points in the order. */
template <class EventType>
class DLL_LOCAL TimeOrderedEventBuffer : public EventBufferBase<EventType, TimeMultiSet< EventType > >
{
public:

	TimeOrderedEventBuffer(size_t aMaxEvents);
};

/** Event buffer that stores all changes to all points in the order. */
template <class EventType>
class DLL_LOCAL InsertionOrderedEventBuffer : public EventBufferBase<EventType, InsertionOrderSet< EventType > >
{
public:

	InsertionOrderedEventBuffer(size_t aMaxEvents);
};

template <class EventType>
SingleEventBuffer<EventType> :: SingleEventBuffer(size_t aMaxEvents) :
	EventBufferBase< EventType, IndexSet< EventType > >(aMaxEvents)
{}

template <class EventType>
TimeOrderedEventBuffer<EventType> :: TimeOrderedEventBuffer(size_t aMaxEvents) :
	EventBufferBase <EventType, TimeMultiSet< EventType > >(aMaxEvents)
{}

template <class EventType>
InsertionOrderedEventBuffer<EventType> :: InsertionOrderedEventBuffer(size_t aMaxEvents) :
	EventBufferBase<EventType, InsertionOrderSet< EventType > >(aMaxEvents)
{}

template <class EventType>
void SingleEventBuffer<EventType> :: _Update(const EventType& arEvent)
{
	typename IndexSet< EventType >::Type::iterator i = this->mEventSet.find(arEvent);

	if(i != this->mEventSet.end() ) {
		if(arEvent.mValue.GetTime() >= i->mValue.GetTime()) {
			this->mEventSet.erase(i);
			this->mEventSet.insert(arEvent); //new event
		}
	}
	else {
		this->mEventSet.insert(arEvent); //new event
		this->mCounter.IncrCount(arEvent.mClass);
	}
}

} //end NS

#endif
