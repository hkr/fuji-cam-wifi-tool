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

uint8_t message1_response_error[] =
{
  0x05, 0x00, 0x00, 0x00,
  0x19, 0x20, 0x00, 0x00
};

uint8_t message2[] =
{
  0x01, 0x00, 0x02, 0x10, 
  0x01, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00
};

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
   0x05, 0x00
};

uint8_t message5[] =
{
  0x01, 0x00, 0x15, 0x10,
  0x04, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00
};

uint8_t message6_1[] =
{
  0x01, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x24, 0xdf, 0x00, 0x00
};

uint8_t message6_2[] =
{
  0x02, 0x00, 0x16, 0x10,
  0x05, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x02, 0x00
};

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

template <size_t N1, size_t N2>
static bool fuji_twopart_message(int const sockfd, uint32_t const id, uint8_t(&msg1)[N1], uint8_t(&msg2)[N2])
{
  fuji_send(sockfd, fill_message_id(id, msg1));
  return fuji_message(sockfd, id, msg2);
}

} // namespace

bool init_control_connection(int const sockfd, char const* deviceName)
{
    auto const reg_msg = generate_registration_message(deviceName);
    fuji_send(sockfd, &reg_msg, sizeof(reg_msg));
    LOG_INFO("sent message 1");

    uint8_t buffer[1024];
    uint32_t receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
    print_hex(buffer, receivedBytes);
    if (receivedBytes == sizeof(message1_response_error) && memcmp(buffer, message1_response_error, receivedBytes) == 0)
    {
        fatal_error("response on message1, 0x error");
    }

    LOG_INFO_FORMAT("received %d bytes", receivedBytes);

    //
    LOG_INFO("message2");
    fuji_message(sockfd, generate_message_id(), message2);

    //
    LOG_INFO("message4");
    fuji_twopart_message(sockfd, generate_message_id(), message4_1, message4_2);

    //
    LOG_INFO("message5");
    fuji_send(sockfd, fill_message_id(generate_message_id(), message5));
    receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);
    receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);

    //
    LOG_INFO("message6");
    fuji_twopart_message(sockfd, generate_message_id(), message6_1, message6_2);

    //
    LOG_INFO("message9");
    fuji_send(sockfd, fill_message_id(generate_message_id(), message9));
    receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);
    receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);

    //
    LOG_INFO("message10");
    fuji_message(sockfd, generate_message_id(), message10);

    return true;
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

bool fuji_message(int const sockfd, uint32_t const id, void* message, size_t size)
{
    fuji_send(sockfd, fill_message_id(id, message, size), size);

    uint8_t buffer[8];
    uint32_t receivedBytes = fuji_receive(sockfd, buffer, sizeof(buffer));

    if (!is_success_response(id, buffer, receivedBytes))
    {
        LOG_INFO_FORMAT("received %d bytes", receivedBytes);
        print_hex(buffer, receivedBytes);
        return false;
    }

    return true;
}

} // namespace fcwt
