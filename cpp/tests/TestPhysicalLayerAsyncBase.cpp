
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
#include <boost/test/unit_test.hpp>

#include "TestHelpers.h"
#include "AsyncPhysBaseTest.h"

#include <opendnp3/Exception.h>

using namespace opendnp3;

BOOST_AUTO_TEST_SUITE(PhysicalLayerAsyncBaseSuite)
BOOST_AUTO_TEST_CASE(ClosedState)
{
	AsyncPhysBaseTest t;
	uint8_t buff;

	BOOST_REQUIRE_THROW(t.phys.AsyncClose(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.upper.SendDown("00"), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.AsyncRead(&buff, 1), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalOpenFailure(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalOpenSuccess(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalSendSuccess(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalSendFailure(), InvalidStateException);

}

BOOST_AUTO_TEST_CASE(OpenCloseNotification)
{
	AsyncPhysBaseTest t;
	const size_t NUM = 3;

	for(size_t i = 1; i <= NUM; ++i) {
		t.phys.AsyncOpen();
		t.phys.SignalOpenSuccess();
		BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerUp, i);
		t.phys.AsyncClose();

		BOOST_REQUIRE_THROW(t.phys.SignalOpenFailure(), InvalidStateException);
		BOOST_REQUIRE_THROW(t.phys.SignalOpenSuccess(), InvalidStateException);
		BOOST_REQUIRE_THROW(t.phys.SignalSendSuccess(), InvalidStateException);
		BOOST_REQUIRE_THROW(t.phys.SignalSendFailure(), InvalidStateException);

		BOOST_REQUIRE_EQUAL(t.phys.NumClose(), i);
		BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, i);
	}
}

BOOST_AUTO_TEST_CASE(ReadState)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();
	t.adapter.StartRead(); //start a read

	BOOST_REQUIRE_THROW(t.phys.SignalOpenFailure(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalOpenSuccess(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalSendSuccess(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalSendFailure(), InvalidStateException);

	t.phys.TriggerRead("00");
	t.upper.BufferEqualsHex("00");
}

BOOST_AUTO_TEST_CASE(CloseWhileReading)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();
	t.adapter.StartRead();

	t.phys.AsyncClose();
	BOOST_REQUIRE_EQUAL(t.phys.NumClose(), 1);
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0); //layer shouldn't go down until the outstanding read comes back
	t.phys.SignalReadFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 1);
}

BOOST_AUTO_TEST_CASE(WriteState)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();

	t.upper.SendDown("00");
	BOOST_REQUIRE_EQUAL(t.phys.Size(), 1);

	BOOST_REQUIRE_THROW(t.phys.SignalOpenFailure(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalOpenSuccess(), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.TriggerRead(""), InvalidStateException);
	BOOST_REQUIRE_THROW(t.phys.SignalReadFailure(), InvalidStateException);
}

BOOST_AUTO_TEST_CASE(CloseWhileWriting)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();

	t.upper.SendDown("00");
	t.phys.AsyncClose();
	BOOST_REQUIRE_EQUAL(t.phys.NumClose(), 1);
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0); //layer shouldn't go down until the outstanding write comes back
	t.phys.SignalSendFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 1);
}

BOOST_AUTO_TEST_CASE(CloseWhileReadingWriting)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();

	t.upper.SendDown("00");
	t.adapter.StartRead();
	t.phys.AsyncClose();
	BOOST_REQUIRE_EQUAL(t.phys.NumClose(), 1);
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0);
	t.phys.SignalSendFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0);
	t.phys.SignalReadFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 1);
}

BOOST_AUTO_TEST_CASE(CloseWhileWritingReading)
{
	AsyncPhysBaseTest t;
	t.phys.AsyncOpen();
	t.phys.SignalOpenSuccess();

	t.upper.SendDown("00");
	t.adapter.StartRead();
	t.phys.AsyncClose();
	BOOST_REQUIRE_EQUAL(t.phys.NumClose(), 1);
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0);
	t.phys.SignalReadFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 0);
	t.phys.SignalSendFailure();
	BOOST_REQUIRE_EQUAL(t.upper.GetState().mNumLayerDown, 1);
}

BOOST_AUTO_TEST_CASE(CloseWhileOpening)
{
	AsyncPhysBaseTest t;

	t.phys.AsyncOpen();
	t.phys.AsyncClose();
	BOOST_REQUIRE(t.phys.IsOpening());
	BOOST_REQUIRE(t.phys.IsClosing());

	/* this could happen for some layers, but we
	   still need to return an open failure to the handler */
	t.phys.SignalOpenSuccess();

	BOOST_REQUIRE_EQUAL(0, t.upper.GetState().mNumLayerUp);
	BOOST_REQUIRE_EQUAL(1, t.adapter.GetNumOpenFailure());
}

BOOST_AUTO_TEST_SUITE_END()

