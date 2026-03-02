#include "GameClient.h"
#include <CoreLib/BinarySerializer.h>
#include <CoreLib/BinaryDeserializer.h>
#include <SDLCoreLib/SDLCoreTime.h>

GameClient::GameClient() {
}

GameClient::~GameClient() {
	Disconnect();
}

bool GameClient::Connect(const std::string& host, uint16_t port) {
	if (IsConnected()  && 
		m_host == host && 
		m_port == port)
		return true;

	m_host = host;
	m_port = port;
	NET_Address* addr = NET_ResolveHostname(host.c_str());
	if (!addr) {
		AddError("Connect: Failed to resolve host!\n");
		return false;
	}

	if (NET_WaitUntilResolved(addr, 5000) != NET_Status::NET_SUCCESS) {
		AddError("Connect: DNS resolution failed!\n");
		NET_UnrefAddress(addr);
		return false;
	}

	m_socket = NET_CreateClient(addr, port);
	NET_UnrefAddress(addr);

	if (!m_socket) {
		AddError("Connect: Failed to connect to server\n");
		return false;
	}

	return true;
}

void GameClient::Disconnect() {
	if (!IsConnected())
		return;

	NET_DestroyStreamSocket(m_socket);
	m_socket = nullptr;
	m_msgQueue.clear();

	for (auto& p : m_pending)
		if(p.second.cb)
			p.second.cb(false, "");
	m_pending.clear();
	m_receiveBuffer.clear();
}

void GameClient::Send(const std::string& payload, Callback&& cb) {
	if (!IsConnected()) {
		cb(false, "no Connection with server");
		return;
	}

	PendingReceive receive;
	receive.cb = std::move(cb);
	NetworkMsgID id = NetworkMsgID(m_idManager.GetNewUniqueIdentifier());
	m_pending[id] = std::move(receive);

	// serialize packet: [id][payload length][payload bytes]
	BinarySerializer ser;
	ser.AddField(id.value);
	std::vector<uint8_t> payloadData(payload.begin(), payload.end());
	ser.AddField(payloadData);// size + data

	std::vector<uint8_t> buf = ser.ToBuffer();

	PendingSend ps;
	uint32_t len = static_cast<uint32_t>(buf.size());

	ps.data.resize(sizeof(uint32_t) + buf.size());
	std::memcpy(ps.data.data(), &len, sizeof(uint32_t));
	std::memcpy(ps.data.data() + sizeof(uint32_t), buf.data(), buf.size());

	ps.id = id;

	m_msgQueue.push_back(std::move(ps));
}

NetworkCallbackID GameClient::AddGlobalCallback(GlobalCallback&& cb) {
	NetworkCallbackID id = NetworkCallbackID(m_callbackIDManager.GetNewUniqueIdentifier());
	m_globalCallbacks[id] = std::move(cb);
	return id;
}

bool GameClient::RemoveGlobalCallback(NetworkCallbackID id) {
	auto it = m_globalCallbacks.find(id);
	bool found = it != m_globalCallbacks.end();
	if (found)
		m_globalCallbacks.erase(it);
	return found;
}

void GameClient::ClearError() {
	m_error.clear();
}

bool GameClient::IsConnected() const {
	if (!m_socket)
		return false;

	NET_Status status = NET_GetConnectionStatus(m_socket);
	if (status == NET_Status::NET_SUCCESS)
		return true;

	return false;
}

OTN::OTNObject GameClient::CreateHeaderBlock(const std::string& action) {
	OTN::OTNObject headerObj{ "header" };
	headerObj.SetNames("action");
	headerObj.SetTypes("String");
	headerObj.AddDataRow(action);
	return headerObj;
}

const std::string& GameClient::GetError() const {
	return m_error;
}

const std::string& GameClient::GetHost() const {
	return m_host;
}

uint16_t GameClient::GetPort() const {
	return m_port;
}

bool GameClient::ProcessSendQueue() {
	if (!IsConnected()) {
		AddError("ProcessSendQueue: Not connected");
		return false;
	}

	while (!m_msgQueue.empty()) {
		PendingSend& ps = m_msgQueue.front();

		bool ok = NET_WriteToStreamSocket(
			m_socket,
			ps.data.data(),
			static_cast<int>(ps.data.size())
		);

		if (!ok) {
			AddError(SDL_GetError());
			CallRequestCallback(ps.id, false, "");
			Disconnect();
			return false;
		}

		m_msgQueue.pop_front();
	}

	return true;
}

bool GameClient::ProcessReceiveQueue() {
	ProcessPendingSentTimeOut();

	if (!IsConnected()) {
		AddError("ProcessReceiveQueue: Not connected");
		return false;
	}

	uint8_t buffer[4096];
	int received = NET_ReadFromStreamSocket(m_socket, buffer, sizeof(buffer));
	
	if (received == 0)
		return true;

	if (received < 0) {
		AddError(SDL_GetError());
		Disconnect();
		return false;
	}

	m_receiveBuffer.insert(m_receiveBuffer.end(), buffer, buffer + received);

	while (true) {
		if (m_receiveBuffer.size() < sizeof(uint32_t))
			break;

		uint32_t length = 0;
		std::memcpy(&length, m_receiveBuffer.data(), sizeof(uint32_t));
		if (m_receiveBuffer.size() < sizeof(uint32_t) + length)
			break;

		std::vector<uint8_t> packet(m_receiveBuffer.begin() + sizeof(uint32_t),
			m_receiveBuffer.begin() + sizeof(uint32_t) + length);
		BinaryDeserializer des(packet);

		uint32_t id = des.Read<uint32_t>();
		bool response = des.Read<bool>();
		std::vector<uint8_t> payloadData = des.ReadVector<uint8_t>();
		std::string payload(payloadData.begin(), payloadData.end());

		m_receiveBuffer.erase(m_receiveBuffer.begin(),
			m_receiveBuffer.begin() + sizeof(uint32_t) + length);

		CallRequestCallback(NetworkMsgID(id), response, payload);
	}

	return true;
}

void GameClient::ProcessPendingSentTimeOut() {
	std::vector<NetworkMsgID> timeOutIDs;
	for (auto& [id, pending] : m_pending) {
		pending.waitedTimeMS += static_cast<size_t>(SDLCore::Time::GetDeltaTimeMSF());
		if (pending.waitedTimeMS >= m_pendingSendTimeOutMS)
			timeOutIDs.push_back(id);
	}

	while (!timeOutIDs.empty()) {
		auto id = timeOutIDs.back();
		timeOutIDs.pop_back(); 

		auto it = m_pending.find(id);
		if (it != m_pending.end()) {
			if (it->second.cb) {
				it->second.cb(false, "Connection time out");
			}
			m_pending.erase(it);
		}
	}
}

void GameClient::CallRequestCallback(NetworkMsgID id, bool result, const std::string& msg) {
	auto it = m_pending.find(id);
	if (it != m_pending.end()) {
		if(it->second.cb)
			it->second.cb(result, msg);
		m_pending.erase(it);
		return;
	}
	if(result)
		CallGlobalCallBacks(msg);
}

void GameClient::AddError(const std::string& msg) {
	m_error += msg;
}

void GameClient::CallGlobalCallBacks(const std::string & msg) {
	auto copy = m_globalCallbacks;
	for (auto& [_, cb] : copy) {
		if(cb)
			cb(msg);
	}
}