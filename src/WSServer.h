/*
obs-websocket
Copyright (C) 2016-2017	Stéphane Lepin <stephane.lepin@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#ifndef WSSERVER_H
#define WSSERVER_H

#include <thread>
#include <libwebsockets.h>
#include "WSRequestHandler.h"
#include "obs-websocket.h"
#include "Config.h"
#include "Utils.h"

class WSServer {
public:
	void Loop();
	void Start();
	void Stop();
	static WSServer* Instance;
private:
	thread th;
	volatile bool enabled = false;
	struct lws_context *context;
	struct lws_context_creation_info info;
	inline ~WebsocketsServer()
	{
		Stop();
	}
};

#endif // WSSERVER_H