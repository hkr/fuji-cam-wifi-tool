#ifndef FUJI_CAM_WIFI_TOOL_COMMANDS_HPP
#define FUJI_CAM_WIFI_TOOL_COMMANDS_HPP

#include <array>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "comm.hpp"
#include "log.hpp"

namespace fcwt {

enum class message_type : uint16_t
{
    hello               = 0x0000, // the first message sent to the camera, see struct registration_message

    start               = 0x1002, // always the second message, don't know what it does yet
    stop                = 0x1003, // used before sending terminate

    image_info_by_index = 0x1008,
    thumbnail_by_index  = 0x100a,
    shutter             = 0x100e,
    single_part         = 0x1015,
    two_part            = 0x1016,
    full_image          = 0x101b,
    camera_remote       = 0x101c, // last command before camera remote works

    camera_last_image   = 0x9022,
    camera_remote_x     = 0x902b, // unknown, app uses it before camera_remote, returns 392 bytes of data, maybe the current settings?
};

char const* to_string(message_type type);

struct message_header
{
    uint16_t index = 1; // all but terminate (0) and two_part_message (2) have 1 here
    message_type type;
};

struct message_id
{
    uint32_t id;
};

template<size_t PayloadBytes>
struct static_message_data
{
    std::array<uint8_t, PayloadBytes> data;
};

template<size_t PayloadBytes>
struct static_message : message_header, message_id, static_message_data<PayloadBytes>
{
    constexpr size_t size() const { return sizeof(message_header) + sizeof(message_id) + PayloadBytes; }
};

uint32_t generate_message_id();

// name is just a guess, the app uses this a lot, maybe the cam uses it as some kind of heart-beat?
// returns 124 bytes of data (I think somethime differs from that, can't remember)
struct status_request_message : static_message<4>
{
    status_request_message()
    {
        index = 1;
        type = message_type::single_part;
        id = 0;
        data = { 0x12, 0xd2, 0x00, 0x00 };
    }
};

template <typename SpecializedMessageType>
SpecializedMessageType generate()
{
    SpecializedMessageType msg = {};
    msg.id = generate_message_id();
    return msg;
}

inline std::array<uint8_t, 4> make_byte_array(uint32_t x)
{
    return {{
        static_cast<uint8_t>(x), 
        static_cast<uint8_t>(x >> 8), 
        static_cast<uint8_t>(x >> 16), 
        static_cast<uint8_t>(x >> 24)
    }};
}

template <size_t N>
static_message<N> make_static_message(message_type type, std::array<uint8_t, N> const& data)
{
    static_message<N> msg;
    msg.type = type;
    msg.data = data;
    msg.id = generate_message_id();
    return msg;
}

template <typename... Ts>
static_message<sizeof...(Ts)> make_static_message(message_type type, Ts... bytes)
{
    std::array<uint8_t, sizeof...(Ts)> payload = {{static_cast<uint8_t>(bytes)...}};
    return make_static_message(type, payload);
}

template <size_t N, size_t PreviousPayloadBytes>
static_message<N> make_static_message_followup(static_message<PreviousPayloadBytes> const& prevMsg, std::array<uint8_t, N> const& data)
{
    static_message<N> msg;
    msg.type = message_type::two_part;
    assert(prevMsg.type == message_type::two_part);
    msg.data = data;
    msg.id = prevMsg.id;
    msg.index = prevMsg.index + 1;
    return msg;
}

template <typename... Ts, size_t PreviousPayloadBytes>
static_message<sizeof...(Ts)> make_static_message_followup(static_message<PreviousPayloadBytes> const& prevMsg, Ts... bytes)
{
    std::array<uint8_t, sizeof...(Ts)> payload = {{static_cast<uint8_t>(bytes)...}};
    return make_static_message_followup(prevMsg, payload);
}

bool is_success_response(uint32_t const id, void const* buffer, uint32_t const size);

bool init_control_connection(int sockfd, char const* deviceName);
void terminate_control_connection(int sockfd);

bool shutter(int const sockfd);
bool set_iso(int sockfd, uint32_t iso);


bool fuji_message(int const sockfd, uint32_t const id, void const* message, size_t size);

template <size_t N>
bool fuji_message(int const sockfd, const static_message<N>& msg)
{
    printf("send: ");
    print_message(msg);
    return fuji_message(sockfd, msg.id, &msg, msg.size());
}

template <size_t N>
void print_message(static_message<N> const& msg, append_newline anl = newline)
{
    printf("%s(%d) [", to_string(msg.type), static_cast<int>(msg.type));
    print_hex(&msg, msg.size(), skip_newline);
    printf("]");
    if (anl == newline)
        printf("\n");
}

template <size_t N>
void fuji_send(int sockfd, static_message<N> const& msg)
{
    printf("send: ");
    print_message(msg);
    fuji_send(sockfd, &msg, msg.size());
}

inline void fuji_send(int sockfd, message_header const& msg)
{
    fuji_send(sockfd, &msg, sizeof(message_header));
}

} // namespace fcwt

#endif
