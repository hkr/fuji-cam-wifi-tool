#include "message.hpp"

#include <atomic>

namespace fcwt {

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
        MESSAGE_TYPE_TO_STRING_CASE(camera_last_image);
        MESSAGE_TYPE_TO_STRING_CASE(camera_remote_x);
        default: return "";
    }
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

uint32_t generate_message_id()
{
  static std::atomic<uint32_t> id_counter;
  return ++id_counter;
}

} // namespace fcwt
