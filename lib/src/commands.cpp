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

void parse_camera_caps_submessage(camera_capabilities& caps,
                                  uint8_t const* data, size_t const size) {
  if (size < 4) return;

  struct submessage_type {
    uint16_t x, y;
  };
  submessage_type type = {};
  memcpy(&type, data, sizeof(type));

  switch (type.y) {
    default: {
      printf("printf_camera_caps_submessage, unknown type (%04X/%04X)\n",
             static_cast<int>(type.x), static_cast<int>(type.y));
      print_hex(data, size);
    } break;
    case 3: {
      switch (type.x) {
        default:
          printf("printf_camera_caps_submessage, unknown type (%04X/%04X)\n",
                 static_cast<int>(type.x), static_cast<int>(type.y));
        case 0x5010:  // Shutter speed auto
        {
          print_hex(data, size);
          // +1/3   4D 01
          // +1     EB 03
          // -1     18 FC
          // -1 1/3 CB FA
          // -3     48 F4
          struct shutter_speed_auto {
            uint8_t unknown0[3];
            uint8_t exposure_compensation_dial_unknown;  // this changes when
                                                         // turning the dial,
                                                         // don't know what it
                                                         // means
            int8_t exposure_compensation_dial_value;     // doesn't match the
                                                      // number on the dial, but
                                                      // whatever
            // more data here
          };
          shutter_speed_auto ssa;
          memcpy(&ssa, data + 4, sizeof(shutter_speed_auto));
          // printf("Exposure compensation: %d %01X\n",
          // static_cast<int>(ssa->exposure_compensation_dial_value),
          // ssa->exposure_compensation_dial_unknown);
          caps.shutter_speed.exposure =
              ssa.exposure_compensation_dial_value;  // TODO: map to -3/+3
                                                     // range?
        }

        break;
      }
    } break;

    case 4: {
      switch (type.x) {
        case 0x5007: {
          uint16_t aperture;
          memcpy(&aperture, data + 7, 2);
          print_hex(data + 4, size - 4);
          caps.aperture.value = aperture;
        } break;
        default:
          printf("printf_camera_caps_submessage, unknown type (%04X/%04X)\n",
                 static_cast<int>(type.x), static_cast<int>(type.y));
        case 0x5012:
        case 0x500C:
        case 0x5005:
        case 0xD001:
        case 0xD019: {
          printf("Submessage: ");
          print_hex(data, size);
        } break;
      }
    } break;

    case 6: {
      switch (type.x) {
        case 0xD02A: {
          const size_t offset =
              16;  // first is type, next 3 are not ISO levels, unknown
          if (size >= offset) {
            caps.iso.numLevels =
                static_cast<uint32_t>(std::min(size - offset, sizeof(caps.iso.levels)) /
                sizeof(*caps.iso.levels));
            memcpy(caps.iso.levels, data + offset,
                   caps.iso.numLevels * sizeof(*caps.iso.levels));
          }
        } break;

        case 0xD240: {
// don't know what caps.shutter_speed[1] means
#if 0
          B: Shutter speed: 1/30s 255 00020200
          T: Shutter speed: 1/4000s 255 00020280
          1: Shutter speed: 1/1s 255 00020200
          2: Shutter speed: 1/2s 255 00020280
          180x: Shutter speed: 1/180s 255 00020280
          Shutter speed: 1/250s 255 00020280
          A: Shutter speed: 1/0s 3 07070000
#endif
          printf("Shutter-speed:\n");
          struct shutter_speed_status {
            uint32_t unknown0;
            uint32_t shutter_speed[2];
            uint32_t unknown1[2];
          };
          print_uint32(data + 4, size - 4);
          print_hex(data + 4, size - 4);
          assert(size == sizeof(shutter_speed_status) + 4);
          shutter_speed_status shutterStatus;
          memcpy(&shutterStatus, data + 4, sizeof(shutterStatus));

          if (static_cast<uint8_t>(shutterStatus.shutter_speed[0]) == 3) {
            caps.shutter_speed.mode = shutter_speed_auto;
          } else {
            caps.shutter_speed.mode = shutter_speed_manual;
            caps.shutter_speed.value =
                (shutterStatus.shutter_speed[0] >> 8) / 1000;
          }
        } break;
        case 0xD17C: {
          printf("Autofocus: ");
          print_hex(data + 4, size - 4);
        } break;

        default:
          printf("printf_camera_caps_submessage, unknown type (%04X/%04X)\n",
                 static_cast<int>(type.x), static_cast<int>(type.y));
          break;
      }
    } break;
  }
}

