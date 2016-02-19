#ifndef PDU_H_7D9F1C84_D666_11E5_86AF_10FEED04CD1C
#define PDU_H_7D9F1C84_D666_11E5_86AF_10FEED04CD1C

#include <cstring>
#include <cstdint>

template <typename ID, uint16_t LEN>
class Pdu {
public:
	static const uint16_t MaxSize = LEN;

	typedef enum : int8_t {
		Valid = 0,
		NotValid,
	} State;

	inline Pdu() : m_id(0), m_size(0), m_state(NotValid) {
	}

	inline Pdu(ID id, uint16_t size, const uint8_t * payload) : m_id(id), m_size(size) {
		if (size <= MaxSize) {
			memcpy(m_payload, payload, size);
			m_state = Valid;
		} else {
			m_id = 0;
			m_size = 0;
			m_state = NotValid;
		}
	}

	inline void set(ID id, uint16_t size, const uint8_t * payload) {
		if (size <= MaxSize) {
			m_id = id;
			m_size = size;
			memcpy(m_payload, payload, size);
			m_state = Valid;
		} else {
			m_id = 0;
			m_size = 0;
			m_state = NotValid;
		}
	}

	inline ID getId() const { return m_id; }
	inline uint16_t getSize() const { return m_size; }
	inline State getState() const { return m_state; }
	inline const uint8_t * getPayload() const { return m_payload; }

private:
	ID m_id;
	uint16_t m_size;
	State m_state;
	uint8_t m_payload[MaxSize];
} __attribute__((packed));

#endif // PDU_H_7D9F1C84_D666_11E5_86AF_10FEED04CD1C
