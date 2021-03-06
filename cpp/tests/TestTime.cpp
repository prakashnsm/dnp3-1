
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

#include <opendnp3/Types.h>
#include <opendnp3/Thread.h>
#include <opendnp3/TimeSource.h>
#include <opendnp3/Util.h>

#include "TestHelpers.h"
#include "Timeout.h"
#include "StopWatch.h"

#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace opendnp3;

BOOST_AUTO_TEST_SUITE(TimeoutTests)

BOOST_AUTO_TEST_CASE(TimeoutCorrectlyNotExpired)
{
	//create a timeout of 1 millisecond
	Timeout to(milliseconds(1000));

	//check that the timeout starts out unexpired
	BOOST_REQUIRE_EQUAL(to.IsExpired(), false);
	BOOST_REQUIRE(to.Remaining() > milliseconds(1));
}

BOOST_AUTO_TEST_CASE(TimeoutCorrectlyExpires)
{
	//create a timeout of 1 millisecond
	Timeout to(milliseconds(1));

	std::this_thread::sleep_for(milliseconds(10));

	//check that the timeout starts out unexpired
	BOOST_REQUIRE(to.IsExpired());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StopWatchTests)

BOOST_AUTO_TEST_CASE(BasicTest)
{
	StopWatch sw;

	BOOST_REQUIRE(sw.Elapsed(false) <= milliseconds(100));

	//sleep for 5 millis
	std::this_thread::sleep_for(milliseconds(100));

	//make sure the stopwatch recorded atleast 5 millis elapsed (override default bool)
	BOOST_REQUIRE(sw.Elapsed(false) >= milliseconds(100));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TimeManagerTests)
BOOST_AUTO_TEST_CASE(TimeSourceSystemOffsetTest)
{

	TimeSourceSystemOffset time;
	timer_clock::time_point base = time.GetUTC();
	time.SetTime(base + milliseconds(5000));
	BOOST_REQUIRE(time.GetUTC() >= base + milliseconds(5000));
}
BOOST_AUTO_TEST_CASE(TimeSourceSystemOffsetTestNeg)
{

	TimeSourceSystemOffset time;
	timer_clock::time_point base = time.GetUTC();
	time.SetTime(base - milliseconds(5000));
	BOOST_REQUIRE(time.GetUTC() >= base - milliseconds(5000));
}
BOOST_AUTO_TEST_SUITE_END()
