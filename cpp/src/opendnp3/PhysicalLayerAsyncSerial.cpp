
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
#include "PhysicalLayerAsyncSerial.h"

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <functional>
#include <string>

#include <opendnp3/Exception.h>
#include <opendnp3/Logger.h>

#include "IHandlerAsync.h"
#include "LoggableMacros.h"
#include "ASIOSerialHelpers.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::system;
using namespace std;

namespace opendnp3
{

PhysicalLayerAsyncSerial::PhysicalLayerAsyncSerial(
        Logger* apLogger,
        boost::asio::io_service* apIOService,
        const SerialSettings& arSettings) :

	PhysicalLayerAsyncASIO(apLogger, apIOService),
	mSettings(arSettings),
	mPort(*apIOService)
{

}

/* Implement the actions */

void PhysicalLayerAsyncSerial::DoOpen()
{
	boost::system::error_code ec;
	mPort.open(mSettings.mDevice, ec);

	//use post to simulate an async open operation
	if(!ec) Configure(mSettings, mPort, ec);

	mExecutor.Post(std::bind(&PhysicalLayerAsyncSerial::OnOpenCallback, this, ec));
}

void PhysicalLayerAsyncSerial::DoClose()
{
	boost::system::error_code ec;
	mPort.close(ec);
	if(ec) LOG_BLOCK(LEV_WARNING, ec.message());
}

void PhysicalLayerAsyncSerial::DoOpenSuccess()
{
	LOG_BLOCK(LEV_INFO, "Port successfully opened");
}

void PhysicalLayerAsyncSerial::DoAsyncRead(uint8_t* apBuffer, size_t aMaxBytes)
{
	mPort.async_read_some(buffer(apBuffer, aMaxBytes),
	                      mStrand.wrap(
	                              std::bind(&PhysicalLayerAsyncSerial::OnReadCallback,
	                                        this,
	                                        std::placeholders::_1,
	                                        apBuffer,
	                                        std::placeholders::_2)
	                      ));
}

void PhysicalLayerAsyncSerial::DoAsyncWrite(const uint8_t* apBuffer, size_t aNumBytes)
{
	async_write(mPort, buffer(apBuffer, aNumBytes),
	            mStrand.wrap(
	                    std::bind(&PhysicalLayerAsyncSerial::OnWriteCallback,
	                              this,
	                              std::placeholders::_1,
	                              aNumBytes)
	            ));
}

}

/* vim: set ts=4 sw=4: */

