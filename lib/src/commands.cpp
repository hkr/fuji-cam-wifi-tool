#include "commands.hpp"

#include "comm.hpp"
#include "log.hpp"

namespace fcwt {

namespace {

struct registration_message {
  uint8_t const header[24] = {0x01, 0x00, 0x00, 0x00, 0xf2, 0xe4, 0x53, 0x8f, 
                              0xad, 0xa5, 0x48, 0x5d, 0x87, 0xb2, 0x7f, 0x0b, 
                              0xd3, 0xd5, 0xde, 0xd0, 0x02, 0x78, 0xa8, 0xc0};
  uint8_t device_name[54] = {};
};

registration_message generate_registration_message(char const* device_name) {
  registration_message msg = {};
  const int max_characters = sizeof(msg.device_name) / 2 - 1;
  for (int i = 0; i < max_characters; ++i) {
    const char c = device_name[i];
    msg.device_name[2 * i + 0] = c;
    msg.device_name[2 * i + 1] = 0;
    if (c == 0) break;
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
//    1a:00:00:00: 02:00:15:10: 2b:00:00:00: 02:00:00:df: 08:00:00:00:
//    20:d2:01:00: 00:00
// after that request image info
//    10:00:00:00: 01:00:08:10: 2c:00:00:00: 01:00:00:00
// -> 9a:00:00:00: 02:00:08:10: 2c:00:00:00: 01:00:00:10: 01:38:00:00:
// 68:81:08:00:
// 01:b9:42:29:00:00:40:01:00:00:f0:00:00:00:f0:06:00:00:a0:04:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:39:00:31:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:35:00:30:00:31:00:33:00:36:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// get actual image in full size
//    18:00:00:00: 01:00:1b:10: 30:00:00:00: 01:00:00:00: 00:00:00:00:
//    68:81:08:00 // last one is image id?
// -> image data
// -> 10:00:00:00: 03:00:01:20: 30:00:00:00: 68:81:08:00 // image complete

// browse mode
//    10:00:00:00: 01:00:08:10: 0c:00:00:00: 09:00:00:00 // last int is image
//    omdex?
// -> 9a:00:00:00: 02:00:08:10: 0c:00:00:00: 01:00:00:10: 01:38:00:00:
// 00:90:01:00:
// 01:b9:82:2b:00:00:40:01:00:00:f0:00:00:00:80:0d:00:00:00:09:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:38:00:33:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:33:00:35:00:30:00:34:00:38:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// get thumbnail
//    10:00:00:00: 01:00:0a:10: 0e:00:00:00: 09:00:00:00
// -> success
// get non-thumbnail, not sure what first msg is about
//    10:00:00:00: 01:00:16:10: 8e:00:00:00: 26:d2:00:00
//    0e:00:00:00: 02:00:16:10: 8e:00:00:00: 01:00
// -> success
// request image info
//    10:00:00:00: 01:00:08:10: 8f:00:00:00: 11:00:00:00 // last int is image
//    index?
// -> 9a:00:00:00: 02:00:08:10: 8f:00:00:00: 01:00:00:10: 01:38:00:00:
// 79:da:09:00:
// 01:b9:82:33:00:00:40:01:00:00:f0:00:00:00:f0:06:00:00:a0:04:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:0d:44:00:53:00:43:00:46:00:30:00:35:00:37:00:35:00:2e:00:4a:00:50:00:47:00:00:00:10:32:00:30:00:31:00:36:00:30:00:31:00:30:00:32:00:54:00:31:00:33:00:32:00:36:00:32:00:30:00:00:00:00:0e:4f:00:72:00:69:00:65:00:6e:00:74:00:61:00:74:00:69:00:6f:00:6e:00:3a:00:31:00:00:00
// request image
//    18:00:00:00: 01:00:1b:10: 91:00:00:00: 11:00:00:00: 00:00:00:00:
//    79:da:09:00 // last int is image id?
// -> 10:00:00:00: 03:00:01:20: 91:00:00:00: 79:da:09:00 // image complete

/*
* Capability messages are of the format:
* --
* 2 bytes          DevicePropertyCode
* 2 bytes          DataType (3 (2bytes), 4 (2bytes) or 6 (4bytes))
* --
* 1 byte           GetSet (0: get only, 1: get and set)
* --
* N bytes          FactoryDefaultValue
* N bytes          CurrentValue
* --
* 1 byte           FormFlag (1: range(min, max, step); 2: list)
* --
* --
* N bytes          MinValue
* N bytes          MaxValue
* N bytes          StepSize
* -- or --
* 2 bytes          NumberOfValues
* count * N bytes  Values
* --
* --
*
* N depends on the DataType field
*/

capability parse_capability(uint8_t const* data, size_t const size) {
    uint16_t count = capability_max_values;
    uint8_t value_size = 2;
    uint8_t offset = 0;
    uint32_t tmp;
    capability cap;

    if (size < 4) return cap;

    // Read DevicePropertyCode
    memcpy(&cap.property_code, data + offset, 2);
    offset += 2;
    // Read DataType
    memcpy(&cap.data_type, data + offset, 2);
    offset += 2;

    if (cap.data_type == 3 || cap.data_type == 4)
        value_size = 2;
    else if (cap.data_type == 6)
        value_size = 4;

    // Read GetSet flag
    memcpy(&cap.get_set, data + offset, 1);
    offset += 1;

    // Read FactoryDefaultValue
    memcpy(&cap.default_value, data + offset, value_size);
    offset += value_size;
    // Read CurrentValue
    memcpy(&cap.current_value, data + offset, value_size);
    offset += value_size;

    // Read FormFlag
    memcpy(&cap.form_flag, data + offset, 1);
    offset += 1;

    if (cap.form_flag == 1) {
        // Read MinValue
        memcpy(&cap.min_value, data + offset, value_size);
        offset += value_size;
        // Read MaxValue
        memcpy(&cap.max_value, data + offset, value_size);
        offset += value_size;
        // Read StepSize
        memcpy(&cap.step_size, data + offset, value_size);
        offset += value_size;

    } else if (cap.form_flag == 2) {
        // Read Count
        memcpy(&cap.count, data + offset, 2);
        count = std::min(cap.count, count);
        offset += 2;

        // Read Value list
        for (uint16_t i = 0; i < count; i++) {
            tmp = 0;
            memcpy(&tmp, data + offset + value_size*i, value_size);
            cap.values[i] = tmp;
        }
    }

    std::string log_capability = std::string("capability: ").append(hex_format(data, size));
    log(LOG_DEBUG2, log_capability.append(property_strings[cap.property_code]));

    return cap;
}

std::vector<capability> parse_camera_caps(void const* data, size_t const size) {
  std::vector<capability> caps;
  size_t remainingBytes = size;
  uint8_t const* bytes = static_cast<uint8_t const*>(data);
  if (remainingBytes < 12) return caps;

  bytes += 12;  // 12 bytes unknown
  remainingBytes -= 12;

  while (remainingBytes > 0) {
    // sub-message size
    if (remainingBytes < 4) {
      log(LOG_ERROR, string_format("Inconsistent message when getting next "
                        "submessage(remaingBytes=%zu)", remainingBytes));
      break;
    }
    uint32_t subMsgSize = 0;
    memcpy(&subMsgSize, bytes, sizeof(subMsgSize));
    if (subMsgSize < 4) {
      log(LOG_ERROR, string_format("Inconsistent submessage(subMsgSize=%d)", subMsgSize));
      break;
    }
    subMsgSize -= 4;
    bytes += sizeof(subMsgSize);
    remainingBytes -= sizeof(subMsgSize);

    // sub-message actual sub-message
    if (remainingBytes < subMsgSize) {
      log(LOG_ERROR, string_format("Inconsistent submessage(subMsgSize=%d)", subMsgSize));
      break;
    }

    caps.push_back(parse_capability(bytes, subMsgSize));

    bytes += subMsgSize;
    remainingBytes -= subMsgSize;
  }

  return caps;
}

}  // namespace

bool update_setting(native_socket sockfd, iso_level iso) {
  if (sockfd <= 0) return false;
  auto const msg_1 =
      make_static_message(message_type::two_part, 0x2A, 0xD0, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(iso));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, image_settings image) {
  return false;
}

bool update_setting(native_socket sockfd, film_simulation_mode film) {
  if (sockfd <= 0) return false;
  auto const msg_1 =
    make_static_message(message_type::two_part, 0x01, 0xd0, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(static_cast<uint16_t>(film)));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, flash_mode flash) {
  if (sockfd <= 0) return false;
  auto const msg_1 =
    make_static_message(message_type::two_part, 0x0c, 0x50, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(static_cast<uint16_t>(flash)));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, timer_mode timer) {
  if (sockfd <= 0) return false;
  auto const msg_1 =
    make_static_message(message_type::two_part, 0x12, 0x50, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(static_cast<uint16_t>(timer)));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, auto_focus_point point) {
  if (sockfd <= 0) return false;
  auto const msg = make_static_message(message_type::focus_point, point.y, point.x, 0x02, 0x03);
  return fuji_message(sockfd, msg);
}

bool unlock_focus(native_socket sockfd) {
  if (sockfd <= 0) return false;
  return fuji_message(sockfd, make_static_message(message_type::focus_unlock));
}

bool update_setting(native_socket sockfd, white_balance_mode white_balance) {
  if (sockfd <= 0) return false;
  //10:00:00:00 01:00:16:10 68:00:00:00 05:50:00:00
  //0e:00:00:00 02:00:16:10 68:00:00:00 06:80
  auto const msg_1 =
    make_static_message(message_type::two_part, 0x05, 0x50, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(static_cast<uint16_t>(white_balance)));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, fnumber_update_direction dir) {
  if (sockfd <= 0) return false;
  auto const msg = make_static_message(
      message_type::aperture, dir == fnumber_increment ? 1 : 0, 0, 0, 0);
  return fuji_message(sockfd, msg);
}

bool update_setting(native_socket sockfd, ss_update_direction dir) {
  if (sockfd <= 0) return false;
  auto const msg = make_static_message(
      message_type::shutter_speed, dir == ss_increment ? 1 : 0, 0, 0, 0);
  return fuji_message(sockfd, msg);
}

bool update_setting(native_socket sockfd, exp_update_direction dir) {
  if (sockfd <= 0) return false;
  auto const msg = make_static_message(
      message_type::exposure_correction, dir == exp_increment ? 1 : 0, 0, 0, 0);
  return fuji_message(sockfd, msg);
}

bool init_control_connection(native_socket const sockfd, char const* deviceName,
                             std::vector<capability>* caps) {
  if (sockfd <= 0) return false;

  if (!deviceName || !deviceName[0]) deviceName = "CameraClient";

  log(LOG_INFO, string_format("init_control_connection (socket %lld)",
                              static_cast<long long>(sockfd)));
  auto const reg_msg = generate_registration_message(deviceName);
  log(LOG_INFO, "send hello");
  fuji_send(sockfd, &reg_msg, sizeof(reg_msg));

  uint8_t buffer[1024];
  size_t const receivedBytes = fuji_receive(sockfd, buffer);
  uint8_t const message1_response_error[] = {0x05, 0x00, 0x00, 0x00,
                                             0x19, 0x20, 0x00, 0x00};

  if (receivedBytes == sizeof(message1_response_error) &&
      memcmp(buffer, message1_response_error, receivedBytes) == 0) {
    return false;
  }

  fuji_message(
      sockfd, make_static_message(message_type::start, 0x01, 0x00, 0x00, 0x00));

  auto const msg4_1 =
      make_static_message(message_type::two_part, 0x01, 0xdf, 0x00, 0x00);
  auto const msg4_2 = make_static_message_followup(msg4_1, 0x05, 0x00);
  fuji_twopart_message(sockfd, msg4_1, msg4_2);

  // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31, 'remote mode':
  // 0x24
  fuji_send(sockfd, make_static_message(message_type::single_part, 0x24, 0xdf,
                                        0x00, 0x00));
  fuji_receive_log(sockfd, buffer);
  fuji_receive_log(sockfd, buffer);

  // 'receive mode': 0x21, 'browse mode': 0x22, 'geo mode': 0x31
  auto const msg6_1 =
      make_static_message(message_type::two_part, 0x24, 0xdf, 0x00, 0x00);
  // 'receive mode', 'browse mode': 0x03 0x00 0x00 0x00, 'geo mode': 0x02, 0x00,
  // 0x00, 0x00
  auto const msg6_2 =
      make_static_message_followup(msg6_1, 0x04, 0x00, 0x02, 0x00);
  fuji_twopart_message(sockfd, msg6_1, msg6_2);

  fuji_send(sockfd, make_static_message(message_type::camera_capabilities));
  auto size = fuji_receive_log(sockfd, buffer);

  *caps = parse_camera_caps(buffer, size);

  fuji_receive_log(sockfd, buffer);

  fuji_message(
      sockfd, make_static_message(message_type::camera_remote, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00));

  return true;
}

void terminate_control_connection(native_socket sockfd) {
  if (sockfd <= 0) return;

  log(LOG_INFO, "terminate_control_connection");
  fuji_message(sockfd, make_static_message(message_type::stop));
  uint32_t terminate = 0xffffffff;
  fuji_send(sockfd, &terminate, sizeof(terminate));
}

bool shutter(native_socket const sockfd, native_socket const sockfd2, const char* thumbnail) {
  if (sockfd <= 0) return false;

  log(LOG_INFO, "shutter");
  bool result = fuji_message(
      sockfd, make_static_message(message_type::shutter, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00));
  if (!result)
    return false;

  uint8_t buffer[20 * 1024];
  uint32_t receivedBytes = 0;

  if (sockfd2) {
    receivedBytes = fuji_receive(sockfd2, buffer);
    log(LOG_DEBUG, string_format("received %d bytes (async1) ", receivedBytes).append(hex_format(buffer, receivedBytes)));

    receivedBytes = fuji_receive(sockfd2, buffer);
    log(LOG_DEBUG, string_format("received %d bytes (async2) ", receivedBytes).append(hex_format(buffer, receivedBytes)));
  }

  uint32_t lastMsgId = 0;
  auto const reqImg = make_static_message(message_type::camera_last_image);
  lastMsgId = reqImg.id;
  fuji_send(sockfd, reqImg);

  receivedBytes = fuji_receive(sockfd, buffer);
  log(LOG_INFO, string_format("received %d bytes (thumbnail)", receivedBytes));
  if (thumbnail && sockfd2 && receivedBytes > 8) {
    if (FILE* out = fopen(thumbnail, "wb")) {
      fwrite(buffer + 8, receivedBytes - 8, 1, out);
      fclose(out);
    }
  }

  receivedBytes = fuji_receive(sockfd, buffer);
  log(LOG_DEBUG, string_format("received %d bytes (response) ", receivedBytes).append(hex_format(buffer, receivedBytes)));

  const bool success = is_success_response(lastMsgId, buffer, receivedBytes);

  if (sockfd2) {
    receivedBytes = fuji_receive(sockfd2, buffer);
    log(LOG_DEBUG, string_format("received %d bytes (async3) ", receivedBytes).append(hex_format(buffer, receivedBytes)));
  }

  return success;
}

static bool parse_image_settings(uint32_t const format,
                                 uint32_t const size_aspect,
                                 uint32_t const image_space_on_sdcard,
                                 image_settings& image) {
  switch (format) {
    default:
      return false;
    case 2:
      image.format = image_format_jpeg;
      image.quality = jpeg_quality_fine;
    case 3:
      image.format = image_format_jpeg;
      image.quality = jpeg_quality_normal;
    case 4:
      image.format = image_format_raw_and_jpeg;
      image.quality = jpeg_quality_fine;
      break;
    case 5:
      image.format = image_format_raw_and_jpeg;
      image.quality = jpeg_quality_normal;
      break;
  }

  switch (size_aspect) {
    default:
      return false;
    case 2:
      image.size = jpeg_size_s;
      image.aspect = jpeg_aspect_3_by_2;
      break;
    case 3:
      image.size = jpeg_size_s;
      image.aspect = jpeg_aspect_16_by_9;
      break;
    case 4:
      image.size = jpeg_size_s;
      image.aspect = jpeg_aspect_1_by_1;
      break;
    case 6:
      image.size = jpeg_size_m;
      image.aspect = jpeg_aspect_3_by_2;
      break;
    case 7:
      image.size = jpeg_size_m;
      image.aspect = jpeg_aspect_16_by_9;
      break;
    case 8:
      image.size = jpeg_size_m;
      image.aspect = jpeg_aspect_1_by_1;
      break;
    case 10:
      image.size = jpeg_size_l;
      image.aspect = jpeg_aspect_3_by_2;
      break;
    case 11:
      image.size = jpeg_size_l;
      image.aspect = jpeg_aspect_16_by_9;
      break;
    case 12:
      image.size = jpeg_size_l;
      image.aspect = jpeg_aspect_1_by_1;
      break;
  }

  image.space_on_sdcard = image_space_on_sdcard;

  return true;
}

static bool parse_auto_focus(uint32_t const autofocus_point,
                             auto_focus_point& focus_point) {
  focus_point.x = static_cast<uint8_t>((autofocus_point >> 8) & 0xff);
  focus_point.y = static_cast<uint8_t>((autofocus_point >> 0) & 0xff);
  return true;
}

bool current_settings(native_socket sockfd, camera_settings& settings) {
  settings = camera_settings();

  auto const msg = generate<status_request_message>();
  fuji_send(sockfd, &msg, sizeof(msg));
  uint8_t buf[1024];
  size_t receivedBytes = fuji_receive(sockfd, buf);

  if (receivedBytes == 0)
    return false;

  log(LOG_DEBUG2, string_format("Status: %zd bytes ", receivedBytes).append(hex_format(buf, receivedBytes)));

  uint8_t* ptr = buf;
  ptr += 8; // skip header
  uint16_t numSettings;
  memcpy(&numSettings, ptr, 2);
  ptr += 2;

  uint32_t image_format = 0;
  uint32_t image_size_aspect = 0;
  uint32_t image_space_on_sdcard = 0;

  for (uint16_t i = 0; i < numSettings; ++i)
  {
    property_codes code;
    uint32_t value;
    uint8_t* data = ptr + i * 6;
    memcpy(&code, data, 2);
    memcpy(&value, data + 2, 4);

    std::string log_setting = std::string("Setting msg: ").append(hex_format(&code, 2))
                                                          .append(hex_format(&value, 4));
    log(LOG_DEBUG2, log_setting.append(property_strings[code]));

    switch(code) {
      case property_white_balance: {
        settings.white_balance = static_cast<white_balance_mode>(value);
      } break;

      case property_aperture: {
        settings.aperture.value = value;
      } break;

      case property_focus_mode: {
        // focus mode, S-AF: 0x8001, C-AF: 0x8002, MF: 1
        settings.focus = static_cast<focus_mode>(value);
      } break;

      case property_flash: {
        settings.flash = static_cast<flash_mode>(value);
      } break;

      case property_shooting_mode: {
        settings.shooting = static_cast<shooting_mode>(value);
      } break;

      case property_exposure_compensation: {
        settings.exposure_compensation = static_cast<int16_t>(static_cast<uint16_t>(value));
      } break;

      case property_self_timer: {
        // self-timer, 0 = disabled, 2 = 2s, 4 = 10s
        settings.self_timer = static_cast<timer_mode>(value);
      } break;

      case property_film_simulation: {
        settings.film_simulation = static_cast<film_simulation_mode>(value);
      } break;

      case property_image_format: {
        image_format = value;
      } break;

      case property_shutter_type: {
        // shutter type when shutter is in auto mode, 0 = MS+ES, 1 = ES
        settings.shutter.type = value ? electronic_shutter : mechanical_shutter;
      } break;

      case property_iso: {
        settings.iso = value;
      } break;

      case property_movie_iso: {
        settings.movie_iso = value;
      } break;

      case property_focus_point: {
        parse_auto_focus(value, settings.focus_point);
      } break;

      case property_focus_lock: {
        // focus lock (S1_LOCK_COLOR)
        settings.focus_lock = value;
      } break;

      case property_device_error: {
        settings.device_error = value;
      } break;

      case property_image_space_sd: {
        image_space_on_sdcard = value;
      } break;

      case property_movie_remaining_time: {
        settings.movie_hd_remaining_time = value;
      } break;

      case property_shutter_speed: {
        // shutter speed; 4 bytes; MSB is a flag to indicate whether this
        // is a subsecond value (i.e. 1/Ns)
        settings.shutter.speed = value;
      } break;

      case property_image_aspect: {
        image_size_aspect = value;
      } break;

      case property_battery_level: {
        // battery level, 4: full (3 bars), 3: mid (2 bars), 2: min (1 bar), 1: critical
        settings.battery = static_cast<battery_level>(value);
      } break;
    }
  }

  parse_image_settings(image_format, image_size_aspect, 
          image_space_on_sdcard, settings.image);

  receivedBytes = fuji_receive(sockfd, buf);
  log(LOG_DEBUG2, std::string("received bytes@@@").append(hex_format(buf, receivedBytes)));

  return true;
}

}  // namespace fcwt
