#include "GameClient.h"
#include <CoreLib/BinarySerializer.h>
#include <CoreLib/BinaryDeserializer.h>

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
}

void GameClient::Send(const std::string& payload, Callback&& cb) {
	if (!IsConnected()) {
		cb(false, "");
		return;
	}

	NetworkMsgID id = NetworkMsgID(m_idManager.GetNewUniqueIdentifier());
	m_pending[id] = std::move(cb);

	// serialize packet: [id][payload length][payload bytes]
	BinarySerializer ser;
	ser.AddField(id.value);
	std::vector<uint8_t> payloadData(payload.begin(), payload.end());
	ser.AddField(payloadData);// size + data

	PendingSend ps;
	std::vector<uint8_t> buf = ser.ToBuffer();

	uint32_t len = static_cast<uint32_t>(buf.size());
	ps.data.resize(sizeof(uint32_t) + buf.size());
	std::memcpy(ps.data.data(), &len, sizeof(uint32_t));
	std::memcpy(ps.data.data() + sizeof(uint32_t), buf.data(), buf.size());

	m_msgQueue.push_back(std::move(ps));
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

		size_t remaining = ps.data.size() - ps.offset;

		int sent = NET_WriteToStreamSocket(
			m_socket,
			ps.data.data() + ps.offset,
			static_cast<int>(remaining)
		);

		if (sent < 0) {
			AddError(SDL_GetError());
			Disconnect();
			return false;
		}

		ps.offset += sent;

		if (ps.offset < ps.data.size()) {
			break;
		}

		m_msgQueue.pop_front();
	}

	return true;
}

bool GameClient::ProcessReceiveQueue() {
	if (!IsConnected()) {
		AddError("ProcessReceiveQueue: Not connected");
		return false;
	}

	uint8_t buffer[4096];
	int received = NET_ReadFromStreamSocket(m_socket, buffer, sizeof(buffer));
	if (received < 0) {
		AddError(SDL_GetError());
		Disconnect();
		return false;
	}

	m_receiveBuffer.insert(m_receiveBuffer.end(), buffer, buffer + received);

	while (true) {
		if (m_receiveBuffer.size() < sizeof(uint32_t))
			break;

		uint32_t length;
		std::memcpy(&length, m_receiveBuffer.data(), sizeof(uint32_t));
		if (m_receiveBuffer.size() < sizeof(uint32_t) + length)
			break;

		std::vector<uint8_t> packet(m_receiveBuffer.begin() + sizeof(uint32_t),
			m_receiveBuffer.begin() + sizeof(uint32_t) + length);
		BinaryDeserializer des(packet);

		uint32_t id = des.Read<uint32_t>();
		std::vector<uint8_t> payloadData = des.ReadVector<uint8_t>();
		std::string payload(payloadData.begin(), payloadData.end());

		m_receiveBuffer.erase(m_receiveBuffer.begin(),
			m_receiveBuffer.begin() + sizeof(uint32_t) + length);

		auto it = m_pending.find(NetworkMsgID(id));
		if (it != m_pending.end()) {
			it->second(true, payload);
			m_pending.erase(it);
		}
	}

	return true;
}

void GameClient::AddError(const std::string& msg) {
	m_error += msg;
}