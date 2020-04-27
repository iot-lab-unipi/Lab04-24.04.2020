#include "contiki.h"
#include "sys/etimer.h"
/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-debug.h"
#include "node-id.h"
#include "simple-udp.h"
#include "lib/random.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 1234
#define SEND_INTERVAL		(6 * CLOCK_SECOND)
static struct simple_udp_connection unicast_connection;

/*---------------------------------------------------------------------------*/

PROCESS(unicast_sender_process, "Unicast sender example process");
AUTOSTART_PROCESSES(&unicast_sender_process);

/*---------------------------------------------------------------------------*/

static void receiver(struct simple_udp_connection *c, const uip_ipaddr_t *sender_addr, uint16_t sender_port, const uip_ipaddr_t *receiver_addr, uint16_t receiver_port, const uint8_t *data, uint16_t datalen){
         printf("Data received on port %d from port %d with length %d, %s\n", receiver_port, sender_port, datalen, data);
}
/*---------------------------------------------------------------------------*/

static void set_global_address(void){

  uip_ipaddr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xfd00, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(unicast_sender_process, ev, data){

  static struct etimer periodic_timer;
  uip_ipaddr_t addr;
  static unsigned int message_number;
  PROCESS_BEGIN();
  set_global_address();
  simple_udp_register(&unicast_connection, UDP_PORT, NULL, UDP_PORT, receiver);
  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_set(&periodic_timer, (random_rand() % (SEND_INTERVAL)));
    uip_ip6addr(&addr, 0xfe80, 0, 0, 0, 0x0202, 0x0002, 0x0002, 0x0002);
    char buf[300];
    printf("Sending unicast to ");
    uip_debug_ipaddr_print(&addr);
    printf("\n");
    sprintf(buf, "Message %d from node %d", message_number, node_id);
    message_number++;
    simple_udp_sendto(&unicast_connection, buf, strlen(buf) + 1, &addr);
  }
  PROCESS_END();
}