camera_capabilities parse_camera_caps(void const* data, size_t const size) {
  camera_capabilities caps = {};
  size_t remainingBytes = size;
  uint8_t const* bytes = static_cast<uint8_t const*>(data);
  if (remainingBytes < 12) return caps;

  bytes += 12;  // 12 bytes unknown
  remainingBytes -= 12;

  while (remainingBytes > 0) {
    // sub-message size
    if (remainingBytes < 4) {
      printf(
          "Inconsistent message when getting next "
          "submessage(remaingBytes=%zu)\n",
          remainingBytes);
      break;
    }
    uint32_t subMsgSize = 0;
    memcpy(&subMsgSize, bytes, sizeof(subMsgSize));
    if (subMsgSize < 4) {
      printf("Inconsistent submessage(subMsgSize=%d)\n", subMsgSize);
      break;
    }
    subMsgSize -= 4;
    bytes += sizeof(subMsgSize);
    remainingBytes -= sizeof(subMsgSize);

    // sub-message actual sub-message
    if (remainingBytes < subMsgSize) {
      printf("Inconsistent message (subMsgSize=%d)\n", subMsgSize);
      break;
    }

    parse_camera_caps_submessage(caps, bytes, subMsgSize);

    bytes += subMsgSize;
    remainingBytes -= subMsgSize;
  }

  return caps;
}

}  // namespace

bool update_setting(native_socket sockfd, iso_level iso) {
  auto const msg_1 =
      make_static_message(message_type::two_part, 0x2A, 0xD0, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(iso));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, image_settings image) {
  return false;
}

bool update_setting(native_socket sockfd, film_simulation_mode film) {
  return false;
}

bool update_setting(native_socket sockfd, auto_focus_point point) {
  return false;
}

bool update_setting(native_socket sockfd, white_balance_mode white_balance) {
  //10:00:00:00 01:00:16:10 68:00:00:00 05:50:00:00
  //0e:00:00:00 02:00:16:10 68:00:00:00 06:80
  auto const msg_1 =
    make_static_message(message_type::two_part, 0x05, 0x50, 0x00, 0x00);
  auto const msg_2 = make_static_message_followup(msg_1, make_byte_array(static_cast<uint16_t>(white_balance)));
  return fuji_twopart_message(sockfd, msg_1, msg_2);
}

bool update_setting(native_socket sockfd, aperture_f_stop aperture) {
  auto const msg = make_static_message(
      message_type::aperture, aperture == aperture_close_third_stop ? 1 : 0, 0, 0, 0);
  return fuji_message(sockfd, msg);
}

bool update_setting(native_socket sockfd, shutter_speed_stop shutter_speed) {
  auto const msg = make_static_message(
      message_type::shutter_speed, shutter_speed == shutter_speed_one_stop_faster ? 1 : 0, 0, 0, 0);
  return fuji_message(sockfd, msg);
}

bool init_control_connection(native_socket const sockfd, char const* deviceName,
                             camera_capabilities* caps) {
  if (sockfd <= 0) return false;

  if (!deviceName || !deviceName[0]) deviceName = "CameraClient";

  LOG_INFO_FORMAT("init_control_connection (socket %lld)", static_cast<long long>(sockfd));
  auto const reg_msg = generate_registration_message(deviceName);
  LOG_INFO("send hello");
  fuji_send(sockfd, &reg_msg, sizeof(reg_msg));

  uint8_t buffer[1024];
  size_t const receivedBytes = fuji_receive(sockfd, buffer);
  uint8_t const message1_response_error[] = {0x05, 0x00, 0x00, 0x00,
                                             0x19, 0x20, 0x00, 0x00};

  if (receivedBytes == sizeof(message1_response_error) &&
      memcmp(buffer, message1_response_error, receivedBytes) == 0) {
	  return false;
  }

  auto msg2 = make_static_message(message_type::start, 0x01, 0x00, 0x00, 0x00);
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
  print_uint32(buffer, size);
  print_ascii(buffer, size);

  if (caps) *caps = parse_camera_caps(buffer, size);

  fuji_receive_log(sockfd, buffer);

  fuji_message(
      sockfd, make_static_message(message_type::camera_remote, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00, 0x00));

  return true;
}

void terminate_control_connection(native_socket sockfd) {
  if (sockfd <= 0) return;

  LOG_INFO("terminate_control_connection");
  fuji_message(sockfd, make_static_message(message_type::stop));
  uint32_t terminate = 0xffffffff;
  fuji_send(sockfd, &terminate, sizeof(terminate));
}

