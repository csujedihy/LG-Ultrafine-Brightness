#include <iostream>
#include <vector>
#include <conio.h>
#include "hidapi.h"

using std::vector;
const uint16_t vendor_id = 0x43e;
const uint16_t product_id = 0x9a40;
const uint16_t max_brightness = 0xd2f0;
const uint16_t min_brightness = 0x0190;

const std::vector<uint16_t> small_steps = {
    0x0190, 0x01af, 0x01d2, 0x01f7,
    0x021f, 0x024a, 0x0279, 0x02ac,
    0x02e2, 0x031d, 0x035c, 0x03a1,
    0x03eb, 0x043b, 0x0491, 0x04ee,
    0x0553, 0x05c0, 0x0635, 0x06b3,
    0x073c, 0x07d0, 0x086f, 0x091b,
    0x09d5, 0x0a9d, 0x0b76, 0x0c60,
    0x0d5c, 0x0e6c, 0x0f93, 0x10d0,
    0x1227, 0x1399, 0x1529, 0x16d9,
    0x18aa, 0x1aa2, 0x1cc1, 0x1f0b,
    0x2184, 0x2430, 0x2712, 0x2a2e,
    0x2d8b, 0x312b, 0x3516, 0x3951,
    0x3de2, 0x42cf, 0x4822, 0x4de1,
    0x5415, 0x5ac8, 0x6203, 0x69d2,
    0x7240, 0x7b5a, 0x852d, 0x8fc9,
    0x9b3d, 0xa79b, 0xb4f5, 0xc35f,
    0xd2f0,
};
const std::vector<uint16_t> big_steps = {
    0x0190, 0x021f, 0x02e2, 0x03eb,
    0x0553, 0x073c, 0x09d5, 0x0d5c,
    0x1227, 0x18aa, 0x2184, 0x2d8b,
    0x3de2, 0x5415, 0x7240, 0x9b3d,
    0xd2f0,
};

uint16_t next_step(uint16_t val, const vector<uint16_t> &steps)
{
    auto start = 0;
    auto end = steps.size() - 1;
    while (start + 1 < end) {
        auto mid = start + (end - start) / 2;
        if (steps[mid] > val) {
            end = mid;
        } else {
            start = mid;
        }
    }
    return steps[end];
}

uint16_t prev_step(uint16_t val, const vector<uint16_t> &steps)
{
    auto start = 0;
    auto end = steps.size() - 1;
    while (start + 1 < end) {
        auto mid = start + (end - start) / 2;
        if (steps[mid] >= val) {
            end = mid;
        }  else {
            start = mid;
        }
    }
    return steps[start];
}

uint16_t get_brightness(hid_device *handle) {
    uint8_t data[7] = {0};
    int res = hid_get_feature_report(handle, data, sizeof(data));
    if (res < 0) {
        printf("Unable to get a feature report.\n");
        printf("%ls", hid_error(handle));
    }
    return data[1] + (data[2] << 8);
}

void set_brightness(hid_device *handle, uint16_t val) {
    uint8_t data[7] = {
        0x00,
        uint8_t(val & 0x00ff),
        uint8_t((val >> 8) & 0x00ff),
        0x00, 0x00, 0x00, 0x00};
    int res = hid_send_feature_report(handle, data, sizeof(data));
    if (res < 0) {
        printf("Unable to set a feature report.\n");
        printf("%ls", hid_error(handle));
    }
}

int main(int argc, char *argv[]) {
    hid_device *handle;
    struct hid_device_info *devs, *cur_dev;

    if (hid_init())
        return -1;

    char* ultrafine_monitor_path = NULL;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        if (cur_dev->vendor_id == 0x043e)   //0x043e means LG
        {
            printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            printf("\n");
            printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            printf("  Product:      %ls\n", cur_dev->product_string);
            printf("  Release:      %hx\n", cur_dev->release_number);
            printf("  Interface:    %d\n", cur_dev->interface_number);
            printf("\n");

            if (wcsstr(cur_dev->product_string, L"BRIGHTNESS"))
            {
                ultrafine_monitor_path = cur_dev->path;
                break;
            }
        }

        cur_dev = cur_dev->next;
    }

    if (ultrafine_monitor_path == NULL)
    {
        printf("device not found!");
        exit(-1);
    }

    handle = hid_open_path(ultrafine_monitor_path);
    if (!handle) {
        printf("unable to open device\n");
        return 1;
    }

    auto brightness = get_brightness(handle);
    printf("Press '-' or '=' to adjust brightness.\n");
    printf("Press '[' or: ']' to fine tune.\n");
    printf("Press 'p' to use the minimum brightness\n");
    printf("Press '\\' to use the maximum brightness\n");
    printf("Press 'q' to quit.\n");

    while (true) {
        printf("Current brightness = %d%4s\r", int((float(brightness) / 54000) * 100.0), " ");
        int c = _getch();
        if (c == 'q') {
            break;
        }
        switch (c) {
            case '+':
            case '=':
                brightness = next_step(brightness, big_steps);
                set_brightness(handle, brightness);
                break;
            case '-':
            case '_':
                brightness = prev_step(brightness, big_steps);
                set_brightness(handle, brightness);
                break;
            case ']':
                brightness = next_step(brightness, small_steps);
                set_brightness(handle, brightness);
                break;
            case '[':
                brightness = prev_step(brightness, small_steps);
                set_brightness(handle, brightness);
                break;
            case '\\':
                brightness = max_brightness;
                set_brightness(handle, brightness);
                break;
            case 'p':
                brightness = min_brightness;
                set_brightness(handle, brightness);
                break;
            default:
                break;
        }
    }

    hid_close(handle);
    hid_exit();
    return 0;
}
