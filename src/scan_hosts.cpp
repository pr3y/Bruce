/* ICMP echo example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
//#include "lwip/inet.h"
//#include "lwip/netdb.h"
//#include "lwip/sockets.h"
#include "esp_console.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "argtable3/argtable3.h"
//#include "protocol_examples_common.h"
#include "ping/ping_sock.h"
#include "display.h"
#include "globals.h"

//#define IPADDR_NONE ((uint32_t)0xffffffffUL)

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    tft.printf("%" PRIu32 " bytes from %s icmp_seq=%" PRIu16 " ttl=%" PRIu16 " time=%" PRIu32 " ms\n",
           recv_len, ipaddr_ntoa((ip_addr_t*)&target_addr), seqno, ttl, elapsed_time);
}

void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;
    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    tft.printf("From %s icmp_seq=%d timeout\n",ipaddr_ntoa((ip_addr_t*)&target_addr), seqno);
}

void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;
    uint32_t loss;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

    if (transmitted > 0) {
        loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
    } else {
        loss = 0;
    }
    if (IP_IS_V4(&target_addr)) {
       //tft.printf("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
    } else {
        //tft.printf("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
    }
    tft.printf("%" PRIu32 " packets transmitted, %" PRIu32 " received, %" PRIu32 "%% packet loss, time %" PRIu32 "ms\n",
           transmitted, received, loss, total_time_ms);
    // delete the ping sessions, so that we clean up all resources and can create a new ping session
    // we don't have to call delete function in the callback, instead we can call delete function from other tasks
    esp_ping_delete_session(hdl);
}

int do_ping_cmd()
{
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();

    ip_addr_t target_addr;
    memset(&target_addr, 0, sizeof(target_addr));


    esp_ping_handle_t ping;
    //esp_ping_new_session(&config, &cbs, &ping);
    esp_ping_start(ping);

    return 0;
}



static esp_console_repl_t *s_repl = NULL;

/* handle 'quit' command */
int do_cmd_quit()
{
    tft.printf("ByeBye\r\n");
    s_repl->del(s_repl);
    return 0;
}

esp_err_t register_quit()
{
    esp_console_cmd_t command = {
        .command = "quit",
        .help = "Quit REPL environment",
       // .func = &do_cmd_quit
    };
    return esp_console_cmd_register(&command);
}

void local_scan_setup()
{
    ip_addr_t target_addr;
    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    esp_ping_callbacks_t cbs = {
        .on_ping_success = cmd_ping_on_ping_success,
        .on_ping_timeout = cmd_ping_on_ping_timeout,
        .on_ping_end = cmd_ping_on_ping_end,
       // .cb_args = NULL,
    };

    for (int i = 1; i <= 254; i++) { 
        char ip_addr_str[16];
        tft.printf(ip_addr_str, "192.168.0.%d", i);
        ip_addr_t addr;
        ipaddr_aton(ip_addr_str, &addr);
        config.target_addr = addr;

        esp_ping_handle_t ping;
        memset(&target_addr, 0, sizeof(target_addr));
        esp_ping_new_session(&config, &cbs, &ping);
        esp_ping_start(ping);
    }

}