bool shutter(native_socket const sockfd) {
  if (sockfd <= 0) return false;

  LOG_INFO("shutter");
  return fuji_message(
      sockfd, make_static_message(message_type::shutter, 0x00, 0x00, 0x00, 0x00,
                                  0x00, 0x00, 0x00, 0x00));

#if 0
  // Not sure why we don't get the image as response
  uint8_t buffer[20 * 1024];
  uint32_t receivedBytes = 0;

  uint32_t lastMsgId = 0;
  bool retry = true;
  while (retry)
  {
    LOG_INFO("shutter_message_2");
    auto const reqImg = make_static_message(message_type::camera_last_image);
    lastMsgId = reqImg.id;
    fuji_send(sockfd, reqImg);

    receivedBytes = fuji_receive(sockfd, buffer);
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);
    retry = receivedBytes <= 8;
    
    receivedBytes = fuji_receive(sockfd, buffer);
    LOG_INFO_FORMAT("received %d bytes", receivedBytes);
    print_hex(buffer, receivedBytes);
  }
  return is_success_response(lastMsgId, buffer, receivedBytes);
#endif
}

static bool parse_film_simulation_mode(uint32_t value,
                                       film_simulation_mode& mode) {
	value >>= 16;
	if (value == 0 || value >> film_simulation_count)
		return false;
 
  mode = static_cast<film_simulation_mode>(value);
  return true;
}

static bool parse_image_settings(uint32_t const format,
                                 uint32_t const size_aspect,
                                 uint32_t const image_space_on_sdcard,
                                 image_settings& image) {
  switch (format >> 16) {
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

  image.space_on_sdcard = image_space_on_sdcard >> 16;

  return true;
}

static bool parse_auto_focus(uint32_t const autofocus_point,
                             auto_focus_point& focus_point) {
  focus_point.x = static_cast<uint8_t>(autofocus_point >> 24);
  focus_point.y = static_cast<uint8_t>((autofocus_point >> 16) & 0xff);
  return true;
}

bool current_settings(native_socket sockfd, camera_settings& settings) {
  settings = camera_settings();

  auto const msg = generate<status_request_message>();
  // printf("Status request %d\n", msg.id);
  fuji_send(sockfd, &msg, sizeof(msg));
  uint8_t buf[1024];
  size_t receivedBytes = fuji_receive(sockfd, buf);
  printf("Status: %zd bytes\n", receivedBytes);
  print_hex(buf, receivedBytes);
  //print_uint32(buf, receivedBytes);
  //print_ascii(buf, receivedBytes);

  bool success = true;

  memcpy(&settings.aperture.value, &buf[8 + 76], 4);

  uint32_t white_balance;
  memcpy(&white_balance, &buf[8 + 88], 4);
  success = success &&
            parse_white_balance_mode(white_balance, settings.white_balance);

  uint32_t film_simulation;
  memcpy(&film_simulation, &buf[8 + 92], 4);
  success = success && parse_film_simulation_mode(film_simulation,
                                                  settings.film_simulation);

  memcpy(&settings.iso, &buf[8 + 58], 4);

  uint32_t shutter_speed;
  memcpy(&shutter_speed, &buf[8 + 70], 4);
  const uint32_t shutter_speed_value_mask = ~0x80000000;
  settings.shutter_speed = shutter_speed & shutter_speed_value_mask;
  settings.one_div_shutter_speed = settings.shutter_speed != shutter_speed;
  
  uint32_t image_format, image_size_aspect, image_space_on_sdcard;
  memcpy(&image_format, &buf[8 + 44], 4);
  memcpy(&image_size_aspect, &buf[8 + 52], 4);
  memcpy(&image_space_on_sdcard, &buf[8 + 20], 4);
  success = success && parse_image_settings(image_format, image_size_aspect, 
          image_space_on_sdcard,
                                            settings.image);

  // only seems to work for single point, have not found data for 'zone'.
  uint32_t autofocus_point;
  memcpy(&autofocus_point, &buf[8 + 104], 4);
  success =
      success && parse_auto_focus(autofocus_point, settings.focus_point);

  // TODO: pop-up flash and flash in general? Don't care for now.
#if 0
  uint32_t flash;
  memcpy(&flash, &buf[8 + 8], 4);
  printf("flash raw=%08X\n", flash);
#endif

  receivedBytes = fuji_receive(sockfd, buf);
  // TODO error check

  return success;
}

}  // namespace fcwt
