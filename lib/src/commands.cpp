#include "commands.hpp"

#include "comm.hpp"
#include "log.hpp"

namespace fcwt {

namespace {

struct registration_message
{
  uint8_t const header[24] =
  {
    0x01, 0x00, 0x00, 0x00, 0xf2, 0xe4, 0x53, 0x8f,
    0xad, 0xa5, 0x48, 0x5d, 0x87, 0xb2, 0x7f, 0x0b, 
    0xd3, 0xd5, 0xde, 0xd0, 0x00, 0x00, 0x00, 0x00
  };
  uint8_t device_name[54] = {};
};

registration_message generate_registration_message(char const* device_name)
{
    registration_message msg = {};
    const int max_characters = sizeof(msg.device_name) / 2 - 1;
    for (int i = 0; i < max_characters; ++i) {
        const char c = device_name[i];
        msg.device_name[2 * i + 0] = c;
        msg.device_name[2 * i + 1] = 0;
        if (c == 0)
            break;
    }

    return msg;
}

#if 0
uint8_t message4_1[] =
{
  0x01, 0x00, 0x16, 0x10,
  0x03, 0x00, 0x00, 0x00,
  0x01, 0xdf, 0x00, 0x00
};

uint8_t message4_2[] =
{
   0x02, 0x00, 0x16, 0x10, 
   0x03, 0x00, 0x00, 0x00, 
   0x05, 0x00 // 'receive mode': 0x08, 'browse mode': 0x08, 'geo mode': 0x0a
};

uint8_t message5[] =
{
  0x01, 0x00, 0x15, 0x10,
  0x04, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00 // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31
};

uint8_t message6_1[] =
{
  0x01, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00 // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31
};

uint8_t message6_2[] =
{
  0x02, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x02, 0x00 // 'receive mode', 'browse mode': 0x03 0x00 0x00 0x00, 'geo mode': 0x02, 0x00, 0x00, 0x00
};

// these two probably configure something for the camera remote mode, maybe one enables the jpg stream? TODO!
uint8_t message9[] =
{
  0x01, 0x00, 0x2b, 0x90,
  0x08, 0x00, 0x00, 0x00
};

uint8_t message10[] =
{
  0x01, 0x00, 0x1c, 0x10,
  0x09, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};
#endif

// geo mode send position part 1
// 10:00:00:00: 01:00:16:10: 08:00:00:00: 00:d5:00:00
// geo mode send position part 2
// some UTF16 string with coordinates
// 9d:00:00:00:02:00:16:10:08:00:00:00:48:35:00:32:00:32:00:39:00:2e:00:31:00
// :33:00:31:00:38:00:34:00:38:00:2c:00:4e:00:20:00:30:00:31:00:33:00:32:00:32
// :00:2e:00:38:00:34:00:32:00:39:00:37:00:32:00:2c:00:45:00:20:00:30:00:30:00
// :30:00:30:00:30:00:2e:00:30:00:30:00:2c:00:4d:00:20:00:30:00:30:00:30:00:2e
// :00:30:00:2c:00:4b:00:20:00:32:00:30:00:31:00:36:00:3a:00:30:00:31:00:3a:00
// :30:00:33:00:20:00:31:00:35:00:3a:00:34:00:31:00:3a:00:34:00:30:00:2e:00:31:00:33:00:30:00:00:00

// receive mode
// first response of "status" message:
//    1a:00:00:00: 02:00:15:10: 2b:00:00:00: 02:00:00:df: 08:00:00:00: 20:d2:01:00: 00:00
// after that request image info
//    10:00:00:00: 01:00:08:10: 2c:00:00:00: 01:00:00:00
// -> 9a:00:00:00: 02:00:08:10: 2c:00:00:00: 01:00:00:10: 01:38:00:00: 68:81:08:00: 01:b9:42:29:00:00:40:01:00:00:f0:00:00:00:f0:06:00:00:a0:04:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:39:00:31:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:35:00:30:00:31:00:33:00:36:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// get actual image in full size
//    18:00:00:00: 01:00:1b:10: 30:00:00:00: 01:00:00:00: 00:00:00:00: 68:81:08:00 // last one is image id?
// -> image data
// -> 10:00:00:00: 03:00:01:20: 30:00:00:00: 68:81:08:00 // image complete

// browse mode
//    10:00:00:00: 01:00:08:10: 0c:00:00:00: 09:00:00:00 // last int is image omdex?
// -> 9a:00:00:00: 02:00:08:10: 0c:00:00:00: 01:00:00:10: 01:38:00:00: 00:90:01:00: 01:b9:82:2b:00:00:40:01:00:00:f0:00:00:00:80:0d:00:00:00:09:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:38:00:33:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:33:00:35:00:30:00:34:00:38:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// get thumbnail
//    10:00:00:00: 01:00:0a:10: 0e:00:00:00: 09:00:00:00
// -> success
// get non-thumbnail, not sure what first msg is about
//    10:00:00:00: 01:00:16:10: 8e:00:00:00: 26:d2:00:00
//    0e:00:00:00: 02:00:16:10: 8e:00:00:00: 01:00
// -> success
// request image info
//    10:00:00:00: 01:00:08:10: 8f:00:00:00: 11:00:00:00 // last int is image index?
// -> 9a:00:00:00: 02:00:08:10: 8f:00:00:00: 01:00:00:10: 01:38:00:00: 79:da:09:00: 01:b9:82:33:00:00:40:01:00:00:f0:00:00:00:f0:06:00:00:a0:04:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:37:00:35:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:33:00:32:00:36:00:32:00:30:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// request image
//    18:00:00:00: 01:00:1b:10: 91:00:00:00: 11:00:00:00: 00:00:00:00: 79:da:09:00 // last int is image id?
// -> 10:00:00:00: 03:00:01:20: 91:00:00:00: 79:da:09:00 // image complete

template <size_t N1, size_t N2>
static bool fuji_twopart_message(int const sockfd, static_message<N1> const& msg1, static_message<N2> const& msg2)
{
  fuji_send(sockfd, msg1);
  return fuji_message(sockfd, msg2);
}

template <size_t N>
size_t fuji_receive_log(int sockfd, uint8_t (&data)[N])
{
    size_t size = fuji_receive(sockfd, data, N);
    printf("receive %zu bytes [", size);
    print_hex(data, size, skip_newline);
    printf("]\n");
    return size;
}

} // namespace

bool init_control_connection(int const sockfd, char const* deviceName)
{
    if (sockfd <= 0)
      return false;

    LOG_INFO_FORMAT("init_control_connection (socket %d)", sockfd);
    auto const reg_msg = generate_registration_message(deviceName);
    LOG_INFO("send hello");
    fuji_send(sockfd, &reg_msg, sizeof(reg_msg));
    
    uint8_t buffer[1024];
    uint32_t const receivedBytes = fuji_receive(sockfd, buffer);
    uint8_t const message1_response_error[] =
    {
      0x05, 0x00, 0x00, 0x00,
      0x19, 0x20, 0x00, 0x00
    };

    if (receivedBytes == sizeof(message1_response_error) && memcmp(buffer, message1_response_error, receivedBytes) == 0)
    {
        fatal_error("response on message1, 0x error");
    }

    auto msg2 = make_static_message(message_type::start, 0x01, 0x00, 0x00, 0x00);
    fuji_message(sockfd, make_static_message(message_type::start, 0x01, 0x00, 0x00, 0x00));

    auto const msg4_1 = make_static_message(message_type::two_part, 0x01, 0xdf, 0x00, 0x00);
    auto const msg4_2 = make_static_message_followup(msg4_1, 0x05, 0x00);
    fuji_twopart_message(sockfd, msg4_1, msg4_2);

    // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31, 'remote mode': 0x24
    fuji_send(sockfd, make_static_message(message_type::single_part, 0x24, 0xdf, 0x00, 0x00));
    fuji_receive_log(sockfd, buffer);
    fuji_receive_log(sockfd, buffer);

    // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31
    auto const msg6_1 = make_static_message(message_type::two_part, 0x24, 0xdf, 0x00, 0x00);
    // 'receive mode', 'browse mode': 0x03 0x00 0x00 0x00, 'geo mode': 0x02, 0x00, 0x00, 0x00
    auto const msg6_2 = make_static_message_followup(msg6_1, 0x02, 0x00, 0x02, 0x00);
    fuji_twopart_message(sockfd, msg6_1, msg6_2);

    fuji_send(sockfd, make_static_message(message_type::camera_remote_x));
    fuji_receive_log(sockfd, buffer);
    fuji_receive_log(sockfd, buffer);

    fuji_message(sockfd, make_static_message(message_type::camera_remote, 
        0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00));

    return true;
}

void terminate_control_connection(int sockfd)
{
  if (sockfd <= 0)
    return;
  
  LOG_INFO("terminate_control_connection");
  fuji_message(sockfd, make_static_message(message_type::stop));
  uint32_t terminate = 0xffffffff;
  fuji_send(sockfd, &terminate, sizeof(terminate));
}

bool is_success_response(uint32_t const id, void const* buffer, uint32_t const size)
{
    if (size != 8)
        return false;

    struct response_success
    {
      uint8_t const type[4] = {0x03, 0x00, 0x01, 0x20};
      uint32_t id;
    };

    response_success success = {};
    success.id = id;
    bool const result = memcmp(&success, buffer, 8) == 0;
    if (!result)
    {
        LOG_INFO("expected: ");
        print_hex(&success, 8);
        LOG_INFO("actual: ");
        print_hex(buffer, 8);
    }
    return result;
}

bool fuji_message(int const sockfd, uint32_t const id, void const* message, size_t size)
{
    fuji_send(sockfd, message, size);

    uint8_t buffer[8];
    uint32_t receivedBytes = fuji_receive_log(sockfd, buffer);

    if (!is_success_response(id, buffer, receivedBytes))
    {
        LOG_INFO_FORMAT("received %d bytes", receivedBytes);
        print_hex(buffer, receivedBytes);
        return false;
    }

    return true;
}

#define MESSAGE_TYPE_TO_STRING_CASE(x) case message_type::x: return #x

char const* to_string(message_type type)
{
    switch(type)
    {
        MESSAGE_TYPE_TO_STRING_CASE(hello);
        MESSAGE_TYPE_TO_STRING_CASE(start);
        MESSAGE_TYPE_TO_STRING_CASE(stop);
        MESSAGE_TYPE_TO_STRING_CASE(image_info_by_index);
        MESSAGE_TYPE_TO_STRING_CASE(thumbnail_by_index);
        MESSAGE_TYPE_TO_STRING_CASE(shutter);
        MESSAGE_TYPE_TO_STRING_CASE(single_part);
        MESSAGE_TYPE_TO_STRING_CASE(two_part);
        MESSAGE_TYPE_TO_STRING_CASE(full_image);
        MESSAGE_TYPE_TO_STRING_CASE(camera_remote);
        MESSAGE_TYPE_TO_STRING_CASE(camera_remote_x);
        default: return "";
    }
}

} // namespace fcwt
